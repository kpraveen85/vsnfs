/*
 * CSE506 - VSNFS
 *
 * vsnfsXdr.c --
 * 
 * Implementation of XDR routines
 *
 *(C) Karthik Balaji <findkb@gmail.com>
 *
 */

#include <linux/pagemap.h>
#include "vsnfs.h"
#include "vsnfsXdr.h"

#define errno_VSNFSERR_IO		EIO

#define VSNFS_fhandle_sz		8
#define VSNFS_filename_sz		(1+(VSNFS_MAXNAMLEN>>2))
#define VSNFS_path_sz			(1+(VSNFS_MAXPATHLEN>>2))
#define VSNFS_info_sz			5
#define VSNFS_entry_sz			(VSNFS_filename_sz+3)

#define VSNFS_enc_void_sz		0
#define	VSNFS_readdirargs_sz	(VSNFS_fhandle_sz+2)

#define VSNFS_dec_void_sz		0
#define VSNFS_readdirres_sz		(VSNFS_fhandle_sz+2)

/*
 * Common VSNFS XDR functions as inlines
 */
static inline __be32 *
xdr_encode_fhandle(__be32 *p, const struct vsnfs_fh *fhandle)
{
	memcpy(p, fhandle->data, VSNFS_FHSIZE);
	return p + XDR_QUADLEN(VSNFS_FHSIZE);
}

static inline __be32 *
xdr_decode_fhandle(__be32 *p, struct vsnfs_fh *fhandle)
{
	/* VSNFS handles have a fixed length */
	fhandle->size = VSNFS_FHSIZE;
	memcpy(fhandle->data, p, VSNFS_FHSIZE);
	return p + XDR_QUADLEN(VSNFS_FHSIZE);
}

/*
 * VSNFS encode functions
 */

/* NULL procedure encode */
static int
vsnfs_xdr_enc_void(struct rpc_rqst *req, __be32 *p, void *dummy)
{
	req->rq_slen = xdr_adjust_iovec(req->rq_svec, p);
	return 0;
}

/* Encode file handle */
static int
vsnfs_xdr_fhandle(struct rpc_rqst *req, __be32 *p, struct vsnfs_fh *fh)
{
	p = xdr_encode_fhandle(p, fh);
	req->rq_slen = xdr_adjust_iovec(req->rq_svec, p);
	return 0;
}

/*
 * Encode arguments to readdir call
 */
static int
vsnfs_xdr_readdirargs(struct rpc_rqst *req, __be32 *p, struct vsnfs_readdirargs *args)
{
	struct rpc_task	*task = req->rq_task;
	struct rpc_auth	*auth = task->tk_msg.rpc_cred->cr_auth;
	unsigned int replen;
	u32 count = args->count;

	p = xdr_encode_fhandle(p, args->fh);
	*p++ = htonl(args->cookie);
	*p++ = htonl(count); /* see above */
	req->rq_slen = xdr_adjust_iovec(req->rq_svec, p);

	/* Inline the page array */
	replen = (RPC_REPHDRSIZE + auth->au_rslack + VSNFS_readdirres_sz) << 2;
	xdr_inline_pages(&req->rq_rcv_buf, replen, args->pages, 0, count);
	return 0;
}

/*
 * VSNFS decode functions
 */

/* NULL procedure decode */
static int
vsnfs_xdr_dec_void(struct rpc_rqst *req, __be32 *p, void *dummy)
{
	return 0;
}

/*
 * Decode the result of a readdir call.
 * We're not really decoding anymore, we just leave the buffer untouched
 * and only check that it is syntactically correct.
 * The real decoding happens in nfs_decode_entry below, called directly
 * from nfs_readdir for each entry.
 */
static int
vsnfs_xdr_readdirres(struct rpc_rqst *req, __be32 *p, void *dummy)
{
	struct xdr_buf *rcvbuf = &req->rq_rcv_buf;
	struct kvec *iov = rcvbuf->head;
	struct page **page;
	size_t hdrlen;
	unsigned int pglen, recvd;
	u32 len;
	int status, nr = 0;
	__be32 *end, *entry, *kaddr;

	if ((status = ntohl(*p++)))
		return vsnfs_stat_to_errno(status);

	hdrlen = (u8 *) p - (u8 *) iov->iov_base;
	if (iov->iov_len < hdrlen) {
		printk("VSNFS: READDIR reply header overflowed:"
				"length %Zu > %Zu\n", hdrlen, iov->iov_len);
		return -errno_VSNFSERR_IO;
	} else if (iov->iov_len != hdrlen) {
		printk("VSNFS: READDIR header is short. iovec will be shifted.\n");
		xdr_shift_buf(rcvbuf, iov->iov_len - hdrlen);
	}

	pglen = rcvbuf->page_len;
	recvd = rcvbuf->len - hdrlen;
	if (pglen > recvd)
		pglen = recvd;
	page = rcvbuf->pages;
	kaddr = p = kmap_atomic(*page, KM_USER0);
	end = (__be32 *)((char *)p + pglen);
	entry = p;

	/* Make sure the packet actually has a value_follows and EOF entry */
	if ((entry + 1) > end)
		goto short_pkt;

	for (; *p++; nr++) {
		if (p + 2 > end)
			goto short_pkt;
		p++; /* fileid */
		len = ntohl(*p++);
		p += XDR_QUADLEN(len) + 1;	/* name plus cookie */
		if (len > VSNFS_MAXNAMLEN) {
			printk("VSNFS: giant filename in readdir (len 0x%x)!\n",
						len);
			goto err_unmap;
		}
		if (p + 2 > end)
			goto short_pkt;
		entry = p;
	}

	/*
	 * Apparently some server sends responses that are a valid size, but
	 * contain no entries, and have value_follows==0 and EOF==0. For
	 * those, just set the EOF marker.
	 */
	if (!nr && entry[1] == 0) {
		printk("VSNFS: readdir reply truncated!\n");
		entry[1] = 1;
	}
 out:
	kunmap_atomic(kaddr, KM_USER0);
	return nr;
 short_pkt:
	/*
	 * When we get a short packet there are 2 possibilities. We can
	 * return an error, or fix up the response to look like a valid
	 * response and return what we have so far. If there are no
	 * entries and the packet was short, then return -EIO. If there
	 * are valid entries in the response, return them and pretend that
	 * the call was successful, but incomplete. The caller can retry the
	 * readdir starting at the last cookie.
	 */
	entry[0] = entry[1] = 0;
	if (!nr)
		nr = -errno_VSNFSERR_IO;
	goto out;
err_unmap:
	nr = -errno_VSNFSERR_IO;
	goto out;
}

__be32 *
vsnfs_decode_dirent(__be32 *p, struct vsnfs_entry *entry, int plus)
{
	if (!*p++) {
		if (!*p)
			return ERR_PTR(-EAGAIN);
		entry->eof = 1;
		return ERR_PTR(-EBADCOOKIE);
	}

	entry->ino	  = ntohl(*p++);
	entry->len	  = ntohl(*p++);
	entry->name	  = (const char *) p;
	p		 += XDR_QUADLEN(entry->len);
	entry->prev_cookie	  = entry->cookie;
	entry->cookie	  = ntohl(*p++);
	entry->eof	  = !p[0] && p[1];

	return p;
}

static struct {
	int stat;
	int errno;
} vsnfs_errtbl[] = {
	{ VSNFS_OK,				0		},
	{ VSNFSERR_NOENT,		-ENOENT		},
	{ VSNFSERR_IO,			-errno_VSNFSERR_IO},
	{ VSNFSERR_NXIO,		-ENXIO		},
	{ VSNFSERR_EAGAIN,		-EAGAIN		},
	{ VSNFSERR_EXIST,		-EEXIST		},
	{ VSNFSERR_NODEV,		-ENODEV		},
	{ VSNFSERR_NOTDIR,		-ENOTDIR	},
	{ VSNFSERR_ISDIR,		-EISDIR		},
	{ VSNFSERR_INVAL,		-EINVAL		},
	{ VSNFSERR_FBIG,		-EFBIG		},
	{ VSNFSERR_NOSPC,		-ENOSPC		},
	{ VSNFSERR_ROFS,		-EROFS		},
	{ VSNFSERR_MLINK,		-EMLINK		},
	{ VSNFSERR_OPNOTSUPP	-EOPNOTSUPP },
	{ VSNFSERR_NAMETOOLONG,	-ENAMETOOLONG	},
	{ VSNFSERR_NOTEMPTY,	-ENOTEMPTY	},
	{ VSNFSERR_DQUOT,		-EDQUOT		},
	{ VSNFSERR_STALE,		-ESTALE		},
	{ VSNFSERR_REMOTE,		-EREMOTE	},
	{ VSNFSERR_BADHANDLE,	-EBADHANDLE	},
	{ VSNFSERR_BAD_COOKIE	-EBADCOOKIE },
	{ -1,					-EIO		}
};

/*
 * Convert an VSNFS error code to a local one.
 */
int
vsnfs_stat_to_errno(int stat)
{
	int i;

	for (i = 0; vsnfs_errtbl[i].stat != -1; i++) {
		if (vsnfs_errtbl[i].stat == stat)
			return vsnfs_errtbl[i].errno;
	}
	printk("vsnfs_stat_to_errno: bad vsnfs status return value: %d\n", stat);
	return vsnfs_errtbl[i].errno;
}

#define PROC(proc, argtype, restype)				\
[VSNFSPROC_##proc] = {							\
	.p_proc	    =  VSNFSPROC_##proc,					\
	.p_encode   =  (kxdrproc_t) vsnfs_xdr_##argtype,			\
	.p_decode   =  (kxdrproc_t) vsnfs_xdr_##restype,			\
	.p_arglen   =  VSNFS_##argtype##_sz,				\
	.p_replen   =  VSNFS_##restype##_sz,				\
	.p_statidx  =  VSNFSPROC_##proc,					\
	.p_name     =  #proc,						\
	}

struct rpc_procinfo vsnfs_procedures[] = {
	PROC(NULL,			enc_void,		dec_void),
	PROC(GETROOT,		fhandle,		dec_void),
//	PROC(LOOKUP,        diropargs,      diropres),
//	PROC(READ,          readargs,       readres),
//	PROC(WRITE,			writeargs,		writeres),
//	PROC(CREATE,        createargs,     diropres),
//	PROC(REMOVE,        diropargs,      stat),
//	PROC(MKDIR,         createargs,     diropres),
//	PROC(RMDIR,         diropargs,      stat),
    PROC(READDIR,       readdirargs,    readdirres),
};

struct rpc_version vsnfs_version1 = {
	.number		= 1,
	.nrprocs	= ARRAY_SIZE(vsnfs_procedures),
	.procs		= vsnfs_procedures
};

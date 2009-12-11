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

/* Size should be number of 32 bits (XDR word size) */
#define VSNFS_fhandle_sz		8
#define VSNFS_filename_sz		(1+(VSNFS_MAXNAMLEN>>2))
#define VSNFS_path_sz			(1+(VSNFS_MAXPATHLEN>>2))
#define VSNFS_info_sz			5
#define VSNFS_entry_sz			(VSNFS_filename_sz+3)

#define VSNFS_nullargs_sz		(1)	/*check this part */
#define VSNFS_getrootargs_sz            (VSNFS_path_sz)
#define VSNFS_lookupargs_sz             (VSNFS_fhandle_sz+VSNFS_path_sz)
/*1 for type and 1 for count*/
#define	VSNFS_readdirargs_sz	        (VSNFS_fhandle_sz+2)
#define	VSNFS_readargs_sz	        (VSNFS_fhandle_sz+3) /*type, offset and len*/

/* 1 for status and 1 for type */
#define VSNFS_fh_sz             (1+VSNFS_fhandle_sz+1)
#define VSNFS_nullres_sz		(1)
#define VSNFS_readdirres_sz		(1)	/*status */
#define VSNFS_readres_sz		(1+1)	/*status and count*/


/*
 * Common VSNFS XDR functions as inlines
 */
static inline __be32 *xdr_encode_fhandle(__be32 * p, const struct vsnfs_fh *fh)
{
	memcpy(p, fh->data, VSNFS_FHSIZE);
	p = p + XDR_QUADLEN(VSNFS_FHSIZE);
	*p++ = htonl(fh->type);
	return p;
}

static inline __be32 *xdr_decode_fhandle(__be32 * p, struct vsnfs_fh *fh)
{
	/* VSNFS handles have a fixed length */
	memcpy(fh->data, p, VSNFS_FHSIZE);
	p = p + XDR_QUADLEN(VSNFS_FHSIZE);
	fh->type = ntohl(*p++);

	return p;
}

/*
 * VSNFS encode functions
 */

/* Encode file handle */
#if 0
static int vsnfs_xdr_fh(struct rpc_rqst *req, __be32 * p, struct vsnfs_fh *fh)
{
	p = xdr_encode_fhandle(p, fh);
	req->rq_slen = xdr_adjust_iovec(req->rq_svec, p);
	return 0;
}
#endif

/* NULL procedure encode */
static int
vsnfs_xdr_nullargs(struct rpc_rqst *req, __be32 * p,
		   struct vsnfs_nullargs *args)
{
	*p++ = htonl(args->dummy);
	req->rq_slen = xdr_adjust_iovec(req->rq_svec, p);
	return 0;
}

/* Encode Function for getroot RPC call */

static int
vsnfs_xdr_getrootargs(struct rpc_rqst *req, __be32 * p,
		      struct vsnfs_getrootargs *args)
{
	p = xdr_encode_array(p, args->path, args->len);
	req->rq_slen = xdr_adjust_iovec(req->rq_svec, p);
	return 0;

}

/* Encode Function for lookup RPC call */

static int
vsnfs_xdr_lookupargs(struct rpc_rqst *req, __be32 * p,
		     struct vsnfs_lookupargs *args)
{
	p = xdr_encode_fhandle(p, &args->fh);
	vsnfs_trace(KERN_DEFAULT, "encode array %s : %d", args->filename,
		    args->len);
	p = xdr_encode_array(p, args->filename, args->len);
	req->rq_slen = xdr_adjust_iovec(req->rq_svec, p);
	return 0;

}

/*
 * Encode arguments to readdir call
 */
static int
vsnfs_xdr_readdirargs(struct rpc_rqst *req, __be32 * p,
		      struct vsnfs_readdirargs *args)
{
	struct rpc_task *task = req->rq_task;
	struct rpc_auth *auth = task->tk_msg.rpc_cred->cr_auth;
	unsigned int replen;
	u32 count = args->count;

	vsnfs_trace(KERN_DEFAULT, "\n");

	p = xdr_encode_fhandle(p, args->fh);
	*p++ = htonl(count);
	req->rq_slen = xdr_adjust_iovec(req->rq_svec, p);

	/* Inline the page array
	   earlier it xdr_buff is just represented by 'head' vec
	   after this call, it's split into head & tail and pages 
	   (which are logically between head and tail) are added to the buff */
	/* check auth->au_rslack field */
	replen = (RPC_REPHDRSIZE + auth->au_rslack + VSNFS_readdirres_sz) << 2;
	xdr_inline_pages(&req->rq_rcv_buf, replen, args->pages, 0, count);
	return 0;
}

/*
 * Encode arguments to read call
 */
static int
vsnfs_xdr_readargs(struct rpc_rqst *req, __be32 * p,
		      struct vsnfs_readargs *args)
{
	struct rpc_task *task = req->rq_task;
	struct rpc_auth *auth = task->tk_msg.rpc_cred->cr_auth;
	unsigned int replen;
	u32 count = args->length;

	vsnfs_trace(KERN_DEFAULT, "\n");

	p = xdr_encode_fhandle(p, args->fh);
	*p++ = htonl(args->offset);
	*p++ = htonl(args->length);
	req->rq_slen = xdr_adjust_iovec(req->rq_svec, p);

	/* Inline the page array
	   earlier it xdr_buff is just represented by 'head' vec
	   after this call, it's split into head & tail and pages 
	   (which are logically between head and tail) are added to the buff */
	/* check auth->au_rslack field */
	replen = (RPC_REPHDRSIZE + auth->au_rslack + VSNFS_readdirres_sz) << 2;
	xdr_inline_pages(&req->rq_rcv_buf, replen, args->pages, 0, count);
	return 0;
}



/*
 * VSNFS decode functions
 */

/* NULL procedure decode */

/* Decode function for NULL proc response
 *  Returns	0 on success
 * 			Error number corr. to  VSNFS status on failure
 */

static int
vsnfs_xdr_nullres(struct rpc_rqst *req, __be32 * p, struct vsnfs_nullres *res)
{
	int status;

	if ((status = ntohl(*p++))) {
		vsnfs_trace(KERN_DEFAULT, "status %d\n", status);
		return vsnfs_stat_to_errno(status);
	}

	res->dummy = ntohl(*p++);
	vsnfs_trace(KERN_DEFAULT, "result %d\n", res->dummy);
	return 0;
}

static int vsnfs_xdr_fh(struct rpc_rqst *req, __be32 * p, struct vsnfs_fh *fh)
{
	/* VSNFS handles have a fixed length */
	int status;

	if ((status = ntohl(*p++))) {
		vsnfs_trace(KERN_DEFAULT, "status %d\n", status);
		return vsnfs_stat_to_errno(status);
	}

	memcpy(fh->data, p, VSNFS_FHSIZE);
	p = p + XDR_QUADLEN(VSNFS_FHSIZE);
	fh->type = ntohl(*p++);

	return 0;
}

/*
 * Decode the result of a readdir call.
 * We're not really decoding anymore, we just leave the buffer untouched
 * and only check that it is syntactically correct.
 * The real decoding happens in nfs_decode_entry below, called directly
 * from nfs_readdir for each entry.
 */
static int vsnfs_xdr_readdirres(struct rpc_rqst *req, __be32 * p, void *dummy)
{	
	int status;

	vsnfs_trace(KERN_DEFAULT, "\n");

	if ((status = ntohl(*p++)))
		return vsnfs_stat_to_errno(status);
	
	return 0;
}

static int vsnfs_xdr_readres(struct rpc_rqst *req, __be32 * p,
		   struct vsnfs_readres *res)
{
	int status;
	vsnfs_trace(KERN_DEBUG, "\n");

	if ((status = ntohl(*p++)))
		return vsnfs_stat_to_errno(status);

	*p++ = htonl(res->count);
	req->rq_slen = xdr_adjust_iovec(req->rq_svec, p);

	vsnfs_trace(KERN_DEBUG, "%d %d\n", status, (int)res->count);	
	return 0;
}


__be32 *vsnfs_decode_dirent(__be32 * p, struct vsnfs_entry *entry)
{
	vsnfs_trace(KERN_DEFAULT, "\n");
	
	if (!*p++) {
		/*if (!*p)
			return ERR_PTR(-EAGAIN);*/
		entry->eof = 1;		
		return ERR_PTR(-EBADCOOKIE); /* shouldn't it be __be32 type? -- check and fix it */
		
	}

	entry->ino = ntohl(*p++);
	entry->len = ntohl(*p++);
	entry->name = (const char *)p;
	p += XDR_QUADLEN(entry->len);
//	entry->prev_cookie = entry->cookie;

	p = xdr_decode_fhandle(p,entry->fh); /* the caller should be the owner of fh */
	entry->offset = ntohl(*p++);
	entry->eof = !p[0] && p[1];

	vsnfs_trace(KERN_DEFAULT, "ino:%d name:%c eof:%d\n",(int)entry->ino, entry->name[0] , entry->eof);

	return p;
}

static struct {
	int stat;
	int errno;
} vsnfs_errtbl[] = {
	{
	VSNFS_OK, 0}, {
	VSNFSERR_NOENT, -ENOENT}, {
	VSNFSERR_IO, -errno_VSNFSERR_IO}, {
	VSNFSERR_NXIO, -ENXIO}, {
	VSNFSERR_EAGAIN, -EAGAIN}, {
	VSNFSERR_EXIST, -EEXIST}, {
	VSNFSERR_NODEV, -ENODEV}, {
	VSNFSERR_NOTDIR, -ENOTDIR}, {
	VSNFSERR_ISDIR, -EISDIR}, {
	VSNFSERR_INVAL, -EINVAL}, {
	VSNFSERR_FBIG, -EFBIG}, {
	VSNFSERR_NOSPC, -ENOSPC}, {
	VSNFSERR_ROFS, -EROFS}, {
	VSNFSERR_MLINK, -EMLINK}, {
	VSNFSERR_OPNOTSUPP - EOPNOTSUPP}, {
	VSNFSERR_NAMETOOLONG, -ENAMETOOLONG}, {
	VSNFSERR_NOTEMPTY, -ENOTEMPTY}, {
	VSNFSERR_DQUOT, -EDQUOT}, {
	VSNFSERR_STALE, -ESTALE}, {
	VSNFSERR_REMOTE, -EREMOTE}, {
	VSNFSERR_BADHANDLE, -EBADHANDLE}, {
	VSNFSERR_BAD_COOKIE - EBADCOOKIE}, {
	-1, -EIO}
};

/*
 * Convert an VSNFS error code to a local one.
 */
int vsnfs_stat_to_errno(int stat)
{
	int i;

	for (i = 0; vsnfs_errtbl[i].stat != -1; i++) {
		if (vsnfs_errtbl[i].stat == stat)
			return vsnfs_errtbl[i].errno;
	}
	printk("vsnfs_stat_to_errno: bad vsnfs status return value: %d\n",
	       stat);
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
	PROC(NULL, nullargs, nullres),
	PROC(GETROOT, getrootargs, fh),
	PROC(LOOKUP, lookupargs, fh),
    PROC(READ,          readargs,       readres),
//      PROC(WRITE,                     writeargs,              writeres),
//      PROC(CREATE,        createargs,     diropres),
//      PROC(REMOVE,        diropargs,      stat),
//      PROC(MKDIR,         createargs,     diropres),
//      PROC(RMDIR,         diropargs,      stat),
	PROC(READDIR, readdirargs, readdirres),
};

struct rpc_version vsnfs_version1 = {
	.number = 1,
	.nrprocs = ARRAY_SIZE(vsnfs_procedures),
	.procs = vsnfs_procedures
};

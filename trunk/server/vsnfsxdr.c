#include <linux/types.h>
#include <linux/sunrpc/xdr.h>
#include <linux/sunrpc/svc.h>
#include "../vsnfs.h"
#include "vsnfsd.h"
#include "xdr.h"
 
/*
 * XDR decode functions
 * 0 on failure 
 * 1 on success
 */ 
int 
vsnfssvc_decode_nullargs(struct svc_rqst *rqstp, __be32 * p,
			 struct vsnfsd_nullargs *args) 
{
	args->dummy = ntohl(*p++);
	return xdr_argsize_check(rqstp, p);
}
static __be32 * decode_fh(__be32 * p, struct vsnfs_fh *fhp) 
{
	memset(fhp, 0, sizeof(*fhp));
	memcpy(&fhp->data, p, VSNFS_FHSIZE);
	p = p + (VSNFS_FHSIZE >> 2);
	fhp->type = ntohl(*p++);
	return p;
}
static __be32 *
decode_filename(__be32 * p, char **namp, unsigned int *lenp) 
{
	char *name;
	unsigned int i;
	if ((p =
	       xdr_decode_string_inplace(p, namp, lenp,
					 VSNFS_MAXNAMLEN)) != NULL) {
		for (i = 0, name = *namp; i < *lenp; i++, name++) {
			if (*name == '\0' || *name == '/')
				return NULL;
		}
	}
	return p;
}
static __be32 *
decode_pathname(__be32 * p, char **namp, unsigned int *lenp) 
{
	char *name;
	unsigned int i;
	if ((p =
	       xdr_decode_string_inplace(p, namp, lenp,
					 VSNFS_MAXPATHLEN)) != NULL) {
		for (i = 0, name = *namp; i < *lenp; i++, name++) {
			if (*name == '\0')
				return NULL;
		}
	}
	return p;
}
int 
vsnfssvc_decode_getrootargs(struct svc_rqst *rqstp, __be32 * p,
			    struct vsnfsd_getrootargs *args) 
{
	if (!(p = decode_pathname(p, &args->path, &args->len)))
		return 0;
	return xdr_argsize_check(rqstp, p);
}
int 
vsnfssvc_decode_lookupargs(struct svc_rqst *rqstp, __be32 * p,
			   struct vsnfsd_lookupargs *args) 
{
	if (!(p = decode_fh(p, &args->fh)) 
	     ||!(p = decode_filename(p, &args->filename, &args->len)))
		return 0;
	return xdr_argsize_check(rqstp, p);
}


/*
* 0 on failure 
* 1 on success
*/ 
int 
vsnfssvc_decode_readdirargs(struct svc_rqst *rqstp, __be32 * p,
			    struct vsnfsd_readdirargs *args) 
{
	vsnfs_trace(KERN_DEFAULT, "\n");
	if (!(p = decode_fh(p, &args->fh)))	/*check this part */
		return 0;
	args->count = ntohl(*p++);
	if (args->count > PAGE_SIZE)
		args->count = PAGE_SIZE;
	args->buffer = page_address(rqstp->rq_respages[rqstp->rq_resused++]);
	return xdr_argsize_check(rqstp, p);
}


/*
 * XDR encode functions
 * 0 on error
 * 1 on success
 */ 
int 
vsnfssvc_encode_nullres(struct svc_rqst *rqstp, __be32 * p,
			struct vsnfsd_nullres *resp) 
{
	*p++ = htonl(resp->dummy);
	return xdr_ressize_check(rqstp, p);
}
static __be32 * encode_fh(__be32 * p, struct vsnfs_fh *fhp) 
{
	memcpy(p, fhp->data, VSNFS_FHSIZE);
	return p + (VSNFS_FHSIZE >> 2);
}
int 
vsnfssvc_encode_fhandle(struct svc_rqst *rqstp, __be32 * p,
			struct vsnfs_fh *resp) 
{
	p = encode_fh(p, resp);
	*p++ = htonl(resp->type);
	return xdr_ressize_check(rqstp, p);
}
int 
vsnfssvc_encode_readdirres(struct svc_rqst *rqstp, __be32 * p,
			   struct vsnfsd_readdirres *resp) 
{
	vsnfs_trace(KERN_DEFAULT, "\n");
	return 0;
	
	    /*continue from here */ 
	    
#if 0
	    vsnfs_trace(KERN_DEFAULT, "\n");
	
	    /*nothing to return */ 
	    xdr_ressize_check(rqstp, p);
	p = resp->buffer;
	
	    /* all dir entries are filled on-the-fly.
	       just mark no more entries and EOF flag */ 
	    *p++ = 0;
	*p++ = htonl((resp->common.err == nfserr_eof));
	rqstp->rq_res.page_len = (((unsigned long)p - 1) & ~PAGE_MASK) + 1;
	return 1;
	
#endif	/*  */
}


/*this is the filler function for readdir - called recursively by vfs_readdir
* stores the nfs error in 'err' field of the buf and returns err_no to vfs*/ 
int 
vsnfssvc_encode_entry(void *buf, const char *name, int namlen, loff_t offset,
		      u64 ino, unsigned int d_type) 
{
	struct vsnfsd_readdirres *res = (struct vsnfsd_readdirres *)buf;
	__be32 * p = res->buffer;
	int buflen, slen;
	vsnfs_trace(KERN_DEFAULT, "\n");
	vsnfs_trace(KERN_DEFAULT, "%c %c %c\n", name[0], name[1], name[2]);
	if (offset > ~((u32) 0)) {
		res->err = vsnfserr_fbig;
		return -EINVAL;
	}
	if (res->offset)
		*(res->offset) = htonl(offset);
	if (namlen > VSNFS_MAXNAMLEN)
		namlen = VSNFS_MAXNAMLEN;	/* truncate filename */
	slen = XDR_QUADLEN(namlen);
	
	    /* -4 because 4 xdr words apart from string would be needed */ 
	    if ((buflen = res->buflen - slen - 4) < 0) {
		res->err = vsnfserr_toosmall;
		return -EINVAL;
	}
	if (ino > ~((u32) 0)) {
		res->err = vsnfserr_fbig;
		return -EINVAL;
	}
	*p++ = xdr_one;	/* mark entry present */
	*p++ = htonl((u32) ino);	/* file id */
	p = xdr_encode_array(p, name, namlen);	/* name length & name */
	
	    /* remember pointer so that the field can be filled in the next pass */ 
	    res->offset = p;
	*p++ = htonl(~0U);	/* offset of next entry - invalid as of now */
	
	    /* update buffer content */ 
	    res->buflen = buflen;
	res->buffer = p;
	res->err = vsnfs_ok;
	return 0;
}



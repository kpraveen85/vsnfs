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
* 0 on failure 
* 1 on success
*/ 
int vsnfssvc_decode_readargs(struct svc_rqst *rqstp, __be32 *p,
					struct vsnfsd_readargs *args)
{
	unsigned int len;
	
	vsnfs_trace(KERN_DEFAULT, "\n");
	if (!(p = decode_fh(p, &args->fh)))
		return 0;

	args->offset	= ntohl(*p++);
	len = ntohl(*p++);
	//p++; /* totalcount - unused */

	if (len > PAGE_SIZE)
		len = PAGE_SIZE;

	args->count = len;

	/* set up somewhere to store response.
	 * We take pages, put them on reslist and include in iovec
	 */
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
	
	/* nothing to return apart from pages */ 
    xdr_ressize_check(rqstp, p);
	p = resp->buffer;
	
    /* all dir entries are filled on-the-fly.
       just mark no more entries and EOF flag */ 
    *p++ = 0;
	*p++ = htonl((resp->err == vsnfs_ok ));
	rqstp->rq_res.page_len = (((unsigned long)p - 1) & ~PAGE_MASK) + 1;
	return 1;	
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
	struct vsnfs_fh fh;
	
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
	
    /* refer below code for -4 */ 
    if ((buflen = res->buflen - slen - (5 + XDR_QUADLEN(VSNFS_FHSIZE))) < 0) {
		res->err = vsnfserr_toosmall;
		return -EINVAL;
	}
	if (ino > ~((u32) 0)) {
		res->err = vsnfserr_fbig;
		return -EINVAL;
	}

	/* -------------imp for client-----------------*/
	*p++ = xdr_one;	/* mark entry present --> -1 for this*/
	*p++ = htonl((u32) ino);	/* file id --> -1 for this*/
	p = xdr_encode_array(p, name, namlen);	/* name length & name --> -1 for name length*/
	
	/*adding file handle as well - nasty op!*/	
	snprintf(fh.data, VSNFS_FHSIZE, "%ld", (long int)ino);
	fh.type = (int)d_type;
	/* TO DO - check the return value */
	p = encode_fh(p, &fh); /* -(XDR_QUADLEN(VSNFS_FHSIZE)) for this */
	*p++ = htonl(fh.type); /* -1 for this */

    /* remember pointer so that the field can be filled in the next pass */ 
    res->offset = p;
	*p++ = htonl(~0U);	/* offset of next entry - invalid as of now ---> -1 for this*/
	/* -------------imp for client-----------------*/

    /* update buffer content */ 
    res->buflen = buflen;
	res->buffer = p;
	//res->err = vsnfs_ok;
	return 0;
}

/*
* 0 on failure 
* 1 on success
*/ 
int vsnfssvc_encode_readres(struct svc_rqst *rqstp, __be32 *p,
					struct vsnfsd_readres *res)
{
	vsnfs_trace(KERN_DEBUG, "\n");
	*p++ = htonl(res->count);
	rqstp->rq_res.page_len = (((unsigned long)p - 1) & ~PAGE_MASK) + 1;

	return xdr_ressize_check(rqstp, p);
}



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
vsnfssvc_decode_nullargs(struct svc_rqst *rqstp, __be32 *p, struct vsnfsd_nullargs *args)
{
	args->dummy = ntohl(*p++);
	return xdr_argsize_check(rqstp, p);
}

static __be32 *
decode_fh(__be32 *p, struct vsnfs_fh *fhp)
{
        memset(fhp, 0, sizeof(*fhp));
        memcpy(&fhp->data, p, VSNFS_FHSIZE);
        p = p + (VSNFS_FHSIZE >> 2);
        fhp->type = ntohl(*p++);

        return p;
}

static __be32 *
decode_filename(__be32 *p, char **namp, unsigned int *lenp)
{
        char            *name;
        unsigned int    i;

        if ((p = xdr_decode_string_inplace(p, namp, lenp, VSNFS_MAXNAMLEN)) != NULL) {
                for (i = 0, name = *namp; i < *lenp; i++, name++) {
                        if (*name == '\0' || *name == '/')
                               return NULL;
    }
  }

        return p;
}


static __be32 *
decode_pathname(__be32 *p, char **namp, unsigned int *lenp)
{
        char            *name;
        unsigned int    i;

        if ((p = xdr_decode_string_inplace(p, namp, lenp, VSNFS_MAXPATHLEN)) != NULL) {
                for (i = 0, name = *namp; i < *lenp; i++, name++) {
                        if (*name == '\0')
                                return NULL;
 }
 }
        return p;
}

int
vsnfssvc_decode_getrootargs(struct svc_rqst *rqstp, __be32 *p, struct vsnfsd_getrootargs *args)
{
  if(!(p=decode_pathname(p, &args->path, &args->len)))
    return 0;

	return xdr_argsize_check(rqstp, p);
}

int
vsnfssvc_decode_lookupargs(struct svc_rqst *rqstp, __be32 *p, struct vsnfsd_lookupargs *args)
{
  if(!(p=decode_fh(p,&args->fh))
   || !(p=decode_filename(p, &args->filename, &args->len)))
    return 0;

	return xdr_argsize_check(rqstp, p);
}


/*
 * XDR encode functions
 * 0 on error
 * 1 on success
 */
int
vsnfssvc_encode_nullres(struct svc_rqst *rqstp, __be32 *p, struct vsnfsd_nullres *resp)
{
	*p++ = htonl(resp->dummy);	
	return xdr_ressize_check(rqstp, p);
}

static __be32 *
encode_fh(__be32 *p, struct vsnfs_fh *fhp)
{
        memcpy(p, fhp->data, VSNFS_FHSIZE);
        return p + (VSNFS_FHSIZE>> 2);
}



int
vsnfssvc_encode_fhandle(struct svc_rqst *rqstp, __be32 *p,
                                        struct vsnfs_fh *resp)
{
        p = encode_fh(p, resp);
        *p++ = htonl(resp->type);	
	return xdr_ressize_check(rqstp, p);
}

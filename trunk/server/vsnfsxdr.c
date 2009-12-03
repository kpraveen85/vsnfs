#include <linux/types.h>
#include <linux/sunrpc/xdr.h>
#include <linux/sunrpc/svc.h>
#include "../vsnfs.h"
#include "vsnfsd.h"
#include "xdr.h"


/*
 * XDR encode/decode helper functions
 */


static struct svc_fh *
fh_init(struct svc_fh *fhp, int maxsize)
{
  memset(fhp,0,sizeof(struct svc_fh));
  return fhp;
}

static __be32 *
decode_fh(__be32 *p,struct svc_fh *fhp)
{
  fh_init(fhp,VSNFS_FHSIZE);
  memcpy(&fhp->fh_handle.data,p,VSNFS_FHSIZE);
  fhp->fh_handle.size=VSNFS_FHSIZE;

  return p + (VSNFS_FHSIZE >> 2);
}




/*
 * XDR decode functions
 */
int
vsnfssvc_decode_void(struct svc_rqst *rqstp, __be32 *p, void *dummy)
{
	return xdr_argsize_check(rqstp, p);
}

/*
int 
vsnfssvc_decode_readargs(struct svc_rqst *rqstp, __be32 *p, struct vsnfsd_readargs *args)
{
  unsigned int len;
  
}
*/
int 
vsnfssvc_decode_readdirargs(struct svc_rqst *rqstp, __be32 *p, struct vsnfsd_readdirargs *args)
{
  if(!(p=decode_fh(p,&args->fh)))
    return 0;
  args->cookie = ntohl(*p++);
  args->count = ntohl(*p++);
  if(args->count > PAGE_SIZE)
    args->count = PAGE_SIZE;
  args->buf = page_address(rqstp->rq_respages[rqstp->rq_resused++]);
  return xdr_argsize_check(rqstp,p);
}





/*
 * XDR encode functions
 */
int
vsnfssvc_encode_void(struct svc_rqst *rqstp, __be32 *p, void *dummy)
{
	return xdr_ressize_check(rqstp, p);
}

/*
int 
vsnfssvc_encode_readres(struct svc_rqst *rqstp, __be32 *p, struct vsnfsd_readres *resp)
{

}
*/

int 
vsnfssvc_encode_readdirres(struct svc_rqst *rqstp, __be32 *p, struct vsnfsd_readdirres *resp)
{
  xdr_ressize_check(rqstp,p);
  p=resp->buf;
  *p++=0;
  *p++=htonl((resp->err==vsnfserr_eof));
  rqstp->rq_res.page_len=(((unsigned long)p-1) & ~PAGE_MASK)+1;
  return 1;
}



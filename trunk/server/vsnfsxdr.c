#include <linux/types.h>
#include <linux/sunrpc/xdr.h>
#include <linux/sunrpc/svc.h>

/*
 * XDR decode functions
 */
int
vsnfssvc_decode_void(struct svc_rqst *rqstp, __be32 *p, void *dummy)
{
	return xdr_argsize_check(rqstp, p);
}

/*
 * XDR encode functions
 */
int
vsnfssvc_encode_void(struct svc_rqst *rqstp, __be32 *p, void *dummy)
{
	return xdr_ressize_check(rqstp, p);
}


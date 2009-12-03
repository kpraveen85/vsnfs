#include <linux/types.h>
#include <linux/sunrpc/svc.h>
#include "../vsnfs.h"
#include "vsnfsd.h"
#include "xdr.h"

static __be32
vsnfsd_proc_null(struct svc_rqst *rqstp, void *argp, void *resp)
{
	vsnfs_trace(KERN_DEFAULT, ":-)\n");
	return vsnfs_ok;
}

/*
static __be32
vsnfsd_proc_read(struct svc_rqst *rqstp, struct vsnfsd_readargs *argp, struct vsnfsd_readres *resp)
{

}

static __be32
vsnfsd_proc_readdir(struct svc_rqst *rqstp, struct vsnfsd_readdirargs *argp, struct vsnfsd_readdirres  *resp)
{
  int count;
  __be32 vsnfserr;
  loff_t offset;

  count=argp->count;
  
  count -= 2;
  if(count<0)
    count =0;
  resp->buf = argp->buf;
  resp->offset=NULL;
  resp->buflen=count;
  resp->err=vsnfs_ok;

  offset=argp->cookie;
  vsnfserr = vsnfsd_readdir(rqstp,&argp->fh, offset, &resp->err,
			    vsnfssvc_encode_entry);
  


}

*/

/*
 * VSNFS Server procedures.
 * No caching of results for any function
 */

#define ST 1		/* status */
#define FH 8		/* filehandle */
#define	AT 18		/* attributes */

#define RC_NOCACHE 0

static struct svc_procedure		vsnfsd_procedures1[VSNFS_NRPROCS] = {
	[VSNFSPROC_NULL] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_null,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_void,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_void,
		.pc_argsize = sizeof(struct vsnfsd_void),
		.pc_ressize = sizeof(struct vsnfsd_void),
		.pc_cachetype = RC_NOCACHE,
		.pc_xdrressize = ST,
	},
        /* filled up the array with dummy values for timebeing */
	/*	[VSNFSPROC_CREATE] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_null,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_void,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_void,
		.pc_argsize = sizeof(struct vsnfsd_void),
		.pc_ressize = sizeof(struct vsnfsd_void),
		.pc_cachetype = RC_NOCACHE,
		.pc_xdrressize = ST,
	},
	 [VSNFSPROC_REMOVE] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_null,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_void,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_void,
		.pc_argsize = sizeof(struct vsnfsd_void),
		.pc_ressize = sizeof(struct vsnfsd_void),
		.pc_cachetype = RC_NOCACHE,
		.pc_xdrressize = ST,
		}, 
		[VSNFSPROC_READ] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_read,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_readargs,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_readres,
               	.pc_argsize = sizeof(struct vsnfsd_readargs),
		.pc_ressize = sizeof(struct vsnfsd_readres),
		.pc_cachetype = RC_NOCACHE,
		.pc_xdrressize = ST+AT+1+RPCSVC_MAXPAYLOAD,
		},
	[VSNFSPROC_WRITE] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_null,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_void,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_void,
		.pc_argsize = sizeof(struct vsnfsd_void),
		.pc_ressize = sizeof(struct vsnfsd_void),
		.pc_cachetype = RC_NOCACHE,
		.pc_xdrressize = ST,
	},
	[VSNFSPROC_MKDIR] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_null,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_void,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_void,
		.pc_argsize = sizeof(struct vsnfsd_void),
		.pc_ressize = sizeof(struct vsnfsd_void),
		.pc_cachetype = RC_NOCACHE,
		.pc_xdrressize = ST,
	},
	[VSNFSPROC_RMDIR] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_null,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_void,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_void,
		.pc_argsize = sizeof(struct vsnfsd_void),
		.pc_ressize = sizeof(struct vsnfsd_void),
		.pc_cachetype = RC_NOCACHE,
		.pc_xdrressize = ST,
	},
	      	[VSNFSPROC_READDIR] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_readdir,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_readdirargs,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_readdirres,
		.pc_argsize = sizeof(struct vsnfsd_readdirargs),
		.pc_ressize = sizeof(struct vsnfsd_readdirres),
		.pc_cachetype = RC_NOCACHE,
		},
	[VSNFSPROC_LOOKUP] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_null,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_void,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_void,
		.pc_argsize = sizeof(struct vsnfsd_void),
		.pc_ressize = sizeof(struct vsnfsd_void),
		.pc_cachetype = RC_NOCACHE,
		.pc_xdrressize = ST,
	},
	*/
        /*add procs here */
};



struct svc_version	vsnfsd_version1 = {
		.vs_vers	= 1,
		.vs_nproc	= VSNFS_NRPROCS,
		.vs_proc	= vsnfsd_procedures1,
		.vs_dispatch	= vsnfsd_dispatch,
		.vs_xdrsize	= VSNFS_SVC_XDRSIZE,
};


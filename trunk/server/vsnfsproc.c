#include <linux/types.h>
#include <linux/sunrpc/svc.h>
#include "../vsnfs.h"
#include "vsnfsd.h"
#include "xdr.h"

static __be32
vsnfsd_proc_null(struct svc_rqst *rqstp, void *argp, void *resp)
{
	vsnfs_trace(KERN_DEFAULT, ":-)\n");
	return nfs_ok;
}


/*
 * VSNFS Server procedures.
 * No caching of results for any function
 */

#define ST 1		/* status */
#define FH 8		/* filehandle */
#define	AT 18		/* attributes */

#define RC_NOCACHE 0

static struct svc_procedure		vsnfsd_procedures1[] = {
	[VSNFSPROC_NULL] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_null,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_void,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_void,
		.pc_argsize = sizeof(struct vsnfsd_void),
		.pc_ressize = sizeof(struct vsnfsd_void),
		.pc_cachetype = RC_NOCACHE,
		.pc_xdrressize = ST,
	},
	/*add procs here*/
};



struct svc_version	vsnfsd_version1 = {
		.vs_vers	= 1,
		.vs_nproc	= NO_OF_VSNFSPROCS,
		.vs_proc	= vsnfsd_procedures1,
		.vs_dispatch	= vsnfsd_dispatch,
		.vs_xdrsize	= VSNFS_SVC_XDRSIZE,
};


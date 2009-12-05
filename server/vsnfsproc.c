/*
 * vsnfsproc.c - contains all server side NFS procedures 
 */

#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/types.h>
#include <linux/sunrpc/svc.h>
#include "../vsnfs.h"
#include "vsnfsd.h"
#include "xdr.h"


/* returns nfs error 
* argp - argument sent by client (after decoded by the corr xdr decode function)
* resp - result to be sent (it will be decoded by the corr xdr encode function)
*/

/* this procedure just return x+1 where x is the value received from client */
static __be32
vsnfsd_proc_null(struct svc_rqst *rqstp, struct vsnfsd_nullargs *argp, 
						struct vsnfsd_nullres *resp)
{
    vsnfs_trace(KERN_DEFAULT, ":-)\n");
	resp->dummy = argp->dummy + 1;
	return VSNFS_OK;
}

static __be32
vsnfsd_proc_getroot(struct svc_rqst *rqstp, struct vsnfsd_getrootargs *argp, struct vsnfsd_fhandle *resp)
{
        struct nameidata *nd;
        struct dentry *root;
        int ret=0;
        vsnfs_trace(KERN_DEFAULT, "in getroot\n");
	nd = kmalloc(sizeof(*nd), GFP_KERNEL);
        if (unlikely(!nd))
                return -ENOMEM;
      	memset(nd,0,sizeof(*nd));
	ret = path_lookup(argp->path, 0, nd);
	if(ret<0)
          vsnfs_trace(KERN_DEFAULT, "failed in path_lookup\n");
        root=dget(nd->path.dentry);
        path_put(&nd->path);
        vsnfs_trace(KERN_DEFAULT, "inode no = %ld\n",root->d_inode->i_ino);
        dput(root);
        kfree(nd);
	return VSNFS_OK;

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
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_nullargs,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_nullres,
		.pc_argsize = sizeof(struct vsnfsd_nullargs),
		.pc_ressize = sizeof(struct vsnfsd_nullres),
		.pc_cachetype = RC_NOCACHE,
		.pc_xdrressize = ST,
	},
    [VSNFSPROC_GETROOT] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_getroot,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_getrootargs,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_fhandle,
		.pc_argsize = sizeof(struct vsnfsd_getrootargs),
		.pc_ressize = sizeof(struct vsnfsd_fhandle),
		.pc_cachetype = RC_NOCACHE,
		.pc_xdrressize = ST+FH,
	},
        
	/*add procs here*/
};



struct svc_version	vsnfsd_version1 = {
		.vs_vers	= 1,
		.vs_nproc	= VSNFS_NRPROCS,
		.vs_proc	= vsnfsd_procedures1,
		.vs_dispatch	= vsnfsd_dispatch,
		.vs_xdrsize	= VSNFS_SVC_XDRSIZE,
};


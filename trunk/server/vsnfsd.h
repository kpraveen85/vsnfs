#ifndef _VSNFSD_H
#define _VSNFSD_H

#define VSNFS_NRSERVS 1 /* no of threads */
/* Block size of VFS V2 specification. In V3 and V4 it's configured based on the system memory */
#define VSNFSSVC_MAXBLKSIZE (8*1024)

/*
 * These macros provide pre-xdr'ed values for faster operation.
 */
#define	nfs_ok			cpu_to_be32(NFS_OK)


extern struct svc_version	vsnfsd_version1;
extern struct svc_program		vsnfsd_program;


int		vsnfsd_dispatch(struct svc_rqst *rqstp, __be32 *statp);
#endif

#ifndef _VSNFSD_H
#define _VSNFSD_H

#define VSNFS_NRSERVS 1 /* no of threads */
/* Block size of VFS V2 specification. In V3 and V4 it's configured based on the system memory */ 
#define VSNFSSVC_MAXBLKSIZE (8*1024)
    
/*
 * These macros provide pre-xdr'ed values for faster operation.
 */ 
#define	vsnfs_ok			cpu_to_be32(VSNFS_OK)
#define	vsnfserr_perm		cpu_to_be32(VSNFSERR_PERM)
#define	vsnfserr_noent		cpu_to_be32(VSNFSERR_NOENT)
#define	vsnfserr_io		cpu_to_be32(VSNFSERR_IO)
#define	vsnfserr_nxio		cpu_to_be32(VSNFSERR_NXIO)
#define	vsnfserr_eagain		cpu_to_be32(VSNFSERR_EAGAIN)
#define	vsnfserr_acces		cpu_to_be32(VSNFSERR_ACCES)
#define	vsnfserr_exist		cpu_to_be32(VSNFSERR_EXIST)
#define	vsnfserr_xdev		cpu_to_be32(VSNFSERR_XDEV)
#define	vsnfserr_nodev		cpu_to_be32(VSNFSERR_NODEV)
#define	vsnfserr_notdir		cpu_to_be32(VSNFSERR_NOTDIR)
#define	vsnfserr_isdir		cpu_to_be32(VSNFSERR_ISDIR)
#define	vsnfserr_inval		cpu_to_be32(VSNFSERR_INVAL)
#define	vsnfserr_fbig		cpu_to_be32(VSNFSERR_FBIG)
#define	vsnfserr_nospc		cpu_to_be32(VSNFSERR_NOSPC)
#define	vsnfserr_rofs		cpu_to_be32(VSNFSERR_ROFS)
#define	vsnfserr_mlink		cpu_to_be32(VSNFSERR_MLINK)
#define	vsnfserr_opnotsupp	cpu_to_be32(VSNFSERR_OPNOTSUPP)
#define	vsnfserr_nametoolong	cpu_to_be32(VSNFSERR_NAMETOOLONG)
#define	vsnfserr_notempty		cpu_to_be32(VSNFSERR_NOTEMPTY)
#define	vsnfserr_dquot		cpu_to_be32(VSNFSERR_DQUOT)
#define	vsnfserr_stale		cpu_to_be32(VSNFSERR_STALE)
#define	vsnfserr_remote		cpu_to_be32(VSNFSERR_REMOTE)
#define	vsnfserr_wflush		cpu_to_be32(VSNFSERR_WFLUSH)
#define	vsnfserr_badhandle	cpu_to_be32(VSNFSERR_BADHANDLE)
#define	vsnfserr_notsync		cpu_to_be32(VSNFSERR_NOT_SYNC)
#define	vsnfserr_badcookie	cpu_to_be32(VSNFSERR_BAD_COOKIE)
#define	vsnfserr_notsupp		cpu_to_be32(VSNFSERR_NOTSUPP)
#define	vsnfserr_toosmall		cpu_to_be32(VSNFSERR_TOOSMALL)
#define	vsnfserr_serverfault	cpu_to_be32(VSNFSERR_SERVERFAULT)
#define	vsnfserr_badtype		cpu_to_be32(VSNFSERR_BADTYPE)
#define	vsnfserr_jukebox		cpu_to_be32(VSNFSERR_JUKEBOX)
#define	vsnfserr_denied		cpu_to_be32(VSNFSERR_DENIED)
#define	vsnfserr_deadlock		cpu_to_be32(VSNFSERR_DEADLOCK)
#define vsnfserr_expired          cpu_to_be32(VSNFSERR_EXPIRED)
#define	vsnfserr_bad_cookie	cpu_to_be32(VSNFSERR_BAD_COOKIE)
#define	vsnfserr_same		cpu_to_be32(VSNFSERR_SAME)
#define	vsnfserr_clid_inuse	cpu_to_be32(VSNFSERR_CLID_INUSE)
#define	vsnfserr_stale_clientid	cpu_to_be32(VSNFSERR_STALE_CLIENTID)
#define	vsnfserr_resource		cpu_to_be32(VSNFSERR_RESOURCE)
#define	vsnfserr_moved		cpu_to_be32(VSNFSERR_MOVED)
#define	vsnfserr_nofilehandle	cpu_to_be32(VSNFSERR_NOFILEHANDLE)
#define	vsnfserr_minor_vers_mismatch	cpu_to_be32(VSNFSERR_MINOR_VERS_MISMATCH)
#define vsnfserr_share_denied	cpu_to_be32(VSNFSERR_SHARE_DENIED)
#define vsnfserr_stale_stateid	cpu_to_be32(VSNFSERR_STALE_STATEID)
#define vsnfserr_old_stateid	cpu_to_be32(VSNFSERR_OLD_STATEID)
#define vsnfserr_bad_stateid	cpu_to_be32(VSNFSERR_BAD_STATEID)
#define vsnfserr_bad_seqid	cpu_to_be32(VSNFSERR_BAD_SEQID)
#define	vsnfserr_symlink		cpu_to_be32(VSNFSERR_SYMLINK)
#define	vsnfserr_not_same		cpu_to_be32(VSNFSERR_NOT_SAME)
#define	vsnfserr_restorefh	cpu_to_be32(VSNFSERR_RESTOREFH)
#define	vsnfserr_attrnotsupp	cpu_to_be32(VSNFSERR_ATTRNOTSUPP)
#define	vsnfserr_bad_xdr		cpu_to_be32(VSNFSERR_BAD_XDR)
#define	vsnfserr_openmode		cpu_to_be32(VSNFSERR_OPENMODE)
#define	vsnfserr_locks_held	cpu_to_be32(VSNFSERR_LOCKS_HELD)
#define	vsnfserr_op_illegal	cpu_to_be32(VSNFSERR_OP_ILLEGAL)
#define	vsnfserr_grace		cpu_to_be32(VSNFSERR_GRACE)
#define	vsnfserr_no_grace		cpu_to_be32(VSNFSERR_NO_GRACE)
#define	vsnfserr_reclaim_bad	cpu_to_be32(VSNFSERR_RECLAIM_BAD)
#define	vsnfserr_badname		cpu_to_be32(VSNFSERR_BADNAME)
#define	vsnfserr_cb_path_down	cpu_to_be32(VSNFSERR_CB_PATH_DOWN)
#define	vsnfserr_locked		cpu_to_be32(VSNFSERR_LOCKED)
#define	vsnfserr_wrongsec		cpu_to_be32(VSNFSERR_WRONGSEC)
    
struct vsnfs_lookup_table {
	
unsigned long ino;
	
 unsigned char *path;
	
 struct list_head list;

};


extern struct svc_version vsnfsd_version1;

extern struct svc_program vsnfsd_program;



/* all nfs functions which interact with vfs go here */ 
    __be32 vsnfsd_readdir(struct vsnfs_fh *fhp, void *buf);


int vsnfsd_dispatch(struct svc_rqst *rqstp, __be32 * statp);


/* returns 0 on success -1 on failure */ 
int vsnfsd_fh_to_path(struct vsnfs_fh *fh, char *path, int len);


__be32 vsnfserrno(int errno);


#endif	/* 
 */

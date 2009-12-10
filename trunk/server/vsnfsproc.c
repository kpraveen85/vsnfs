/*
 * vsnfsproc.c - contains all server side NFS procedures 
 */  
    
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/types.h>
#include <linux/sunrpc/svc.h>
#include <linux/list.h>
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
		 
{
	
	
	


vsnfsd_proc_lookup(struct svc_rqst *rqstp, struct vsnfsd_lookupargs *argp,
		   struct vsnfs_fh *resp) 
{
	
	
	
	
	
	
	
	
	
	
	
		
			
			
				
				
			
			
			
			
			    //  path[strlen(node->path)]='\0';
			    goto skip_loop;
		
		
		    //  vsnfs_trace(KERN_DEFAULT, "list: %s : %s\n",node->data,node->path);  
	}
      
		
		    //    argp->filename[argp->len]='\0';
		    nullt = strchr(path, '\0');
		
		
		
		
			     path, len);
		
		
			
			
		
		
		
		
			
		
		
		
			     dentry->d_inode->i_ino);
		
			  dentry->d_inode->i_ino);
		
			
		
		
		else if (S_ISDIR(dentry->d_inode->i_mode)) {
			
			
			
				
				
			
		
		
		    /* Adding the ino & path to the lookup table */ 
		    vsnfs_trace(KERN_DEFAULT, "GRRR PATH = %s : %d", path,
				strlen(path));
		
		    (struct vsnfs_lookup_table *)
		    kmalloc(sizeof(struct vsnfs_lookup_table), GFP_KERNEL);
		
		
		
			
			
		
		
		
	
      
	
      
	


vsnfsd_proc_getroot(struct svc_rqst *rqstp, struct vsnfsd_getrootargs *argp,
		    struct vsnfs_fh *resp) 
{
	
	
	
	
	
	
	
	
		
	
	
	
	
		
	
	
	
		
		
	
	
	else {
		
	
	
	
		
	
	
	
	
	
		
		
	
	
	else if (S_ISDIR(root->d_inode->i_mode)) {
		
	
	
	
	    /* Adding the ino & path to the lookup table */ 
	    
	
	    (struct vsnfs_lookup_table *)
	    kmalloc(sizeof(struct vsnfs_lookup_table), GFP_KERNEL);
	
	
	
		
		
	
	
	
      
	
	
	



/*
 * VSNFS Server procedures.
 * No caching of results for any function
 */ 
    
#define ST 1		/* status */
#define FH 8		/* filehandle */
#define	AT 18		/* attributes */
    
#define RC_NOCACHE 0

	    [VSNFSPROC_NULL] = {
				(svc_procfunc) vsnfsd_proc_null, 
				(kxdrproc_t) vsnfssvc_decode_nullargs,
				
				(kxdrproc_t) vsnfssvc_encode_nullres,
				
				sizeof(struct vsnfsd_nullargs), 
				sizeof(struct vsnfsd_nullres), 
				RC_NOCACHE, 
				ST + 1, 
								    (svc_procfunc)
								    vsnfsd_proc_getroot,
								    
								    pc_decode =
								    (kxdrproc_t)
								    vsnfssvc_decode_getrootargs,
								    
								    pc_encode =
								    (kxdrproc_t)
								    vsnfssvc_encode_fhandle,
								    
								    pc_argsize =
								    sizeof
								    (struct
								     vsnfsd_getrootargs),
								    
								    pc_ressize =
								    sizeof
								    (struct
								     vsnfs_fh),
								    
								    pc_cachetype
								    =
								    RC_NOCACHE,
								    
								    pc_xdrressize
								    =
								    ST + FH, 
    
			   (svc_procfunc) vsnfsd_proc_lookup, 
			   (kxdrproc_t) vsnfssvc_decode_lookupargs,
			   
			   (kxdrproc_t) vsnfssvc_encode_fhandle, 
			   sizeof(struct vsnfsd_lookupargs), 
			   sizeof(struct vsnfs_fh), 
			   RC_NOCACHE, 
    /*add procs here */ 
};

	    VSNFS_NRPROCS, 
	    vsnfsd_dispatch, 
};


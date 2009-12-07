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

struct vsnfs_lookup_table vsnfs_lp_tab;

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
	return vsnfs_ok;
}

static __be32
vsnfsd_proc_lookup(struct svc_rqst *rqstp, struct vsnfsd_lookupargs *argp, struct vsnfs_fh *resp)
{
        struct vsnfs_lookup_table *node;
        unsigned long ino=0;  
        char *path=NULL;
	char *tmp = NULL;
	int rc=vsnfs_ok;
        struct nameidata *nd=NULL;
        struct dentry *dentry=NULL;
        
	tmp = argp->fh.data;

        ino=simple_strtoul(argp->fh.data, &tmp,0);
        list_for_each_entry(node,&vsnfs_lp_tab.list,list){
          if(node->ino==ino){
	    path=kmalloc(VSNFS_MAXPATHLEN,GFP_KERNEL);
	    if(!path){
	      rc=-ENOMEM;
	      goto out_lookup;
	    }
            strcpy(path,node->path);
            goto skip_loop;
          
	  }
	  //  vsnfs_trace(KERN_DEFAULT, "list: %s : %s\n",node->data,node->path);  
	}

 skip_loop:

	if(path!=NULL){
        strcat(path,argp->filename);
	nd = kmalloc(sizeof(*nd), GFP_KERNEL);
        if (!nd){
                rc= -ENOMEM;
		goto out_lookup;
	}
      	memset(nd,0,sizeof(*nd));
	rc = path_lookup(path, 0, nd);
	if(rc<0)
          vsnfs_trace(KERN_DEFAULT, "failed in path_lookup\n");
        dentry=dget(nd->path.dentry);
        path_put(&nd->path);
        vsnfs_trace(KERN_DEFAULT, "inode no = %ld\n",dentry->d_inode->i_ino);
        snprintf(resp->data,VSNFS_FHSIZE,"%ld",dentry->d_inode->i_ino);
        if(S_ISREG(dentry->d_inode->i_mode))
	  resp->type=VSNFS_REG;
	 
	else if(S_ISDIR(dentry->d_inode->i_mode))
	  resp->type=VSNFS_DIR;        
  
        /* Adding the ino & path to the lookup table */

        node=(struct vsnfs_lookup_table *)kmalloc(sizeof(struct vsnfs_lookup_table), GFP_KERNEL);
        node->ino=dentry->d_inode->i_ino;
        node->path=kstrndup(path,strlen(path),GFP_KERNEL);
        if(!node->path){
	  rc= -ENOMEM;
	  goto out_lookup_dput; 
	}
        list_add(&(node->list),&(vsnfs_lp_tab.list));
       
	}
	 
 out_lookup_dput:
       
        dput(dentry);
        kfree(nd);

 out_lookup:
	kfree(path);
	return rc;
}


static __be32
vsnfsd_proc_getroot(struct svc_rqst *rqstp, struct vsnfsd_getrootargs *argp, struct vsnfs_fh *resp)
{

        struct nameidata *nd;
        struct dentry *root;
        int ret=vsnfs_ok;
        struct vsnfs_lookup_table *node;
  
        vsnfs_trace(KERN_DEFAULT, "in getroot\n");
	nd = kmalloc(sizeof(*nd), GFP_KERNEL);
        if (unlikely(!nd)){
                return -ENOMEM;
	}
      	memset(nd,0,sizeof(*nd));
	ret = path_lookup(argp->path, 0, nd);
	if(ret<0)
          vsnfs_trace(KERN_DEFAULT, "failed in path_lookup\n");
        root=dget(nd->path.dentry);
        path_put(&nd->path);
        vsnfs_trace(KERN_DEFAULT, "inode no = %ld\n",root->d_inode->i_ino);
        snprintf(resp->data,VSNFS_FHSIZE,"%ld",root->d_inode->i_ino);
        if(S_ISREG(root->d_inode->i_mode)){
	  ret=-vsnfserr_notdir;
	  goto out_getroot;
	}
	else if(S_ISDIR(root->d_inode->i_mode))
	  resp->type=VSNFS_DIR;        
  
        /* Adding the ino & path to the lookup table */

        INIT_LIST_HEAD(&vsnfs_lp_tab.list);
        node=(struct vsnfs_lookup_table *)kmalloc(sizeof(struct vsnfs_lookup_table), GFP_KERNEL);
        node->ino=root->d_inode->i_ino;
        node->path=kstrndup(argp->path,strlen(argp->path),GFP_KERNEL);
        if(!node->path){
	  ret= -ENOMEM;
          goto out_getroot;
	} 
        list_add(&(node->list),&(vsnfs_lp_tab.list));
                
 out_getroot:        
        dput(root);
        kfree(nd);
	return ret;


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
		.pc_xdrressize = ST+1,
	},
    [VSNFSPROC_GETROOT] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_getroot,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_getrootargs,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_fhandle,
		.pc_argsize = sizeof(struct vsnfsd_getrootargs),
		.pc_ressize = sizeof(struct vsnfs_fh),
		.pc_cachetype = RC_NOCACHE,
		.pc_xdrressize = ST+FH,
	},
    [VSNFSPROC_LOOKUP] = {
		.pc_func = (svc_procfunc) vsnfsd_proc_lookup,
		.pc_decode = (kxdrproc_t) vsnfssvc_decode_lookupargs,
		.pc_encode = (kxdrproc_t) vsnfssvc_encode_fhandle,
		.pc_argsize = sizeof(struct vsnfsd_lookupargs),
		.pc_ressize = sizeof(struct vsnfs_fh),
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


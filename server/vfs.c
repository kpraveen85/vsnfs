#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/types.h>
#include <linux/sunrpc/svc.h>
#include <linux/list.h>
#include "../vsnfs.h"
#include "vsnfsd.h"
#include "xdr.h"

/*
 * Open an existing file or directory.
 * The access argument indicates the type of open (read/write/lock)
 * N.B. After this call fhp needs an fh_put
 */ 
    __be32 vsnfsd_open(struct vsnfs_fh *fhp, struct file **filp, int mode) 
{
	
	
	
	
	
	
		
		
	
	
	
		
		
		
	
	
	
		
		
	
	
      
		
	



/* closes the file */ 
    __be32 vsnfsd_close(struct file * filp)
{
	
	
	
		
		
		
		
	
	else
		
		
		
		



/*
 * Read entries from a directory.
* offset - offset from which the file is to be read
 */ 
    __be32 
{
	
	
	
	    //loff_t                offset = *offsetp;
	int ret;
	
	
		
	
	    /*ret = vfs_llseek(file, offset, 0);
	       if (ret < 0) {
	       err = vsnfserrno(ret);
	       goto out_close;
	       } */ 
	    
	    //err = nfsd_buffered_readdir(file, func, cdp, offsetp);
	    ret = vfs_readdir(file, vsnfssvc_encode_entry, buf);
	
		
		
	
      
      



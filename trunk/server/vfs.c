#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/types.h>
#include <linux/sunrpc/svc.h>
#include <linux/list.h>
#include <asm/uaccess.h>

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
	char *path = NULL;
	int ret;
	__be32 be_err = vsnfs_ok;

	vsnfs_trace(KERN_DEFAULT, "\n");
	path = kmalloc(VSNFS_MAXPATHLEN, GFP_KERNEL);
	if (!path) {
		be_err = vsnfserr_nospc;	/* equivalent of -ENOMEM */
		goto out;
	}
	
	ret = vsnfsd_fh_to_path(fhp, path, VSNFS_MAXPATHLEN);
	if (ret) {
		be_err = vsnfserr_noent;
		vsnfs_trace(KERN_DEFAULT, "no ent\n");
		goto out;
	}
	
	*filp = filp_open(path, mode, 0);
	if (IS_ERR(*filp)) {
		be_err = vsnfserr_io;
		goto out;
	}
	
	vsnfs_trace(KERN_DEFAULT, "success\n");
out:
	if (path)
		kfree(path);

	return be_err;
}


/* closes the file */ 
__be32 vsnfsd_close(struct file * filp)
{
	int ret;

	ret = filp_close(filp, NULL);
	if (ret)
		{
		vsnfs_trace(KERN_DEFAULT, "failure\n");
		return vsnfserr_io;
		}
	else
		{
		vsnfs_trace(KERN_DEFAULT, "success\n");
		return vsnfs_ok;
		}
}


/*
 * Read entries from a directory.
* offset - offset from which the file is to be read
 */ 
__be32  vsnfsd_readdir(struct vsnfs_fh * fhp, void *buf)
{
	__be32 err = vsnfs_ok;
	struct file *file;
		
	int ret;
	err = vsnfsd_open(fhp, &file, O_RDONLY);
	if (err)
		goto out;
	
    /*ret = vfs_llseek(file, offset, 0);
       if (ret < 0) {
       err = vsnfserrno(ret);
       goto out_close;
       } */ 
	    
    //err = nfsd_buffered_readdir(file, func, cdp, offsetp);   
    ret = vfs_readdir(file, vsnfssvc_encode_entry, buf);
	if (ret < 0) {
		err = vsnfserrno(ret);
		goto out_close;
	}
out_close:
	vsnfsd_close(file);
out:
	return err;
}


__be32
vsnfsd_read(struct vsnfs_fh *fhp, char *buffer, size_t count, loff_t *pos)
{
	__be32		err = vsnfs_ok;
	struct file *filp;	
	mm_segment_t	oldfs;
	int host_err;

	vsnfs_trace(KERN_DEFAULT, "\n");
	err = vsnfsd_open(fhp, &filp, O_RDONLY);
	if(err != vsnfs_ok)
		goto out;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	host_err = vfs_read(filp, buffer, count, pos);
	set_fs(oldfs);

	if(host_err >= 0)
		{		
		*pos = (loff_t)host_err;
		}
	else
		{
		err = vsnfserrno(host_err);		
		}

	vsnfs_trace(KERN_DEFAULT, "%d %c %c %c\n",(int)*pos, buffer[0],buffer[1],buffer[2]); 

	vsnfsd_close(filp);	
out:
	return err;
}



/*
 * CSE506 - VSNFS
 *
 * vsnfsProc.c --
 * 
 * OS-independent vsnfs remote procedure call functions
 *
 * (C) Karthik Balaji <findkb@gmail.com>
 * (C) Praveen Krishnamoorthy <kpraveen85@gmail.com>
 * (C) Prabakar Radhakrishnan <prabakarcse@gmail.com>
 */

#include <linux/types.h>
#include <linux/param.h>
#include <linux/mm.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/in.h>
#include <linux/pagemap.h>
#include <linux/lockd/bind.h>
#include <linux/fs.h>
#include "vsnfs.h"
#include "vsnfsXdr.h"
#include "vsnfsClient.h"

/* vsnfs proc definitions */

/* NULL procedure - Just to test the client-server communicaion */
/* Returns 0 on success
  * err_no on failure
  */
static int vsnfs_proc_null(struct vsnfs_server *server, int input, int *output)
{
	int status;
	struct vsnfs_nullargs arg = {
		.dummy = input,
	};
	struct vsnfs_nullres res;

	struct rpc_message msg = {
		.rpc_proc = &vsnfs_procedures[VSNFSPROC_NULL],
		.rpc_argp = &arg,
		.rpc_resp = &res,
	};

	status = rpc_call_sync(server->cl_rpcclient, &msg, 0);

	if (status == 0)
		*output = res.dummy;

	return status;
}

static int
vsnfs_proc_getroot(struct vsnfs_server *server, struct vsnfs_getrootargs *args,
		   struct vsnfs_fh *fh)
{
	int status;

	struct rpc_message msg = {
		.rpc_proc = &vsnfs_procedures[VSNFSPROC_GETROOT],
		.rpc_argp = args,
		.rpc_resp = fh,
	};
	printk("%s: VSNFS call  getroot\n", __func__);
	status = rpc_call_sync(server->cl_rpcclient, &msg, 0);
	printk("%s: VSNFS reply getroot\n", __func__);
	return status;
}

static int
vsnfs_proc_lookup(struct vsnfs_server *server, struct vsnfs_lookupargs *args,
		  struct vsnfs_fh *fh)
{
	int status;

	struct rpc_message msg = {
		.rpc_proc = &vsnfs_procedures[VSNFSPROC_LOOKUP],
		.rpc_argp = args,
		.rpc_resp = fh,
	};
	printk("%s: VSNFS call lookup\n", __func__);
	status = rpc_call_sync(server->cl_rpcclient, &msg, 0);
	printk("%s: VSNFS reply lookup\n", __func__);
	return status;
}

static int
vsnfs_proc_readdir(struct dentry *dentry, struct page *page,
		   unsigned int count)
{
	int status;
	struct inode *dir = dentry->d_inode;
	struct vsnfs_readdirargs arg = {
		.fh = VSNFS_FH(dir),
		.count = count,
		.pages = &page,
	};
	struct rpc_message msg = {
		.rpc_proc = &vsnfs_procedures[VSNFSPROC_READDIR],
		.rpc_argp = &arg,
		.rpc_cred = NULL,
	};

	status = rpc_call_sync(VSNFS_CLIENT(dir), &msg, 0);

	printk("VSNFS reply readdir: %d\n", status);
	return status;
}


static int
vsnfs_proc_read(struct dentry *dentry, struct page *page,
		   loff_t *offset, unsigned int count, int *pos)
{
	int status;
	struct inode *dir = dentry->d_inode;
	struct vsnfs_readargs arg = {
		.fh = VSNFS_FH(dir),
		.offset = (unsigned int)(*offset),
		.length = count,
		.pages = &page,
	};
	struct vsnfs_readres resp;
	
	struct rpc_message msg = {
		.rpc_proc = &vsnfs_procedures[VSNFSPROC_READ],
		.rpc_argp = &arg,
		.rpc_resp = &resp,
		.rpc_cred = NULL,
	};

	status = rpc_call_sync(VSNFS_CLIENT(dir), &msg, 0);
	*pos = resp.count;
	vsnfs_trace(KERN_DEFAULT, "VSNFS reply read: %d\n", (int)resp.count);
	return status;
}



const struct vsnfs_rpc_ops vsnfs_clientops = {
	.version = VSNFS_VERSION,
	.nullproc = vsnfs_proc_null,
	.getroot = vsnfs_proc_getroot,
	.dentry_ops = &vsnfs_dentry_operations,
	.dir_inode_ops = &vsnfs_dir_inode_operations,
	.file_inode_ops = &vsnfs_file_inode_operations,
	.lookup = vsnfs_proc_lookup,
    .read             = vsnfs_proc_read,
	/*  .create                  = vsnfs_proc_create,
	   .remove                   = vsnfs_proc_remove,
	   .mkdir                    = vsnfs_proc_mkdir,
	   .rmdir                    = vsnfs_proc_rmdir,	
	   .write                    = vsnfs_proc_write, */
	.readdir = vsnfs_proc_readdir,
	.decode_dirent = vsnfs_decode_dirent,
};

/*
 * CSE506 - VSNFS
 *
 * vsnfsProc.c --
 * 
 * OS-independent vsnfs remote procedure call functions
 *
 *(C) Karthik Balaji <findkb@gmail.com>
 *
 */

#include <linux/types.h>
#include <linux/param.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/mm.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/in.h>
#include <linux/pagemap.h>
#include <linux/sunrpc/clnt.h>
#include <linux/nfs.h>
#include <linux/nfs2.h>
#include <linux/nfs_fs.h>
#include <linux/nfs_page.h>
#include <linux/lockd/bind.h>
#include "vsnfsClient.h"

const struct vsnfs_rpc_ops vsnfs_clientops = {
	.version		 = 1;
	.dentry_op		 = &vsnfs_dentry_operations,
	.dir_inode_ops	 = &vsnfs_dir_inode_operations,
	.file_inode_ops	 = &vsnfs_file_inode_operations,
	.lookup			 = vsnfs_proc_lookup,
	.create			 = vsnfs_proc_create,
	.remove			 = vsnfs_proc_remove,
	.mkdir			 = vsnfs_proc_mkdir,
	.rmdir			 = vsnfs_proc_rmdir,
	.readdir		 = vsnfs_proc_readdir,
	.read			 = vsnfs_proc_read_setup,
	.write			 = vsnfs_proc_write_setup,
};


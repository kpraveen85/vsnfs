/*
 * CSE506 - VSNFS
 *
 * vsnfsClient.c --
 * 
 * This file primarily contains VSNFS client procedures.
 *
 *(C) Karthik Balaji <findkb@gmail.com>
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/sunrpc/clnt.h>
#include <linux/sunrpc/stats.h>
#include <linux/sunrpc/metrics.h>
#include <linux/sunrpc/xprtsock.h>
#include <linux/sunrpc/xprtrdma.h>
#include <linux/vfs.h>
#include <linux/inet.h>
#include <linux/fs.h>

#include <asm/system.h>

#include "vsnfs.h"
#include "vsnfsClient.h"
#include "vsnfsMount.h"
#include "vsnfsXdr.h"

/* RPC registration details */

struct rpc_version vsnfs_version = &vsnfs_version1;

struct rpc_program vsnfs_program = {
	.name		= "vsnfs"
	.number		= VSNFS_PROGRAM,
	.nrvers		= 1,
	.version	= vsnfs_version,
	.stats		= &vsnfs_rpcstat,
};

struct rpc_stat vsnfs_rpcstat = {
	.program	= &vsnfs_program
};

static int vsnfs_get_sb(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *raw_data, struct vfsmount *mnt)
{
	int ret = 0;
	struct vsnfs_fh *mntfh;
	struct vsnfs_mount_data *data;
	printk("Inside get_sb\n");

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	mntfh = kzalloc(sizeof(*mntfh), GFP_KERNEL);

	/* Validate and copy the mount data */
	ret = vsnfs_parse_mount_options((char *)raw_data, data, mntfh, dev_name);
	return ret;
}

static void vsnfs_kill_sb(struct super_block *sb)
{
	printk("Inside kill_sb\n");
}

static struct file_system_type vsnfs_type = {
	.owner		= THIS_MODULE,
	.name		= "vsnfs",
	.get_sb		= vsnfs_get_sb,
	.kill_sb	= vsnfs_kill_sb
};

int VSNFSClientInit(void) 
{
	return register_filesystem(&vsnfs_type);
}

int VSNFSClientCleanup(void)
{
	return unregister_filesystem(&vsnfs_type);
}

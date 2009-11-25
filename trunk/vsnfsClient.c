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

#include <linux/sched.h>
#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/stat.h>
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

#include "vsnfsClient.h"
#include "vsnfs.h"

static int vsnfs_get_sb(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *raw_data, struct vfsmount *mnt)
{
	printk("Inside get_sb\n");
	return 0;
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

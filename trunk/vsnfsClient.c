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

#include <linux/module.h>
#include <linux/sunrpc/xprtsock.h>
#include <linux/sunrpc/xprtrdma.h>

#include <asm/system.h>

#include "vsnfs.h"
#include "vsnfsXdr.h"
#include "vsnfsClient.h"
#include "vsnfsMount.h"

/* 1. RPC registration details */

static struct rpc_version *vsnfs_version[2] = {
	[1]					= &vsnfs_version1,
};

struct rpc_program vsnfs_program = {
	.name		= "vsnfs",
	.number		= VSNFS_PROGRAM,
	.nrvers		= ARRAY_SIZE(vsnfs_version),
	.version	= vsnfs_version,
	.stats		= &vsnfs_rpcstat,
};

struct rpc_stat vsnfs_rpcstat = {
	.program	= &vsnfs_program
};

/* 2. Filesystem registration and superblock operations */

static int vsnfs_get_sb(struct file_system_type *, int, const char *,
						void *, struct vfsmount *);
static void vsnfs_kill_sb(struct super_block *);
static void vsnfs_umount_begin(struct super_block *sb);

static struct file_system_type vsnfs_type = {
	.owner		= THIS_MODULE,
	.name		= "vsnfs",
	.get_sb		= vsnfs_get_sb,
	.kill_sb	= vsnfs_kill_sb,
};

static const struct super_operations vsnfs_sops = {
	.alloc_inode	= vsnfs_alloc_inode,
	.destroy_inode	= vsnfs_destroy_inode,
	.write_inode	= vsnfs_write_inode,
	.umount_begin	= vsnfs_umount_begin,
};

int VSNFSClientInit(void)
{
    return register_filesystem(&vsnfs_type);
}

int VSNFSClientCleanup(void)
{
    return unregister_filesystem(&vsnfs_type);
}

static int vsnfs_get_sb(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *raw_data, struct vfsmount *mnt)
{
	int ret = 0;
//	struct super_block *s = NULL;
	struct vsnfs_server *server = NULL;
	printk("Inside get_sb\n");

	server = kzalloc(sizeof(struct vsnfs_server), GFP_KERNEL);
	if (IS_ERR(server)) {
		ret = PTR_ERR(server);
		goto out;
	}

	/* Validate and copy the mount data */
	ret = vsnfs_parse_mount_options((char *)raw_data, dev_name, server);

out:
	return ret;
}

static void vsnfs_kill_sb(struct super_block *sb)
{
	printk("Inside kill_sb\n");
}

static void vsnfs_umount_begin(struct super_block *sb)
{
	printk("Inside Umount\n");
}

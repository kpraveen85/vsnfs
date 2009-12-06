/*
 * CSE506 - VSNFS
 *
 * vsnfsClient.c --
 * 
 * This file primarily contains VSNFS client procedures.
 *
 *(C) Karthik Balaji <findkb@gmail.com>
 *(C) Praveen Krishnamoorthy <kpraveen85@gmail.com>
 *(C) Prabakar Radhakrishnan <prabakarcse@gmail.com>
 */

#include <linux/module.h>
#include <linux/sunrpc/xprtsock.h>
#include <linux/sunrpc/xprtrdma.h>
#include <linux/sunrpc/msg_prot.h>

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

static int vsnfs_create_rpcclient(struct vsnfs_server *server)
{
    struct rpc_clnt *clnt = NULL;
    struct rpc_create_args args =
		{
		    .protocol = IPPROTO_TCP,
    		.address = (struct sockaddr *)&server->cl_addr,
    		.addrsize = server->cl_addrlen,
			//.timeout  = &server->timeout,
    		.servername = server->ip_addr,
    		.program = &vsnfs_program,
    		.version = server->cl_rpc_ops->version,
    		.authflavor = RPC_AUTH_UNIX,
    		.flags = RPC_CLNT_CREATE_NOPING, /* check the flags options */
		};
    printk(KERN_ERR "inside vsnfs_Create_rpcClient\n");



    printk(KERN_ERR "calling rpc_create\n");
	
    clnt = rpc_create(&args);

    if(IS_ERR(clnt)){
        printk(KERN_ERR "%s: cannot create RPC client = %ld\n",__func__, PTR_ERR(clnt));
        return PTR_ERR(clnt);
    }
	printk(KERN_ERR "RPC client created\n");
	server->cl_rpcclient = clnt;	

/*to be removed*/
#if 0
{
	int input, output, ret;
	input = 45;
	ret = server->cl_rpc_ops->nullproc(server, input, &output);
	if(ret == 0)
		{
		vsnfs_trace(KERN_DEFAULT, "success :-) %d\n", output);
		}
	else
		{		
		vsnfs_trace(KERN_DEFAULT, "failure :-( %d\n", ret);
		}
	BUG_ON(1);	
}
#endif


    return 0;
}

/* 2. Filesystem registration and superblock operations */

static int vsnfs_get_sb(struct file_system_type *, int, const char *,
						void *, struct vfsmount *);
static void vsnfs_kill_sb(struct super_block *);

static void vsnfs_umount_begin(struct super_block *sb)
{
	struct vsnfs_server *server = VSNFS_SB(sb);
	struct rpc_clnt	*rpc;

	/* -EIO all pending I/O */
	if ((rpc = server->cl_rpcclient) != NULL)
		rpc_killall_tasks(rpc);
}

struct inode *vsnfs_alloc_inode(struct super_block *sb)
{
	struct vsnfs_inode *nfsi;
	nfsi = (struct vsnfs_inode *)kmalloc(sizeof(struct vsnfs_inode), GFP_KERNEL);
	if (!nfsi)
		return NULL;
//	nfsi->fh = 
	return &nfsi->vfs_inode;
}

void vsnfs_destroy_inode(struct inode *inode)
{
	kfree(VSNFS_I(inode));
}

static struct file_system_type vsnfs_type = {
	.name		= "vsnfs",
	.get_sb		= vsnfs_get_sb,
	.kill_sb	= vsnfs_kill_sb
};

static const struct super_operations vsnfs_sops = {
	.alloc_inode	= vsnfs_alloc_inode,
	.destroy_inode	= vsnfs_destroy_inode,
	.umount_begin	= vsnfs_umount_begin,
};

int VSNFSClientInit(void)
{
  int ret = 0;
    printk(KERN_ERR "inside vsnfs_ClientInit\n");
	ret= register_filesystem(&vsnfs_type);
	printk(KERN_ERR "register_filesys : %d\n", ret);

	return ret;
}

int VSNFSClientCleanup(void)
{
    return unregister_filesystem(&vsnfs_type);
}

static void vsnfs_fill_super(struct super_block *sb)
{
	struct inode *root = iget_locked(sb, 2);
        
        printk(KERN_ERR "inside vsnfs_fill_super\n");
	sb->s_root = d_alloc_root(root);
	sb->s_blocksize_bits = VSNFS_FILE_BSIZE_BITS;
	sb->s_blocksize = VSNFS_FILE_IO_SIZE;
	sb->s_magic = VSNFS_SB_MAGIC;

	snprintf(sb->s_id, sizeof(sb->s_id),
		 "%x:%x", MAJOR(sb->s_dev), MINOR(sb->s_dev));
	sb->s_maxbytes = MAX_LFS_FILESIZE;

	sb->s_op = &vsnfs_sops;
}

static int vsnfs_set_super(struct super_block *s, void *data)
{
	struct vsnfs_server *server = data;
	s->s_fs_info = server;
	return set_anon_super(s, server);
}

static int vsnfs_init_server(struct vsnfs_server *server, const char *dev_name, void *raw_data)
{
	int ret = 0;
	
    server->cl_rpc_ops = &vsnfs_clientops;
	ret = vsnfs_parse_mount_options((char *)raw_data, dev_name, server);
	if (ret < 0) {
		vsnfs_trace(KERN_DEFAULT, "Error parsing mount options\n");
		goto out;
		}
	
	out:
		return ret;		
}
static int vsnfs_get_sb(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *raw_data, struct vfsmount *mnt)
{
	int ret = 0;
	struct super_block *s = NULL;
	struct vsnfs_server *server = NULL;
//	struct vsnfs_fh *mntfh;
//	struct dentry *mntroot;
        printk(KERN_ERR "inside vsnfs_getsb\n");

	
	server = kzalloc(sizeof(struct vsnfs_server), GFP_KERNEL);
	if (IS_ERR(server)) {
		ret = PTR_ERR(server);
		goto out;
	}

	/* Validate and copy the mount data */
	ret = vsnfs_init_server(server, dev_name, raw_data);
	if (ret < 0)
		goto out;

    ret = vsnfs_create_rpcclient(server);
    if (ret < 0) {
      printk(KERN_DEFAULT "failed in vsnfs_create_rpcclient\n");
        goto out;
    }
	s = sget(fs_type, NULL, vsnfs_set_super, server);
	if (IS_ERR(s)) {
		ret = PTR_ERR(s);
		goto out;
	}

	vsnfs_fill_super(s);

/*	mntfh = &server->root_fh;
	mntroot = vsnfs_get_root(s, mntfh);
	if (IS_ERR(mntroot)) {
		error = PTR_ERR(mntroot);
		goto err_no_root;*/

out:
	kfree(server);
//	kfree(mntroot);
	return ret;
/*err_no_root:
	dput(mntroot);
	goto out;*/
}

static void vsnfs_kill_sb(struct super_block *s)
{
	struct vsnfs_server *server = VSNFS_SB(s);
	printk("Inside kill_sb\n");

	kill_anon_super(s);
	kfree(server);
}

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
#include <linux/stat.h>
#include <linux/namei.h>
#include <linux/mount.h>
#include <linux/dcache.h>
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
    struct vsnfs_fh *resp;
    struct vsnfs_getrootargs argp =
      {
	.path = server->mnt_path,
        .len  = strlen(server->mnt_path),
      };
    int ret=VSNFS_OK;
    struct rpc_create_args args =
		{
		    .protocol = IPPROTO_TCP,
    		.address = (struct sockaddr *)&server->cl_addr,
    		.addrsize = server->cl_addrlen,
			//.timeout  = &server->timeout,
    		.servername = server->ip_addr,
    		.program = &vsnfs_program,
    		.version = server->cl_rpc_ops->version,
    		.authflavor = RPC_AUTH_NULL,
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
    
        resp=kmalloc(sizeof(struct vsnfs_fh), GFP_KERNEL);
	if(!resp){
	  ret=-ENOMEM;
	}

        ret = server->cl_rpc_ops->getroot(server, &argp, resp);
	if(ret == 0)
		{
		  vsnfs_trace(KERN_DEFAULT, "success :-) inode : %s  type: %d\n", resp->data, resp->type );
		}
	else
		{		
		vsnfs_trace(KERN_DEFAULT, "failure :-( %d\n", ret);
                return -VSNFSERR_REMOTE;
 		}
        memcpy(&server->root_fh,resp,sizeof(struct vsnfs_fh));
	vsnfs_trace(KERN_DEFAULT, "success :-) inode : %s\n", server->root_fh.data );

	//	BUG_ON(1);
      kfree(resp);
      /* REMOVE THIS RETURN TO PROCEED FURTHER */
      //    return -1;
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

int vsnfs_write_inode(struct inode *inode, int sync)
{
	printk("Inside wrrite inode\n");
	return EOPNOTSUPP;
}

void vsnfs_clear_inode(struct inode *inode)
{
    printk("Inside clear inode\n");
}


/*struct inode *vsnfs_alloc_inode(struct super_block *sb)
{
	struct vsnfs_inode *nfsi;
	nfsi = (struct vsnfs_inode *)kmalloc(sizeof(struct vsnfs_inode), GFP_KERNEL);
	if (!nfsi)
		return NULL;
	nfsi->vfs_inode = (struct inode *)kmalloc(sizeof(struct inode), GFP_KERNEL);
	return &nfsi->vfs_inode;
}

void vsnfs_destroy_inode(struct inode *inode)
{
	
	kfree(VSNFS_I(inode));
}*/

static struct file_system_type vsnfs_type = {
	.name		= "vsnfs",
	.get_sb		= vsnfs_get_sb,
	.kill_sb	= vsnfs_kill_sb,
};

static const struct super_operations vsnfs_sops = {
	.alloc_inode	= NULL,
	.destroy_inode	= NULL,
	.write_inode	= vsnfs_write_inode,
	.clear_inode	= vsnfs_clear_inode,
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

static inline
void vsnfs_fill_super(struct super_block *sb)
{
	printk(KERN_ERR "inside vsnfs_fill_super\n");
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

/* To set the super block dentry */
static int vsnfs_init_server(struct vsnfs_server *server, const char *dev_name, void *raw_data)
{
	int ret = 0;
	printk(KERN_INFO "Came in init server\n");	
    server->cl_rpc_ops = &vsnfs_clientops;
	ret = vsnfs_parse_mount_options((char *)raw_data, dev_name, server);
	if (ret < 0) {
		vsnfs_trace(KERN_DEFAULT, "Error parsing mount options\n");
		goto out;
		}
	printk(KERN_INFO "Contents of server are Mnt_Path:%s\n", server->mnt_path);
	out:
		return ret;		
}

/*
 * Wrapper for iget. We look up by inode number only
 */
struct inode *
vsnfs_fhget(struct super_block *sb, struct vsnfs_fh *fh)
{
	struct inode *inode = ERR_PTR(-ENOENT);
	unsigned long hash;
	char *tmp = NULL;
	tmp = fh->data;
	hash = simple_strtoul(fh->data, &tmp, 0);

	printk(KERN_INFO "Received hash valuse %lu\n", hash);
	
	inode = iget_locked(sb, hash);
	if (inode == NULL || IS_ERR(inode)) {
		inode = ERR_PTR(-ENOMEM);
		goto out_no_inode;
	}

	if (inode->i_state & I_NEW) {

		inode->i_ino = hash;
		inode->i_flags |= S_NOATIME|S_NOCMTIME;

		inode->i_op = VSNFS_SB(sb)->cl_rpc_ops->file_inode_ops;
		if (fh->type == VSNFS_REG) {
			printk("Came in regular file\n");
			inode->i_fop = &vsnfs_file_operations;
			inode->i_mode = (S_IFREG | S_IALLUGO);
//			inode->i_data.a_ops = &vsnfs_file_aops;
//			inode->i_size = VSNFS_MAXDATA;
		} else if (fh->type == VSNFS_DIR) {
			printk("Came in directory\n");
			inode->i_op = VSNFS_SB(sb)->cl_rpc_ops->dir_inode_ops;
			inode->i_fop = &vsnfs_dir_operations;
			inode->i_mode = (S_IFDIR | S_IALLUGO);
//			inode->i_size = VSNFS_DIRSIZE;
			inode->i_nlink = 2;
		} else {
			vsnfs_trace(KERN_ERR, "vsnfs_fhget: Invalid filetype. Aborting\n");
			unlock_new_inode(inode);
			iput(inode);
			goto out_no_inode;
		}

		inode->i_uid = 0;
		inode->i_gid = 0;
		inode->i_private = (void *) fh;

		unlock_new_inode(inode);
	} else
		vsnfs_trace(KERN_INFO, "Inode Found\n");

	printk("VSNFS: nfs_fhget(%s/%Ld ct=%d)\n",
		inode->i_sb->s_id,
		(long long)VSNFS_FILEID(inode),
		atomic_read(&inode->i_count));
out:
	return inode;

out_no_inode:
	printk("vsnfs_fhget: iget failed\n");
	goto out;
}

static int vsnfs_superblock_set_dummy_root(struct super_block *sb, struct inode *inode)
{
	if (sb->s_root == NULL) {
		sb->s_root = d_alloc_root(inode);
		if (sb->s_root == NULL) {
			printk("Error in d_alloc\n");
			iput(inode);
			return -ENOMEM;
		}

		atomic_inc(&inode->i_count);
		spin_lock(&dcache_lock);
		list_del_init(&sb->s_root->d_alias);
		spin_unlock(&dcache_lock);
	}
	return 0;
}
	
struct dentry *vsnfs_get_root(struct super_block *sb, struct vsnfs_fh *mntfh)
{
	struct vsnfs_server *server = VSNFS_SB(sb);
	struct dentry 		*mntroot;
	struct inode 		*rootinode = NULL;
	int 				ret;

	rootinode = vsnfs_fhget(sb, mntfh);
	if (IS_ERR(rootinode) || !rootinode) {
		vsnfs_trace(KERN_ERR, "vsnfs_get_root: Getting root inode failed\n");
		return ERR_CAST(rootinode);
	}

	ret = vsnfs_superblock_set_dummy_root(sb, rootinode);
	if (ret != 0)
		return ERR_PTR(ret);

	mntroot = d_obtain_alias(rootinode);
	if (IS_ERR(mntroot)) {
		vsnfs_trace(KERN_ERR, "vsnfs_get_root: Getting root dentry failed\n");
		return mntroot;
	}

	if (!mntroot->d_op)
		mntroot->d_op = server->cl_rpc_ops->dentry_ops;

	return mntroot;
}
	
static int vsnfs_get_sb(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *raw_data, struct vfsmount *mnt)
{
	int ret = 0;
	struct super_block *s = NULL;
	struct vsnfs_server *server = NULL;
	struct vsnfs_fh *mntfh;
	struct dentry *mntroot;

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
	if (IS_ERR(s) || !s) {
		ret = PTR_ERR(s);
		goto out_err_nosb;
	}

	/* Inline void function */
	if (!s->s_root)
		vsnfs_fill_super(s);

	mntfh = &server->root_fh;
	mntroot = vsnfs_get_root(s, mntfh);
	if (IS_ERR(mntroot) || !mntroot) {
		ret = PTR_ERR(mntroot);
		printk("Getting Mntroot failed:%d\n",ret);
		goto err_no_root;
	}

	s->s_flags |= MS_ACTIVE;
	mnt->mnt_sb = s;
	mnt->mnt_root = mntroot;
	ret = 0;

out:
	return ret;
out_err_nosb:
	kfree(server);
	goto out;
err_no_root:
	dput(mntroot);
	goto out;
}

static void vsnfs_kill_sb(struct super_block *s)
{
	struct vsnfs_server *server = VSNFS_SB(s);
	printk("Inside kill_sb\n");

	kill_anon_super(s);
	kfree(server);
}

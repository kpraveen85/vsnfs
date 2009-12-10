/*
 * CSE506 - VSNFS
 *
 * vsnfsDir.c --
 *
 *      Directory related operations for VSNFS
 *
 * (C) Karthik Balaji <findkb@gmail.com>
 * (C) Praveen Krishnamoorthy <kpraveen85@gmail.com>
 * (C) Prabakar Radhakrishnan <prabakarcse@gmail.com>
 */
#include <linux/dcache.h>
#include <linux/namei.h>
#include "vsnfs.h"
#include "vsnfsClient.h"
#include "vsnfsXdr.h"

static int vsnfs_opendir(struct inode *, struct file *);
static int vsnfs_readdir(struct file *, void *, filldir_t);
static struct dentry *vsnfs_lookup(struct inode *, struct dentry *,
				   struct nameidata *);
static int vsnfs_create(struct inode *, struct dentry *, int,
			struct nameidata *);
static int vsnfs_mkdir(struct inode *, struct dentry *, int);
static int vsnfs_rmdir(struct inode *, struct dentry *);
static int vsnfs_lookup_revalidate(struct dentry *dentry, struct nameidata *nd);
static int vsnfs_dentry_delete(struct dentry *dentry);
static void vsnfs_dentry_iput(struct dentry *dentry, struct inode *inode);

const struct file_operations vsnfs_dir_operations = {
	.read = generic_read_dir,
	.readdir = vsnfs_readdir,
	.open = vsnfs_opendir,
};

const struct inode_operations vsnfs_dir_inode_operations = {
	.create = vsnfs_create,
	.lookup = vsnfs_lookup,
	.mkdir = vsnfs_mkdir,
	.rmdir = vsnfs_rmdir,
	.permission = vsnfs_permission,
};

const struct dentry_operations vsnfs_dentry_operations = {
	.d_revalidate = vsnfs_lookup_revalidate,
	.d_delete = vsnfs_dentry_delete,
	.d_iput = vsnfs_dentry_iput,
};

static int vsnfs_opendir(struct inode *inode, struct file *filp)
{
	vsnfs_trace(KERN_INFO, "In OpenDir\n");
	return 0;
}

static int vsnfs_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
	vsnfs_trace(KERN_INFO, "In ReadDir\n");
	return 0;
}

static int
vsnfs_create(struct inode *dir, struct dentry *dentry, int mode,
	     struct nameidata *nd)
{
	vsnfs_trace(KERN_INFO, "In Create\n");
	return 0;
}

static struct dentry *vsnfs_lookup(struct inode *dir, struct dentry *dentry,
				   struct nameidata *nd)
{
	int ret;
	struct vsnfs_fh *newfh =
	    (struct vsnfs_fh *)kmalloc(sizeof(struct vsnfs_fh), GFP_KERNEL);
	struct inode *inode = NULL;

	vsnfs_trace(KERN_INFO, "VSNFS: lookup(%s/%s)\n",
		    dentry->d_parent->d_name.name, dentry->d_name.name);

	ret = -ENAMETOOLONG;

	if (dentry->d_name.len > VSNFS_MAXNAMLEN)
		goto out;

	ret = -ENOMEM;
	dentry->d_op = &vsnfs_dentry_operations;

	ret = vsnfs_do_lookup(dir, &dentry->d_name, newfh);
	if (ret == -ENOENT)
		goto no_entry;
	if (ret < 0)
		goto out;

	ret = -EINVAL;		/* We don't have permission checks */
	inode = vsnfs_fhget(dentry->d_sb, newfh);
	if (IS_ERR(inode))
		goto out;

      no_entry:
	ret = 0;
	dentry = d_materialise_unique(dentry, inode);
	if (IS_ERR(dentry)) {
		vsnfs_trace(KERN_ERR,
			    "vsnfs_lookup:Error allocating dentry:%ld\n",
			    PTR_ERR(dentry));
		return dentry;
	}
	return dentry;
      out:
	return ERR_PTR(ret);
}

static int vsnfs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	vsnfs_trace(KERN_INFO, "In Mkdir\n");
	return 0;
}

static int vsnfs_rmdir(struct inode *dir, struct dentry *dentry)
{
	vsnfs_trace(KERN_INFO, "In Rmdir\n");
	return 0;
}

int vsnfs_permission(struct inode *inode, int mask)
{
	vsnfs_trace(KERN_INFO, "In permissions\n");
	return 0;
}

static int vsnfs_lookup_revalidate(struct dentry *dentry, struct nameidata *nd)
{
	vsnfs_trace(KERN_INFO, "In Lookup_Revalidate\n");
	return 0;
}

static int vsnfs_dentry_delete(struct dentry *dentry)
{
	vsnfs_trace(KERN_INFO, "In dentry delete\n");
	return 0;
}

static void vsnfs_dentry_iput(struct dentry *dentry, struct inode *inode)
{
	vsnfs_trace(KERN_INFO, "In dentry iput\n");
}

int
vsnfs_do_lookup(struct inode *dir, struct qstr *filename, struct vsnfs_fh *fh)
{
	struct vsnfs_lookupargs *argp = NULL;
	int ret = 0;
	struct vsnfs_server *server = NULL;
	BUG_ON(dir->i_sb->s_fs_info == NULL);
	server = dir->i_sb->s_fs_info;
	vsnfs_trace(KERN_DEFAULT, "root_path : %s\n", server->mnt_path);
	vsnfs_trace(KERN_DEFAULT, "filename : %s\n", filename->name);
	vsnfs_trace(KERN_DEFAULT, "filename len : %d\n", filename->len);

	argp = kmalloc(sizeof(struct vsnfs_lookupargs), GFP_KERNEL);
	if (argp == NULL) {
		ret = -ENOMEM;
		vsnfs_trace(KERN_DEFAULT, "memory not available\n");
		goto out_do_lookup;
	}
	BUG_ON(dir->i_private == NULL);
	memcpy(&argp->fh, dir->i_private, sizeof(struct vsnfs_fh));
	vsnfs_trace(KERN_DEFAULT, "argp->fh %s : %d\n", argp->fh.data,
		    argp->fh.type);

	argp->filename = filename->name;
	argp->len = filename->len;
	ret = server->cl_rpc_ops->lookup(server, argp, fh);
	if (ret == 0) {
		vsnfs_trace(KERN_DEFAULT,
			    "lookup success :-) inode : %s type : %d\n",
			    fh->data, fh->type);
	} else {
		vsnfs_trace(KERN_DEFAULT, "lookup failure :-( %d\n", ret);
		ret = -VSNFSERR_REMOTE;
		goto out_do_lookup;
	}

      out_do_lookup:
	kfree(argp);
	return ret;
}

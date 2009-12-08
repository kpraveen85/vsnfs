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

#include "vsnfs.h"
#include "vsnfsClient.h"

static int vsnfs_opendir(struct inode *, struct file *);
static int vsnfs_readdir(struct file *, void *, filldir_t);
static struct dentry *vsnfs_lookup(struct inode *, struct dentry *, struct nameidata *);
static int vsnfs_create(struct inode *, struct dentry *, int, struct nameidata *);
static int vsnfs_mkdir(struct inode *, struct dentry *, int);
static int vsnfs_rmdir(struct inode *, struct dentry *);
static int vsnfs_lookup_revalidate(struct dentry * dentry, struct nameidata *nd);
static int vsnfs_dentry_delete(struct dentry *dentry);
static void vsnfs_dentry_iput(struct dentry *dentry, struct inode *inode);

const struct file_operations vsnfs_dir_operations = {
	.read		= generic_read_dir,
	.readdir	= vsnfs_readdir,
	.open		= vsnfs_opendir,
};

const struct inode_operations vsnfs_dir_inode_operations = {
	.create		= vsnfs_create,
	.lookup		= vsnfs_lookup,
	.mkdir		= vsnfs_mkdir,
	.rmdir		= vsnfs_rmdir,
	.permission	= vsnfs_permission,
};

const struct dentry_operations vsnfs_dentry_operations = {
	.d_revalidate	= vsnfs_lookup_revalidate,
	.d_delete		= vsnfs_dentry_delete,
	.d_iput			= vsnfs_dentry_iput,
};

static int
vsnfs_opendir(struct inode *inode, struct file *filp)
{
	vsnfs_trace(KERN_INFO, "In OpenDir\n");
	return 0;
}

static int
vsnfs_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
    vsnfs_trace(KERN_INFO, "In ReadDir\n");
    return 0;
}

static int 
vsnfs_create(struct inode *dir, struct dentry *dentry, int mode, struct nameidata *nd)
{
    vsnfs_trace(KERN_INFO, "In Create\n");
    return 0;
}

static struct dentry *
vsnfs_lookup(struct inode *dir, struct dentry * dentry, struct nameidata *nd)
{
    vsnfs_trace(KERN_INFO, "In Create\n");
    return 0;
}

static int 
vsnfs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
    vsnfs_trace(KERN_INFO, "In Mkdir\n");
    return 0;
}

static int 
vsnfs_rmdir(struct inode *dir, struct dentry *dentry)
{
    vsnfs_trace(KERN_INFO, "In Rmdir\n");
    return 0;
}

int
vsnfs_permission(struct inode *inode, int mask)
{
	vsnfs_trace(KERN_INFO, "In permissions\n");
	return 0;
}

static int
vsnfs_lookup_revalidate(struct dentry * dentry, struct nameidata *nd)
{
    vsnfs_trace(KERN_INFO, "In Lookup_Revalidate\n");
    return 0;
}

static int
vsnfs_dentry_delete(struct dentry *dentry)
{
    vsnfs_trace(KERN_INFO, "In dentry delete\n");
    return 0;
}

static void
vsnfs_dentry_iput(struct dentry *dentry, struct inode *inode)
{
    vsnfs_trace(KERN_INFO, "In dentry iput\n");
}


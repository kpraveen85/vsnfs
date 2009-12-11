/*
 * CSE506 - VSNFS
 *
 * vsnfsFile.c --
 *
 *      File operations for VSNFS
 *
 * (C) Karthik Balaji <findkb@gmail.com>
 * (C) Praveen Krishnamoorthy <kpraveen85@gmail.com>
 * (C) Prabakar Radhakrishnan <prabakarcse@gmail.com>
 */
#include <linux/aio.h>

#include "vsnfs.h"
#include "vsnfsClient.h"

static int vsnfs_file_open(struct inode *, struct file *);
static ssize_t vsnfs_file_read(struct file *, char *, size_t, loff_t *);
static ssize_t vsnfs_file_write(struct file *, const char *, size_t, loff_t *);

const struct file_operations vsnfs_file_operations = {
	.read = vsnfs_file_read,
	.write = vsnfs_file_write,
	.open = vsnfs_file_open,
};

const struct inode_operations vsnfs_file_inode_operations = {
	.permission = vsnfs_permission,
};

static int vsnfs_file_open(struct inode *inode, struct file *filp)
{
	return -EOPNOTSUPP;
}

static ssize_t
vsnfs_file_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	struct dentry *dentry = file->f_dentry;
	ssize_t result = 0;

	vsnfs_trace(KERN_INFO, "vsnfs: read(%s/%s, %lu@%lu)\n",
		dentry->d_parent->d_name.name, dentry->d_name.name,
		(unsigned long) count, (unsigned long) *ppos);
	return result;
}

static ssize_t
vsnfs_file_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	vsnfs_trace(KERN_INFO, "Inside file write\n");
	return -EOPNOTSUPP;
}

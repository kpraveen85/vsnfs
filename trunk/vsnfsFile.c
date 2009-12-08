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
static int vsnfs_file_read(struct kiocb *, const struct iovec *iov,
				unsigned long nr_segs, loff_t pos);
static ssize_t vsnfs_file_write(struct kiocb *, const struct iovec *iov,
				unsigned long nr_segs, loff_t pos);

const struct file_operations vsnfs_file_operations = {
	.read		= do_sync_read,
	.write		= do_sync_write,
	.aio_write	= vsnfs_file_read,
	.aio_read	= vsnfs_file_write,
	.open		= vsnfs_file_open,
};

const struct inode_operations vsnfs_file_inode_operations = {
	.permission	= vsnfs_permission,
};

static int
vsnfs_file_open(struct inode *inode, struct file *filp)
{
	vsnfs_trace(KERN_INFO, "Inside file open\n");
	return 0;
}

static ssize_t
vsnfs_file_read(struct kiocb *iocb, const struct iovec *iov,
		unsigned long nr_segs, loff_t pos)
{
	vsnfs_trace(KERN_INFO, "Inside file read\n");
	return 0;
}

static int
vsnfs_file_write(struct kiocb *iocb, const struct iovec *iov,
				unsigned long nr_segs, loff_t pos)
{
	vsnfs_trace(KERN_INFO, "Inside file write\n");
	return 0;
}

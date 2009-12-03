/*
 * CSE506 - VSNFS
 *
 * vsnfs.h --
 *
 *      Protocol definitions for VSNFS
 *
 *(C) Karthik Balaji <findkb@gmail.com>
 */
#ifndef _VSNFS_H
#define _VSNFS_H

#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/dcache.h>
#include <linux/gfp.h>
#include <linux/mm.h>

#define VSNFS_PROGRAM	 110003
#define VSNFS_VERSION	 1
#define VSNFS_PORT		 6789
#define VSNFS_MAXDATA	 8192
#define VSNFS_MAXNAMLEN	 255
#define VSNFS_MAXPATHLEN 1024
#define VSNFS_FHSIZE	 64
#define VSNFS_COOKIESIZE 4

/* vsnfs stats. These are the error codes that
 * are meaningful in RPC context
 */

enum vsnfs_stat {
	VSNFS_OK = 0,
	VSNFSERR_NOENT = 2,
	VSNFSERR_IO = 5,
	VSNFSERR_NXIO = 6,
	VSNFSERR_EAGAIN = 11,
	VSNFSERR_EXIST = 17,
	VSNFSERR_NODEV = 19,
	VSNFSERR_NOTDIR = 20,
	VSNFSERR_ISDIR = 21,
	VSNFSERR_INVAL = 22,
	VSNFSERR_FBIG = 27,
	VSNFSERR_NOSPC = 28,
	VSNFSERR_ROFS = 30,
	VSNFSERR_MLINK = 31,
	VSNFSERR_OPNOTSUPP = 45,
	VSNFSERR_NAMETOOLONG = 63,
	VSNFSERR_NOTEMPTY = 66,
	VSNFSERR_DQUOT = 69,
	VSNFSERR_STALE = 70,
	VSNFSERR_REMOTE = 71,
	VSNFSERR_BADHANDLE = 72,
};

enum vsnfs_ftype {
	VSNFSNON  = 0,
	VSNFSREG  = 1,
	VSNFSDIR  = 2
};


struct vsnfs_fh {
	unsigned short size;
	unsigned char data[VSNFS_FHSIZE];
};

#define VSNFS_SERVER(inode)		(&(inode)->i_sb->u.vsnfs_sb.s_server)
#define VSNFS_CLIENT(inode)		(NFS_SERVER(inode)->client)
/*
 * Returns a zero iff the size and data fields match.
 * Checks only "size" bytes in the data field.
 */
static inline int vsnfs_compare_fh(const struct vsnfs_fh *a, const struct vsnfs_fh *b)
{
	return a->size != b->size || memcmp(a->data, b->data, a->size) != 0;
}

static inline void vsnfs_copy_fh(struct vsnfs_fh *target, const struct vsnfs_fh *source)
{
	target->size = source->size;
	memcpy(target->data, source->data, source->size);
}

#define VSNFSPROC_NULL		0
#define VSNFSPROC_CREATE	1
#define VSNFSPROC_REMOVE	2
#define VSNFSPROC_READ		3
#define VSNFSPROC_WRITE		4
#define VSNFSPROC_MKDIR		5
#define VSNFSPROC_RMDIR		6
#define VSNFSPROC_READDIR	7
#define VSNFSPROC_LOOKUP	8
#define VSNFSPROC_GETROOT	9

#endif

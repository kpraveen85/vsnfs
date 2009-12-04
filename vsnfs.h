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
#include <linux/socket.h>
#include <linux/sunrpc/debug.h>

#ifdef VSNFS_DEBUG
#define vsnfs_trace(type, fmt, arg...) printk(type "%s" fmt, __func__, ## arg)
#else
#define vsnfs_trace(type, fmt, arg...)
#endif

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
	VSNFSERR_NAMETOOLONG = 63,
	VSNFSERR_NOTEMPTY = 66,
	VSNFSERR_DQUOT = 69,
	VSNFSERR_STALE = 70,
	VSNFSERR_REMOTE = 71,
	VSNFSERR_BADHANDLE = 72,
	VSNFSERR_OPNOTSUPP = 95,
	VSNFSERR_BAD_COOKIE = 10003,
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

/* NFS Client Parameters stored in superblock */

struct vsnfs_server {
    struct rpc_clnt         *cl_rpcclient;  /* RPC client handle */
    struct vsnfs_rpc_ops    *cl_rpc_ops;    /* VSNFS protocol vector */
    int                     cl_proto;
    struct sockaddr_storage cl_addr;        /* server identifier */
    size_t                  cl_addrlen;
/*  int                     flags; */
    int                     timeout;
    char                    hostname[VSNFS_MAXNAMLEN + 1];
    int                     namlen;
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

/* list no. of vsnfs procedures here */
#define NO_OF_VSNFSPROCS 1


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

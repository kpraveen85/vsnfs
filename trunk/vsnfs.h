/*
 * CSE506 - VSNFS
 *
 * vsnfs.h --
 *
 *      Protocol definitions for VSNFS
 *
 * (C) Karthik Balaji <findkb@gmail.com>
 * (C) Praveen Krishnamoorthy <kpraveen85@gmail.com>
 * (C) Prabakar Radhakrishnan <prabakarcse@gmail.com>
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
#include <linux/inet.h>
#include <linux/list.h>
#include <linux/vfs.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/bitops.h>
#include <linux/sunrpc/debug.h>
#include <linux/sunrpc/sched.h>
#include <linux/sunrpc/clnt.h>

#ifdef VSNFS_DEBUG
#define vsnfs_trace(type, fmt, arg...) printk(type "%s: " fmt, __func__, ## arg)
#else
#define vsnfs_trace(type, fmt, arg...)
#endif

#define VSNFS_PROGRAM	 110003
#define VSNFS_VERSION	 1
#define VSNFS_PORT		 6789
#define VSNFS_MAXDATA	 8192
#define VSNFS_DIRSIZE	 4096
#define VSNFS_MAXNAMLEN	 255
#define VSNFS_MAXPATHLEN 1024
#define VSNFS_FHSIZE	 8
#define VSNFS_COOKIESIZE 4
#define VSNFS_NRPROCS    2
#define VSNFS_TIMEOUT    600
#define VSNFS_SB_MAGIC	 0x7979

#define VSNFS_REG 		 1
#define VSNFS_DIR		 2

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

/*enum vsnfs_ftype {
	VSNFSNON  = 0,
	VSNFSREG  = 1,
	VSNFSDIR  = 2
};*/


struct vsnfs_fh {
	short int type;
	unsigned char data[VSNFS_FHSIZE];
};

/*struct svc_fh {
        struct vsnfs_fh fh;
};*/

/* NFS Client Parameters stored in superblock */

struct vsnfs_server {
	struct rpc_clnt*		cl_rpcclient;  /* RPC client handle */
	const struct vsnfs_rpc_ops*	cl_rpc_ops;    /* VSNFS protocol vector */
	struct sockaddr_in		cl_addr;        /* server identifier */
	size_t					cl_addrlen;
	int						flags; 
	int						timeout;
	char					ip_addr[16];
	int						server_port;
	char*					mnt_path; 
	struct vsnfs_fh			root_fh;
};

struct vsnfs_inode {
	__u64				fileid;	/* inode number */
	struct vsnfs_fh		fh; /* vsnfs filehandle */
	unsigned long		flags;
	struct inode		vfs_inode;
};

/*
 * Inode flags as bit offsets
 */

#define VSNFS_INO_STALE		(0)	/* Stale Inode */
#define VSNFS_INO_FLUSH		(4)	/* Due for flushing */

static inline struct vsnfs_inode *VSNFS_I(const struct inode *inode)
{
	return container_of(inode, struct vsnfs_inode, vfs_inode);
}

static inline struct vsnfs_server *VSNFS_SB(const struct super_block *s)
{
	return (struct vsnfs_server *)(s->s_fs_info);
}

static inline struct vsnfs_fh *VSNFS_FH(const struct inode *inode)
{
	//return &VSNFS_I(inode)->fh;
	return (struct vsnfs_fh *)inode->i_private;
}

static inline struct vsnfs_server *VSNFS_SERVER(const struct inode *inode)
{
	return VSNFS_SB(inode->i_sb);
}

static inline struct rpc_clnt *VSNFS_CLIENT(const struct inode *inode)
{
	return VSNFS_SERVER(inode)->cl_rpcclient;
}

static inline const struct vsnfs_rpc_ops *VSNFS_PROTO(const struct inode *inode)
{
	return VSNFS_SERVER(inode)->cl_rpc_ops;
}

static inline int VSNFS_STALE(const struct inode *inode)
{
	return test_bit(VSNFS_INO_STALE, &VSNFS_I(inode)->flags);
}

static inline __u64 VSNFS_FILEID(const struct inode *inode)
{
//	return VSNFS_I(inode)->fileid;
	return inode->i_ino;
}

static inline void set_vsnfs_fileid(struct inode *inode, __u64 fileid)
{
	VSNFS_I(inode)->fileid = fileid;
}

/*
 * Returns a zero iff the size and data fields match.
 * Checks only "size" bytes in the data field.
 */
/*static inline int vsnfs_compare_fh(const struct vsnfs_fh *a, const struct vsnfs_fh *b)
{
	return a->size != b->size || memcmp(a->data, b->data, a->size) != 0;
}

static inline void vsnfs_copy_fh(struct vsnfs_fh *target, const struct vsnfs_fh *source)
{
	target->size = source->size;
	memcpy(target->data, source->data, source->size);
}*/


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

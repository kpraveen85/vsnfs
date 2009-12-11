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
#include <linux/namei.h>
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
#define VSNFS_NRPROCS    10
#define VSNFS_TIMEOUT    600
#define VSNFS_SB_MAGIC	 0x7979

#define VSNFS_REG 		 1
#define VSNFS_DIR		 2
#define VSNFS_OTHER		 3

/* vsnfs stats. These are the error codes that
 * are meaningful in RPC context
 */

enum vsnfs_stat {
	VSNFS_OK = 0,		/* v2 v3 v4 */
	VSNFSERR_PERM = 1,	/* v2 v3 v4 */
	VSNFSERR_NOENT = 2,	/* v2 v3 v4 */
	VSNFSERR_IO = 5,	/* v2 v3 v4 */
	VSNFSERR_NXIO = 6,	/* v2 v3 v4 */
	VSNFSERR_EAGAIN = 11,	/* v2 v3 */
	VSNFSERR_ACCES = 13,	/* v2 v3 v4 */
	VSNFSERR_EXIST = 17,	/* v2 v3 v4 */
	VSNFSERR_XDEV = 18,	/*    v3 v4 */
	VSNFSERR_NODEV = 19,	/* v2 v3 v4 */
	VSNFSERR_NOTDIR = 20,	/* v2 v3 v4 */
	VSNFSERR_ISDIR = 21,	/* v2 v3 v4 */
	VSNFSERR_INVAL = 22,	/* v2 v3 v4 */
	VSNFSERR_FBIG = 27,	/* v2 v3 v4 */
	VSNFSERR_NOSPC = 28,	/* v2 v3 v4 */
	VSNFSERR_ROFS = 30,	/* v2 v3 v4 */
	VSNFSERR_MLINK = 31,	/*    v3 v4 */
	VSNFSERR_OPNOTSUPP = 45,	/* v2 v3 */
	VSNFSERR_NAMETOOLONG = 63,	/* v2 v3 v4 */
	VSNFSERR_NOTEMPTY = 66,	/* v2 v3 v4 */
	VSNFSERR_DQUOT = 69,	/* v2 v3 v4 */
	VSNFSERR_STALE = 70,	/* v2 v3 v4 */
	VSNFSERR_REMOTE = 71,	/* v2 v3 */
	VSNFSERR_WFLUSH = 99,	/* v2    */
	VSNFSERR_BADHANDLE = 10001,	/*    v3 v4 */
	VSNFSERR_NOT_SYNC = 10002,	/*    v3 */
	VSNFSERR_BAD_COOKIE = 10003,	/*    v3 v4 */
	VSNFSERR_NOTSUPP = 10004,	/*    v3 v4 */
	VSNFSERR_TOOSMALL = 10005,	/*    v3 v4 */
	VSNFSERR_SERVERFAULT = 10006,	/*    v3 v4 */
	VSNFSERR_BADTYPE = 10007,	/*    v3 v4 */
	VSNFSERR_JUKEBOX = 10008,	/*    v3 v4 */
	VSNFSERR_SAME = 10009,	/*       v4 */
	VSNFSERR_DENIED = 10010,	/*       v4 */
	VSNFSERR_EXPIRED = 10011,	/*       v4 */
	VSNFSERR_LOCKED = 10012,	/*       v4 */
	VSNFSERR_GRACE = 10013,	/*       v4 */
	VSNFSERR_FHEXPIRED = 10014,	/*       v4 */
	VSNFSERR_SHARE_DENIED = 10015,	/*       v4 */
	VSNFSERR_WRONGSEC = 10016,	/*       v4 */
	VSNFSERR_CLID_INUSE = 10017,	/*       v4 */
	VSNFSERR_RESOURCE = 10018,	/*       v4 */
	VSNFSERR_MOVED = 10019,	/*       v4 */
	VSNFSERR_NOFILEHANDLE = 10020,	/*       v4 */
	VSNFSERR_MINOR_VERS_MISMATCH = 10021,	/* v4 */
	VSNFSERR_STALE_CLIENTID = 10022,	/*       v4 */
	VSNFSERR_STALE_STATEID = 10023,	/*       v4 */
	VSNFSERR_OLD_STATEID = 10024,	/*       v4 */
	VSNFSERR_BAD_STATEID = 10025,	/*       v4 */
	VSNFSERR_BAD_SEQID = 10026,	/*       v4 */
	VSNFSERR_NOT_SAME = 10027,	/*       v4 */
	VSNFSERR_LOCK_RANGE = 10028,	/*       v4 */
	VSNFSERR_SYMLINK = 10029,	/*       v4 */
	VSNFSERR_RESTOREFH = 10030,	/*       v4 */
	VSNFSERR_LEASE_MOVED = 10031,	/*       v4 */
	VSNFSERR_ATTRNOTSUPP = 10032,	/*       v4 */
	VSNFSERR_NO_GRACE = 10033,	/*       v4 */
	VSNFSERR_RECLAIM_BAD = 10034,	/*       v4 */
	VSNFSERR_RECLAIM_CONFLICT = 10035,	/*       v4 */
	VSNFSERR_BAD_XDR = 10036,	/*       v4 */
	VSNFSERR_LOCKS_HELD = 10037,	/*       v4 */
	VSNFSERR_OPENMODE = 10038,	/*       v4 */
	VSNFSERR_BADOWNER = 10039,	/*       v4 */
	VSNFSERR_BADCHAR = 10040,	/*       v4 */
	VSNFSERR_BADNAME = 10041,	/*       v4 */
	VSNFSERR_BAD_RANGE = 10042,	/*       v4 */
	VSNFSERR_LOCK_NOTSUPP = 10043,	/*       v4 */
	VSNFSERR_OP_ILLEGAL = 10044,	/*       v4 */
	VSNFSERR_DEADLOCK = 10045,	/*       v4 */
	VSNFSERR_FILE_OPEN = 10046,	/*       v4 */
	VSNFSERR_ADMIN_REVOKED = 10047,	/*       v4 */
	VSNFSERR_CB_PATH_DOWN = 10048,	/*       v4 */
};

/*enum vsnfs_ftype {
	VSNFSNON  = 0,
	VSNFSREG  = 1,
	VSNFSDIR  = 2
};*/

/* TO DO cleanup of the structure required */
struct vsnfs_fh {
	unsigned char data[VSNFS_FHSIZE];
	int type;
};

/*struct svc_fh {
        struct vsnfs_fh fh;
};*/

/* NFS Client Parameters stored in superblock */

struct vsnfs_server {
	struct rpc_clnt *cl_rpcclient;	/* RPC client handle */
	const struct vsnfs_rpc_ops *cl_rpc_ops;	/* VSNFS protocol vector */
	struct sockaddr_in cl_addr;	/* server identifier */
	size_t cl_addrlen;
	int flags;
	int timeout;
	char ip_addr[16];
	int server_port;
	char *mnt_path;
	struct vsnfs_fh root_fh;
};

/*
 * Inode flags as bit offsets
 */

#define VSNFS_INO_STALE		(0)	/* Stale Inode */
#define VSNFS_INO_FLUSH		(4)	/* Due for flushing */
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

static inline __u64 VSNFS_FILEID(const struct inode *inode)
{
//      return VSNFS_I(inode)->fileid;
	return inode->i_ino;
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
#define VSNFSPROC_GETROOT	1
#define VSNFSPROC_REMOVE	2
#define VSNFSPROC_READ		3
#define VSNFSPROC_WRITE		4
#define VSNFSPROC_MKDIR		5
#define VSNFSPROC_RMDIR		6
#define VSNFSPROC_READDIR	7
#define VSNFSPROC_LOOKUP	8
#define VSNFSPROC_CREATE	9

#endif

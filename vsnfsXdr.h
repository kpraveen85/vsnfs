/*
 * CSE506 - VSNFS
 *
 * vsnfsXdr.h --
 * 
 * Contains the declaration of arguments for RPC in Server.
 *
 *(C) Karthik Balaji <findkb@gmail.com>
 *(C) Praveen Krishnamoorthy <kpraveen85@gmail.com>
 *(C) Prabakar Radhakrishnan <prabakarcse@gmail.com>
 */

#ifndef _VSNFSXDR_H
#define _VSNFSXDR_H

#include <linux/sunrpc/clnt.h>
#include <linux/sunrpc/xdr.h>
#include <linux/sunrpc/xprt.h>
#include <linux/mm.h>

#define VSNFS_FILE_IO_SIZE		(4096U)
#define VSNFS_FILE_BSIZE_BITS	(12)

/* Put all arguments and response structure here */
/* Argument for NULL proc */
struct vsnfs_nullargs {
	int dummy;
};
/* Response from NULL proc*/
struct vsnfs_nullres {
	int dummy;
};

/*
 * Arguements to readdir call
 */

struct vsnfs_readdirargs {
	struct vsnfs_fh *fh;
	unsigned int count;
	struct page **pages;
};

struct vsnfs_readargs {
	struct vsnfs_fh *fh;
	unsigned int offset;
	unsigned int length;
	struct page **pages;
};

/* Response from NULL proc*/
struct vsnfs_readres {
	unsigned long count;
};


struct vsnfs_entry {
	__u64 ino;
	__u64 offset;
	const char *name;
	unsigned int len;
	int eof;
	struct vsnfs_fh *fh;
};

struct vsnfs_getrootargs {
	char *path;
	unsigned int len;
};

struct vsnfs_lookupargs {
	struct vsnfs_fh fh;
	const char *filename;
	unsigned int len;
};

struct vsnfs_rpc_ops {
	int version;		/*Protocol Version */
	const struct dentry_operations *dentry_ops;
	const struct inode_operations *dir_inode_ops;
	const struct inode_operations *file_inode_ops;

	int (*nullproc) (struct vsnfs_server *, int, int *);
	int (*getroot) (struct vsnfs_server *, struct vsnfs_getrootargs *,
			struct vsnfs_fh *);
	int (*lookup) (struct vsnfs_server *, struct vsnfs_lookupargs *,
		       struct vsnfs_fh *);
	/*    int             (*create)  (struct inode *, struct dentry *, struct iattr *,
	   int, struct vsnfs_fh *);
	   int                (*remove)  (struct dentry *, struct qstr *);
	   int                (*mkdir)   (struct inode *, struct dentry *, struct iattr *);
	   int                (*rmdir)   (struct inode *, struct qstr *); */
	int (*readdir) (struct dentry *, struct page *, unsigned int);
	int (*read)	(struct dentry *dentry, struct page *page,
				   loff_t *offset, unsigned int count, int *pos);	
	__be32 *(*decode_dirent) (__be32 *, struct vsnfs_entry *);
};

int vsnfs_stat_to_errno(int stat);

extern int vsnfs_stat_to_errno(int);
extern struct rpc_procinfo vsnfs_procedures[];
extern __be32 *vsnfs_decode_dirent(__be32 *, struct vsnfs_entry *);

extern const struct vsnfs_rpc_ops vsnfs_clientops;
extern struct rpc_version vsnfs_version1;
extern struct rpc_program vsnfs_program;
extern struct rpc_stat vsnfs_rpcstat;

#endif

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

/*
 * Arguements to readdir call (JUST A SAMPLE)
 */

struct vsnfs_readdirargs {
	struct vsnfs_fh*	fh;
	__u32				cookie;
	unsigned int		count;
	struct page**		pages;
};

struct vsnfs_entry {
	__u64				ino;
	__u64				cookie,
					prev_cookie;
	const char *		name;
	unsigned int		len;
	int					eof;
	struct vsnfs_fh *	fh;
};	

struct vsnfs_rpc_ops {
	int		version;		/*Protocol Version*/
	const struct dentry_operations *dentry_ops;
	const struct inode_operations *dir_inode_ops;
	const struct inode_operations *file_inode_ops;

	int		(*getroot) (struct vsnfs_server *, struct vsnfs_fh *);
/*	int		(*lookup)  (struct inode *, struct qstr *, struct vsnfs_fh *);
	int		(*create)  (struct inode *, struct dentry *, struct iattr *,
						int, struct vsnfs_fh *);
	int		(*remove)  (struct dentry *, struct qstr *);
	int		(*mkdir)   (struct inode *, struct dentry *, struct iattr *);
	int		(*rmdir)   (struct inode *, struct qstr *); */
	int		(*readdir) (struct dentry *, u64, struct page *,
						unsigned int, int);
	__be32 *(*decode_dirent) (__be32 *, struct vsnfs_entry *, int plus);
};

int
vsnfs_stat_to_errno(int stat);

extern int vsnfs_stat_to_errno(int);
extern struct rpc_procinfo vsnfs_procedures[];
extern __be32 *vsnfs_decode_dirent(__be32 *, struct vsnfs_entry *, int);

extern struct vsnfs_rpc_ops vsnfs_clientops;
extern struct rpc_version vsnfs_version1;
extern struct rpc_program vsnfs_program;
extern struct rpc_stat vsnfs_rpcstat;

#endif

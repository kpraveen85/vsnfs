/*
 * CSE506 - VSNFS
 *
 * vsnfsClient.h --
 *
 *      VSNFS client headers
 *
 *(C) Karthik Balaji <findkb@gmail.com>
 */
#ifndef _VSNFSCLIENT_H
#define _VSNFSCLIENT_H
#include <linux/module.h>
#include <linux/socket.h>
#include "vsnfs.h"

/* Some callers of 'ls' use the file block size returned by a stat of a
 * directory as the size of buffer supplied to 'ls'. Sizes smaller that 4096
 * might result in more READDIR calls to the server than we need
 */

#define VSNFS_DEFAULT_DISK_BLOCK_SIZE	512
#define VSNFS_DEFAULT_FILE_BLOCK_SIZE	4096

/* The maximum size of the RPC reply header and VSNFS reply header for
 * a READ or WRITE request. Since we know these sizes are fixed this keeps
 * us from reading more data out of the socket than we need to
 */

#define VSNFS_MAX_IO_HEADER_SIZE	256

/* Client Initialization routines */
extern int VSNFSClientInit(void);
extern int VSNFSClientCleanup(void);

/* NFS Client Parameters stored in superblock */

struct vsnfs_server {
	struct rpc_clnt			*cl_rpcclient;	/* RPC client handle */
	struct vsnfs_rpc_ops	*cl_rpc_ops;	/* VSNFS protocol vector */
	int						cl_proto;
	struct sockaddr_storage cl_addr;		/* server identifier */
	size_t					cl_addrlen;
/*	int 					flags; */
	int     				timeout;
	char    				hostname[VSNFS_MAXNAMLEN + 1];
	int     				namlen;
};


/*
 * vsnfs/file.c
 */

extern const struct inode_operations vsnfs_file_inode_operations;
extern const struct file_operations vsnfs_file_operations;

/*
 * vsnfs/dir.c
 */

extern const struct inode_operations vsnfs_dir_inode_operations;
extern const struct file_operations vsnfs_dir_operations;
extern const struct dentry_operations vsnfs_dentry_operations;

/*
 * vsnfs/read_write.c
 */

#define TO_BE_FILLED 0

#endif

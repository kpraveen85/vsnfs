/*
 * CSE506 - VSNFS
 *
 * vsnfsMount.h --
 * 
 * Header file for mount related routines and structures.
 *
 * (C) Karthik Balaji <findkb@gmail.com>
 *
 */

#include <linux/in.h>
#include "vsnfs.h"

/* Structure passed from user-space to kernel-space
 * during vsnfs mount
 */

struct vsnfs_mount_data {
	int 	fd;
	struct 	vsnfs_fh old_root;
	int 	flags;
	int 	timeout;
	int		rsize;
	int 	wsize;
	struct 	sockaddr_in addr;
	char 	hostname[VSNFS_MAXNAMLEN + 1];
	int 	namlen;
	unsigned int 	bsize;
	struct 	vsnfs_fh root;
};

/* Bits in the flags field */

#define VSNFS_MOUNT_SOFT	0x0001
#define VSNFS_MOUNT_TCP		0x0040
#define VSNFS_FLAG_MASK		0xFFFF

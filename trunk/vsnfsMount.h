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
#ifndef _VSNFSMOUNT_H
#define _VSNFSMOUNT_H

#include <linux/in.h>
/* Structure passed from user-space to kernel-space
 * during vsnfs mount
 */

struct vsnfs_mount_data {
	int 	fd;
	struct 	vsnfs_fh old_root;
	int 	flags;
	int 	timeout;
	struct 	sockaddr_in addr;
	char 	hostname[VSNFS_MAXNAMLEN + 1];
	int 	namlen;
	struct 	vsnfs_fh root;
};

extern int
vsnfs_parse_mount_options(char *options, const struct vsnfs_mount_data *data,
							struct vsnfs_fh *mntfh, const char *dev_name);

#endif

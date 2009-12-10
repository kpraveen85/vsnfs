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

extern int
vsnfs_parse_mount_options(char *options, const char *dev_name,
			  struct vsnfs_server *server);

#endif

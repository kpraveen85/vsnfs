/*
 * CSE506 - VSNFS
 *
 * vsnfsMount.c --
 * 
 * This file contains to manipulate MountPointEntry. This
 * includes the ability to mount and umount systems.
 *
 * (C) Karthik Balaji <findkb@gmail.com>
 *
 */

#include <linux/types.h>
#include <linux/parser.h>
#include "vsnfsMount.h"
#include "vsnfs.h"

enum { vsnfs_opt_port, vsnfs_opt_ip, vsnfs_opt_err };

static const match_table_t tokens = {
	{vsnfs_opt_port, "port=%u"},
	{vsnfs_opt_ip, "ip=%s"},
	{vsnfs_opt_err, NULL}
};

/* Read and validate the mount data */
int vsnfs_parse_mount_options(char *raw_data, const struct vsnfs_mount_data *data,
                            struct vsnfs_fh *mntfh, const char *dev_name)
{
	char *ip;
	char *p;
	char *port_src;
	u_int port = 0;
	int rc = 0;
	int token;
	substring_t args[MAX_OPT_ARGS];
	printk("Came in parse options\n");

	if (!raw_data) {
		rc = -EINVAL;
		goto out;
	}

	while ((p = strsep(&raw_data, ",")) != NULL) {
		if (!*p)
			continue;
		token = match_token(p, tokens, args);
		switch (token) {
		case vsnfs_opt_port:
			port_src = args[0].from;
			port = (int)simple_strtol(port_src, &port_src, 0);
			printk("Received port from US %d\n", port);
			break;
		case vsnfs_opt_ip:
			ip = kstrdup(args[0].from, GFP_KERNEL);
			printk("Received IP : %s\n", ip);
			break;
		case vsnfs_opt_err:
		default:
			printk(KERN_WARNING
					"%s: vsnfs: unrecognized option [%s]\n",
					__func__, p);
		}
	}
out:
	return 0;
}

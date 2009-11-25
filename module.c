/*
 * CSE506 - VSNFS
 *
 * module.c
 * 
 * File that holds all the act together. Contains 
 * initialization routines.
 *
 * (C) Karthik Balaji <findkb@gmail.com>
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/uaccess.h>
#include <linux/errno.h>

#include "vsnfsClient.h"

static int
__init vsnfs_init(void)
{
	if (!VSNFSClientInit())
		return -1;
	return 0;
}

static void
__exit vsnfs_cleanup(void)
{
	VSNFSClientCleanup();
}

module_init(vsnfs_init);
module_exit(vsnfs_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KB");

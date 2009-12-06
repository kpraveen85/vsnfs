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

#include "vsnfs.h"
#include "vsnfsClient.h"

static int
__init vsnfs_init(void)
{
	int ret;
	ret = VSNFSClientInit();
	if (ret < 0)
		printk(KERN_ERR "MODULE INSERT FAILED WITH %d\n", ret);
	return ret;
}

static void
__exit vsnfs_cleanup(void)
{
	int ret;
	ret = VSNFSClientCleanup();
	if (ret < 0)
		printk(KERN_ERR "UNABLE TO REMOVE MODULE:%d\n", ret);
}

module_init(vsnfs_init);
module_exit(vsnfs_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KB");

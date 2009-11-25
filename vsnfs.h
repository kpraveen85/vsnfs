/*
 * CSE506 - VSNFS
 *
 * vsnfs.h --
 *
 *      Protocol definitions for VSNFS
 *
 *(C) Karthik Balaji <findkb@gmail.com>
 */
#ifndef _VSNFS_H
#define _VSNFS_H

#define VSNFS_PORT	6789
#define VSNFS_MAXDATA	8192
#define VSNFS_FHSIZE	64
#define VSNFS_MAXNAMLEN	255

enum vsnfs_ftype {
	VSNFSNON  = 0,
	VSNFSREG  = 1,
	VSNFSDIR  = 2
};

struct vsnfs_fh {
	unsigned short size;
	unsigned char data[VSNFS_FHSIZE];
};

#define VSNFS_VERSION		1
#define VSNFSPROC_NULL		0
#define VSNFSPROC_CREATE	1
#define VSNFSPROC_REMOVE	2
#define VSNFSPROC_READ		3
#define VSNFSPROC_WRITE		4
#define VSNFSPROC_MKDIR		5
#define VSNFSPROC_RMDIR		6
#define VSNFSPROC_READDIR	7
#define VSNFSPROC_LOOKUP	8

#endif

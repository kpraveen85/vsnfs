/*
 * CSE506 - VSNFS
 *
 * vsnfs.h --
 *
 *      Protocol definitions for VSNFS
 *
 *(C) Karthik Balaji <findkb@gmail.com>
 */
#ifndef _LINUX_VSNFS_H
#define _LINUX_VSNFS_H

#ifndef Bool
typedef int Bool
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define VSNFS_PORT	6789
#define VSNFS_MAXDATA	8192
#define VSNFS_FHSIZE	64

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

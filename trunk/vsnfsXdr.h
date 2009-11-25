/*
 * CSE506 - VSNFS
 *
 * vsnfsXdr.h --
 * 
 * Contains the declaration of arguments for RPC in Server.
 *
 *(C) Karthik Balaji <findkb@gmail.com>
 *
 */

#include "vsnfs.h"
#include "vsnfsClient.h"

/*
 * Arguements to read call (JUST A SAMPLE)
 */

struct vsnfs_readargs {
	struct vsnfs_fh *fh;
	__u64 offset;
	__u64 count;
}
	

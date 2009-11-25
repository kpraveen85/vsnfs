/*
 * CSE506 - VSNFS
 *
 * vsnfsClient.h --
 *
 *      VSNFS client headers
 *
 *(C) Karthik Balaji <findkb@gmail.com>
 */

#include "vsnfs.h"

/* Some callers of 'ls' use the file block size returned by a stat of a
 * directory as the size of buffer supplied to 'ls'. Sizes smaller that 4096
 * might result in more READDIR calls to the server than we need
 */

#define VSNFS_DEFAULT_DISK_BLOCK_SIZE	512
#define VSNFS_DEFAULT_FILE_BLOCK_SIZE	4096

/* The mazimum size of the RPC replt header and VSNFS reply header for
 * a READ or WRITE request. Since we know these sizes are fixed this keeps
 * us from reading more data out of the socket than we need to
 */

#define VSNFS_MAX_IO_HEADER_SIZE	256

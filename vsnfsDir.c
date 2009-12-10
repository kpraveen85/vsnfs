/*
 * CSE506 - VSNFS
 *
 * vsnfsDir.c --
 *
 *      Directory related operations for VSNFS
 *
 * (C) Karthik Balaji <findkb@gmail.com>
 * (C) Praveen Krishnamoorthy <kpraveen85@gmail.com>
 * (C) Prabakar Radhakrishnan <prabakarcse@gmail.com>
 */
#include <linux/dcache.h>
#include <linux/namei.h>
#include <linux/pagemap.h>
#include <linux/page-flags.h>
#include <linux/gfp.h>
#include "vsnfs.h"
#include "vsnfsClient.h"
#include "vsnfsXdr.h"

static int vsnfs_opendir(struct inode *, struct file *);
static int vsnfs_readdir(struct file *, void *, filldir_t);
static struct dentry *vsnfs_lookup(struct inode *, struct dentry *,
				   struct nameidata *);
static int vsnfs_create(struct inode *, struct dentry *, int,
			struct nameidata *);
static int vsnfs_mkdir(struct inode *, struct dentry *, int);
static int vsnfs_rmdir(struct inode *, struct dentry *);
static int vsnfs_lookup_revalidate(struct dentry *dentry, struct nameidata *nd);
static int vsnfs_dentry_delete(struct dentry *dentry);
static void vsnfs_dentry_iput(struct dentry *dentry, struct inode *inode);

const struct file_operations vsnfs_dir_operations = {
	.read = generic_read_dir,
	.readdir = vsnfs_readdir,
	.open = vsnfs_opendir,
};

const struct inode_operations vsnfs_dir_inode_operations = {
	.create = vsnfs_create,
	.lookup = vsnfs_lookup,
	.mkdir = vsnfs_mkdir,
	.rmdir = vsnfs_rmdir,
	.permission = vsnfs_permission,
};

const struct dentry_operations vsnfs_dentry_operations = {
	.d_revalidate = vsnfs_lookup_revalidate,
	.d_delete = vsnfs_dentry_delete,
	.d_iput = vsnfs_dentry_iput,
};

typedef __be32 * (*decode_dirent_t)(__be32 *, struct vsnfs_entry *);

/* vsnfs_readdir_descriptor_t :- This structure is used to describe the pages 
 * containing dirents returned by the server. Though we have fields like page_index 
 * we just support reading a single page full of dirents as of now. In other words, 
 * when you do a "ls" you will see files whose dirents were present in that single 
 * page returned by the server. The unused fields are for extensibility.
 */
typedef struct {
    struct file*        file;
    struct page*        page;
    unsigned long       page_index;
    u32*                ptr;
    u64                 target;
    struct vsnfs_entry* entry;
    decode_dirent_t     decode;
    int                 error;
}vsnfs_readdir_descriptor_t;

static int vsnfs_opendir(struct inode *inode, struct file *filp)
{
	vsnfs_trace(KERN_INFO, "In OpenDir\n");
	return 0;
}

static inline
int dir_decode(vsnfs_readdir_descriptor_t *desc)
{
    __be32 *p = desc->ptr;

    p = desc->decode(p, desc->entry);
    if(IS_ERR(p))
        return PTR_ERR(p);

    desc->ptr = p;
    return 0;
}

/* Helper to release the page in the descriptor once read */
static inline
void dir_page_release(vsnfs_readdir_descriptor_t *desc)
{
    kunmap(desc->page);
    page_cache_release(desc->page);
    desc->page = NULL;
    desc->ptr = NULL;
}

/* vsnfs_do_filldir :- Main routine that reads the dirents from the page returned
 * by server and fills them in the client.
 */
static
int vsnfs_do_filldir(vsnfs_readdir_descriptor_t *desc, void *dirent,
           filldir_t filldir)
{
    struct vsnfs_entry *entry = desc->entry;
    unsigned long fileid;
    unsigned int d_type;
    int res;

    for(;;) {
        fileid = entry->ino;
        if (entry->fh->type == VSNFS_REG)
            d_type = 8;
        else if (entry->fh->type == VSNFS_DIR)
            d_type = 4;
        else
            d_type = DT_UNKNOWN;
        res = filldir(dirent, entry->name, entry->len, entry->offset, fileid, d_type);

        if (res < 0)
            break;
        entry->offset++;
        /* Need to add code here if we use cookie */
    }
    dir_page_release(desc);

    return res;
}

/* simple_readdir :- We don't maintain any entry in the page cache for
 * dirents. So whenever readdir is called we issue a request to server and
 * it returns a single page full of dirents.
 */
static inline
int simple_readdir(vsnfs_readdir_descriptor_t *desc, void *dirent,
                    filldir_t filldir)
{
    struct file *file = desc->file;
    struct inode *inode = file->f_path.dentry->d_inode;
    struct page *page = NULL;
    int status;

    page = alloc_page(GFP_HIGHUSER);
    if (!page) {
        status = -ENOMEM;
        goto out;
    }

    status = VSNFS_PROTO(inode)->readdir(file->f_path.dentry, page, PAGE_CACHE_SIZE);

    if (status < 0)
        goto out;

    desc->page = page;
    desc->ptr = kmap(page);

    status = vsnfs_do_filldir(desc, dirent, filldir);

    desc->page_index = 0;
    desc->entry->offset = 0;
    desc->entry->eof = 1; /* Hardcoded since we just support one page read */

out:
    return status;

}

/* -->reaaddir() procedure for VSNFS */
static int
vsnfs_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
    struct dentry   *dentry = filp->f_dentry;
    struct inode    *inode = dentry->d_inode;
    vsnfs_readdir_descriptor_t my_desc, *desc = &my_desc;
    struct vsnfs_entry my_entry;
    struct vsnfs_fh fh;
    long res;

    memset(desc, 0, sizeof(*desc));

    desc->file = filp;
    desc->target = filp->f_pos;
    desc->decode = VSNFS_PROTO(inode)->decode_dirent;

    my_entry.eof = 0;
    my_entry.fh = &fh;
    desc->entry = &my_entry;

    while(!desc->entry->eof) {
        res = simple_readdir(desc, dirent, filldir);
        if (res == -EBADCOOKIE) {
            if (res >= 0)
                continue;
        }

        if (res < 0)
            break;
    }

    if (res > 0)
        res = 0;
    vsnfs_trace(KERN_INFO, "VSNFS: readdir(%s/%s) returns %ld\n",
            dentry->d_parent->d_name.name, dentry->d_name.name,
            res);
    return res;
}

static int
vsnfs_create(struct inode *dir, struct dentry *dentry, int mode,
	     struct nameidata *nd)
{
	vsnfs_trace(KERN_INFO, "In Create\n");
	return -EOPNOTSUPP;
}

static struct dentry *vsnfs_lookup(struct inode *dir, struct dentry *dentry,
				   struct nameidata *nd)
{
	int ret;
	struct vsnfs_fh *newfh =
	    (struct vsnfs_fh *)kmalloc(sizeof(struct vsnfs_fh), GFP_KERNEL);
	struct inode *inode = NULL;

	vsnfs_trace(KERN_INFO, "VSNFS: lookup(%s/%s)\n",
		    dentry->d_parent->d_name.name, dentry->d_name.name);

	ret = -ENAMETOOLONG;

	if (dentry->d_name.len > VSNFS_MAXNAMLEN)
		goto out;

	ret = -ENOMEM;
	dentry->d_op = &vsnfs_dentry_operations;

	ret = vsnfs_do_lookup(dir, &dentry->d_name, newfh);
	if (ret == -ENOENT)
		goto no_entry;
	if (ret < 0)
		goto out;

	ret = -EINVAL;		/* We don't have permission checks */
	inode = vsnfs_fhget(dentry->d_sb, newfh);
	if (IS_ERR(inode))
		goto out;

      no_entry:
	ret = 0;
	dentry = d_materialise_unique(dentry, inode);
	if (IS_ERR(dentry)) {
		vsnfs_trace(KERN_ERR,
			    "vsnfs_lookup:Error allocating dentry:%ld\n",
			    PTR_ERR(dentry));
		return dentry;
	}
	return dentry;
      out:
	return ERR_PTR(ret);
}

static int vsnfs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	vsnfs_trace(KERN_INFO, "In Mkdir\n");
	return -EOPNOTSUPP;
}

static int vsnfs_rmdir(struct inode *dir, struct dentry *dentry)
{
	vsnfs_trace(KERN_INFO, "In Rmdir\n");
	return -EOPNOTSUPP;
}

/* -->permission procedure for VSNFS. We dont have permission
 * checks as of now. So we always return 0.
 */
int vsnfs_permission(struct inode *inode, int mask)
{
	vsnfs_trace(KERN_INFO, "In permissions\n");
	return 0;
}

static int vsnfs_lookup_revalidate(struct dentry *dentry, struct nameidata *nd)
{
	vsnfs_trace(KERN_INFO, "In Lookup_Revalidate\n");
	return -EOPNOTSUPP;
}

static int vsnfs_dentry_delete(struct dentry *dentry)
{
	vsnfs_trace(KERN_INFO, "In dentry delete\n");
	return -EOPNOTSUPP;
}

static void vsnfs_dentry_iput(struct dentry *dentry, struct inode *inode)
{
	vsnfs_trace(KERN_INFO, "In dentry iput\n");
}

int
vsnfs_do_lookup(struct inode *dir, struct qstr *filename, struct vsnfs_fh *fh)
{
	struct vsnfs_lookupargs *argp = NULL;
	int ret = 0;
	struct vsnfs_server *server = NULL;
	BUG_ON(dir->i_sb->s_fs_info == NULL);
	server = dir->i_sb->s_fs_info;
	vsnfs_trace(KERN_DEFAULT, "root_path : %s\n", server->mnt_path);
	vsnfs_trace(KERN_DEFAULT, "filename : %s\n", filename->name);
	vsnfs_trace(KERN_DEFAULT, "filename len : %d\n", filename->len);

	argp = kmalloc(sizeof(struct vsnfs_lookupargs), GFP_KERNEL);
	if (argp == NULL) {
		ret = -ENOMEM;
		vsnfs_trace(KERN_DEFAULT, "memory not available\n");
		goto out_do_lookup;
	}
	BUG_ON(dir->i_private == NULL);
	memcpy(&argp->fh, dir->i_private, sizeof(struct vsnfs_fh));
	vsnfs_trace(KERN_DEFAULT, "argp->fh %s : %d\n", argp->fh.data,
		    argp->fh.type);

	argp->filename = filename->name;
	argp->len = filename->len;
	ret = server->cl_rpc_ops->lookup(server, argp, fh);
	if (ret == 0) {
		vsnfs_trace(KERN_DEFAULT,
			    "lookup success :-) inode : %s type : %d\n",
			    fh->data, fh->type);
	} else {
		vsnfs_trace(KERN_DEFAULT, "lookup failure :-( %d\n", ret);
		ret = -VSNFSERR_REMOTE;
		goto out_do_lookup;
	}

      out_do_lookup:
	kfree(argp);
	return ret;
}

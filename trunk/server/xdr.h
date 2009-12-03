#ifndef XDR_H
#define XDR_H

int vsnfssvc_decode_void(struct svc_rqst *, __be32 *, void *);
int vsnfssvc_encode_void(struct svc_rqst *, __be32 *, void *);

/* For read Operation */

int vsnfssvc_decode_readargs(struct svc_rqst *, __be32 *,
        			   struct vsnfsd_readargs *);
int vsnfssvc_encode_readres(struct svc_rqst *, __be32 *, 
			            struct vsnfsd_readres *);

/* For Readdir Operation */

int vsnfssvc_decode_readdirargs(struct svc_rqst *, __be32 *, struct vsnfsd_readdirargs *);
int vsnfssvc_encode_readdirres(struct svc_rqst *, __be32 *, struct vsnfsd_readdirres *);

struct vsnfsd_void { int dummy; };

struct vsnfsd_readargs {
  struct svc_fh fh;
  __u32 offset;
  __u32 count;
  int vlen;
};

struct vsnfsd_readres {
  struct svc_fh fh;
  unsigned long count;
  struct kstat stat;
};

struct vsnfsd_readdirargs {
         struct svc_fh fh;
         __u32 cookie;
         __u32 count;
         __be32 *buf;
};

struct vsnfsd_readdirres {
         int count;
         __be32 err;
         __be32 *buf;
         int buflen;
         __be32 *offset;
};

/*
 * Storage requirements for XDR arguments and results.
 */
union vsnfsd_xdrstore {
	struct vsnfsd_void null; 
	struct vsnfsd_readargs	read;
	struct vsnfsd_readdirargs readdir;


	/*put all the xdr args here*/
	/* 
	struct nfsd_sattrargs	sattr;
	struct nfsd_diropargs	dirop;
	struct nfsd_writeargs	write;
	struct nfsd_createargs	create;
	struct nfsd_renameargs	rename;
	struct nfsd_linkargs	link;
	struct nfsd_symlinkargs	symlink;
	*/
};

#define VSNFS_SVC_XDRSIZE	sizeof(union vsnfsd_xdrstore)

#endif

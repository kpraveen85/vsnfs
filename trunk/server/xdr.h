#ifndef XDR_H
#define XDR_H
 
/*struct vsnfsd_void { int dummy; };*/ 
struct vsnfsd_nullargs  {
	int dummy;
};
struct vsnfsd_nullres  {
	int dummy;
};
struct vsnfsd_readdirargs {
	struct vsnfs_fh fh;
	__u32 count;
	__be32 * buffer;
};
struct readdir_cd {
	__be32 err;		/* 0, nfserr, or nfserr_eof */
};

/*add any field through which you want to communicate 
between vfs_readdir and vsnfsd_filldir*/ 
    struct vsnfsd_readdirres {
	int count;		/* no. of valid bytes in the filled up buffer */
	 __be32 err;		/* 0, nfserr, or nfserr_eof */
	 __be32 * buffer;	/* keeps track of next location to be written */
	 int buflen;		/* space remaining in the buffer */
	 __be32 * offset;	/* stores offset of the dir entry location in the file */
};
struct vsnfsd_getrootargs  {
	char *path;
	 unsigned int len;
};
struct vsnfsd_lookupargs  {
	struct vsnfs_fh fh;
	char *filename;
	 unsigned int len;
};
int 
vsnfssvc_decode_nullargs(struct svc_rqst *rqstp, __be32 * p,
			 struct vsnfsd_nullargs *args);
int  vsnfssvc_encode_nullres(struct svc_rqst *rqstp, __be32 * p,
				struct vsnfsd_nullres *resp);
int  vsnfssvc_decode_readdirargs(struct svc_rqst *rqstp, __be32 * p,
				    struct vsnfsd_readdirargs *args);
int  vsnfssvc_encode_readdirres(struct svc_rqst *rqstp, __be32 * p,
				   struct vsnfsd_readdirres *resp);
int vsnfssvc_decode_getrootargs(struct svc_rqst *, __be32 *,
				  struct vsnfsd_getrootargs *);
int vsnfssvc_encode_fhandle(struct svc_rqst *, __be32 *, struct vsnfs_fh *);
int vsnfssvc_decode_lookupargs(struct svc_rqst *, __be32 *,
				 struct vsnfsd_lookupargs *);
int vsnfssvc_encode_entry(void *buf, const char *name, int namlen,
			    loff_t offset, u64 ino, unsigned int d_type);

/*
 * Storage requirements for XDR arguments and results.
 */ 
    union vsnfsd_xdrstore {
	struct vsnfsd_nullargs null;
	struct vsnfsd_getrootargs getroot;
	struct vsnfsd_lookupargs lookup;
	struct vsnfsd_readdirargs readdir;
	
	    /*put all the xdr args here */ 
	    /* 
	       struct nfsd_sattrargs        sattr;
	       struct nfsd_diropargs        dirop;
	       struct nfsd_readargs read;
	       struct nfsd_writeargs        write;
	       struct nfsd_createargs       create;
	       struct nfsd_renameargs       rename;
	       struct nfsd_linkargs link;
	       struct nfsd_symlinkargs      symlink;
	       struct nfsd_readdirargs      readdir;
	     */ 
};

#define VSNFS_SVC_XDRSIZE	sizeof(union vsnfsd_xdrstore)
    
#endif	/*  */

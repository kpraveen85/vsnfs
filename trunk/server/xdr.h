#ifndef XDR_H
#define XDR_H

int vsnfssvc_decode_void(struct svc_rqst *, __be32 *, void *);
int vsnfssvc_encode_void(struct svc_rqst *, __be32 *, void *);

struct vsnfsd_void { int dummy; };


/*
 * Storage requirements for XDR arguments and results.
 */
union vsnfsd_xdrstore {
	struct vsnfsd_void null; 
	/*put all the xdr args here*/
	/* 
	struct nfsd_sattrargs	sattr;
	struct nfsd_diropargs	dirop;
	struct nfsd_readargs	read;
	struct nfsd_writeargs	write;
	struct nfsd_createargs	create;
	struct nfsd_renameargs	rename;
	struct nfsd_linkargs	link;
	struct nfsd_symlinkargs	symlink;
	struct nfsd_readdirargs	readdir;
	*/
};

#define VSNFS_SVC_XDRSIZE	sizeof(union vsnfsd_xdrstore)

#endif

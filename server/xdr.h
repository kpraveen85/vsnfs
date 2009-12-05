#ifndef XDR_H
#define XDR_H


/*struct vsnfsd_void { int dummy; };*/

struct vsnfsd_nullargs
{
	int dummy;
};

struct vsnfsd_nullres
{
	int dummy;
};


struct vsnfsd_getrootargs
{
  char *path;
  unsigned int len;
};

struct vsnfsd_fhandle
{
  struct svc_fh fh;
};

int
vsnfssvc_decode_nullargs(struct svc_rqst *rqstp, __be32 *p, struct vsnfsd_nullargs *args);

int
vsnfssvc_encode_nullres(struct svc_rqst *rqstp, __be32 *p, struct vsnfsd_nullres *resp);


int vsnfssvc_decode_getrootargs(struct svc_rqst *, __be32 *,
				struct vsnfsd_getrootargs *);


int vsnfssvc_encode_fhandle(struct svc_rqst *, __be32 *,
			    struct vsnfsd_fhandle *);


/*
 * Storage requirements for XDR arguments and results.
 */
union vsnfsd_xdrstore {
	struct vsnfsd_nullargs null; 
    struct vsnfsd_getrootargs getroot;
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

#ifndef XDR_H
#define XDR_H

/*struct vsnfsd_void { int dummy; };*/ 

	


	


	
	


	
	
	


vsnfssvc_decode_nullargs(struct svc_rqst *rqstp, __be32 * p,
			 struct vsnfsd_nullargs *args);

				struct vsnfsd_nullres *resp);

				   


				 

/*
 * Storage requirements for XDR arguments and results.
 */ 
    union vsnfsd_xdrstore {
	
	
	
	
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
    
#endif	/* 
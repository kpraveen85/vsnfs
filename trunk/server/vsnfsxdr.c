#include <linux/types.h>
#include <linux/sunrpc/xdr.h>
#include <linux/sunrpc/svc.h>
#include "../vsnfs.h"
#include "vsnfsd.h"
#include "xdr.h"

/*
 * XDR decode functions
 * 0 on failure 
 * 1 on success
 */ 
int 
vsnfssvc_decode_nullargs(struct svc_rqst *rqstp, __be32 * p,
			 struct vsnfsd_nullargs *args) 
{
	
	


{
	
	
	
	    // p = p + (VSNFS_FHSIZE >> 2);
	    p = p + XDR_QUADLEN(VSNFS_FHSIZE);
	
	


decode_filename(__be32 * p, char **namp, unsigned int *lenp) 
{
	
	
	
	       xdr_decode_string_inplace(p, namp, lenp,
					 VSNFS_MAXNAMLEN)) != NULL) {
		
			
			    //  if (*name == '\0' || *name == '/')
			    if (*name == '\0')
				
		
	
	


decode_pathname(__be32 * p, char **namp, unsigned int *lenp) 
{
	
	
	
	       xdr_decode_string_inplace(p, namp, lenp,
					 VSNFS_MAXPATHLEN)) != NULL) {
		
			
				
		
	
	


vsnfssvc_decode_getrootargs(struct svc_rqst *rqstp, __be32 * p,
			    struct vsnfsd_getrootargs *args) 
{
	
		
	


vsnfssvc_decode_lookupargs(struct svc_rqst *rqstp, __be32 * p,
			   struct vsnfsd_lookupargs *args) 
{
	
	     ||!(p = decode_filename(p, &args->filename, &args->len)))
		
	
		      args->len);
	



/*
 * XDR encode functions
 * 0 on error
 * 1 on success
 */ 
int 
vsnfssvc_encode_nullres(struct svc_rqst *rqstp, __be32 * p,
			struct vsnfsd_nullres *resp) 
{
	
	


{
	
	
	    //     return p + (VSNFS_FHSIZE>> 2);
	    return p + XDR_QUADLEN(VSNFS_FHSIZE);


vsnfssvc_encode_fhandle(struct svc_rqst *rqstp, __be32 * p,
			
{
	
	
	



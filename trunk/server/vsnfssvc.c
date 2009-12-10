#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/freezer.h>
#include <linux/fs_struct.h>
#include <linux/sunrpc/types.h>
#include <linux/sunrpc/stats.h>
#include <linux/sunrpc/svc.h>
#include <linux/sunrpc/svcsock.h>
#include <linux/sunrpc/cache.h>
#include <linux/lockd/bind.h>

#include "../vsnfs.h"
#include "vsnfsd.h"
/*
#include <linux/unistd.h>
#include <linux/mman.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/atomic.h>
#include <asm/mman.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <asm/cacheflush.h>
*/ 
    
/* vsnfs daemon object*/ 
static struct svc_serv *vsnfsd_serv;

/* vsnfsd version objects */ 
static struct svc_version *vsnfsd_versions[] = { 
};


#define VSNFSD_NRVERS		ARRAY_SIZE(vsnfsd_versions)

};

{
	
	



/* vsnfs rpc program object */ 
struct svc_program vsnfsd_program = { 
	    .pg_nvers = VSNFSD_NRVERS, /* nr of entries in nfsd_version */ 
	    .pg_vers = vsnfsd_versions, /* version table */ 
	    .pg_name = "vsnfsd", /* program name */ 
	    .pg_class = "vsnfsd", /* authentication class */ 
	    .pg_stats = &vsnfsd_svcstats, 
	    &vsnfs_authentication,
	    /*&svc_set_client,      export authentication */ 
	
};


/*
 * This is the NFS server kernel thread
 */ 
static int 
{
	
	
	
	
	    /* At this point, the thread shares current->fs
	     * with the init process. We need to create files with a
	     * umask of 0 instead of init's umask. */ 
	    if (unshare_fs_struct() < 0) {
		
			     "error creating new fs_struct:out of mem\n");
		
	
	
	
	    /* the signal and flag settings are kept the same as nfsd */ 
	    /*
	     * thread is spawned with all signals set to SIG_IGN, re-enable
	     * the ones that will bring down the thread
	     */ 
	    allow_signal(SIGKILL);
	
	
	
	
	    /*
	     * We want less throttling in balance_dirty_pages() so that nfs to
	     * localhost doesn't cause nfsd to lock up due to all the client's
	     * dirty pages.
	     */ 
	    current->flags |= PF_LESS_THROTTLE;
	
	
	    /*
	     * The main request loop
	     */ 
	    for (;;) {
		
		    /*
		     * Find a socket with data available and call its
		     * recvfrom routine.
		     */ 
		    while ((err = svc_recv(rqstp, 60 * 60 * HZ)) == -EAGAIN)
			
		
			
		
		else if (err < 0) {
			
				
					     "from svc_recv (%d)\n", -err);
				
			
			
			
		
		
		    /* Locking -  exp_readlock() */ 
		    
#ifdef VSNFS_DEBUG		
		{
			
			
			
			
			
			
			
			
			
			
			
				     rpc_vers, prog, vers, proc);
			
			
		
#endif	/* 
		    
	
	
	    /* Clear signals before calling svc_exit_thread() */ 
	    flush_signals(current);
      
	    /* Release the thread */ 
	    svc_exit_thread(rqstp);
	
	



/* shutdown function of rpc svc */ 
static void vsnfsd_last_thread(struct svc_serv *serv) 
{
	
	
	
	
		
		
			     node->path);
		
		
		
	
	
	    lockd_down();
	
	    /* need not delete serv. It would be done by svc */ 
} 

/* creates the nfsd svc 
* NOTE: vsnfsd_serv->sv_nrthreads initialized to 1 
* (the server can be destroyed only it's reduced to 0
*/ 
int vsnfsd_create_serv(void) 
{
	
	
	    svc_create_pooled(&vsnfsd_program, VSNFSSVC_MAXBLKSIZE,
			      
	
		
		
	
	
	else
		
		    /*set_max_drc(); */ 
		    ;
	


{
	
	
		
	
	    svc_create_xprt(vsnfsd_serv, "udp", PF_INET, port,
			    
	
		
	
	
		
	
	    svc_create_xprt(vsnfsd_serv, "tcp", PF_INET, port,
			    
	
		
	
	
		
	



/* starts the vsnfsd service
* port - rpc port of the server
* nrservs - no. of threads
* returns # of server threads (on success) or error code (on failure)
*/ 
int 
{
	
	
	
	    /* TO DO: locking to be checked */ 
	    error = vsnfsd_create_serv();
	
		
		
	
	
	
	
		
	
	
	    /* threads are started */ 
	    /* NOTE: module is got for each thread so unless you kill all threads
	       module cannot be rmmod'ed */ 
	    error = svc_set_num_threads(vsnfsd_serv, NULL, nrservs);
	
		
		    /*We are holding a reference to nfsd_serv (bcoz of vsnfsd_create_serv)
		     * which we don't want to count in the return value, 
		     * so subtract 1
		     */ 
		    error = vsnfsd_serv->sv_nrthreads - 1;
      
					/* Release server */
      


{
	
	
	
	
	
	
	
	    /* Decode arguments */ 
	    xdr = proc->pc_decode;
	
	     && !xdr(rqstp, (__be32 *) rqstp->rq_arg.head[0].iov_base,
		     
		
		
		
	
	
	    /* Now call the procedure handler, and encode NFS status. */ 
	    nfserr = proc->pc_func(rqstp, rqstp->rq_argp, rqstp->rq_resp);
	
	    /* store the status */ 
	    nfserrp = rqstp->rq_res.head[0].iov_base 
	    +rqstp->rq_res.head[0].iov_len;
	
	
	
	    /* Encode result.
	     * additional info is never returned in case of an error.
	     */ 
	    if (!nfserr) {
		
		
			
			    /* Failed to encode result */ 
			    vsnfs_trace(KERN_DEFAULT,
					"failed to encode result!\n");
			
			
		
	
	



/*module init function*/ 
int init_vsnfsd(void) 
{
	
	
	
	    /*start the vsnfs daemon */ 
	    rc = vsnfsd_svc(VSNFS_PORT, VSNFS_NRSERVS);
	
	


{
	
	    /* should nfsd svc shutdown be called? */ 
	    vsnfs_trace(KERN_DEFAULT, "exiting \n");
	
	    /*
	       vsnfsd_last_thread(vsnfsd_serv);
	       return; */ 
} 



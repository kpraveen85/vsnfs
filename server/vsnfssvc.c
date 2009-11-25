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
static struct svc_serv 		*vsnfsd_serv;

/* vsnfsd version objects */
static struct svc_version *vsnfsd_versions[] = {
	[1] = &vsnfsd_version1,
};

#define VSNFSD_NRVERS		ARRAY_SIZE(vsnfsd_versions)

struct svc_stat		vsnfsd_svcstats = {
	.program	= &vsnfsd_program,
};

/* vsnfs rpc program object */
struct svc_program		vsnfsd_program = {
	.pg_prog		= VSNFS_PROGRAM,		/* program number */
	.pg_nvers		= VSNFSD_NRVERS,		/* nr of entries in nfsd_version */
	.pg_vers		= vsnfsd_versions,	/* version table */
	.pg_name		= "vsnfsd",		/* program name */
	.pg_class		= "vsnfsd",		/* authentication class */
	.pg_stats		= &vsnfsd_svcstats,
	.pg_authenticate	= &svc_set_client,	/* export authentication */

};

/*
 * This is the NFS server kernel thread
 */
static int
vsnfsd(void *vrqstp)
{
	struct svc_rqst *rqstp = (struct svc_rqst *) vrqstp;
	int err, preverr = 0;

	vsnfs_trace(KERN_DEFAULT, "Daemon started\n");
	/* At this point, the thread shares current->fs
	 * with the init process. We need to create files with a
	 * umask of 0 instead of init's umask. */
	if (unshare_fs_struct() < 0) {
		vsnfs_trace(KERN_DEFAULT, "error creating new fs_struct:out of mem\n");
		goto out;
	}

	current->fs->umask = 0;

	/* the signal and flag settings are kept the same as nfsd */
	/*
	 * thread is spawned with all signals set to SIG_IGN, re-enable
	 * the ones that will bring down the thread
	 */	 
	allow_signal(SIGKILL);
	allow_signal(SIGHUP);
	allow_signal(SIGINT);
	allow_signal(SIGQUIT);


	/*
	 * We want less throttling in balance_dirty_pages() so that nfs to
	 * localhost doesn't cause nfsd to lock up due to all the client's
	 * dirty pages.
	 */
	current->flags |= PF_LESS_THROTTLE;
	set_freezable();

	/*
	 * The main request loop
	 */
	for (;;) {
		/*
		 * Find a socket with data available and call its
		 * recvfrom routine.
		 */
		while ((err = svc_recv(rqstp, 60*60*HZ)) == -EAGAIN)
			;
		if (err == -EINTR)
			break;
		else if (err < 0) {
			if (err != preverr) {
				vsnfs_trace(KERN_WARNING, "unexpected error "
					"from svc_recv (%d)\n", -err);
				preverr = err;
			}
			schedule_timeout_uninterruptible(HZ);
			continue;
		}

		/* Locking -  exp_readlock() */
		svc_process(rqstp);

	}
	vsnfs_trace(KERN_DEFAULT, "Daemon ended\n");
	/* Clear signals before calling svc_exit_thread() */
	flush_signals(current);

out:
	/* Release the thread */
	svc_exit_thread(rqstp);

	module_put_and_exit(0);
	return 0;
}


/* shutdown function of rpc svc */
static void vsnfsd_last_thread(struct svc_serv *serv)
{
	struct svc_xprt *xprt;

	vsnfs_trace(KERN_DEFAULT, "\n");
	
	list_for_each_entry(xprt, &serv->sv_permsocks, xpt_list)
		lockd_down();
	/* need not delete serv. It would be done by svc */
}

/* creates the nfsd svc 
* NOTE: vsnfsd_serv->sv_nrthreads initialized to 1 
* (the server can be destroyed only it's reduced to 0
*/
int vsnfsd_create_serv(void)
{
	int err = 0;

	vsnfsd_serv = svc_create_pooled(&vsnfsd_program, VSNFSSVC_MAXBLKSIZE,
				      vsnfsd_last_thread, vsnfsd, THIS_MODULE);
	
	if (vsnfsd_serv == NULL) {
		vsnfs_trace(KERN_DEFAULT, "no mem\n");
		err = -ENOMEM;
		}
	else		
		/*set_max_drc();*/
		;

	return err;
}

static int vsnfsd_init_socks(int port)
{
	int error;
	if (!list_empty(&vsnfsd_serv->sv_permsocks))
		return 0;

	error = svc_create_xprt(vsnfsd_serv, "udp", PF_INET, port,
					SVC_SOCK_DEFAULTS);
	if (error < 0)
		return error;

	error = lockd_up();
	if (error < 0)
		return error;

	error = svc_create_xprt(vsnfsd_serv, "tcp", PF_INET, port,
					SVC_SOCK_DEFAULTS);
	if (error < 0)
		return error;

	error = lockd_up();
	if (error < 0)
		return error;

	return 0;
}

/* starts the vsnfsd service
* port - rpc port of the server
* nrservs - no. of threads
* returns # of server threads (on success) or error code (on failure)
*/
int
vsnfsd_svc(unsigned short port, int nrservs)
{
	int	error = 0;
	
	vsnfs_trace(KERN_DEFAULT, "\n");
	
	/* TO DO: locking to be checked */	
	error = vsnfsd_create_serv();

	if (error) {
		vsnfs_trace(KERN_DEFAULT, "nfsd create failure\n");
		goto out;
		}	
	vsnfs_trace(KERN_DEFAULT, "server created\n");
	
	error = vsnfsd_init_socks(port);
	if (error)
		goto failure;
	vsnfs_trace(KERN_DEFAULT, "sock init'ed\n");

	/* threads are started */
	/* NOTE: module is got for each thread so unless you kill all threads
	module cannot be rmmod'ed*/
	error = svc_set_num_threads(vsnfsd_serv, NULL, nrservs);

	if (error == 0)
		/*We are holding a reference to nfsd_serv (bcoz of vsnfsd_create_serv)
		* which we don't want to count in the return value, 
		* so subtract 1
		*/
		error = vsnfsd_serv->sv_nrthreads - 1;

 failure:
	svc_destroy(vsnfsd_serv);		/* Release server */
 out:
	return error;
}

int
vsnfsd_dispatch(struct svc_rqst *rqstp, __be32 *statp)
{
	struct svc_procedure	*proc;
#if 0
	kxdrproc_t		xdr;
	__be32			nfserr;
	__be32			*nfserrp;
#endif	


	vsnfs_trace(KERN_DEFAULT, "proc %d\n", rqstp->rq_proc);
	proc = rqstp->rq_procinfo;

#if 0

	/* Decode arguments */
	xdr = proc->pc_decode;
	if (xdr && !xdr(rqstp, (__be32*)rqstp->rq_arg.head[0].iov_base,
			rqstp->rq_argp)) {
		vsnfs_trace(KERN_DEFAULT, "failed to decode arguments!\n");
		*statp = rpc_garbage_args;
		return 1;
	}

	/* need to grab the location to store the status
	 */
	nfserrp = rqstp->rq_res.head[0].iov_base
		+ rqstp->rq_res.head[0].iov_len;
	rqstp->rq_res.head[0].iov_len += sizeof(__be32);


	/* Now call the procedure handler, and encode NFS status. */
	nfserr = proc->pc_func(rqstp, rqstp->rq_argp, rqstp->rq_resp);


	if (rqstp->rq_proc != 0)
		*nfserrp++ = nfserr;

	/* Encode result.
	 * additional info is never returned in case of an error.
	 */
	if (!nfserr) {
		xdr = proc->pc_encode;
		if (xdr && !xdr(rqstp, nfserrp,
				rqstp->rq_resp)) {
			/* Failed to encode result*/
			vsnfs_trace(KERN_DEFAULT, "failed to encode result!\n");
			*statp = rpc_system_err;
			return 1;
		}
	}
#endif	

	return 1;

}

/*module init function*/
int init_vsnfsd(void)
{
	int rc = 0;
	vsnfs_trace(KERN_DEFAULT, "\n");
	/*start the vsnfs daemon*/
	rc = vsnfsd_svc(VSNFS_PORT, VSNFS_NRSERVS);
	vsnfs_trace(KERN_DEFAULT, "%d threads created\n", rc);
	return 0;
}

void exit_vsnfsd(void)
{
	/* should nfsd svc shutdown be called? */
	vsnfs_trace(KERN_DEFAULT, "\n");
	/*
	vsnfsd_last_thread(vsnfsd_serv);
	return;*/
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RP");
module_init(init_vsnfsd)
module_exit(exit_vsnfsd)


/*
 * CSE506 - VSNFS
 *
 * vsnfsMount.c --
 * 
 * This file contains to manipulate MountPointEntry. This
 * includes the ability to mount and umount systems.
 *
 * (C) Karthik Balaji <findkb@gmail.com>
 * (C) Praveen Krishnamoorthy <kpraveen85@gmail.com>
 * (C) Prabakar Radhakrishnan <prabakarcse@gmail.com>
 */

#include <linux/types.h>
#include <linux/parser.h>
#include <linux/sunrpc/xprtsock.h>
#include <linux/inet.h>
#include "vsnfs.h"
#include "vsnfsMount.h"

enum { vsnfs_opt_port, vsnfs_opt_timeout, vsnfs_opt_err };

static const match_table_t tokens = {
	{vsnfs_opt_port, "port=%u"},
	{vsnfs_opt_timeout, "timeout=%u"},
       	{vsnfs_opt_err, NULL}
};

static int vsnfs_inet_pton(const int address_family,const char *cp, void *dst)
{
        int ret = 0;
        printk(KERN_ERR "in vsnfs_inet_pton\n");
	
        /* calculate length by finding first slash or NULL */
        if (address_family == AF_INET)
                ret = in4_pton(cp, -1 /* len */, dst, '\\', NULL);
        else if (address_family == AF_INET6)
                ret = in6_pton(cp, -1 /* len */, dst , '\\', NULL);

       if (ret > 0)
                ret = 1;
       return ret;
}

int vsnfs_parse_mntpath(const char *dev_name, struct vsnfs_server *server)
{
  size_t len;
  char *colon;
  printk(KERN_ERR "in vsnfs_parse_mntpath\n");
	
  colon = strchr(dev_name, ':');
  if(colon==NULL)
    return -VSNFSERR_INVAL;
 
  len=colon-dev_name;
  if(len > VSNFS_MAXNAMLEN)
    return -VSNFSERR_NAMETOOLONG;

  strncpy(server->ip_addr,dev_name,len);
  server->ip_addr[len]='\0';

  printk(KERN_ERR "Ip address = %s\n",server->ip_addr);

 
  colon++;
  len=strlen(colon);
  if(len>VSNFS_MAXPATHLEN)
    return -VSNFSERR_NAMETOOLONG;
  
  server->mnt_path=kstrndup(colon,len,GFP_KERNEL);
  if(!server->mnt_path)
    return -ENOMEM;

  printk(KERN_ERR "directory = %s\n",server->mnt_path);
  

  return 0;
} 


/* Read and validate the mount data */
int vsnfs_parse_mount_options(char *raw_data, const char *dev_name,
								struct vsnfs_server *server)
{
	char *p;
	char *port_src;
	char *timeout;
	int rc = 0;
	int token;
	substring_t args[MAX_OPT_ARGS];
	printk(KERN_ERR "in parse options\n");
	

	// server->protocol = XPRT_TRANSPORT_TCP;
        server->server_port = VSNFS_PORT;
        server->timeout = VSNFS_TIMEOUT;  
        rc = vsnfs_parse_mntpath(dev_name, server); 
        if(rc != 0) 
	  goto out_parse;
	
        server->cl_addr.sin_family=AF_INET;
        server->cl_addr.sin_port =htons(VSNFS_PORT);
        rc=vsnfs_inet_pton(AF_INET,server->ip_addr,&server->cl_addr.sin_addr);
        if(rc<0)
	  goto out_parse;


	while ((p = strsep(&raw_data, ",")) != NULL) {
		if (!*p)
			continue;
		token = match_token(p, tokens, args);
		switch (token) {
		case vsnfs_opt_port:
			port_src = args[0].from;
			server->server_port = (int)simple_strtol(port_src, &port_src, 0);
	                server->cl_addr.sin_port =htons(server->server_port);
       	              	printk(KERN_ERR "Received port from US %d\n", server->server_port);
			break;
		case vsnfs_opt_timeout:
			timeout = args[0].from;
			server->timeout = (int)simple_strtol(timeout, &timeout, 0);
	                printk(KERN_ERR "Received timeout from US %d\n", server->timeout);
			break;
	
		case vsnfs_opt_err:
		default:
		  printk(KERN_ERR
					"%s: vsnfs: unrecognized option [%s]\n",
					__func__, p);
		}
	}
out_parse:
	return rc;
}

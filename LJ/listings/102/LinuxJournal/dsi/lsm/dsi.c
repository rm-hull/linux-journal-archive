/*
 * Distributed Security Module (DSM)
 *
 * dsi.c
 *
 * This file contains the DSM hooks to the kernel.
 * The set of function was defined by the LSM interface.
 *
 * Copyright (C) 2002 Ericsson, Inc <miroslaw.zakrzewski@lmc.ericsson.se>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 */


#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/security.h>
#include <linux/netfilter.h>
#include <linux/netlink.h>
#include <linux/mm.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/tcp.h>


#include <linux/elf.h>

#include "dsi.h"

#include "dsi_ip_options.h"

int     dsi_node_id = 0;
int	dsi_alarm = 0;

static int dsi_sethostname (char *hostname)
{
	return 0;
}

static int dsi_setdomainname (char *domainname)
{
	return 0;
}

static int dsi_reboot (unsigned int cmd)
{
	return 0;
}

static int dsi_ioperm (unsigned long from, unsigned long num, int turn_on)
{
	return 0;
}

static int dsi_iopl (unsigned int old, unsigned int level)
{
	return 0;
}

static int dsi_ptrace (struct task_struct *parent, struct task_struct *child)
{
	return 0;
}

static int dsi_capget (struct task_struct *target, kernel_cap_t * effective,
		       kernel_cap_t * inheritable, kernel_cap_t * permitted)
{
	return 0;
}

static int dsi_capset_check (struct task_struct *target,
			     kernel_cap_t * effective,
			     kernel_cap_t * inheritable,
			     kernel_cap_t * permitted)
{
	return 0;
}

static void dsi_capset_set (struct task_struct *target,
			    kernel_cap_t * effective,
			    kernel_cap_t * inheritable,
			    kernel_cap_t * permitted)
{
	return;
}

static int dsi_acct (struct file *file)
{
	return 0;
}

static int dsi_capable (struct task_struct *tsk, int cap)
{
	if (cap_is_fs_cap (cap) ? tsk->fsuid == 0 : tsk->euid == 0)
		/* capability granted */
		return 0;

	/* capability denied */
	return -EPERM;
}

static int dsi_sysctl (ctl_table * table, int op)
{
	return 0;
}

static int dsi_sys_security (unsigned int id, unsigned int call,
			     unsigned long *args)
{
	struct task_struct *p;
	unsigned int task_pid;
	int task_sid;
	task_security_t *tsec;
	int ssid;//,sid;
	int tsid;
	int tclass;
	void *cache_node;
	int permission;
	int rc;

	DSI_PRINT("DSI sys_security : id=%x,call=%x,args=%x\n",id,call,(int)args);

	if(id != DSI_MAGIC)
		return -ENOSYS;

	rc = 0;

	switch(call)
	{
	case DSI_SET_NODE_ID:
		dsi_node_id = args[0];
		break;
	case DSI_SET_TASK_SECURITY:
		printk("set security\n");
		task_pid = args[0];
		task_sid = args[1];
		for_each_task(p)
			{
				if(p->pid == task_pid){
					printk("task found pid = %x\n",task_pid);
					tsec = (task_security_t *)(p->security);
					if(tsec) {
						tsec->sid = task_sid;
						printk("sid = %x\n",tsec->sid);
					}

					break;
				}
			}
		break;
	case DSI_SET_POLICY:
		ssid = args[0];
		tsid = args[1];
		tclass = args[2];
		permission = args[3];
		
		cache_node = dsi_cache_update(ssid,tsid,tclass,permission);
		if(!cache_node) {
			printk("Can't update cache\n");
			rc = -ENOSYS;
		}
		break;
	case DSI_ALARM_CHECK:
		rc = dsi_alarm;
		dsi_alarm = 0;
		break;
	default:
		rc = -ENOSYS;
		break;
	}

	return rc;
}

static int dsi_swapon (struct swap_info_struct *swap)
{
	return 0;
}

static int dsi_swapoff (struct swap_info_struct *swap)
{
	return 0;
}

static int dsi_nfsservctl (int cmd, struct nfsctl_arg *arg)
{
	return 0;
}

static int dsi_quotactl (int cmds, int type, int id, struct super_block *sb)
{
	return 0;
}

static int dsi_quota_on (struct file *f)
{
	return 0;
}

static int dsi_bdflush (int func, long data)
{
	return 0;
}

static int dsi_syslog (int type)
{
	return 0;
}

static int dsi_netlink_send (struct sk_buff *skb)
{
	if (current->euid == 0)
		cap_raise (NETLINK_CB (skb).eff_cap, CAP_NET_ADMIN);
	else
		NETLINK_CB (skb).eff_cap = 0;
	return 0;
}

static int dsi_netlink_recv (struct sk_buff *skb)
{
	if (!cap_raised (NETLINK_CB (skb).eff_cap, CAP_NET_ADMIN))
		return -EPERM;
	return 0;
}

static int dsi_binprm_alloc_security (struct linux_binprm *bprm)
{
        task_security_t *tsec;
        char elf_hdr[EI_NIDENT] = {0};

        kernel_read(bprm->file,0,elf_hdr,EI_NIDENT);

	/* First of all, some simple consistency checks */
      	if (memcmp(elf_hdr, ELFMAG, SELFMAG) == 0) {
		/* Validate that we have a SID */
	        if (elf_hdr[DSI_ELF_SID_POS]) {
	                tsec = (task_security_t *)current->security;
		        if (tsec) {
			        tsec->sid = elf_hdr[DSI_ELF_SID_POS];
				DSI_PRINT("Found a sid : tsec->sid %d\n",tsec->sid);
			}
	        }
        }
	
	return 0;
}

static void dsi_binprm_free_security (struct linux_binprm *bprm)
{
	return;
}

static void dsi_binprm_compute_creds (struct linux_binprm *bprm)
{
	return;
}

static int dsi_binprm_set_security (struct linux_binprm *bprm)
{
	return 0;
}

static int dsi_sb_alloc_security (struct super_block *sb)
{
	return 0;
}

static void dsi_sb_free_security (struct super_block *sb)
{
	return;
}

static int dsi_sb_statfs (struct super_block *sb)
{
	return 0;
}

static int dsi_mount (char *dev_name, struct nameidata *nd, char *type,
		      unsigned long flags, void *data)
{
	return 0;
}

static int dsi_check_sb (struct vfsmount *mnt, struct nameidata *nd)
{
	return 0;
}

static int dsi_umount (struct vfsmount *mnt, int flags)
{
	return 0;
}

static void dsi_umount_close (struct vfsmount *mnt)
{
	return;
}

static void dsi_umount_busy (struct vfsmount *mnt)
{
	return;
}

static void dsi_post_remount (struct vfsmount *mnt, unsigned long flags,
			      void *data)
{
	return;
}


static void dsi_post_mountroot (void)
{
	return;
}

static void dsi_post_addmount (struct vfsmount *mnt, struct nameidata *nd)
{
	return;
}

static int dsi_inode_alloc_security (struct inode *inode)
{
        internal_inode_alloc_security(inode);

	return 0;
}

static void dsi_inode_free_security (struct inode *inode)
{
	internal_inode_free_security(inode);

	return;
}

static int dsi_inode_create (struct inode *inode, struct dentry *dentry,
			     int mask)
{
	return 0;
}

static void dsi_inode_post_create (struct inode *inode, struct dentry *dentry,
				   int mask)
{
	return;
}

static int dsi_inode_link (struct dentry *old_dentry, struct inode *inode,
			   struct dentry *new_dentry)
{
	return 0;
}

static void dsi_inode_post_link (struct dentry *old_dentry,
				 struct inode *inode,
				 struct dentry *new_dentry)
{
	return;
}

static int dsi_inode_unlink (struct inode *inode, struct dentry *dentry)
{
	return 0;
}

static int dsi_inode_symlink (struct inode *inode, struct dentry *dentry,
			      const char *name)
{
	return 0;
}

static void dsi_inode_post_symlink (struct inode *inode,
				    struct dentry *dentry, const char *name)
{
	return;
}

static int dsi_inode_mkdir (struct inode *inode, struct dentry *dentry,
			    int mask)
{
	return 0;
}

static void dsi_inode_post_mkdir (struct inode *inode, struct dentry *dentry,
				  int mask)
{
	return;
}

static int dsi_inode_rmdir (struct inode *inode, struct dentry *dentry)
{
	return 0;
}

static int dsi_inode_mknod (struct inode *inode, struct dentry *dentry,
			    int major, dev_t minor)
{
	return 0;
}

static void dsi_inode_post_mknod (struct inode *inode, struct dentry *dentry,
				  int major, dev_t minor)
{
	return;
}

static int dsi_inode_rename (struct inode *old_inode,
			     struct dentry *old_dentry,
			     struct inode *new_inode,
			     struct dentry *new_dentry)
{
	return 0;
}

static void dsi_inode_post_rename (struct inode *old_inode,
				   struct dentry *old_dentry,
				   struct inode *new_inode,
				   struct dentry *new_dentry)
{
	return;
}

static int dsi_inode_readlink (struct dentry *dentry)
{
	return 0;
}

static int dsi_inode_follow_link (struct dentry *dentry,
				  struct nameidata *nameidata)
{
	return 0;
}

static int dsi_inode_permission (struct inode *inode, int mask)
{
	/*DSI_PRINT("dsi_node_permission inode=%x,mask=%x\n",(int)inode,mask);*/
	return 0;
}

static int dsi_inode_revalidate (struct dentry *inode)
{
	return 0;
}

static int dsi_inode_setattr (struct dentry *dentry, struct iattr *iattr)
{
	return 0;
}

static int dsi_inode_stat (struct inode *inode)
{
	return 0;
}

static void dsi_post_lookup (struct inode *ino, struct dentry *d)
{
	return;
}

static void dsi_delete (struct inode *ino)
{
	return;
}

static int dsi_file_permission (struct file *file, int mask)
{
	return 0;
}

static int dsi_file_alloc_security (struct file *file)
{
	return 0;
}

static void dsi_file_free_security (struct file *file)
{
	return;
}

static int dsi_file_llseek (struct file *file)
{
	return 0;
}

static int dsi_file_ioctl (struct file *file, unsigned int command,
			   unsigned long arg)
{
	return 0;
}

static int dsi_file_mmap (struct file *file, unsigned long prot,
			  unsigned long flags)
{
	return 0;
}

static int dsi_file_mprotect (struct vm_area_struct *vma, unsigned long prot)
{
	return 0;
}

static int dsi_file_lock (struct file *file, unsigned int cmd)
{
	return 0;
}

static int dsi_file_fcntl (struct file *file, unsigned int cmd,
			   unsigned long arg)
{
	return 0;
}

static int dsi_file_set_fowner (struct file *file)
{
	return 0;
}

static int dsi_file_send_sigiotask (struct task_struct *tsk,
				    struct fown_struct *fown, int fd,
				    int reason)
{
	return 0;
}

static int dsi_file_receive (struct file *file)
{
	return 0;
}

static int dsi_task_create (unsigned long clone_flags)
{
  	DSI_PRINT("dsi_task_create flags=%x\n",clone_flags);
	return(dsi_check_task(current, current, PROCESS_FORK));
	
}


static int dsi_task_alloc_security (struct task_struct *p)
{
	int rc;
	task_security_t *tsec1, *tsec2;

	DSI_PRINT("dsi_task_alloc_security : task=%x\n",(int)p);

	rc = internal_task_alloc_security(p);
	if(rc) {
		printk("dsi_tak_alloc_security - PROBLEM\n");
		return(rc);
	}

	tsec1 = current->security;
	tsec2 = p->security;

	if((tsec1 == NULL) || (tsec2 == NULL)) {
		printk("dsi_task_alloc_security - NO SECURITY STRUCTURE\n");
		return (-1);
	}
	
	tsec2->osid = tsec1->osid;
	tsec2->sid = tsec1->sid;

	return 0;
}

static void dsi_task_free_security (struct task_struct *p)
{

	DSI_PRINT("dsi_task_free_security : task=%x\n",(int)p);

	internal_task_free_security(p);

	return;
}

static int dsi_task_setuid (uid_t id0, uid_t id1, uid_t id2, int flags)
{
	return 0;
}

static int dsi_task_post_setuid (uid_t id0, uid_t id1, uid_t id2, int flags)
{
	return 0;
}

static int dsi_task_setgid (gid_t id0, gid_t id1, gid_t id2, int flags)
{
	return 0;
}

static int dsi_task_setpgid (struct task_struct *p, pid_t pgid)
{
	return 0;
}

static int dsi_task_getpgid (struct task_struct *p)
{
	return 0;
}

static int dsi_task_getsid (struct task_struct *p)
{
	return 0;
}

static int dsi_task_setgroups (int gidsetsize, gid_t * grouplist)
{
	return 0;
}

static int dsi_task_setnice (struct task_struct *p, int nice)
{
	return 0;
}

static int dsi_task_setrlimit (unsigned int resource, struct rlimit *new_rlim)
{
	return 0;
}

static int dsi_task_setscheduler (struct task_struct *p, int policy,
				  struct sched_param *lp)
{
	return 0;
}

static int dsi_task_getscheduler (struct task_struct *p)
{
	return 0;
}

static int dsi_task_wait (struct task_struct *p)
{
	return 0;
}

static int dsi_task_kill (struct task_struct *p, struct siginfo *info,
			  int sig)
{
	return 0;
}

static int dsi_task_prctl (int option, unsigned long arg2, unsigned long arg3,
			   unsigned long arg4, unsigned long arg5)
{
	return 0;
}

static void dsi_task_kmod_set_label (void)
{
	return;
}


int dsi_ip_options_get(struct ip_options **optp, unsigned char *data, int optlen)
{
	struct ip_options *opt=NULL;

	opt = kmalloc(sizeof(struct ip_options)+((optlen+3)&~3), DSI_SAFE_ALLOC);
	
	if (!opt)
		return -ENOMEM;

	memset(opt, 0, sizeof(struct ip_options));
	if (optlen) {
		memcpy(opt->__data, data, optlen);
	}
	while (optlen & 3)
		opt->__data[optlen++] = IPOPT_END;

	opt->optlen = optlen;
	opt->is_data = 1;
	opt->is_setbyuser = 1;	
	*optp = opt;
	return 0;
}

/*
 * If not a bypass label, always include serial and ssid.  The msid and
 * dsid tags are only included if they have been explicitly specified via
 * the extended socket API.
 */

static inline __u8 dsi_calculate_optlen()
{

	return (NSID_BASE_LEN + NSID_SSID_LEN + NSID_NODEID_LEN);

}

/*
 * Fill optptr buffer with IP options.  Note that optlen is not 32-bit
 * aligned here.
 */

static inline void dsi_options_fill(inode_security_t *isec, unsigned char *optptr, __u8 optlen)
{
	__u8 i;
	
	optptr[0] = IPOPT_CIPSO;
	optptr[1] = optlen;
	
	*(__u32 *)&optptr[2] = __constant_htonl(NSID_DOI);
	
	i = NSID_BASE_LEN;
	
	optptr[i] = NSID_SSID;
	optptr[i+1] = NSID_SSID_LEN;

	if ( isec == NULL ) {
		*(__u32 *)&optptr[i+2] = htonl(DEFAULT_SID);
		/*DSI_ERROR("No SID associated with sk_buff, assigning default SID\n");*/
	}
	else *(__u32 *)&optptr[i+2] = htonl(isec->sid);

	i += NSID_SSID_LEN;
	
	optptr[i] = NSID_NODEID;
	optptr[i+1] = NSID_NODEID_LEN;
	*(__u32 *)&optptr[i+2] = htonl(dsi_node_id);
	i += NSID_NODEID_LEN;
	
	/* out_tag: */
	optptr[6] = NSID_FREEFORM;
	optptr[7] = i - NSID_BASE_LEN + 2;
	return;    
}

static inline int dsi_parse_u32_param(  __u8 paramtype, __u8 optlen, int i,
					const char *optptr,
					unsigned char **pp_ptr, __u32 *param)
{
	__u8 paramlen;

	if ( (paramtype != NSID_SSID) &&
	     (paramtype != NSID_NODEID) ) {
		printk("Wrong paramtype\n");	
		*pp_ptr = (char *)(optptr + i);
		return -EINVAL;
	}
	
	paramlen = optptr[i + 1];
	if (paramlen != NSID_U32_LEN || paramlen > optlen - i) {
		printk("Wrong paramlen\n");
		*pp_ptr = (char*)(optptr + i + 1);
		return -EINVAL;
	}

	*param = ntohl(*(__u32 *)(optptr + i + 2));
	return paramlen;
}


/*
 * Decode IP options on incoming packets into labels.
 *
 * Note: we're guaranteed to have a minimum of 2 bytes at optptr, with
 * optptr[1] being a valid length of data to access.
 */
static inline int dsi_ip_decode_options(struct sk_buff *skb,
					const char *optptr,
					unsigned char **pp_ptr)
{
	__u8 opttype = optptr[0];
	__u8 optlen = optptr[1];
	__u8 i, tagtype, taglen;
	__u32 doi;
	struct iphdr *iph;
	sk_buff_security_t *ssec;
	__u32 rsid = 0;         /* Remote SID */
	__u32 nodeid = 0;       /* Remote node ID */


	if (!skb) {	
		printk("dsi_ip_deocde_options No skb\n");
		return 0;
	}

	iph = skb->nh.iph;
	ssec = skb->lsm_security;

	if(!ssec) {
		printk("dsi_ip_decode_options security bufffer is NULL\n");
	}
	
	/* This is the only type of security option which we accept */
	if (opttype != IPOPT_CIPSO) {
		printk("Not CIPSO option\n");
		*pp_ptr = (char*)optptr;
		return -EPERM;
	}

	if (optlen < NSID_MIN_LEN) {
		printk("Bad length\n");
		*pp_ptr = (char*)optptr;
		return -EINVAL;
	}

	doi = *(__u32 *)&optptr[2];
	if (doi != __constant_htonl(NSID_DOI)) {
		printk("Bad DOI\n");
		*pp_ptr = (char*)(optptr + 3);
		return -EINVAL;
	}

	tagtype = optptr[6];
	if (tagtype != NSID_FREEFORM) {
		printk("Not freeform tag\n");
		*pp_ptr = (char*)(optptr + 7);
		return -EINVAL;
	}

	taglen = optptr[7];
	if (taglen != (optlen - 6)) {
		printk("Bad internal freeform length\n");
		*pp_ptr = (char*)(optptr + 8);
		return -EINVAL;
	}

	for (i = NSID_BASE_LEN; i < optlen; ) {
		__u8 paramtype = optptr[i];
		int rc = 0;
		
		switch (paramtype) {
		case NSID_SSID:
		        rc = dsi_parse_u32_param(paramtype, optlen,
						 i, optptr, pp_ptr,
						 &rsid);
			break;

		case NSID_NODEID:
		        rc = dsi_parse_u32_param(paramtype, optlen,
						 i, optptr, pp_ptr,
						 &nodeid);
			break;

		default:
			*pp_ptr = (char *)(optptr + i + 1);
			return -EINVAL;
		}
		
		if (rc < 0) {
			printk("dsi : Bad options\n");
			return rc;
		}
		i += rc;
	}

	if (i != optlen) {
		printk("Invalid option size\n");
		*pp_ptr = (char*)(optptr + 2);
		return -EINVAL;
	}

	if (!skb && !capable(CAP_NET_RAW)) {
		printk("Invalid capabilities\n");
		*pp_ptr = (char*)optptr;
		return -EPERM;
	}

        /* DSI: Now perform SID and Node ID mapping to a single NSID (Network SID) */
	if(ssec)
		ssec->sid = rsid + 16 * nodeid;         /* This is a bogus function for now */
	else
		printk("lsm_security is NULL\n");		

	return 0;
}


/* 
 * This is critical for TCP to operate with our CIPSO options.  The effective
 * MSS is the maximum size of segments which are transmitted, and it must
 * take the size of the options into account
 */


static inline void dsi_adjust_effective_mss(struct sock *sk)
{
	struct tcp_opt *tp = &sk->tp_pinfo.af_tcp;

	if (tp->ext_header_len < NSID_ALIGN(NSID_MAX_TCP_LEN)) {
		tp->ext_header_len = NSID_ALIGN(NSID_MAX_TCP_LEN);
		tcp_sync_mss(sk, tp->pmtu_cookie);
	}
	return;
}


static unsigned int dsi_ip_preroute_first (unsigned int hooknum,
					   struct sk_buff **pskb,
					   const struct net_device *in,
					   const struct net_device *out,
					   int (*okfn) (struct sk_buff *))
{
	return NF_ACCEPT;
}

static unsigned int dsi_ip_preroute_last (unsigned int hooknum,
					  struct sk_buff **pskb,
					  const struct net_device *in,
					  const struct net_device *out,
					  int (*okfn) (struct sk_buff *))
{
	return NF_ACCEPT;
}

static unsigned int dsi_ip_input_first (unsigned int hooknum,
					struct sk_buff **pskb,
					const struct net_device *in,
					const struct net_device *out,
					int (*okfn) (struct sk_buff *))
{
	return NF_ACCEPT;
}

static unsigned int dsi_ip_input_last (unsigned int hooknum,
				       struct sk_buff **pskb,
				       const struct net_device *in,
				       const struct net_device *out,
				       int (*okfn) (struct sk_buff *))
{
	return NF_ACCEPT;
}

static unsigned int dsi_ip_forward_first (unsigned int hooknum,
					  struct sk_buff **pskb,
					  const struct net_device *in,
					  const struct net_device *out,
					  int (*okfn) (struct sk_buff *))
{
	return NF_ACCEPT;
}

static unsigned int dsi_ip_forward_last (unsigned int hooknum,
					 struct sk_buff **pskb,
					 const struct net_device *in,
					 const struct net_device *out,
					 int (*okfn) (struct sk_buff *))
{
	return NF_ACCEPT;
}

static unsigned int dsi_ip_output_first (unsigned int hooknum,
					 struct sk_buff **pskb,
					 const struct net_device *in,
					 const struct net_device *out,
					 int (*okfn) (struct sk_buff *))
{
	return NF_ACCEPT;
}

static unsigned int dsi_ip_output_last (unsigned int hooknum,
					struct sk_buff **pskb,
					const struct net_device *in,
					const struct net_device *out,
					int (*okfn) (struct sk_buff *))
{
	return NF_ACCEPT;
}

static unsigned int dsi_ip_postroute_first (unsigned int hooknum,
					    struct sk_buff **pskb,
					    const struct net_device *in,
					    const struct net_device *out,
					    int (*okfn) (struct sk_buff *))
{
	return NF_ACCEPT;
}

static unsigned int dsi_ip_postroute_last (unsigned int hooknum,
					   struct sk_buff **pskb,
					   const struct net_device *in,
					   const struct net_device *out,
					   int (*okfn) (struct sk_buff *))
{
	return NF_ACCEPT;
}

static void dsi_ip_fragment (struct sk_buff *newskb,
			     const struct sk_buff *oldskb)
{
	return;
}

static int dsi_ip_defragment (struct sk_buff *skb)
{
        return NF_ACCEPT;
}

static void dsi_ip_decapsulate (struct sk_buff *skb)
{
	return;
}

static void dsi_ip_encapsulate (struct sk_buff *skb)
{
	return;
}

static void dsi_netdev_unregister (struct net_device *dev)
{
	return;
}

static int dsi_socket_create (int family, int type, int protocol)
{
	task_security_t *tsec;
	int rc=0;

	tsec = current->security;

	if(tsec)
		rc = dsi_check_permission(tsec->sid, tsec->sid, DSI_CLASS_SOCKET, SOCKET_CREATE);
	else
	        rc = 0;/*-ENOSYS;*/
	
	if(rc) {
		printk("Can't create a socket\n");
		dsi_alarm |= ALARM_SOCKET_CREATE;
	}

	return(rc);
}

static void dsi_socket_post_create (struct socket *sock, int family, int type,
				    int protocol) 
{

	inode_security_t *isec = NULL;
	task_security_t *tsec = NULL;

	/* set the security id to inode related to socket */
	DSI_PRINT("dsi_socket_post_create(%x) inode = %x\n",(int)sock,(int)sock->inode);
	if(sock->inode)
		isec = sock->inode->i_security;

	if(!isec){
		internal_inode_alloc_security(sock->inode);
		isec = sock->inode->i_security;
	}


	if(isec) {
		isec->sid = DSI_SID_NORMAL;
		tsec = current->security;
		if(tsec) {
			isec->task_sid = tsec->sid;
			isec->sid = tsec->sid;
		}
		isec->class = DSI_CLASS_SOCKET;
		isec->magic = DSI_MAGIC;
	}

	return;
}

static int dsi_socket_bind (struct socket *sock, struct sockaddr *address,
			    int addrlen)
{
	return 0;
}

static int dsi_socket_connect (struct socket *sock, struct sockaddr *address,
			       int addrlen)
{

	task_security_t *tsec;
	inode_security_t *isec;
	int rc=0;

	tsec = current->security;
	if (sock->inode) {
                isec = sock->inode->i_security;
		if(isec->class == 0) {
			isec = 0;
		}
	}
	else
		isec = 0;


	
	if (tsec && isec)
	{
		rc = dsi_check_permission(tsec->sid, isec->sid, isec->class, SOCKET_CONNECT);
	} else {
		rc = 0; /*-ENOSYS*/;
	}
	
	if (rc) {
		dsi_alarm = ALARM_SOCKET_CONNENT;
		DSI_ERROR("dsi_socket_connect - No Permission\n");

	}

	return rc;
}

static int dsi_socket_listen (struct socket *sock, int backlog)
{
	return 0;
}

static int dsi_socket_accept (struct socket *sock, struct socket *newsock)
{


	inode_security_t *isec = NULL;
	inode_security_t *isecnew = NULL;

	/* set the security id to inode related to socket */
	DSI_PRINT("dsi_socket_post_create\n");
	if(newsock->inode)
		isecnew = newsock->inode->i_security;

	if(!isecnew){
		internal_inode_alloc_security(newsock->inode);
		isecnew = newsock->inode->i_security;
	}


	if(isecnew) {
		isecnew->sid = DSI_SID_NORMAL;
		isec = sock->inode->i_security;
		if(isec) {
			isecnew->task_sid = isec->task_sid;
			isecnew->sid = isec->sid;
		}
		isecnew->class = DSI_CLASS_SOCKET;
		isecnew->magic = DSI_MAGIC;
	}

	
	return 0;
}

static int dsi_socket_sendmsg (struct socket *sock, struct msghdr *msg,
			       int size)
{
	task_security_t *tsec;
	inode_security_t *isec;
	int rc=0;
	__u8 optlen;
	struct sock *sk;
	struct ip_options * opt = NULL;
	unsigned char optptr[optlen = dsi_calculate_optlen()];
    
        tsec = current->security;
        if(sock->inode){
                isec = sock->inode->i_security;
		if(isec && ( (isec->magic != DSI_MAGIC) || (isec->class == 0) )) {
			isec = 0;
		}	
	}
        else
                isec = 0;

        if (tsec && isec)
        {
		rc = dsi_check_permission(tsec->sid, isec->sid, isec->class, SOCKET_SEND);
        } else {
		rc = 0;//-ENOSYS;
        }

        if (rc) {
                dsi_alarm = ALARM_SOCKET_SEND;
                DSI_ERROR("dsi_socket_sendmsg(%x) - No Permission\n",(int)sock);
		if (isec)
			printk("ssid=%d,tsid=%d,class=%d,magic=%x\n",tsec->sid,isec->sid,
			       isec->class,(int)isec->magic);	
        }
	else {
		/* No error so add the security information to the outgoing packet*/

		sk = sock->sk; 

		/* add option only for the ip (AF_INET) family and none local message */

		if ((sk->family == AF_INET)  && (RTN_LOCAL != inet_addr_type(sk->daddr)) ) {
			opt = sk->protinfo.af_inet.opt;
			if (!opt || (*opt->__data != IPOPT_CIPSO) ) {
				opt = NULL;
				dsi_options_fill(isec, optptr, optlen);
				dsi_ip_options_get(&opt,optptr,optlen);
				opt = xchg(&sk->protinfo.af_inet.opt, opt);
				if (opt)
				{
					kfree(opt);
				}
			}
		}
	}

        return rc;
}

static int dsi_socket_recvmsg (struct socket *sock, struct msghdr *msg,
			       int size, int flags)
{
	return 0;
}

static int dsi_socket_getsockname (struct socket *sock)
{
	return 0;
}

static int dsi_socket_getpeername (struct socket *sock)
{
	return 0;
}

static int dsi_socket_setsockopt (struct socket *sock, int level, int optname)
{
	return 0;
}

static int dsi_socket_getsockopt (struct socket *sock, int level, int optname)
{
	return 0;
}

static int dsi_socket_shutdown (struct socket *sock, int how)
{
	return 0;
}

static int dsi_sock_rcv_skb (struct sock *sk, struct sk_buff *skb)
{
	inode_security_t *isec;
	sk_buff_security_t *ssec;
	int sid,nid;
	int rc;

	if((sk == NULL) || (skb == NULL)) {
		printk("dsi_sock_rcv_skb sk=%x,skb=%x\n",(int)sk,(int)skb);
		return(0);
	}

	if (TCP_TIME_WAIT == sk->state) {
		/* the sk point to a tcp_tw_bucket that doesn't contain
		 sk->socket */
		return 0;
	}

	isec = 0;
	if (sk->socket) {
		if (sk->socket->inode) {
			isec = sk->socket->inode->i_security;
		}
	}

	if (!isec) {
		printk("dsi_socket_rcv_skb - security is not ATTACHED\n");
	}
	else {
		sid = isec->sid;
	}

	ssec = skb->lsm_security;
	nid = 0;

	if(!ssec) {
		printk("dsi_sock_rcv_skb is NULL\n");
	}
	else {
		nid = ssec->sid;	
	}
	
	if (isec && ssec) {
		rc = dsi_check_permission(isec->sid, ssec->sid, DSI_CLASS_NETWORK, NETWORK_RECEIVE);
        } else {
		rc = 0;//-ENOSYS;
        }

        if (rc) {		
                dsi_alarm = ALARM_NETWORK_RECEIVE;
                DSI_ERROR("dsi_sock_rcv_skb(%x) - No Permission\n",(int)sk);
        }	
	
	return(rc);
}

static int dsi_socket_unix_stream_connect (struct socket *sock,
					   struct socket *other)
{
	return 0;
}

static int dsi_socket_unix_may_send (struct socket *sock,
				     struct socket *other)
{
	return 0;
}

static int dsi_module_create_module (const char *name_user, size_t size)
{
	return 0;
}

static int dsi_module_init_module (struct module *mod_user)
{
	/*DSI_PRINT("dsi_module_init\n");*/
	return 0;
}

static int dsi_module_delete_module (const struct module *mod)
{
	/*DSI_PRINT("dsi_module_delete\n");*/
	return 0;
}

static int dsi_ipc_permission (struct kern_ipc_perm *ipcp, short flag)
{
	return 0;
}

static int dsi_ipc_getinfo (int id, int cmd)
{
	return 0;
}

static int dsi_msg_msg_alloc_security (struct msg_msg *msg)
{
	return 0;
}

static void dsi_msg_msg_free_security (struct msg_msg *msg)
{
	return;
}

static int dsi_msg_queue_alloc_security (struct msg_queue *msq)
{
	return 0;
}

static void dsi_msg_queue_free_security (struct msg_queue *msq)
{
	return;
}

static int dsi_msg_queue_associate (struct msg_queue *msq, int msqid,
				    int msqflg)
{
	return 0;
}

static int dsi_msg_queue_msgctl (struct msg_queue *msq, int msqid, int cmd)
{
	return 0;
}

static int dsi_msg_queue_msgsnd (struct msg_queue *msq, struct msg_msg *msg,
				 int msqid, int msgflg)
{
	return 0;
}

static int dsi_msg_queue_msgrcv (struct msg_queue *msq, struct msg_msg *msg,
				 struct task_struct *target, long type,
				 int mode)
{
	return 0;
}

static int dsi_shm_alloc_security (struct shmid_kernel *shp)
{
	return 0;
}

static void dsi_shm_free_security (struct shmid_kernel *shp)
{
	return;
}

static int dsi_shm_associate (struct shmid_kernel *shp, int shmid, int shmflg)
{
	return 0;
}

static int dsi_shm_shmctl (struct shmid_kernel *shp, int shmid, int cmd)
{
	return 0;
}

static int dsi_shm_shmat (struct shmid_kernel *shp, int shmid, char *shmaddr,
			  int shmflg)
{
	return 0;
}

static int dsi_sem_alloc_security (struct sem_array *sma)
{
	return 0;
}

static void dsi_sem_free_security (struct sem_array *sma)
{
	return;
}

static int dsi_sem_associate (struct sem_array *sma, int semid, int semflg)
{
	return 0;
}

static int dsi_sem_semctl (struct sem_array *sma, int semid, int cmd)
{
	return 0;
}

static int dsi_sem_semop (struct sem_array *sma, int semid,
			  struct sembuf *sops, unsigned nsops, int alter)
{
	return 0;
}

static int dsi_skb_alloc_security (struct sk_buff *skb)
{
	int rc; 

	rc = internal_sk_buff_alloc_security(skb);	
	return rc;
}

static int dsi_skb_clone (struct sk_buff *newskb,
			  const struct sk_buff *oldskb)
{

	return 0;

}

static void dsi_skb_copy (struct sk_buff *newskb,
			  const struct sk_buff *oldskb)
{
	return;
}

static void dsi_skb_set_owner_w (struct sk_buff *skb, struct sock *sk)
{
	sk_buff_security_t *ssec;
	inode_security_t *isec;


	if (!sk->socket) {		
		DSI_PRINT("dsi_skb_set_owner_w NO SOCKET\n");
		return;
	}

	if (!sk->socket->inode) {
		DSI_PRINT("dsi_skb_set_owner_w NO INODE\n");
		return;
	}

	isec = sk->socket->inode->i_security;
	ssec = skb->lsm_security;

	if (!isec) {
		internal_inode_alloc_security(sk->socket->inode);
		isec = sk->socket->inode->i_security;	
	}

	if (!ssec) {
                internal_sk_buff_alloc_security(skb);
                ssec = skb->lsm_security;
        }


	if ((ssec == 0) || (isec == 0)) {
		DSI_PRINT("dsi_skb_set_owner ssec=%x,isec=%x\n",(int)ssec,(int)isec);
		return;
	}

	ssec->sid = isec->sid;
	
	return;
}

static void dsi_skb_free_security (struct sk_buff *skb)
{
	internal_sk_buff_free_security(skb);
	return;
}

static int dsi_register (const char *name, struct security_operations *ops)
{
	return -EINVAL;
}

static int dsi_unregister (const char *name, struct security_operations *ops)
{
	return -EINVAL;
}

static struct binprm_security_ops dsi_binprm_ops = {
	alloc_security:	dsi_binprm_alloc_security,
	free_security:	dsi_binprm_free_security,
	compute_creds:	dsi_binprm_compute_creds,
	set_security:	dsi_binprm_set_security,
};

static struct super_block_security_ops dsi_sb_ops = {
	alloc_security:	dsi_sb_alloc_security,
	free_security:	dsi_sb_free_security,
	statfs:		dsi_sb_statfs,
	mount:		dsi_mount,
	check_sb:	dsi_check_sb,
	umount:		dsi_umount,
	umount_close:	dsi_umount_close,
	umount_busy:	dsi_umount_busy,
	post_remount:	dsi_post_remount,
	post_mountroot:	dsi_post_mountroot,
	post_addmount:	dsi_post_addmount,
};

static struct inode_security_ops dsi_inode_ops = {
	alloc_security:	dsi_inode_alloc_security,
	free_security:	dsi_inode_free_security,
	create:		dsi_inode_create,
	post_create:	dsi_inode_post_create,
	link:		dsi_inode_link,
	post_link:	dsi_inode_post_link,
	unlink:		dsi_inode_unlink,
	symlink:	dsi_inode_symlink,
	post_symlink:	dsi_inode_post_symlink,
	mkdir:		dsi_inode_mkdir,
	post_mkdir:	dsi_inode_post_mkdir,
	rmdir:		dsi_inode_rmdir,
	mknod:		dsi_inode_mknod,
	post_mknod:	dsi_inode_post_mknod,
	rename:		dsi_inode_rename,
	post_rename:	dsi_inode_post_rename,
	readlink:	dsi_inode_readlink,
	follow_link:	dsi_inode_follow_link,
	permission:	dsi_inode_permission,
	revalidate:	dsi_inode_revalidate,
	setattr:	dsi_inode_setattr,
	stat:		dsi_inode_stat,
	post_lookup:	dsi_post_lookup,
	delete:		dsi_delete,
};

static struct file_security_ops dsi_file_ops = {
	permission:	dsi_file_permission,
	alloc_security:	dsi_file_alloc_security,
	free_security:	dsi_file_free_security,
	llseek:		dsi_file_llseek,
	ioctl:		dsi_file_ioctl,
	mmap:		dsi_file_mmap,
	mprotect:	dsi_file_mprotect,
	lock:		dsi_file_lock,
	fcntl:		dsi_file_fcntl,
	set_fowner:	dsi_file_set_fowner,
	send_sigiotask:	dsi_file_send_sigiotask,
	receive:	dsi_file_receive,
};

static struct task_security_ops dsi_task_ops = {
	create:		dsi_task_create,
	alloc_security:	dsi_task_alloc_security,
	free_security:	dsi_task_free_security,
	setuid:		dsi_task_setuid,
	post_setuid:	dsi_task_post_setuid,
	setgid:		dsi_task_setgid,
	setpgid:	dsi_task_setpgid,
	getpgid:	dsi_task_getpgid,
	getsid:		dsi_task_getsid,
	setgroups:	dsi_task_setgroups,
	setnice:	dsi_task_setnice,
	setrlimit:	dsi_task_setrlimit,
	setscheduler:	dsi_task_setscheduler,
	getscheduler:	dsi_task_getscheduler,
	wait:		dsi_task_wait,
	kill:		dsi_task_kill,
	prctl:		dsi_task_prctl,
	kmod_set_label:	dsi_task_kmod_set_label,
};

static struct socket_security_ops dsi_socket_ops = {
	create:			dsi_socket_create,
	post_create:		dsi_socket_post_create,
	bind:			dsi_socket_bind,
	connect:		dsi_socket_connect,
	listen:			dsi_socket_listen,
	accept:			dsi_socket_accept,
	sendmsg:		dsi_socket_sendmsg,
	recvmsg:		dsi_socket_recvmsg,
	getsockname:		dsi_socket_getsockname,
	getpeername:		dsi_socket_getpeername,
	getsockopt:		dsi_socket_getsockopt,
	setsockopt:		dsi_socket_setsockopt,
	shutdown:		dsi_socket_shutdown,
	sock_rcv_skb:		dsi_sock_rcv_skb,
	unix_stream_connect:	dsi_socket_unix_stream_connect,
	unix_may_send:		dsi_socket_unix_may_send,
};

static struct skb_security_ops dsi_skb_ops = {
	alloc_security:	dsi_skb_alloc_security,
	clone:		dsi_skb_clone,
	copy:		dsi_skb_copy,
	set_owner_w:	dsi_skb_set_owner_w,
	free_security:	dsi_skb_free_security,
};

static struct ip_security_ops dsi_ip_ops = {
	preroute_first:		dsi_ip_preroute_first,
	preroute_last:		dsi_ip_preroute_last,
	input_first:		dsi_ip_input_first,
	input_last:		dsi_ip_input_last,
	forward_first:		dsi_ip_forward_first,
	forward_last:		dsi_ip_forward_last,
	output_first:		dsi_ip_output_first,
	output_last:		dsi_ip_output_last,
	postroute_first:	dsi_ip_postroute_first,
	postroute_last:		dsi_ip_postroute_last,
	fragment:		dsi_ip_fragment,
	defragment:		dsi_ip_defragment,
	encapsulate:		dsi_ip_encapsulate,
	decapsulate:		dsi_ip_decapsulate,
	decode_options:		dsi_ip_decode_options,
};

static struct netdev_security_ops dsi_netdev_ops = {
	unregister:	dsi_netdev_unregister,
};

static struct module_security_ops dsi_module_ops = {
	create_module:	dsi_module_create_module,
	init_module:	dsi_module_init_module,
	delete_module:	dsi_module_delete_module,

};

static struct ipc_security_ops dsi_ipc_ops = {
	permission:	dsi_ipc_permission,
	getinfo:	dsi_ipc_getinfo,
};

static struct msg_msg_security_ops dsi_msg_msg_ops = {
	alloc_security:	dsi_msg_msg_alloc_security,
	free_security:	dsi_msg_msg_free_security,
};

static struct msg_queue_security_ops dsi_msg_queue_ops = {
	alloc_security:	dsi_msg_queue_alloc_security,
	free_security:	dsi_msg_queue_free_security,
	associate:	dsi_msg_queue_associate,
	msgctl:		dsi_msg_queue_msgctl,
	msgsnd:		dsi_msg_queue_msgsnd,
	msgrcv:		dsi_msg_queue_msgrcv,
};

static struct shm_security_ops dsi_shm_ops = {
	alloc_security:	dsi_shm_alloc_security,
	free_security:	dsi_shm_free_security,
	associate:	dsi_shm_associate,
	shmctl:		dsi_shm_shmctl,
	shmat:		dsi_shm_shmat,
};

static struct sem_security_ops dsi_sem_ops = {
	alloc_security:	dsi_sem_alloc_security,
	free_security:	dsi_sem_free_security,
	associate:	dsi_sem_associate,
	semctl:		dsi_sem_semctl,
	semop:		dsi_sem_semop,
};

struct security_operations dsi_security_ops = {
	sethostname:		dsi_sethostname,
	setdomainname:		dsi_setdomainname,
	reboot:			dsi_reboot,
	ioperm:			dsi_ioperm,
	iopl:			dsi_iopl,
	ptrace:			dsi_ptrace,
	capget:			dsi_capget,
	capset_check:		dsi_capset_check,
	capset_set:		dsi_capset_set,
	acct:			dsi_acct,
	capable:		dsi_capable,
	sysctl:			dsi_sysctl,
	sys_security:		dsi_sys_security,
	swapon:			dsi_swapon,
	swapoff:		dsi_swapoff,
	nfsservctl:		dsi_nfsservctl,
	quotactl:		dsi_quotactl,
	quota_on:		dsi_quota_on,
	bdflush:		dsi_bdflush,
	syslog:			dsi_syslog,
	netlink_send:		dsi_netlink_send,
	netlink_recv:		dsi_netlink_recv,

	bprm_ops:		&dsi_binprm_ops,
	sb_ops:			&dsi_sb_ops,
	inode_ops:		&dsi_inode_ops,
	file_ops:		&dsi_file_ops,
	task_ops:		&dsi_task_ops,
	socket_ops:		&dsi_socket_ops,
	skb_ops:		&dsi_skb_ops,
	ip_ops:			&dsi_ip_ops,
	ipc_ops:		&dsi_ipc_ops,
	netdev_ops:		&dsi_netdev_ops,
	module_ops:		&dsi_module_ops,
	msg_msg_ops:		&dsi_msg_msg_ops,
	msg_queue_ops:		&dsi_msg_queue_ops,
	shm_ops:		&dsi_shm_ops,
	sem_ops:		&dsi_sem_ops,

	register_security:	dsi_register,
	unregister_security:	dsi_unregister,
};


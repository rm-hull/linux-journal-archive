/*
 * Distributed Security Module (DSM)
 *
 * dsi_inode.c	
 *
 * This file contains the DSM inode security implementation
 * 
 *
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
#include <linux/sched.h>
#include <linux/personality.h>

#include <sys/syscall.h>

#include <asm/uaccess.h>

#include "dsi.h"


/* 
 * Allocation  security memory for inode 
 *
 */

inline void *get_inode_memory(struct inode *inode)
{

	void *ret;

        ret = kmalloc(sizeof(inode_security_t),DSI_SAFE_ALLOC);

        return(ret);


}

/*
 * Free inode security memory
 *
 */


inline void free_inode_memory(inode_security_t *tsec)
{

        kfree(tsec);


}


int internal_inode_alloc_security (struct inode *inode)
{


        task_security_t  *tsec = current->security;
	inode_security_t *isec;

        int rc = 0;

        DSI_PRINT("internal_iknode_alloc_security inode=%x\n",(int)inode);
 
        isec = inode->i_security;
        if (isec && tsec->magic == DSI_MAGIC)
                goto out;

        isec = (inode_security_t *)get_inode_memory(inode);
        if (!isec) {
                rc = -ENOMEM;
                goto out;
        }
        memset(isec, 0, sizeof(inode_security_t));
        isec->magic = DSI_MAGIC;
        isec->inode = inode;
 
        isec->sid = DSI_SID_NORMAL;
	
	if(tsec)
		isec->task_sid = tsec->sid;

        inode->i_security = isec;

out:

        return rc;

}


void internal_inode_free_security (struct inode *inode)
{

        inode_security_t *isec = inode->i_security;

        DSI_PRINT("dsi_inode_free_security inode=%x\n",(int)inode);

        if (!isec || isec->magic != DSI_MAGIC)
                return;

        inode->i_security = NULL;

        free_inode_memory(isec);

        return;
}

int dsi_check_inode( struct task_struct *tsk1,
		     struct task_struct *tsk2,
		     int permission)
{
	int rc;
	task_security_t *tsec1, *tsec2;

        tsec1 = tsk1->security;
        tsec2 = tsk2->security;

	rc = dsi_check_permission(tsec1->sid, tsec2->sid, DSI_CLASS_SOCKET, permission);
	DSI_PRINT("dsi_check_inode=%d\n",rc);

	return(rc);
}





/*
 * Distributed Security Module (DSM)
 *
 * dsi_net.c
 *	
 * This file contains the DSM network implementation
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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/security.h>
#include <linux/netfilter.h>
#include <linux/netlink.h>
#include <linux/sched.h>
#include <linux/personality.h>

#include <sys/syscall.h>

#include <asm/uaccess.h>

#include "dsi.h"



extern int dsi_alarm;

/*
 * allocate sk_buff security structure
 *
 */

inline void * get_sk_buff_memory(struct sk_buff *skb)
{

	void * ret;

        ret = kmalloc(sizeof(sk_buff_security_t),DSI_SAFE_ALLOC);

        return(ret);


}

/* 
 * free sk_buff security structure
 *
 */


inline void free_sk_buff_memory(sk_buff_security_t *tsec)
{
        kfree(tsec);
}


int internal_sk_buff_alloc_security (struct sk_buff *skb)
{

        sk_buff_security_t *sksec;
        int rc = 0;

        DSI_PRINT("internal_sk_buff_alloc_security task=%x\n",(int)skb);

        sksec = skb->lsm_security;
        if (sksec && sksec->magic == DSI_MAGIC)
                goto out;

        sksec = (sk_buff_security_t *)get_sk_buff_memory(skb);
        if (!sksec) {
                rc = -ENOMEM;
                goto out;
        }
        memset(sksec, 0, sizeof(sk_buff_security_t));
        sksec->magic = DSI_MAGIC;
        sksec->skb = skb;
	sksec->sid = DSI_SID_NORMAL;
	skb->lsm_security = sksec;

out:

        return rc;

}


void internal_sk_buff_free_security (struct sk_buff *skb)
{

        sk_buff_security_t *ssec = skb->lsm_security;

        DSI_PRINT("internal_sk_buff_free_security task=%x\n",(int)skb);

        if (!ssec || ssec->magic != DSI_MAGIC)
                return;

        skb->lsm_security = NULL;

        free_sk_buff_memory(ssec);

        return;
}

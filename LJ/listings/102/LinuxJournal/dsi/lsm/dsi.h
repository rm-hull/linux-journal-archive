/* 
 * Distributed Security Module (DSM)
 *
 * dsi.h
 *
 * The main header file for DSM
 *
 * Copyright (C) 2002 Ericsson, Inc <miroslaw.zakrzewski@lmc.ericsson.se>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 */


#ifndef __DSI_H
#define __DSI_H

#ifdef MODULE         /* This prevents user programs that include dsi.h to inherit kernel stuff */
#include <linux/kernel.h>
#include <linux/interrupt.h>
#endif

#define DSI_MODULE_NAME "DSI-LSM MODULE" 

/* Security syhstem calls */
#define DSI_SET_NODE_ID         1
#define DSI_SET_TASK_SECURITY   2
#define DSI_SET_POLICY		3
#define DSI_ALARM_CHECK         4
#define DSI_SET_IMAGE_SID	5
#define DSI_SET_PSIDS		5

#define DSI_SID_LOW		1
#define DSI_SID_NORMAL		2
#define DSI_SID_HIGH		3

/* for now we define only 3 classes */
#define DSI_CLASS_PROCESS	1
#define DSI_CLASS_SOCKET	2
#define DSI_CLASS_NETWORK	3

/* permission bits for process class */
#define PROCESS_FORK		0x00000001

/* permission bits for socket class */
#define SOCKET_CREATE		0x00000001
#define SOCKET_CONNECT		0x00000002
#define SOCKET_SEND		0x00000004

/* permission bits for network */
#define NETWORK_RECEIVE         0x00000001

/* allarms types */
#define ALARM_PROCESS_CREATE    0x00000001
#define ALARM_SOCKET_CREATE     0x00000002
#define ALARM_SOCKET_CONNENT    0x00000004
#define ALARM_SOCKET_SEND       0x00000008
#define ALARM_NETWORK_RECEIVE   0x00000010

#define DSI_MAGIC       	0x7754ffa5

#ifdef MODULE         /* This prevents user programs that include dsi.h to inherit kernel stuff */

/*#define DSI_DEBUG*/
#ifdef DSI_DEBUG
#define DSI_PRINT(fmt, arg...) printk(DSI_MODULE_NAME" - " fmt,##arg);	
#else
#define DSI_PRINT(fmt, arg...)
#endif

#define DSI_WARNING(fmt, arg...) printk(DSI_MODULE_NAME"-WARNING-" fmt,##arg);
#define DSI_ERROR(fmt, arg...) printk(DSI_MODULE_NAME"-ERROR-" fmt,##arg);

#define DSI_SAFE_ALLOC (in_interrupt () ? GFP_ATOMIC : GFP_KERNEL)

#define DSI_ELF_SID_POS 7

typedef struct {
        int     sid;
        int     osid;
        int     magic;
        void    *task;
} task_security_t;


typedef struct /*inode_security_struct*/ {
        unsigned long magic;           /* magic number for this module */
        struct inode *inode;           /* back pointer to inode object */
        int           task_sid;        /* SID of creating task */
        int           sid;             /* SID of this object */
        int           class;          /* security class of this object */
} inode_security_t;


typedef struct {
        unsigned long 	magic;          /* magic number for this module */
        struct sk_buff *skb;            /* back pointer */
        atomic_t 	use;            /* reference count */
        int		sid;            /* Source SID */
} sk_buff_security_t;


int dsi_label_tasks();
int internal_task_alloc_security(struct task_struct *p);
void internal_task_free_security(struct task_struct *p);
int dsi_check_task( struct task_struct *tsk1,
                    struct task_struct *tsk2,
                    int permission);
int internal_inode_alloc_security (struct inode *inode);
void internal_inode_free_security (struct inode *inode);

int internal_sk_buff_alloc_security (struct sk_buff *skb);
void internal_sk_buff_free_security (struct sk_buff *skb);



#include "dsi_cache.h"
#endif /* MODULE */

#endif

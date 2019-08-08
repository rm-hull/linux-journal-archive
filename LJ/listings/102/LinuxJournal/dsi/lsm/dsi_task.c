/*
 * Distributed Security Module (DSM)
 *
 * dsi_task.c
 *	
 * This file contains the DSM task security implementation
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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/security.h>
#include <linux/netfilter.h>
#include <linux/netlink.h>
#include <linux/sched.h>
#include <linux/personality.h>

#include <linux/elf.h>
#include <sys/syscall.h>

#include <asm/uaccess.h>

#include "dsi.h"



#define END_OF_TASK_STRUCT

extern int dsi_alarm;

/*
 * Allocate the task security structure.
 * We can control how the security structure is allocated
 * by defining or undefining END_OF_TASK_STRUCT.
 * If END_OF_TASK_STRUCT is defined then we don't
 * allocate the memory but the security structure is 
 * alocated on top of the memory used for task_struct.
 * If the security structure is big, then probably,
 * there must be the memory allocation because the kernel
 * stack can overwrite the security area.
 *
 */

inline int get_task_memory(struct task_struct *p)
{

	int ret;


#ifdef END_OF_TASK_STRUCT

        ret = (int)p + sizeof(struct task_struct);
#else
        ret = kmalloc(sizeof(task_security_t),DSI_SAFE_ALLOC);

#endif

        return(ret);

}


inline void free_task_memory(task_security_t *tsec)
{
#ifndef END_OF_TASK_STRUCT
        kfree(tsec);
#endif

}


int internal_task_alloc_security (struct task_struct *p)
{
        task_security_t *tsec;
        int rc = 0; 
        char elf_hdr[EI_NIDENT] = {0};
 
        DSI_PRINT("internal_task_alloc_security task=%x\n",(int)p);

        tsec = p->security;
        if (tsec && tsec->magic == DSI_MAGIC)
                goto out;

        tsec = (task_security_t *)get_task_memory(p);
        if (!tsec) {
                rc = -ENOMEM;
                goto out;
        }
        memset(tsec, 0, sizeof(task_security_t));
        tsec->magic = DSI_MAGIC;
        tsec->task = p;
        tsec->osid = tsec->sid = DSI_SID_NORMAL;

	if (p->mm && p->mm->mmap)
	{
		kernel_read(p->mm->mmap->vm_file,0,elf_hdr,EI_NIDENT);
	    
		/* First of all, some simple consistency checks */
		if (memcmp(elf_hdr, ELFMAG, SELFMAG) == 0) {
			/* Validate that we have a SID */
			if (elf_hdr[DSI_ELF_SID_POS]) {
				tsec->sid = elf_hdr[DSI_ELF_SID_POS];/***need to modify osid ***/
				DSI_PRINT("SID modified in internal alloc task  %p %d\n",p,tsec->sid);
			}
		}
        }

        p->security = tsec;

out:

        return rc;

}


void internal_task_free_security (struct task_struct *p)
{

        task_security_t *tsec = p->security;

        DSI_PRINT("dsi_task_free_security task=%x\n",(int)p);

        if (!tsec || tsec->magic != DSI_MAGIC)
                return;

        p->security = NULL;

        free_task_memory(tsec);

        return;
}

/*
 * When module is loaded, we scan the task list and attach
 * the security structure to them.
 *
 */
int dsi_label_tasks()
{
	struct task_struct 	*p;
        int 			rc=0;

	for_each_task(p) {
		rc = internal_task_alloc_security (p);
		if(rc)
			break;
	}
	
	if(!rc) {
		p = &init_task;
		rc = internal_task_alloc_security (p);
	}

	return(rc);

}

int dsi_check_task( struct task_struct *tsk1,
		    struct task_struct *tsk2,
		    int permission)
{
	int rc;
	task_security_t *tsec1, *tsec2;


        tsec1 = tsk1->security;
        tsec2 = tsk2->security;

	rc = dsi_check_permission(tsec1->sid, tsec2->sid, DSI_CLASS_PROCESS, permission);

	if(rc)
		dsi_alarm |= ALARM_PROCESS_CREATE; 
	return(rc);
}


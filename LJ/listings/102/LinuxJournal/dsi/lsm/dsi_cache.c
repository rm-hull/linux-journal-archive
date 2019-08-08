/*
 * Distributed Security Module (DSM)
 *
 * dsi_cache.c
 *	
 * This file contains the DSM cache implementation
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

#include <linux/netfilter.h>

#include <sys/syscall.h>

#include <asm/uaccess.h>

#include <linux/string.h>

#include "dsi_cache.h"

#include "dsi.h"



static dsi_cache_t dsi_cache;

static dsi_cache_node_t  *dsi_free_node_list = NULL;

/*
 * Initialize the cache.
 */
int dsi_cache_init(void)
{
        dsi_cache_node_t	*new;
        int             	i;
	int			err = 0;

        for (i = 0; i < DSI_CACHE_SLOTS; i++)
                dsi_cache.slots[i] = 0;

       	dsi_cache.nhit		= 0;
        dsi_cache.activeNodes	= 0;

        for (i = 0; i < DSI_CACHE_MAX_NODES; i++) {
                new = (dsi_cache_node_t *) kmalloc(sizeof(dsi_cache_node_t),
                                             GFP_KERNEL);

                if (!new) {
                        DSI_ERROR("avc:  only able to allocate %d entries\n", i);
			err = -ENOMEM;
                        break;
                }
                memset(new, 0, sizeof(dsi_cache_node_t));
                new->next = dsi_free_node_list;
                dsi_free_node_list = new;
        }

	if (!err) {
		printk("DSI:  allocated %d bytes during initialization.\n",
		       i * sizeof(dsi_cache_node_t));
	}

	return err;
  
}


/*
 * Free the cache
 */

void dsi_cache_free(void)
{
	dsi_cache_node_t	*cur, *tmp;
	int i, activeNode=0;

	/* free node list */
	while(dsi_free_node_list) {
		cur = dsi_free_node_list;
		dsi_free_node_list = dsi_free_node_list->next;
		kfree(cur);
	}

	for (i = 0; i < DSI_CACHE_SLOTS; i++) {
                cur = dsi_cache.slots[i];
		while(cur) {
			tmp = cur;
			cur = cur->next;
			kfree(tmp);
			activeNode++;
		}
		dsi_cache.slots[i] = 0;
	}

	if (dsi_cache.activeNodes != activeNode) {
		DSI_WARNING("Active node count(%d) different from predicted(%d)\n",
			    activeNode,dsi_cache.activeNodes );
	}

}

/*
 * Get a new node from the list
 * and load SIDs pair, class and permission.
 */
inline dsi_cache_node_t *dsi_load_node(int ssid, int tsid, int tclass, unsigned int permission)
{

        dsi_cache_node_t     *new;
        int             hvalue;


	new = NULL;
        hvalue = DSI_HASH(ssid, tsid, tclass);

        if (dsi_free_node_list) {
                new = dsi_free_node_list;
                dsi_free_node_list = dsi_free_node_list->next;
                dsi_cache.activeNodes++;
        } else {
               DSI_ERROR("No free elements to the free list\n");
        }

	if(!new)
		return(NULL);
	
        new->node.ssid = ssid;
        new->node.tsid = tsid;
        new->node.class = tclass;
	new->node.permission = permission;
        new->next = dsi_cache.slots[hvalue];
        dsi_cache.slots[hvalue] = new;
	DSI_PRINT("dsi_load_node node = %x\n",(int)new);
        return new;
}
 
#define DSI_HARDCODED_POLICY

/*
 * This function is use to load the hardcoded policy
 * and is used to populate the cache if the external policy
 * is not available.
 * We use it for testing
 *
 */
int dsi_load_policy()
{
	
	int err=0;

#ifdef DSI_HARDCODED_POLICY

	DSI_PRINT("dsi_load_policy\n");

/* process class */
	dsi_load_node(DSI_SID_LOW,DSI_SID_LOW,DSI_CLASS_PROCESS,0xffffffff);
	dsi_load_node(DSI_SID_LOW,DSI_SID_NORMAL,DSI_CLASS_PROCESS,0xffffffff);
	dsi_load_node(DSI_SID_LOW,DSI_SID_HIGH,DSI_CLASS_PROCESS,0xffffffff);
	dsi_load_node(DSI_SID_NORMAL,DSI_SID_LOW,DSI_CLASS_PROCESS,0xffffffff);
	dsi_load_node(DSI_SID_NORMAL,DSI_SID_NORMAL,DSI_CLASS_PROCESS,0xffffffff);
	dsi_load_node(DSI_SID_NORMAL,DSI_SID_HIGH,DSI_CLASS_PROCESS,0xffffffff);
	dsi_load_node(DSI_SID_HIGH,DSI_SID_LOW,DSI_CLASS_PROCESS,0xffffffff);
	dsi_load_node(DSI_SID_HIGH,DSI_SID_NORMAL,DSI_CLASS_PROCESS,0xffffffff);
	dsi_load_node(DSI_SID_HIGH,DSI_SID_HIGH,DSI_CLASS_PROCESS,0xffffffff);
	

/* socket class */
	dsi_load_node(DSI_SID_LOW,DSI_SID_LOW,DSI_CLASS_SOCKET,0xffffffff);
        dsi_load_node(DSI_SID_LOW,DSI_SID_NORMAL,DSI_CLASS_SOCKET,0xffffffff);
        dsi_load_node(DSI_SID_LOW,DSI_SID_HIGH,DSI_CLASS_SOCKET,0xffffffff);
        dsi_load_node(DSI_SID_NORMAL,DSI_SID_LOW,DSI_CLASS_SOCKET,0xffffffff);
        dsi_load_node(DSI_SID_NORMAL,DSI_SID_NORMAL,DSI_CLASS_SOCKET,0xffffffff);
        dsi_load_node(DSI_SID_NORMAL,DSI_SID_HIGH,DSI_CLASS_SOCKET,0xffffffff);
        dsi_load_node(DSI_SID_HIGH,DSI_SID_LOW,DSI_CLASS_SOCKET,0xffffffff);
        dsi_load_node(DSI_SID_HIGH,DSI_SID_NORMAL,DSI_CLASS_SOCKET,0xffffffff);
        dsi_load_node(DSI_SID_HIGH,DSI_SID_HIGH,DSI_CLASS_SOCKET,0xffffffff);

/* network class */
	dsi_load_node(DSI_SID_LOW,DSI_SID_LOW,DSI_CLASS_NETWORK,0xffffffff);
        dsi_load_node(DSI_SID_LOW,DSI_SID_NORMAL,DSI_CLASS_NETWORK,0xffffffff);
        dsi_load_node(DSI_SID_LOW,DSI_SID_HIGH,DSI_CLASS_NETWORK,0xffffffff);
        dsi_load_node(DSI_SID_NORMAL,DSI_SID_LOW,DSI_CLASS_NETWORK,0xffffffff);
        dsi_load_node(DSI_SID_NORMAL,DSI_SID_NORMAL,DSI_CLASS_NETWORK,0xffffffff);
        dsi_load_node(DSI_SID_NORMAL,DSI_SID_HIGH,DSI_CLASS_NETWORK,0xffffffff);
        dsi_load_node(DSI_SID_HIGH,DSI_SID_LOW,DSI_CLASS_NETWORK,0xffffffff);
        dsi_load_node(DSI_SID_HIGH,DSI_SID_NORMAL,DSI_CLASS_NETWORK,0xffffffff);
        dsi_load_node(DSI_SID_HIGH,DSI_SID_HIGH,DSI_CLASS_NETWORK,0xffffffff);
#endif
	return err;
}



/*
 * Search for a node that has the specified
 * SID pair and class.
 */
inline dsi_cache_node_t *dsi_search_node(
        int ssid,
        int tsid,
        int tclass)
{
        dsi_cache_node_t     *cur;
        int             hvalue;
        int             tprobes = 1;


        hvalue = DSI_HASH(ssid, tsid, tclass);

        cur = dsi_cache.slots[hvalue];
        while (cur != NULL &&
               (ssid != cur->node.ssid ||
                tclass != cur->node.class ||
                tsid != cur->node.tsid)) {
                tprobes++;
                cur = cur->next;
        }
        if (cur == NULL) {
                /* cache miss */
		printk("cache miss ssid=%x, tsid=%x, class=%x,hvalue=%x\n",ssid,tsid,tclass,hvalue);
                return NULL;
        }

        /* cache hit */

        return cur;
}


/*
 * Look up an AVC entry that is valid for the
 * `requested' permissions between the SID pair
 * (`ssid', `tsid'), interpreting the permissions
 * based on `tclass'.  If a valid AVC entry exists,
 * then this function updates `aeref' to refer to the
 * entry and returns 0. Otherwise, this function
 * returns -ENOENT.
 */
int dsi_check_permission(
        int ssid,             /* IN */
        int tsid,             /* IN */
        int tclass,        /* IN */
        int requested)       
{
        int rc = 0;//-ENOENT;
        dsi_cache_node_t     *node;

        node = dsi_search_node(ssid, tsid, tclass);

        if (node) {
	        if (node->node.permission & requested ) {
		        return 0;
		}
                else {
		        rc = -ENOENT;        
                }
        }

        if (rc) {
		printk("NO permission node =%x,requested=%x\n",(int)node,requested);
		if(node) {
			printk("node permission=%x\n",node->node.permission);	
			printk("node ssid=%x\n",node->node.ssid);
			printk("node tsid=%x\n",node->node.tsid);
			printk("node class=%x\n",node->node.class);
		}
	}

        return rc;
}

dsi_cache_node_t *dsi_cache_update(int ssid, int tsid, int tclass, unsigned int permission)
{
	dsi_cache_node_t     *node;

	node = dsi_search_node(ssid, tsid, tclass);
                
        if(node) {
		node->node.ssid = ssid;
		node->node.tsid = tsid;
		node->node.class = tclass;
		node->node.permission = permission;
        }
        else{
		node = dsi_load_node( ssid, tsid, tclass, permission);

        }

	return (node);

}

/*
 * Distributed Security Module (DSM)
 *
 * dsi_cache.h
 *
 * This file contains the header file cache functions
 *
 * Copyright (C) 2002 Ericsson, Inc <miroslaw.zakrzewski@lmc.ericsson.se>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 */


#ifndef __DSI_CACHE_H
#define __DSI_CACHE_H

typedef struct dsi_cache_entry {
        int ssid;
        int tsid;
        int class;
        unsigned int permission;
} dsi_cache_entry_t;


typedef struct dsi_cache_node {
        dsi_cache_entry_t node;
        struct dsi_cache_node *next;
} dsi_cache_node_t;


#define DSI_CACHE_SLOTS         512
#define DSI_CACHE_MAX_NODES     410

typedef struct {
        dsi_cache_node_t *slots[DSI_CACHE_SLOTS];
        int nhit;
        int activeNodes;
} dsi_cache_t;


#define DSI_HASH(ssid,tsid,class) \
((ssid ^ (tsid<<2) ^ (class<<4)) & (DSI_CACHE_SLOTS - 1))


/* functions prototypes */

int dsi_cache_init(void);
void dsi_cache_free(void);
dsi_cache_node_t *dsi_load_node(int ssid, int tsid, int tclass, unsigned int permission);

inline dsi_cache_node_t *dsi_search_node(
        int ssid,
        int tsid,
        int tclass);


int dsi_check_permission(
        int ssid,             /* IN */
        int tsid,             /* IN */
        int tclass,        /* IN */
        int requested);
int dsi_load_policy();


typedef struct dsi_cache_task {
	int sid;
        struct dsi_cache_task *next;
        char name[1];
} dsi_cache_task_t;


typedef struct dsi_cache_nid {
	int nodeId;
        int ssid;
        int netId;
} dsi_cache_nid_t;


dsi_cache_node_t *dsi_cache_update(int ssid, int tsid, int tclass, unsigned int permission);
int dsi_cache_nid_load(int nodeId, int ssid, int netId);
#endif

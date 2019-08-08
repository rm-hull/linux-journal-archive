/*
 * Distributed Security Module (DSM) Initialization function
 *
 * Copyright (C) 2002 Ericsson, Inc <miroslaw.zakrzewski@lmc.ericsson.se>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/security.h>

#include "dsi.h"

extern struct security_operations dsi_security_ops;

int init_module()
{
	int err=0;

	DSI_PRINT("Initializing DSM\n");

	if ((err = dsi_label_tasks())) {
		DSI_ERROR("Can't attach labels to tasks\n");
		return err;
	}

	if ((err = dsi_cache_init())) {
		DSI_PRINT("Can't initialize the cache\n");
		dsi_cache_free();
		return err;
	}

	if ((err = dsi_load_policy())) {
		DSI_PRINT("Can't load policy\n");
		dsi_cache_free();
		return err;
	}

	if ((err = register_security(&dsi_security_ops))) {
		DSI_ERROR("Wrong security parameter\n");
		dsi_cache_free();
		return err;
	}	

	return(0);

}

void cleanup_module()
{
	DSI_PRINT("Deinitializing module\n");

	dsi_cache_free();

	unregister_security(&dsi_security_ops);
}

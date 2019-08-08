/*
 * Distributed Security Module (DSM)
 *
 * dsi_ip_options.h
 *
 * This is the header file for IP options.
 * The options are used to add the security information to the IP packet 
 *
 * Copyright (C) 2002 Ericsson, Inc <miroslaw.zakrzewski@lmc.ericsson.se>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 */



/*
 * Network SID API
 *
 * Copyright (C) 2002 Ericsson, Inc
 *
 *	This file contains functions to modify IP options.
 *      The set of function are called by hooks defined by the LSM interface.
 */


#ifndef _DSI_IP_OPTIONS_H_
#define _DSI_IP_OPTIONS_H_

#define DEFAULT_SID                0x12341234    /* NSID_U32_LEN */
#define SAFE_ALLOC (in_interrupt () ? GFP_ATOMIC : GFP_KERNEL)
#define SELOPT_ALIGN(val)		((val + 3) & ~3)
#define NSID_NODEID             6
#define NSID_NODEID_LEN         NSID_U32_LEN

#define NSID_DOI		0x10001000	/* Made up */
#define NSID_FREEFORM		7		/* FIPS-188, type 7 tag */
#define NSID_BASE_LEN		8		/* type+len+doi+tagtype+taglen*/
#define NSID_NULL_LEN		2		/* Length of null parameter */
#define NSID_U32_LEN		6		/* Length of 32-bit parameter */
#define NSID_BYPASS		1		/* Bypass label */
#define NSID_BYPASS_LEN	NSID_NULL_LEN	/* Bypass label length */
#define NSID_SERIAL		2		/* Policy serial label */
#define NSID_SERIAL_LEN	NSID_U32_LEN	/* Policy serial length */
#define NSID_SSID		3		/* Source SID label */
#define NSID_SSID_LEN		NSID_U32_LEN	/* Source SID length */
#define NSID_MSID		4		/* Message SID label */
#define NSID_MSID_LEN		NSID_U32_LEN	/* Message SID length */
#define NSID_DSID		5		/* Destination SID label */
#define NSID_DSID_LEN		NSID_U32_LEN	/* Destination SID length */

#define SCMP_PORT		40000		/* IANA unassigned */

#ifdef __KERNEL__

#define DEBUG_LABELING

#define NSID_ALIGN(val)		((val + 3) & ~3)
#define NSID_MIN_LEN			NSID_BASE_LEN + NSID_BYPASS_LEN
#define NSID_STD_LEN			(NSID_BASE_LEN \
                                         + NSID_SERIAL_LEN + NSID_SSID_LEN)
#define NSID_MAX_TCP_LEN		(NSID_BASE_LEN + NSID_SERIAL_LEN \
                                         + NSID_SSID_LEN + NSID_DSID_LEN)

#define ip_warn(iph, fmt, args...)                                     \
                                                                       \
       do { printk(KERN_WARNING __FUNCTION__ ": " fmt, ##args);        \
            printk(" [%u.%u.%u.%u->%u.%u.%u.%u]\n",                    \
            NIPQUAD(iph->saddr), NIPQUAD(iph->daddr));                 \
       } while (0);


#ifdef DEBUG_LABELING
#define SDBG(x)		do { x; } while (0);
#else
#define SDBG(x)		do { } while (0);
#endif	/* DEBUG_LABELING */

#endif  /* __KERNEL__ */

#endif  /* _DSI_IP_OPTIONS__H_ */

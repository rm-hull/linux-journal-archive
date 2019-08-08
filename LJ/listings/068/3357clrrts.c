/*
 * Copyright 1999 Dennis Fox - Micro Time Inc.
 * clrrts.c	1.00 01/09/99	Please leave this notice intact
 * 
 * This program Clears the RTS line on a serial port.
 * The Micronta (Radio Shack) No. 22-182 Digital Mulitmeter needs RTS
 * to be in the MARK (low) condition before the TXD line operates correctly.
 *
 * Credit given to Doug Hughes Auburn University
 * I used the header and open from his program upsstat.c 1.3 dated 08/26/96
 */

#include <stdio.h>
#include <sys/time.h>
#include <termio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/signal.h>
#include <sys/stat.h>

main(int argc, char *argv[]) 
{
	int fd;
	int flag, i, j;
	struct stat sb;
	char devname[20]="/dev/";
	char status[40];

	if (argc < 2) {
		fprintf(stderr, "must supply device name (e.g. ttyS0, term/a, ttya)\n");
		exit(1);
	}

	if (strcmp(argv[1], "-ttytab") == 0) {
		fd = 0;		/* terminal has already been opened as fd 0 */
					/* This is for experimental support of SunOS */
	} else  {
		strcat(devname, argv[1]);
		if ((fd = open(devname, O_RDONLY)) < 0) {
			fprintf(stderr, "couldn't open device: %s\n", devname);
			exit(1);
		}
	}
	
	/* Get command register status bits */
	if (ioctl(fd, TIOCMGET, &j) < 0) {
		perror("Getting hardware status bits");
		exit(1);
	}
	/* Mask off the RTS bit, and send back to port with ioctl */
	j=j & (~TIOCM_RTS);
	ioctl(fd, TIOCMSET, &j);
}


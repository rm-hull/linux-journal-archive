/*
 * Copyright 1999 Dennis Fox - Micro Time Inc.
 * modctl.c     1.00 01/09/99   Please leave this notice intact
 *
 * This program can Set or Clr, RTS or DTR on a serial port to facilitate
 * using these lines to provide non-standard signals to connected equipment.
 *
 * For instance the Micronta (Radio Shack) No. 22-182 Digital Mulitmeter
 * needs RTS to be in the MARK (low) condition before the TXD line operates
 * correctly.
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

	if (argc < 4) {
		fprintf(stderr, "Usage: modctl port (no /dev/) S|C RTS|DTR\n");
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
		perror("Error getting hardware status bits");
		exit(1);
	}
	
	/* Test for Clear or Set, then RTS or DTR, mask appropriate bit */
	if (strcmp(argv[2], "C") == 0) {
		if (strcmp(argv[3], "RTS") == 0) {
			j=j & (~TIOCM_RTS);
		} else {
			j=j & (~TIOCM_DTR);
		}
	} else {
		if (strcmp(argv[3], "RTS") == 0) {
			j=j | (TIOCM_RTS);
		} else {
			j=j | (TIOCM_DTR);
		}

	}
	
	
	/* Send back to port with ioctl */
	ioctl(fd, TIOCMSET, &j);
}


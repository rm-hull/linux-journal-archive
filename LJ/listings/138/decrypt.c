/*
 Used to Decrypt the shell code
*/

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/**
 * 'Alleged RC4' Source Code picked up from the news.
 * From: allen@gateway.grumman.com (John L. Allen)
 * Newsgroups: comp.lang.c
 * Subject: Shrink this C code for fame and fun
 * Date: 21 May 1996 10:49:37 -0400
 */

static unsigned char state[256], indx, jndx;

/*
 * Reset rc4 state. 
 */
void state_0(void)
{
	indx = jndx = 0;
	do {
		state[indx] = indx;
	} while (++indx);
}

/*
 * Set key. Can be used more than once. 
 */
void key(char * str, int len)
{
	unsigned char tmp, * ptr = (unsigned char *)str;
	while (len > 0) {
		do {
			tmp = state[indx];
			jndx += tmp;
			jndx += ptr[(int)indx % len];
			state[indx] = state[jndx];
			state[jndx] = tmp;
		} while (++indx);
		ptr += 256;
		len -= 256;
	}
}

/*
 * Crypt data. 
 */
void rc4(char * str, int len)
{
	unsigned char tmp, * ptr = (unsigned char *)str;
	jndx = 0;
	while (len > 0) {
		indx++;
		tmp = state[indx];
		jndx += tmp;
		state[indx] = state[jndx];
		state[jndx] = tmp;
		tmp += state[indx];
		*ptr ^= state[tmp];
		ptr++;
		len--;
	}
}
/*
 * Key with file invariants.
 */
int key_with_file(char * file)
{
        struct stat statf[1];
        struct stat control[1];
                                                                                
        if (stat(file, statf) < 0)
                return -1;
                                                                                
        /* Turn on stable fields */
        memset(control, 0, sizeof(control));
        control->st_ino = statf->st_ino;
        control->st_dev = statf->st_dev;
        control->st_rdev = statf->st_rdev;
        control->st_uid = statf->st_uid;
        control->st_gid = statf->st_gid;
        control->st_size = statf->st_size;
        control->st_mtime = statf->st_mtime;
        control->st_ctime = statf->st_ctime;
        key((char *)control, sizeof(control));
        return 0;
}


#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "data.h"

void decrypt(Arrayparam *ap)
{
	/* Decrypt cipher text */
	state_0();
	key(ap->pswd, ap->pswd_len);
        rc4(ap->shll, ap->shll_len);
	printf("SHELL=%s\n", ap->shll);
        rc4(ap->inlo, ap->inlo_len);
	printf("INLO=%s\n", ap->inlo);
        rc4(ap->xecc, ap->xecc_len);
	printf("XECC=%s\n", ap->xecc);
        rc4(ap->lsto, ap->lsto_len);
	printf("LSTO=%s\n", ap->lsto);
        rc4(ap->chk1, ap->chk1_len);
	printf("CHK1=%s\n", ap->chk1);
	if(ap->relax)
		key_with_file(ap->shll);
	rc4(ap->opts, ap->opts_len);
	printf("OPTS=%s\n", ap->opts);
	rc4(ap->txt, ap->txt_len);
	printf("CIPHERTEXT:---------V\n%s\n", ap->txt);
}

/* 
 * inp.c -- read all the ports specified in hex on the command line
 *
 * Tested with 1.2 and 2.0 on the x86
 * It won't run on other platforms.
 */

#ifndef __i386__
#  error "This program can't compile or run on non-intel computers"
#else

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <asm/io.h>

int main(int argc, char **argv)
{
    unsigned int i,n;

    setuid(0); /* if we're setuid, do it really */
    for (i=1;i<argc;i++) {
        sscanf(argv[i],"%x",&n);
        if (ioperm(n,1,1)) {perror("ioperm()"); exit(1);}
        printf("%03x: %02x\n",n,inb(n));
    }
    return 0;
    
}

#endif /* __i386__ */




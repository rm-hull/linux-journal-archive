/* 
 * outp.c -- write the port/value pairs specified in hex on the command line
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
    unsigned int i,n,v;

    setuid(0); /* if we're setuid, do it really */
    for (i=1;i<argc-1;i++) {
        sscanf(argv[i],"%x",&n);
        sscanf(argv[++i],"%x",&v);
        if (ioperm(n,1,1)) {perror("ioperm()"); exit(1);}
        outb(v,n);
    }
    return 0;
}

#endif /* __i386__ */

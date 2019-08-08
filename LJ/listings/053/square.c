#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int hz, minb, maxb;
    double amp;
    unsigned long count=0;
    FILE *res;
    if (argc!=3) {
        fprintf(stderr,"Use: quadra <min> <max>\n");
        exit(1);
    }
        
    hz=10;
    minb=atoi(argv[1]);
    maxb=atoi(argv[2]);

    fprintf(stderr,"%lf hz, %i and %i\n",4000.0/hz,minb,maxb);
    res = freopen("/dev/audio","w",stdout);
    if (!res) {
        fprintf(stderr,"/dev/audio: %s\n",strerror(errno));
        exit(1);
    }

    while (count <8000) {
/*       fprintf(stderr,"arg=%lf\n",count/8000.0*hz*2*M_PI); */
        putchar((count/hz)&1 ? minb : maxb);
        count++;
    }
    fclose(stdout);
    return 0;
}



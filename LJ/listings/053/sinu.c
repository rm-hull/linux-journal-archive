#include <stdio.h>
#include <math.h>


int main(int argc, char **argv)
{
    int hz, minb, maxb, out;
    double amp;
    double count=0;

    if (argc!=4) {
        fprintf(stderr,"Use: sinu <Hz> <min> <max>\n");
        exit(1);
    }
        
    hz=atoi(argv[1]);
    minb=atoi(argv[2]);
    maxb=atoi(argv[3]);
    amp=(double)(maxb-minb)/2.0;

fprintf(stderr,"%i hz, %i to %i (amp %lf)\n",hz,minb,maxb,amp);


    while (1) {

	/* u8 b = byte & 0x80 ? byte&0x7f : 0xff^byte; */
	
	out=minb+amp+floor(amp*sin(count/8000.0*hz*2*M_PI));

	putchar(out<128? out+128 : out^0xff);
        count++;
    }
}

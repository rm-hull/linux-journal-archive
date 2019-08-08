#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <sys/ioctl.h>

#include <time.h>
#include <errno.h>
#include <math.h>

#define VERSION "0.91"

int tmp;	/* for debugging */

#define OPTION "medium raw"

extern int getfd();
int fd;
int oldkbmode;
struct termios old, new;


struct hz {
    int key;
    double freq;
    } hz[] = {

#define DO(k,h) {k,h*2*M_PI/8000.0},


DO(86,246.94)

DO(44,261.62)
	DO(31,277.18)
DO(45,293.66)
	DO(32,311.12)
DO(46,329.62)
DO(47,349.22)
	DO(34,369.99)
DO(48,391.99)
	DO(35,415.30)
DO(49,440.00)
	DO(36,466.16)
DO(50,493.88)

DO(51,523.25)
	DO(38,554.36)
DO(52,587.32)
	DO(39,622.25)
DO(53,659.25)

DO(16,523.25)
	DO(3,554.36)
DO(17,587.32)
	DO(4,622.25)
DO(18,659.25)
DO(19,698.45)
	DO(6,739.99)
DO(20,783.99)
	DO(7,830.61)
DO(21,880.00)
	DO(8,932.32)
DO(22,987.76)

DO(23,1046.50)
	DO(10,1108.73)
DO(24,1174.66)
	DO(11,1244.50)
DO(25,1318.51)
DO(26,1396.91)
	DO(13,1479.98)
DO(27,1567.98)

{0,0}
};






/*
 * version 0.81 of showkey would restore kbmode unconditially to XLATE,
 * thus making the console unusable when it was called under X.
 */
void get_mode(void) {
        char *m;

	if (ioctl(fd, KDGKBMODE, &oldkbmode)) {
		perror("KDGKBMODE");
		exit(1);
	}
	switch(oldkbmode) {
	  case K_RAW:
	    m = "RAW"; break;
	  case K_XLATE:
	    m = "XLATE"; break;
	  case K_MEDIUMRAW:
	    m = "MEDIUMRAW"; break;
	  default:
	    m = "?UNKNOWN?"; break;
	}
	printf("kb mode was %s\n", m);
	if (oldkbmode != K_XLATE) {
	    printf("[ if you are trying this under X, it might not work\n");
	    printf("since the X server is also reading /dev/console ]\n");
	}
	printf("\n");
}

void clean_up(void) {
	if (ioctl(fd, KDSKBMODE, oldkbmode)) {
		perror("KDSKBMODE");
		exit(1);
	}
	tmp = tcsetattr(fd, 0, &old);
	if (tmp)
		printf("tcsetattr = %d\n", tmp);
	close(fd);
}

void die(int x) {
	printf("caught signal %d, cleaning up...\n", x);
	clean_up();
	exit(1);
}

void watch_dog(int x) {
	clean_up();
	exit(0);
}

int addkeytoset(unsigned char key, int islast)
{
    static keyset[10];
    static double fset[10];
    static unsigned long count=0;
    static int fd=-1;
    static unsigned char buffer[80000];
    fd_set fdset;
    int i, j, val, nnote, maxnote;
    struct hz *ptr;
    static int change=0;
    struct timeval notime={0,0};

    if (fd<0) {
	fd=open("/dev/audio",O_WRONLY|O_NONBLOCK);
        count=0;
    }
    if (fd<0) {
	fprintf(stderr,"/dev/audio: %s\n",strerror(errno));
	clean_up();
	exit(1);
    }
    if(key&0x80) { /* old key, cleanup */
	for (i=0; i<10; i++) {
	    if (keyset[i]==(key&0x7f)) {
		keyset[i]=0;
		fset[i]=0.0;
		change++;
		break;
            }	
        }
    } else { /* new (or repeated) key */
	for (i=0; i<10; i++) 
	    if (keyset[i]==key) 
		break;
 	if(i==10) {
	    for (i=0; i<10; i++) 
	    	if (keyset[i]==0) 
		    break;
	    if (key==10)
		return 0;
	    for (ptr=hz; ptr->key; ptr++) {
		if (ptr->key==key) {
		    keyset[i]=key;
		    fset[i]=ptr->freq;
		    change++;
                }
            }
        }
    }
    if (!islast) return 0;
    if (change) {
        close(fd);
        change=0;
        count=0;
        fd=open("/dev/audio",O_WRONLY|O_NONBLOCK);
        if (fd<0) {
            fprintf(stderr,"/dev/audio: %s\n",strerror(errno));
            clean_up();
            exit(1);
        }
    }
    nnote=0; maxnote=0;
    for (i=0; i<10; i++)
        if (fset[i]) {
            nnote++;
            maxnote=i+1;
        }
    for (j=count; (j<count+2000) && (j<80000); j++) {
        val=128;
        for (i=0; i<maxnote; i++)
            if (fset[i])
                val += floor(120.0*sin(j*fset[i])/nnote);
        buffer[j]=(unsigned char)(val<128? val+128 : val^0xff);
    }
#if 0
    FD_ZERO(&fdset);
    FD_SET(fd,&fdset);
    while(select(fd+1,&fdset,NULL,NULL,&notime)==1 && count<j) {
        count += write(fd,buffer+count,j-count);
    }
#else
    count += write(fd,buffer+count,j-count);
#endif
    return 0;
}


int
main (int argc, char *argv[]) {
    int show_keycodes = 0;
    int i, j, n;
    int last=0;
    char buf[10];
    fd = 0; /* stdin */

	/*
		if we receive a signal, we want to exit nicely, in
		order not to leave the keyboard in an unusable mode
	*/
    signal(SIGHUP, die);
    signal(SIGINT, die);
    signal(SIGQUIT, die);
    signal(SIGILL, die);
    signal(SIGTRAP, die);
    signal(SIGABRT, die);
    signal(SIGIOT, die);
    signal(SIGFPE, die);
    signal(SIGKILL, die);
    signal(SIGUSR1, die);
    signal(SIGSEGV, die);
    signal(SIGUSR2, die);
    signal(SIGPIPE, die);
    signal(SIGTERM, die);
    signal(SIGSTKFLT, die);
    signal(SIGCHLD, die);
    signal(SIGCONT, die);
    signal(SIGSTOP, die);
    signal(SIGTSTP, die);
    signal(SIGTTIN, die);
    signal(SIGTTOU, die);

    get_mode();
    tmp = tcgetattr(fd, &old);
    if (tmp)
        printf("tcgetattr = %d\n", tmp);
    tmp = tcgetattr(fd, &new);
    if (tmp)
        printf("tcgetattr = %d\n", tmp);
    
    new.c_lflag &= ~ (ICANON | ECHO | ISIG);
    new.c_iflag = 0;
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 1;	/* 0.1 sec intercharacter timeout */
    
    tmp = tcsetattr(fd, TCSAFLUSH, &new);
    if (tmp)
        printf("tcsetattr = %d\n", tmp);
    if (ioctl(fd, KDSKBMODE,
              show_keycodes ? K_MEDIUMRAW : K_RAW)) {
        perror("KDSKBMODE");
        exit(1);
	}

    printf("Press <ESC> to terminate\n");
    while (1) {
	unsigned char key;
        n = read(fd, buf, sizeof(buf));
        for (i = 0; i < n; i++) {
	    key=buf[i];
	    /* printf("%02x\n",key); */
	    addkeytoset(key,i==n-1);

            if (key&0x80) {
		if ( (buf[i]&0x7f) == last) {
                    ioctl(fd,KIOCSOUND,0);	
                    last = 0;
                }
                continue;
            }
            if (buf[i]==last)
                continue;
            if (buf[i]==1) {
                clean_up();
                exit(0);
            }
            last = key;
	}
    }
	clean_up();
	exit(0);
}




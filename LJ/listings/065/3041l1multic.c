/* 
 * This is free software released under the GPL license.
 * See the GNU GPL for details.
 *
 * (c) Juan-Mariano de Goyeneche. 1998, 1999.
 *
 */

#include <stdio.h>          /* printf(), snprintf() */
#include <stdlib.h>         /* strtol(), exit() */
#include <sys/types.h>
#include <sys/socket.h>     /* socket(), setsockopt(), bind(), recvfrom(), sendto() */
#include <errno.h>          /* perror() */
#include <netinet/in.h>     /* IPPROTO_IP, sockaddr_in, htons(), htonl() */
#include <arpa/inet.h>      /* inet_addr() */
#include <unistd.h>         /* fork(), sleep() */
#include <sys/utsname.h>    /* uname() */
#include <string.h>         /* memset() */

#define MAXLEN 1024
#define DELAY 2
#define TTL 1


int main(int argc, char* argv[])
{
    u_char no = 0;
    u_int yes = 1;      /* Used with SO_REUSEADDR. In Linux both u_int */
                        /* and u_char are valid. */
    int send_s, recv_s;     /* Sockets for sending and receiving. */
    u_char ttl;
    struct sockaddr_in mcast_group;
    struct ip_mreq mreq;
    struct utsname name;

    if ((argc<3) || (argc>4)) {
        fprintf(stderr, "Usage: %s mcast_group port [ttl]\n", argv[0]);
        exit(1);
    }

    memset(&mcast_group, 0, sizeof(mcast_group));
    mcast_group.sin_family = AF_INET;
    mcast_group.sin_port = htons((unsigned short int)strtol(argv[2], NULL, 0));
    mcast_group.sin_addr.s_addr = inet_addr(argv[1]);
    
    if ( (send_s=socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror ("send socket");
        exit(1);
    }   

    /* If ttl supplied, set it */   
    if (argc == 4) {
        ttl = strtol(argv[3], NULL, 0);
    } else {
        ttl = TTL;
    }

    if (setsockopt(send_s, IPPROTO_IP, IP_MULTICAST_TTL, &ttl,
            sizeof(ttl)) < 0) {
        perror ("ttl setsockopt");
        exit(1);
    }

    /* Disable Loop-back */
    if (setsockopt(send_s, IPPROTO_IP, IP_MULTICAST_LOOP, &no,
        sizeof(no)) < 0) {
        perror ("loop setsockopt");
        exit(1);
    }

    if ( (recv_s=socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror ("recv socket");
        exit(1);
    }

    if (setsockopt(recv_s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("reuseaddr setsockopt");
        exit(1);
    }

    if (bind(recv_s, (struct sockaddr*)&mcast_group, sizeof(mcast_group)) < 0) {
        perror ("bind");
        exit(1);
    }

    /* Tell the kernel we want to join that multicast group. */
    mreq.imr_multiaddr = mcast_group.sin_addr;
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(recv_s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror ("add_membership setsockopt");
        exit(1);
    }

    if (uname(&name) < 0) {
        perror ("uname");
        exit(1);
    }

    switch (fork()) {
        case -1: /* Error fork()ing */
            perror("fork");
            exit(1);
        case 0: {  /* Child -> receive. */
            int n;
            int len;
            struct sockaddr_in from;
            char message [MAXLEN+1];

            for (;;) {
                len=sizeof(from);
                if ( (n=recvfrom(recv_s, message, MAXLEN, 0, 
                                (struct sockaddr*)&from, &len)) < 0) {
                    perror ("recv");
                    exit(1);
                }
                message[n] = 0; /* null-terminate string */
                printf("%s: Received message from %s.\n", name.nodename,
                        inet_ntoa(from.sin_addr));
                printf("\t%s", message);
            }
            /* Not reached. */
        }
        default: { /* Parent -> send. */
            char message [MAXLEN];
            snprintf (message, sizeof(message), "Hi, I'm %s. "
                   "Merry Christmas!\t(TTL==%d)\n", name.nodename, ttl);
            for (;;) {
                if (sendto(send_s, message, strlen(message), 0,
                            (struct sockaddr*)&mcast_group, 
                            sizeof(mcast_group)) < strlen (message)) {
                    perror("sendto");
                    exit(1);
                }
                sleep(DELAY);
            }   
            /* Not reached. */
        }
    }
    /* Not really reached. */
}

// author : Asanga Udugama (adu@comnets.uni-bremen.de)
// date : 2005-09-10
// compile & link : gcc -o mon_routing_table mon_routing_table.c 

#include <bits/sockaddr.h>
#include <asm/types.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

// buffer to hold the RTNETLINK request
struct {
  struct nlmsghdr nl;
  struct rtmsg    rt;
  char            buf[8192];
} req;

// variables used for
// socket communications
int fd;
struct sockaddr_nl la;
struct sockaddr_nl pa;
struct msghdr msg;
struct iovec iov;
int rtn;

// buffer to hold the RTNETLINK reply(ies)
char buf[8192];

// RTNETLINK message pointers & lengths
// used when processing messages
struct nlmsghdr *nlp;
int nll;
struct rtmsg *rtp;
int rtl;
struct rtattr *rtap;

void recv_reply()
{
  char *p;

  // initialize the socket read buffer
  bzero(buf, sizeof(buf));

  p = buf;
  nll = 0;

  // read from the socket until the NLMSG_DONE is
  // returned in the type of the RTNETLINK message
  // or if it was a monitoring socket
  while(1) {
    rtn = recv(fd, p, sizeof(buf) - nll, 0);

    nlp = (struct nlmsghdr *) p;

    if(nlp->nlmsg_type == NLMSG_DONE) 
      break;

    // increment the buffer pointer to place
    // next message
    p += rtn;

    // increment the total size by the size of 
    // the last received message
    nll += rtn;

    if((la.nl_groups & RTMGRP_IPV4_ROUTE) 
                      == RTMGRP_IPV4_ROUTE)
      break;
  }
}

void read_reply()
{
  // string to hold content of the route 
  // table (i.e. one entry) 
  char dsts[24], gws[24], ifs[16], ms[24];

  // outer loop: loops thru all the NETLINK
  // headers that also include the route entry
  // header 
  nlp = (struct nlmsghdr *) buf;
  for(;NLMSG_OK(nlp, nll);nlp=NLMSG_NEXT(nlp, nll))
  {

    // get route entry header
    rtp = (struct rtmsg *) NLMSG_DATA(nlp);

    // we are only concerned about the
    // main route table
    if(rtp->rtm_table != RT_TABLE_MAIN)
      continue;

    // init all the strings
    bzero(dsts, sizeof(dsts));
    bzero(gws, sizeof(gws));
    bzero(ifs, sizeof(ifs));
    bzero(ms, sizeof(ms));

    // inner loop: loop thru all the attributes of
    // one route entry 
    rtap = (struct rtattr *) RTM_RTA(rtp);
    rtl = RTM_PAYLOAD(nlp);
    for(;RTA_OK(rtap, rtl);rtap=RTA_NEXT(rtap,rtl))
    {
      switch(rtap->rta_type) 
      {
        // destination IPv4 address
        case RTA_DST:
          inet_ntop(AF_INET, RTA_DATA(rtap), 
                                     dsts, 24);
          break;

        // next hop IPv4 address
        case RTA_GATEWAY:
          inet_ntop(AF_INET, RTA_DATA(rtap), 
                                     gws, 24);
          break;

        // unique ID associated with the network 
        // interface
        case RTA_OIF:
          sprintf(ifs, "%d", 
                   *((int *) RTA_DATA(rtap)));
        default:
          break;
      }
    }
    sprintf(ms, "%d", rtp->rtm_dst_len);

    printf("dst %s/%s gw %s if %s\n", 
                          dsts, ms, gws, ifs);
  }
}

int main(int argc, char *argv[])
{

  // open socket
  fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

  // setup local address & bind using
  // this address
  bzero(&la, sizeof(la));
  la.nl_family = AF_NETLINK;
  la.nl_pid = getpid();
  la.nl_groups = RTMGRP_IPV4_ROUTE | RTMGRP_NOTIFY;
  bind(fd, (struct sockaddr*) &la, sizeof(la));


  // sub functions to create RTNETLINK message,
  // send over socket, reveive reply & process
  // message
  recv_reply();
  read_reply();

  // close socket
  close(fd);
}

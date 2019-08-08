// author : Asanga Udugama (adu@comnets.uni-bremen.de)
// date : 2005-09-10
// compile & link : gcc -o set_routing_table set_routing_table.c 

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

void send_request()
{
  // create the remote address
  // to communicate
  bzero(&pa, sizeof(pa));
  pa.nl_family = AF_NETLINK;

  // initialize & create the struct msghdr supplied
  // to the sendmsg() function
  bzero(&msg, sizeof(msg));
  msg.msg_name = (void *) &pa;
  msg.msg_namelen = sizeof(pa);

  // place the pointer & size of the RTNETLINK
  // message in the struct msghdr 
  iov.iov_base = (void *) &req.nl;
  iov.iov_len = req.nl.nlmsg_len;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  // send the RTNETLINK message to kernel
  rtn = sendmsg(fd, &msg, 0);
}


void form_request()
{
  // attributes of the route entry
  char dsts[24] = "134.102.155.14";
  int ifcn = 2, pn = 32;

  // initalize RTNETLINK request buffer
  bzero(&req, sizeof(req));

  // compute the initial length of the
  // service request
  rtl = sizeof(struct rtmsg);

  // add first attrib: 
  // set destnation IP addr and increment the 
  // RTNETLINK buffer size 
  rtap = (struct rtattr *) req.buf;
  rtap->rta_type = RTA_DST;
  rtap->rta_len = sizeof(struct rtattr) + 4;
  inet_pton(AF_INET, dsts,
     ((char *)rtap) + sizeof(struct rtattr));
  rtl += rtap->rta_len;

  // add second attrib: 
  // set ifc index and increment the size 
  rtap = (struct rtattr *) (((char *)rtap)
            + rtap->rta_len);
  rtap->rta_type = RTA_OIF;
  rtap->rta_len = sizeof(struct rtattr) + 4;
  memcpy(((char *)rtap) + sizeof(struct rtattr),
           &ifcn, 4);
  rtl += rtap->rta_len;

  // setup the NETLINK header
  req.nl.nlmsg_len = NLMSG_LENGTH(rtl);
  req.nl.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE;
  req.nl.nlmsg_type = RTM_NEWROUTE;

  // setup the service header (struct rtmsg)
  req.rt.rtm_family = AF_INET;
  req.rt.rtm_table = RT_TABLE_MAIN;
  req.rt.rtm_protocol = RTPROT_STATIC;
  req.rt.rtm_scope = RT_SCOPE_UNIVERSE;
  req.rt.rtm_type = RTN_UNICAST;
  // set the network prefix size
  req.rt.rtm_dst_len = pn;
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
  bind(fd, (struct sockaddr*) &la, sizeof(la));


  // sub functions to create RTNETLINK message,
  // send over socket, reveive reply & process
  // message
  form_request();
  send_request();

  // close socket
  close(fd);
}

#include <linux/linkage.h>
#include <linux/unistd.h>
#include <stdio.h>
#include <string.h>

#define __NR_my_sys_call 223

#include "ftp.h"
_syscall1(long long int, my_sys_call, struct params *, p);
unsigned char *getip(char *);

int main(int argc, char **argv)
{
  int i;
  long long r = -1;    
  struct params pm;
  if (argc != 7) {
    fprintf(stderr, "usage: %s <source ip> <destination ip> src filename> <dst filename> <username> <password>\n", argv[0]);
    exit(-1);
  }
  strncpy(pm.srcip,getip(argv[1]),4);
  strncpy(pm.destip,getip(argv[2]),4);

  strcpy(pm.src, argv[3]);
  strcpy(pm.dst, argv[4]);

  strcpy(pm.user, argv[5]);
  strcpy(pm.pass, argv[6]);
  r = my_sys_call(&pm);
  printf("return = %d (length of file copied)\n", r);
  return 0;
}
unsigned char *getip(char *s)
{
  unsigned char *str;
  unsigned char *ip;
  int i = 0;
  ip = (unsigned char *) calloc(4,sizeof(unsigned char));
  str = strtok(s,".");
  ip[i] = atoi(str);
  ++i;
  while(str = strtok(NULL,".")) {
	  ip[i] = atoi(str);
	  ++i;
  }
  return ip;
}

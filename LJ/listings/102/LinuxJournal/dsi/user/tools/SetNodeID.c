/* Set the Node ID of DSI cluster architecture */

#include <stdio.h>
#include <linux/unistd.h>
#include <stdlib.h>
#include "dsi.h"

_syscall3(int, security, unsigned int, id, unsigned int, call, unsigned int *, args);



int main(int argc, char **argv)
{
	int ret;
	unsigned int id	= DSI_MAGIC;
	unsigned int call = DSI_SET_NODE_ID;
	int snid;
	unsigned int args[2];


	if ( argc != 2 ) {
		printf("Usage: SetNodeID  snid\n");
		return 2;
	}


	snid = atoi(argv[1]);

	args[0] = snid;
	ret = security( id, call, (unsigned int *)args);
	printf("ret=%d\n",ret);
  
	return 0;
}


#include <linux/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "dsi.h"

_syscall3(int, security, unsigned int, id, unsigned int, call, unsigned int *, args);


int main(int argc, char **argv)
{
	int ret;
	unsigned int id	= DSI_MAGIC;
	unsigned int call 	= DSI_SET_POLICY;
	unsigned int args[4]	;
	unsigned int rc = 1;
	int ssid, tsid, tclass, permission;

	FILE *policy = fopen(argv[1], "r");

	if ( argc != 2 ) {
		printf("Usage: UpdatePolicy policy_filename\n");
		return 2;
	}

	if ( policy == NULL ) {
		printf("Error opening file %s\n", argv[1]);
		return 1;
	}

	rc = fscanf(policy, "%d %d %d %x", &ssid, &tsid, &tclass, &permission);

	while ( (rc != 0) && (rc != EOF) ) {
		args[0] = ssid;
		args[1] = tsid;
		args[2] = tclass;
		args[3] = permission;

		printf("%d %d %d %x\n", ssid, tsid, tclass, permission);

		ret = security( id, call, (unsigned int *)args);
		printf("ret=%d\n",ret);
    

		rc = fscanf(policy, "%d %d %d %x", &ssid, &tsid, &tclass, &permission);
	}

	if ( rc == 0 ) printf("Invalid input in policy\n");

	return 0;
}


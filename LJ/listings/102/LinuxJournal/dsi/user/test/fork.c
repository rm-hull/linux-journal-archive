#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>

int main()
{
	pid_t child_pid;
	int child_status;
	int i;

	printf("Start fork test\n");

	for(i=0;i<100000;i++) {	
		child_pid = fork();
		if(child_pid != 0) {
			wait(&child_status);
		} else {
			exit(0);
		}
	}

	return(0);
}

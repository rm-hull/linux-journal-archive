#include <stdio.h>

/* Example of Post-Read */

int main(void) 
{
	char *foo;

	foo = (char *) malloc(10);
	memset(foo,0x20,sizeof(foo));
	foo[0] = 'e';
	foo[1] = 'x';
	foo[2] = 'a';

	/* The string is not null terminated */
	printf("%s\n",foo);

	free(foo);
	
	return(0);
}

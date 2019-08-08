#include <stdio.h>
#include <string.h>

/* Example of Pre-Read */

int main(void) 
{
	char *foo;

	foo = (char *) malloc(10);
	strcpy(foo,"hello");

	foo--;

	/* The printf access before the allocated area */
	printf("%s\n",foo);

	free(foo);

	return(0);
}

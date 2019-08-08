#include <stdio.h>
#include <string.h>

/* Example of Pre-Write */

int main(void) 
{
	char *foo;

	foo = (char *) malloc(10);
	foo-;

	/* The strcpy access before the allocated area */
	strcpy(foo,"123456789");

	printf("%s\n",foo);

	free(foo);

	return(0);
}

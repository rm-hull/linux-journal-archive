#include <stdio.h>
#include <string.h>

/* Example of Post-Write */

int main(void) 
{
	char *foo;

	foo = (char *) malloc(10);

	/* The strcpy write beyond the allocated area */
	strcpy(foo,"12345678901");

	printf("%s\n",foo);

	free(foo);

	return(0);
}

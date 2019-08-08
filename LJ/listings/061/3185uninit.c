/* Example of use of uninitialized memory area */

#include <stdio.h>

int main(void)
{

	char *ptr = NULL;

	*ptr = 10;
	
	return(0);
}

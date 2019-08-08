/* Example of missed memory free */

#include <stdio.h>

int main(void)
{

	char *ptr;
	
	ptr = (char *) calloc(5,sizeof(char) * 10);
	
	/* I forgot to release the memory */

	return(0);
}

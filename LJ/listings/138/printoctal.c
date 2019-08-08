#include <stdio.h>

void printoctal(unsigned char *octaltext, int octallength)
{
	int i;
	for(i=0; i< octallength; i++){
		printf("\\%03o", octaltext[i]);
		if( i > 15 && !(i%16)) 
			printf("\n");
	}
	printf("\n");
}

#include <stdio.h>
#include <ctype.h>

unsigned char * hex2octal (char *hexstring)
{
        unsigned char *ostring=NULL, octval=0;
	unsigned char hex1=0, hex2=0;
	int decval = 0;
        int hexlength=0, olength=0, next=0;
        int no_of_words=0, i=0;

	/*
		computations to define size of the octal string
	*/
	hexlength = (int) strlen(hexstring);
        no_of_words = (int) hexlength/9;
        olength = (int) (no_of_words*4) + 1;
        ostring = (unsigned char *) calloc( olength,  sizeof(char));

	/*
		Convert hex to octal string
	*/
        for(i=0; i< hexlength; i++){
		/* Ignore space */
		if(isspace(hexstring[i])) 
			continue;
		hex1 = (unsigned char) hexstring[i++];
		hex2 = (unsigned char) hexstring[i];	
		decval = (int) hex2dec(hex1, hex2);
		ostring[next++] =  (unsigned char) decval;
        }
	ostring[next] = '\0';
        return ostring;
}

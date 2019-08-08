#include <stdio.h>
#include <ctype.h>

int decimal( unsigned char hex)
{
        int val=0;
        if(isalpha(hex)){
                val = (toupper(hex) - 'A') + 10;
        }else{
                val = hex - '0';
        }
        return val;
}
                                                                                                                            
int hex2dec(unsigned char hex1, unsigned char hex2)
{
        return( decimal(hex1) * 16 + decimal(hex2));
}


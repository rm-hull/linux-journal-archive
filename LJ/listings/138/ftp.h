#ifndef __FTP_H
#define __FTP_H

struct params {
    unsigned char destip[4]; 
    unsigned char srcip[4]; 
    char src[64]; 
    char dst[64];
    char user[16];
    char pass[64];
};
#endif

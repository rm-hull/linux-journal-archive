#include <stdio.h>
#include <string.h>
#include "data.h"
#define SIZE_BUF 512

/* Function defs */
char * read_file(char * file);
Hexdata * hexparse(char * hexunparsed, Hexdata * data);

Hexdata * hextract(char * execfile, Hexdata * data)
{
	char *tempfile;
	char *hexunparsed;
	char objcmnd[SIZE_BUF];

	tempfile = tmpnam(NULL);
	sprintf(objcmnd, "/usr/bin/objdump --section=.data -s %s > %s\n", execfile, tempfile);
	system(objcmnd);
	hexunparsed =  read_file(tempfile);
	unlink(tempfile);
	hexparse(hexunparsed, data);
	free(hexunparsed);
	return(data);
}

#define SIZE 4096
char * read_file(char * file)
{
        FILE * fp;
        char * hexunparsed;
	char tmptxt[SIZE];
        int cnt=0, l=0;
	tmptxt[0] = '\0';

        hexunparsed = (char *) malloc(SIZE);
        if (!hexunparsed)
                return NULL;
        fp = fopen(file, "r");
        if (!fp)
                return NULL;

	/* Ignore parts before the data section */
	while(1){
		if(!fgets(tmptxt, SIZE, fp))
			return NULL;
		if(strstr(tmptxt, "Contents of section .data:"))
			break;
	}
	/* Now read the data section */
        for (l = 0;;) {
                hexunparsed = (char *) realloc(hexunparsed, l + SIZE);
                if (!hexunparsed)
                        return NULL;
                cnt = fread(&hexunparsed[l], 1, SIZE, fp);
                if (!cnt)
                        break;
                l += cnt;
        }
        fclose(fp);
        hexunparsed = (char *) realloc(hexunparsed, l + 1);
        if (!hexunparsed)
                return NULL;
        hexunparsed[l] = '\0';
	return hexunparsed;
}

Hexdata * hexparse( char *hexunparsed, Hexdata * data)
{
	char tempbuf[SIZE];
	int hexaddress=0;
	char s2[56], s3[56], s4[56], s5[56], junk[56];
	char * hexstream;
	int s_char;
	int first = 0,i=0, pos=0, nscan=0;
	char * tok;

	/* Now extract useful data only */
	hexstream = (char *) malloc((int)strlen(hexunparsed) * sizeof(char)); 
	if(!(hexstream))
		return NULL;
	
	sprintf(hexstream, " ");

	first = 1;
	s_char = ' ';
	pos = -1;
	while(1){
		tok = (char *)strchr(hexunparsed + pos + 1, (char) s_char);
		if(tok == NULL)
			break;
		nscan = sscanf(tok, "%x %s %s %s %s %s", &hexaddress, s2, s3, s4, s5, junk);
		if(nscan <= 2 || nscan > 6)
			break;
		if(nscan == 3){
			sprintf(tempbuf, "%s ", s2);
		}
		if(nscan == 4){
			sprintf(tempbuf, "%s %s ", s2, s3);
		}
		if(nscan == 5){
			sprintf(tempbuf, "%s %s %s ", s2, s3, s4);
		}
		if(nscan == 6){
			sprintf(tempbuf, "%s %s %s %s ", s2, s3, s4, s5);
		}
		strcat(hexstream, tempbuf);
		pos = tok - hexunparsed;
		/* capture first address */
		if(first){
			data->startaddress = hexaddress;
			first = 0;
			s_char = '\n';
			continue;
		} else {
		/* Capture the end address */
			data->endaddress = hexaddress;
		}
	}
	data->hexstream = hexstream;
	return data;
}

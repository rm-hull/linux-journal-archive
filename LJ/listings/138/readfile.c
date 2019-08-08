#include <stdio.h>

#define SIZE 4096
char * read_file(char * file)
{
        FILE * i;
        char * text;
        int cnt=0, l=0;
                                                                                                            
        text = (char *) malloc(SIZE);
        if (!text)
                return NULL;
        i = fopen(file, "r");
        if (!i)
                return NULL;
        for (l = 0;;) {
                text = (char *) realloc(text, l + SIZE);
                if (!text)
                        return NULL;
                cnt = fread(&text[l], 1, SIZE, i);
                if (!cnt)
                        break;
                l += cnt;
        }
        fclose(i);
        text = (char *) realloc(text, l + 1);
        if (!text)
                return NULL;
        text[l] = '\0';
                                                                                                            
	return text;
}


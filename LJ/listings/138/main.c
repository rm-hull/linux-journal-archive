#include <stdio.h>
#include "data.h"
#define DEBUGEXEC 0
unsigned char *hex2octal(char *hextext);
Hexdata *hextract (char *execfile, Hexdata *data);
Arrayparam *arrayp(char *arrayparamfile, Arrayparam *arrayparams);

void printoctal(unsigned char *octaltext, int olength);

void usage(char *progname)
{
	fprintf(stderr, "USAGE:\n\t%s Executable_Program Array_Parameters_File\n", progname);
}

int main (int argc, char *argv[])
{
	char *executable, *parameters;
	int olength=0;
	Hexdata data;
	Arrayparam ap;

	if (argc != 3){
		usage(argv[0]);
		return 0;
	}

	executable = argv[1];
	parameters = argv[2];


	if(!hextract(executable, &data)){
		fprintf(stderr, "Hex data extraction error\n");
		return 0;
	}
	data.octaltext = hex2octal(data.hexstream);
        /*
                computations to define size of the octal string
		Do not rely on NULL character. 
        */
        olength = (int) (strlen(data.hexstream)/9)*4 + 1;
#if DEBUGEXEC
	printoctal(data.octaltext, olength);
#endif
	arrayp(parameters, &ap);
	assignarray(&ap, &data);
#if DEBUGEXEC
	printparams(&ap);
#endif
	decrypt(&ap);
	free(data.hexstream);
	fprintf(stderr, "\n[If clear text is not visible then add \"relax 1 1\" to input file %s]\n", parameters);
}

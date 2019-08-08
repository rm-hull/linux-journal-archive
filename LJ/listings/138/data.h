typedef struct hexdata {
	int startaddress;
	int endaddress;
	char *hexstream;
	unsigned char *octaltext;
} Hexdata;

typedef struct arrayparam {
	int pswd_len;
	long int pswd_addrs;
	char *pswd;
	int shll_len;
	long int shll_addrs;
	char *shll;
	int inlo_len;
	long int inlo_addrs;
	char *inlo;
	int xecc_len;
	long int xecc_addrs;
	char *xecc;
	int lsto_len;
	long int lsto_addrs;
	char *lsto;
	int chk1_len;
	long int chk1_addrs;
	char *chk1;
	int opts_len;
	long int opts_addrs;
	char *opts;
	int txt_len;
	long int txt_addrs;
	char *txt;
	int relax;
} Arrayparam;

Arrayparam *readparamfile(char *arrayparamfile, Arrayparam *arrayparams);
Arrayparam *arrayp(char *arrayparamfile, Arrayparam *arrayparams);
Arrayparam *assignarray(Arrayparam *ap, Hexdata *hd);
void printparams(Arrayparam *arrayparams);


/*
faxgate.h
Copyright David Weis, 1997
*/

/* configuration stuff */
#define LISTDIR "/data/fax/config/"
#define PSFILEDIR "/data/fax/psfiles/"

/* http header */
#define TTYPE(x) fprintf(cgiOut, "Content-Type: %s\n\n", ##x##);

/* define some common html tags. cgic.h -must- be included */
#define THTML fprintf(cgiOut, "<HTML>\n");
#define THTMLE fprintf(cgiOut, "</HTML>\n");
#define THEAD fprintf(cgiOut, "<HEAD>\n");
#define THEADE fprintf(cgiOut, "</HEAD>\n");
#define TTITLE(x) fprintf(cgiOut, "<TITLE>%s</TITLE>\n", ##x##);

#define TBODYA(x) fprintf(cgiOut, "<BODY %s>\n", ##x##);
#define TBODY fprintf(cgiOut, "<BODY>\n");
#define TBODYE fprintf(cgiOut, "</BODY>\n");

#define TP fprintf(cgiOut, "<P>\n");

struct docinfo {
  char name[100];
  char faxnum[100];
  char from[100];
  char comments[200];
  int numdocs;
  char docs[10][100]; /* room for 10 pathnames */
} docinfo;

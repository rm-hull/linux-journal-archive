
#include "cgic.h"
#include "faxgate.h"
#include <errno.h>

int cgiMain()
{
  FILE * faxstat;
  char line[80];

  TTYPE("text/html");

  THTML;
  THEAD;
  TTITLE("System Status");
  THEADE;

  TBODYA("BGCOLOR=#ffffff");


  fprintf(cgiOut, "<CENTER><FONT COLOR=red><H1>System Status</H1></FONT>");

  fprintf(cgiOut, "<IMG SRC=""images/logo.gif""></CENTER>");

  fprintf(cgiOut, "<H3>");

  fprintf( cgiOut, "Your hostname is %s with IP address %s", cgiRemoteHost, cgiRemoteAddr);
  
  fprintf(cgiOut, "</H3><P>\n");
  TP;
  
  fprintf(cgiOut, "<PRE>\n");

  faxstat = popen("/usr/local/bin/faxstat -sf", "r");
  while (1) {
    fgets(line, 80, faxstat);
    if (feof(faxstat))
      break;
    fprintf(cgiOut, "%s", line);
  }
  pclose(faxstat);
  
  fprintf(cgiOut, "</PRE>\n");

  TBODYE;
  THTMLE;

  return(0);

}



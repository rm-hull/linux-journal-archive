
/*
fax.c - part of faxgate
Copyright 1997, David Weis
*/

#include "cgic.h"
#include "faxgate.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <stdlib.h>


/* decls */
void print_form();
void process_form();
void print_doclist();
void print_reciplist();
void print_list(char * decl_line, char * directory);
void send_to_person(struct docinfo * request);
void send_to_list(struct docinfo * request, char * listname);
void print_user();
void get_user_gecos(char * name);

void print_form()
{
  FILE * template;
  char linein[200];

  /* read the template html file and sub the doclist and reciplist */
  template = fopen("/home/httpd/html/fax/fax.html", "r");

  if (!template) {
    printf("template is null, errno is %i\n", errno);
  }

  while (!feof(template)) {
    fgets(linein, sizeof(linein), template);

    /* check for **DOCLIST** or **RECIPLIST** or **FROM** */
    if (strstr(linein, "**")) {
      if (strstr(linein, "DOCLIST"))
	print_doclist();
      else if (strstr(linein, "RECIPLIST"))
	print_reciplist();
      else if (strstr(linein, "FROM"))
	print_user();
      else
	printf("<H1>Unrecognized Directive</H1>");
    } else {
      puts(linein);
    } /* if ** */
    
  } /* while */

}


void get_user_gecos(char * name)
{

  char * username;
  char * hostname = cgiRemoteHost;
  struct passwd * pw;

  /* chop off the domain name from the client machine name */
  username = strtok(hostname, ".");

  /* see if the user exists */
  pw = getpwnam(username);

  if (pw) {
    strcpy(name, pw->pw_gecos);
  } else {
    strcpy(name, hostname);
  }

}


void print_user()
{
  char username[50];

  get_user_gecos(username);

  fprintf(cgiOut, "\"%s\"", username);

}


void print_doclist()
{
  print_list("<SELECT NAME=\"doclist\" MULTIPLE>", PSFILEDIR);
}


void print_reciplist()
{
  print_list("<SELECT NAME=\"reciplist\">", LISTDIR);
}


void print_list(char * decl_line, char * directory)
{

  DIR * dp;
  struct dirent *dir;

  if ((dp = opendir(directory)) == NULL) {
    printf("<H1>ERROR!</H1>\n");
    /* should quit */
  }

  fprintf(cgiOut, decl_line);
  fprintf(cgiOut, "\n");

  while ((dir = readdir(dp))) {
    if (dir->d_name[0] != '.') /* skip hidden stuff */
      printf("<OPTION VALUE=\"%s\">%s\n", dir->d_name, dir->d_name);
  }

  fprintf(cgiOut, "</SELECT>\n");

  closedir(dp);

}


void send_to_person(struct docinfo * request)
{
  /*  char username[50]; */
  char cmdline[600];
  char scratch[100];
  int i;
  FILE * cmd;

#ifdef DEBUG
  printf("send_to_person %s\n", request->name);
#endif

  /*   get_user_gecos(username); */

  strcpy(cmdline, "/usr/local/bin/sendfax ");

  sprintf(scratch, "-c \"%s\" ", request->comments); 
  strcat(cmdline, scratch);


  sprintf(scratch, "-d \"%s@%s\" -f \"%s\" ", request->name, request->faxnum,
	  request->from);
  strcat(cmdline, scratch);

  for ( i = 0 ; i < request->numdocs; i++) {
    sprintf(scratch, "%s%s ", PSFILEDIR, request->docs[i]);
    strcat(cmdline, scratch);
  }

  cmd = popen(cmdline, "r");
  while (!feof(cmd)) {
    fgets(scratch, sizeof(scratch), cmd);
#ifdef DEBUG
    fprintf(cgiOut, scratch);
#endif
  }
  pclose(cmd);

#ifdef DEBUG
  printf("%s", cmdline);
#endif


  fprintf(cgiOut, "<P><H3>Notice: Your fax to %s at %s was sucessfully queued.</H3><P>\n", request->name, request->faxnum);

}


void send_to_list(struct docinfo * request, char * listname)
{
  FILE * reciplist;
  char filename[200];

  sprintf(filename, "%s%s", LISTDIR, listname);
  reciplist = fopen(filename, "r");

  for (;;) {
    fgets(request->name, sizeof(request->name), reciplist);
    request->name[strlen(request->name) - 1] = 0x00;
    if (feof(reciplist))
      break;
    fgets(request->faxnum, sizeof(request->faxnum), reciplist);
    request->faxnum[strlen(request->faxnum) - 1] = 0x00;

    send_to_person(request);

  }

#ifdef DEBUG
  printf("send_to_list %s\n", listname);
#endif

}


void remove_returns(char * instring)
{
  char * workstring = instring;

  while (*workstring) {
    if ((*workstring == 10) || (*workstring == 13))
      *workstring = ' ';  /* space */
    workstring++;
  }


}


void process_form()
{
  int num_recips; /* 1 for single, 2 for mult */
  char ** doclist;
  struct docinfo request;
  int i;
  int errs = 0;

  THTML;
  THEAD;
  TTITLE("Confirm");
  THEADE;
  TBODYA("BGCOLOR=white");
  fprintf(cgiOut, "<CENTER><H1><FONT COLOR=red>Fax Confirmation</FONT></H1>\n");
  fprintf(cgiOut, "<IMG SRC=\"images/logo.gif\"></CENTER>\n");


  cgiFormInteger("mult", &num_recips, 1);

  i = cgiFormStringMultiple("doclist", &doclist);
  if ( i == cgiFormNotFound) {
    /* forgot to pick any docs */
    fprintf(cgiOut, "<H3>You forgot to pick one or more documents to send!</H3>\n");
    cgiStringArrayFree(doclist);
    TBODYE;
    THTMLE;
    exit(0);
  } /* doclist not found */

  /* put the requested documents into the request struct */
  i = 0;
  while (doclist[i]) {
    strcpy(request.docs[i], doclist[i]);
#ifdef DEBUG
    printf("%s\n", doclist[i]);
#endif
    i++;
  }
  request.numdocs = i;
#ifdef DEBUG
  printf("numdocs %i\n", request.numdocs);
#endif

  cgiStringArrayFree(doclist);

  /* decide 1 or multiple */
  if (num_recips == 1) {
    /* process to send it to one person */
    i = cgiFormStringNoNewlines("name", request.name, sizeof(request.name));
    if ( i == cgiFormEmpty )
      fprintf(cgiOut, "<H3>Warning: The fax was sent with no recipient name on the cover page</H3><P>\n");
    i  = cgiFormStringNoNewlines("faxnumber", request.faxnum, sizeof(request.faxnum));
    if ( i == cgiFormEmpty ) {
      fprintf(cgiOut, "<H3>Error: Fax Number not entered but is required!</H3>\n");
      errs++;
    }
    i = cgiFormStringNoNewlines("from", request.from, sizeof(request.from));
    if ( i == cgiFormEmpty )
      fprintf(cgiOut, "<H3>Warning: The fax was sent with no sender name on the cover page</H3><P>\n");

    i = cgiFormString("comments", request.comments, sizeof(request.comments));
    remove_returns(request.comments);

    if (errs)
      exit(0); /* don't do if it there is a problem, done here to get all messages out */
    send_to_person(&request);

  } else {
    /* process to send it to a list */
    char reciplist[100];

    cgiFormStringNoNewlines("reciplist", reciplist, sizeof(reciplist));
    send_to_list(&request, reciplist);

  }

  TBODYE;
  THTMLE;

}


int cgiMain()
{

  /* output content type */
  cgiHeaderContentType("text/html");

  /* get method - if it's a get */
  if (!strcmp(cgiRequestMethod, "GET")) {
    /* print the form */
    print_form();
  } else {
    /* else a post */
    /* take the info and process */
    process_form();
  }

  return(0);
}


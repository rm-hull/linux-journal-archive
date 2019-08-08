/**************************************************************************
*
*                              funcs.h
*
*     Programmer:    Graydon Ekdahl
*
*     Date:          06.15.97
*
*     What:          defines support functions for deltree.cpp
*
****************************************************************************/

#ifndef FUNCS
#define FUNCS

#include "strstack.h"

#include <errno.h>
#include <stdio.h>
#include <iostream.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <ftw.h>
#include <sys/stat.h>
#include <stack.h>
#include <termios.h>

#define DIRSIZE 256  //  max size of file pathname
#define DEPTH 8      //  max number of levels in directory tree open at once
#define ERROR -1     


void  Center ( int linenum, char * str );

void  CheckErrNo ( char * path );

void  Clrscr ( void );

int   DelEntry ( char * file, struct stat *sb, int flag );      

void  DelFiles ( char * path );

void  DelDirectories ( void );      

void  GotoXY ( int x, int y );
         
int   Wish_to_Continue ( char * path );

#endif



/**************************************************************************
*
*                              funcs.cpp
*
*     Programmer:    Graydon Ekdahl
*
*     Date:          06.15.97
*
*     What:          support functions for deltree.cpp
*
****************************************************************************/

#include "funcs.h"

StrStack Stk;        //  external StrStack variable global to this file

void Clrscr ( void )
   {
   cout << char ( 27 ) << "[H"  <<          
           char ( 27 ) << "[2J" << flush;  // home cursor, erase display
   }
      
void GotoXY ( int x, int y )
   {
   cout << char ( 27 ) << "[" << y << ";" << x << "H" << flush;
   return;
   }

void Center ( int linenum, char * str )
   {
   int htab = 40 - ( strlen ( str ) / 2 );
   GotoXY ( htab, linenum );
   cout << str << endl;
   }
     
int Wish_to_Continue ( char * path )
   {      
   char ch;
   cout << "deltree will delete " << path << ", its files " <<
           "and its subdirectories." << endl << endl;
   cout << "Do you wish to continue? (Y/N)" << endl;
   cin.get ( ch );
   cin.ignore ( 80, '\n' );
   if ( ( ch == 'Y') || ( ch == 'y' ) )
      {
      cout << "Are you sure?  (Y/N)" << endl;
      cin.get ( ch );
      cin.ignore ( 80, '\n' );
      if ( ( ch == 'Y' ) || ( ch == 'y' ) )
         return 1;
      }   
   return 0;
   }

void CheckErrNo ( char * path )
   {   
   switch ( errno )     //  errno is a system global value
      {
      case  EFAULT  :   cout << "Path points outside your addr. space:  " 
                             << path << endl;
                        break;
                        
      case  EACCES  :   cout << "Access denied:  " << path << endl;
                        break;                       
                        
      case  EPERM   :   cout << "Permanent Entry:  " << path << endl; 
                        break;
                        
      case  ENOENT  :   cout << path << " does not exist." << endl;
                        break;
                        
      case  EISDIR  :   cout << path << " refers to a directory." << endl;
                        break;
                        
      case  ENOMEM  :   cout << path << ".  Insufficient kernel memory. "
                             << "Exiting..." << endl;
                        exit ( ENOMEM );
                        break;                       

      case  EROFS   :   cout << path << " is read only." << endl;
                        break;

      case ENOTDIR  :   cout << path << ".  Component in path not a directory."
                             << endl;                       
                        break;
      }                            
   }
  
int DelEntry ( char * file, struct stat *sb, int flag )
   {
   int result = 0;
   switch ( flag )
      {
      case FTW_F:       if ( ( result = remove ( file ) ) == ERROR )
                           {
                           cout << file << " not removed." << endl;
                           CheckErrNo ( file );
                           }
                        break;
                        
      case FTW_D:       Stk.push ( file );
                        break;
                                               
      case FTW_NS:      cout << "Stat failure on:  "
                             << file << endl;
                        CheckErrNo ( file );
                        break;

      case FTW_DNR:     cout << "Directory cannot be read:  "
                             << file << endl;
                        CheckErrNo ( file );
                        break;
      }                          
   return result;
   }

void DelFiles ( char * path )
   {
   if ( ftw ( path, DelEntry, DEPTH ) == ERROR )
      {
      CheckErrNo ( path );
      }
   }

void DelDirectories ( void )
   {       
   char *dir = new char [ DIRSIZE ];
   while ( Stk.is_not_empty ( ) )
      {
      if ( Stk.top_strlen ( ) > DIRSIZE )
         {
         delete [ ] dir;
         dir = new char [ Stk.top_strlen ( ) + 1 ];
         }
      Stk.top ( dir );
      if ( rmdir ( dir ) )
         CheckErrNo ( dir );
      Stk.pop (  );
      }
   delete [ ] dir;
   }


/*****************************************************************************
*
*                                deltree.cpp
*
*     Programmer:    Graydon Ekdahl
*
*     Date:          15 June 1997
*
*     What:          implements a deltree command which deletes all files
*                    and directories in the path
*
*     Call:          deltree <pathname>
*
*****************************************************************************/

#include <iostream.h>
#include "funcs.h" 
   
void main ( int argc, char * argv [ ] )
   {
   Clrscr ( );
   Center ( 2, " DelTree " );
   cout << endl << endl;
   if ( argc < 2 )   
      {
      cerr << endl << "Usage:  deltree <pathname> " << endl << endl;
      exit ( 0 );
      }
   if ( Wish_to_Continue ( argv [ 1 ] ) )
      {
      DelFiles ( argv [ 1 ] );
      DelDirectories ( );
      }
   }          

//formfeed   
   

      
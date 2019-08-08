/**************************************************************************
*
*
*                                  strnode.h
*
*    Programmer:      Graydon Ekdahl
*
*    Date:            06.15.97
*
*    What:            holds declarations for node holding pointers
*                     to char array 
*
***************************************************************************/

#ifndef STRNODE
#define STRNODE

#include <string.h>
#include <assert.h>


class StrNode
{
friend class StrStack;

private:

char * strptr_;              //pointer to a char array
StrNode * next_;            //pointer to next node

StrNode ( void );
   /*
   Function:      default class constructor
   
   Pre:           strptr and next are undefined.
   
   Post:          strptr = 0 && next = 0
   */
   
StrNode ( const char *newstr, StrNode *next );
   /*
   Function:      cell constructor
   
   Pre:           strptr and next are undefined.
                  newstr points to a valid char array
                  next points to a valid StrNode
   
   Post:          strptr_ contains a ptr to a discrete copy of newstr
                  next_ contains next
                  newstr is unchanged
   */
   
StrNode ( const char *newstr );
   /*
   Function:      cell constructor
   
   Pre:           strptr and next are undefined
                  newstr points to a valid char array
   
   Post:          strptr_ contains a ptr to a discrete copy of newstr
                  next_ contains null
                  newstr is unchanged
   */

StrNode ( const StrNode & srcnode );
   /*
   Function:      StrNode copy constructor
   
   Pre:           srcnode is a valid reference to a StrNode instance
   
   Post:          *this is a discrete copy of srcnode
                  srcnode is unchanged
   */
   
~StrNode ( void );
   /*
   Function:      StrNode destructor
   
   Pre:           *this is a valid StrNode
   
   Post:          *this is deallocated
   */

   };  // end of class StrNode
   

#endif


/**************************************************************************
*
*
*                               strnode.cpp
*
*    Programmer:      Graydon Ekdahl
*
*    Date:            06.15.97
*
*    What:            implementation file for node to hold pointers
*                     to char. arrays
*
***************************************************************************/

#include "strnode.h"

StrNode::StrNode ( void )
   /*
   Function:      default class constructor
   
   Pre:           strptr and next are undefined.
   
   Post:          strptr = 0 && next = 0
   */
   {
   strptr_ = 0;
   next_   = 0;
   }
   
   
StrNode::StrNode ( const char *newstr, StrNode *next )
   /*
   Function:      cell constructor
   
   Pre:           strptr and next are undefined.
                  newstr points to a valid char array
                  next points to a valid StrNode
   
   Post:          strptr_ contains a ptr to a discrete copy of newstr
                  next_ contains next
                  newstr is unchanged
   */
   
   {
   strptr_ = new ( char [ strlen ( newstr ) + 1 ] );
   assert ( strptr_ );              // out of memory?
   strcpy ( strptr_, newstr );
   next_ = next;
   }

StrNode::StrNode ( const char *newstr )
   /*
   Function:      cell constructor
   
   Pre:           strptr and next are undefined
                  newstr points to a valid char array
   
   Post:          strptr_ contains a ptr to a discrete copy of newstr
                  next_ contains null
                  newstr is unchanged
   */
   {
   strptr_ = new ( char [ strlen ( newstr ) + 1 ] );
   assert ( strptr_ );
   strcpy ( strptr_, newstr );
   next_ = 0;
   }

StrNode::StrNode ( const StrNode & srcnode )
   /*
   Function:      StrNode copy constructor
   
   Pre:           srcnode is a valid node instance
   
   Post:          *this is a discrete copy of srcnode
                  srcnode is unchanged
   */
   {
   strptr_ = new ( char [ strlen ( srcnode.strptr_ ) + 1 ] );
   assert ( strptr_ );
   strcpy ( strptr_, srcnode.strptr_ );
   if ( srcnode.next_ )
      {
      next_ = new StrNode ( *srcnode.next_ );   //  indirect recursion
      assert ( next_ );                      //  out of memory?
      }
   else
      {
      next_ = 0;
      }
   }  
   
StrNode::~StrNode ( void )
   /*
   Function:      StrNode destructor
   
   Pre:           *this is a valid node
   
   Post:          *this is deallocated
   */
   {
   delete [ ] strptr_;      //  delete memory allocated for string
   delete next_;            //  indirect recursion deletes all nodes
   }   
      

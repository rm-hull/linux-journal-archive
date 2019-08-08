/**************************************************************************
*
*
*                               strstack.cpp
*
*    Programmer:      Graydon Ekdahl
*
*    Date:            05.27.97
*
*    What:            implementation file for a simple stack of
*                     char arrays.
*
***************************************************************************/

#include "strstack.h"

StrStack::StrStack ( void )
   /*
   Function:      default class constructor
   
   Pre:           head_ and size_ are undefined.
   
   Post:          head_ = NULL && size_ = 0
   */    
   {
   head_ = 0;
   size_ = 0;
   }

StrStack::StrStack ( const StrStack & src )
   /*
   Function:      class copy constructor
   
   Pre:           src is a valid StrStack instance
   
   Post:          *this is a valid discrete copy of src
                  src is unchanged 
   */
   {
   size_ = src.size_;
   if ( src.head_ )
      {
      head_ = new StrNode ( *src.head_ );   //  indirect recursion copies nodes
      assert ( head_ );
      }
   else
      {
      head_ = 0;
      }
   }
   
StrStack::~StrStack ( void )
    /*
    Function:     class destructor
    
    Pre:          *this is valid StrStack instance
    
    Post:         head_ = 0  && next_ = 0
                  all memory is deallocated.
   */
   {
   delete head_;
   head_ = 0;  
   size_ = 0;
   }
         
void StrStack::push ( const char *str )
   /*
   Function:      pushes a new string on stack
   
   Pre:           *this is a valid StrStack instance
                  str is a valid char array
                  
   Post:          head_ -> next_ points to old StrStack head_
                  head_ points to new node
                  newnode holds ptr to str
                  str is unchanged
   */
   {
   StrNode * newnode;
   newnode = new StrNode  ( str, head_ );
   assert ( newnode );
   head_ = newnode;
   size_ ++;
   }
   
   
 int  StrStack::top  ( char * str ) const
   /*
   Function:      retrieves the string at top of stack
   
   Pre:           *this is a valid StrStack instance 
                  str is a valid char array of sufficient size
                  
   Post:          *this is unchanged 
                  str points to a discrete copy of head_ -> strptr_

   RetVal:        returns True if successful, false otherwise
   */
   {
   if ( is_not_empty ( ) )
      {
      strcpy ( str, head_ -> strptr_ );
      return 1;
      }
   else
       {
      str [ 0 ] = 0;
      return 0;
      }
   }

int StrStack::is_not_empty ( void ) const 
   /*
   Function:      determines if stack is not empty
   
   Pre:           *this is a valid stack instance
   
   Post:          *this is unchanged
   
   RetVal:        True if not empty, False otherwise.
   */
   {
   return ( size_ != 0 );
   }
   
int  StrStack::pop  ( void )
   /*
   Function:      removes top StrNode from the stack
   
   Pre:           *this is a valid stack instance
   
   Post:          head_ -> StrNode is removed
                  size_ = size_ - 1

   RetVal:        returns True if successful, false otherwise                  
   */                  
   {
   if  ( is_not_empty ( ) )
      {
      StrNode *oldnode = head_;
      head_ = head_ -> next_;
      oldnode -> next_ = NULL;
      delete oldnode;
      size_ --;
      return 1;
      }
   return 0;
   }

int  StrStack::top_strlen ( void ) const
   /*
   Function:      finds length of string pointed to by top node
   
   Pre:           *this is a valid StrStack instance
   
   Post:          *this is unchanged
   
   RetVal:        returns length of char array pointed to by 
                  head_ -> strptr_ if successful, 0 otherwise.
   */

   {
   if ( is_not_empty ( ) )
      return ( strlen ( head_ -> strptr_ ) );
   else
      return 0;
   }

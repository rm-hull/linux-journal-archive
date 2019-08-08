
/**************************************************************************
*
*
*                                  strstack.h
*
*    Programmer:    Graydon Ekdahl
*
*    Date:          06.15.97
*
*    What:          holds declarations for a simple char array stack
*                   implemented in stack.cpp
*
***************************************************************************/

#ifndef STRSTACK
#define STRSTACK

#include "strnode.h"

class StrStack    :    private StrNode

{
private:

   StrNode *head_;         //pointer to first StrNode in stack
   int      size_;         //number of StrNodes in stack
   
public:

StrStack ( void );
   /*
   Function:      default class constructor
   
   Pre:           head_ and size_ are undefined.
   
   Post:          head_ = NULL  && size_ = 0
   */    

StrStack ( const StrStack & src );
   /*
   Function:      class copy constructor
   
   Pre:           src is a valid StrStack instance
   
   Post:          *this is a valid discrete copy of src
                  src is unchanged
   */
   
 ~StrStack ( void );
    /*
    Function:     class destructor
    
    Pre:          *this is valid StrStack instance
    
    Post:         head_ = 0  && next_ = 0
                  all memory is deallocated.
   */
   
void push ( const char *str );
   /*
   Function:      pushes a new string on stack
   
   Pre:           *this is a valid StrStack instance  &&
                  str is a valid char array

   Post:          head_ -> next_ points to old StrStack head_
                  head_ points to new StrNode
                  new StrNode holds ptr to str
                  str is unchanged
   */
   
int  top  ( char * str ) const; 
   /*
   Function:      retrieves the string at top of stack
   
   Pre:           *this is a valid StrStack instance 
                  str is a valid char array of sufficient size
                  
   Post:          *this is unchanged 
                  str points to a discrete copy of head_ -> strptr_
                  
   RetVal:        True if successful, false otherwise. 
   */
   

int is_not_empty ( void ) const;
   /*
   Function:      determines if stack is not empty
   
   Pre:           *this is a valid stack instance
   
   Post:          *this is unchanged
   
   RetVal:        True if not empty, False otherwise.
   */
   
int  pop  ( void );
   /*
   Function:      removes top StrNode from the stack
   
   Pre:           *this is a valid stack instance
   
   Post:          head_ -> StrNode is removed
                  size_ = size_ - 1

   RetVal:        True if successful, otherwise false              
   */
   
int  top_strlen ( void ) const;
   /*
   Function:      finds length of string pointed to by top StrNode
   
   Pre:           *this is a valid StrStack instance
   
   Post:          *this is unchanged
   
   RetVal:        returns length of char array pointed to by 
                  head_ -> strptr_ if successful, 0 otherwise
   */
   
};   // end of StrStack   ******************************************

#endif

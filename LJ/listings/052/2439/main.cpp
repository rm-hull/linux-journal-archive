#include <iostream.h>
#include <iomanip.h>
#include <conio.h>
#include <stdio.h>
#include "strstack.h"

void main ( void )

{
StrStack stk;

char str [ 100 ];
cout << "Enter a string:  ";
cin >> str;

stk.push ( str );
stk.pop ( );

cout << "This is the end of the program" << endl;



}

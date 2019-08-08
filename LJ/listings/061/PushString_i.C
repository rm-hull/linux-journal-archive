// C++. In file PushString_i.C  This is the CORBA object's implementation.
#include "PushString_i.h"
#include <iostream.h>

// define the constructor
PushString_i::PushString_i(const char * theName)
{
	cerr << "PushString_i implementation is being created" << endl;
}

// define the destructor
PushString_i::~PushString_i()
{
	cerr << "PushString_i implementation is being destroyed" << endl;
}

// Here is the actual implementation of pushStr.
CORBA::Boolean PushString_i::pushStr(const char * inStr)
	throw(CORBA::SystemException)
{
	int retval;
	cerr << "in PushStr\n";
	char * m_str = new char[strlen(inStr)+1];
	strcpy(m_str,inStr);
	if( strlen(m_str) > 5 ) // just for fun, play around with the boolean return
		retval = 1;
	else
		retval = 0;
	cerr << "The string pushed was " << m_str << endl;
	delete [] m_str;
	cerr << "Implementation leaving PushStr..." << endl;
	return(retval);
}







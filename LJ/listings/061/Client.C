// C++. In file Client.C

#include <iostream.h>
#include <fstream.h>
#include "PushString.hh"

int main(int argc, char ** argv)
{
	char tmpFilePath[1024];
	char *IOR = new char[1024];

	ifstream f_in("ior.out");
	if(!f_in)
	{
		cerr << "\nCould not open ior.out for reading: Things to check:" << endl;
		cerr << "1. Is server running?" << endl;
		cerr << "2. If so, is its ior.out file accessible from the" << endl;
		cerr << "directory that client is running from currently?" << endl;
		exit(-1);
	}
	f_in >> IOR;

	CORBA::ORB_ptr orb = CORBA::ORB_init(argc,argv,"omniORB2");
	CORBA::BOA_ptr boa = orb->BOA_init(argc,argv,"omniORB2_BOA");

	PushString_var pushStringVar;
	try {
		CORBA::Object_var obj = orb->string_to_object(IOR);
		pushStringVar = PushString::_narrow(obj);
		CORBA::String_var src = (const char *) "Hello World";
		CORBA::String_var dest;
		cerr << "client callint PushStr with string: " << src << endl;
		pushStringVar->pushStr(src); // Call the remote object's pushStr function
		cerr << "client returned from PushStr call without an exception" << endl;
	}
	catch( CORBA::COMM_FAILURE & ex) {
		cerr << "Caught system exception COMM_FAILURE" << endl;
		cerr << "We seem to be missing a server object" << endl;
		cerr << "Make sure that (1) the server is running and" << endl;
		cerr << "(2) that the ior.out file that server writes" << endl;
		cerr << "out is accessible from this client's" << endl;
		cerr << "present working directory" << endl;
	}
	catch( omniORB::fatalException & ex) {
		cerr << "Caught omniORB2 fatalException. This is a bug in omniORB" << endl;
	}
}


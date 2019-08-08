// C++. In file Srv_Main.C

#include <iostream.h>
#include <fstream.h>
#include "PushString_i.h"

int main(int argc, char **argv)
{
	ofstream f_out;
	f_out.open("ior.out");
	if(!f_out)
	{
		cerr << "\nCould not open output file ... Disk full???" << endl;
		exit(-1);
	}
	cout << "Server starting, creating PushString\n";
	CORBA::ORB_ptr orb = CORBA::ORB_init(argc,argv,"omniORB2");
	CORBA::BOA_ptr boa = orb->BOA_init(argc,argv,"omniORB2_BOA");

	PushString_i *myPushString = new PushString_i("hi");
	myPushString->_obj_is_ready(boa);
	{
		PushString_var myobjRef = myPushString->_this();
		CORBA::String_var p = orb->object_to_string(myobjRef);
		cerr << "'" << (char *) p << "'" << endl;
		f_out << (char *) p;
		f_out << flush;
	}
	boa->impl_is_ready();

	cout << "Server terminating." << endl;
	return(0);
}

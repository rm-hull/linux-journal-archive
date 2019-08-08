//supplier.C
#include <unistd.h>
#include <iostream.h>
#include <CORBA.h>
#include <mico/naming.h>
#include <mico/CosEventComm.h>
#include <mico/CosEventChannelAdmin.h>

class PushSupplierImpl : virtual public CosEventComm::PushSupplier_skel
{
public:
  PushSupplierImpl() { }
  void disconnect_push_supplier();
};

void PushSupplierImpl::disconnect_push_supplier()
{
  cerr << "disconnect_push_supplier() executing" << endl;
}

int main(int argc, char** argv)
{
  CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "mico-local-orb");

  PushSupplierImpl * supplier = new PushSupplierImpl();
  
  CORBA::Object_var nsobj = orb->resolve_initial_references("NameService");
  assert(! CORBA::is_nil(nsobj));
  cerr << "Supplier: successful call to resolve_initial_references()" << endl;
  
  CosNaming::NamingContext_var context = CosNaming::NamingContext::_narrow(nsobj);
  assert(! CORBA::is_nil(context));

  CosNaming::Name name;
  name.length(1);
  name[0].id = CORBA::string_dup("TestEventChannel");
  name[0].kind = CORBA::string_dup("");

  CORBA::Object_var obj;

  try { 
    cerr << "Supplier: Attempting to resolve EventChannel ..." << endl;
    obj = context->resolve(name);
  }
  catch(...) {
    cerr << "Supplier: Failed to resolve EventChannel" << endl;
    return -1;
  }
  
  CosEventChannelAdmin::EventChannel_var event_channel;
  CosEventChannelAdmin::SupplierAdmin_var supplier_admin;
  CosEventChannelAdmin::ProxyPushConsumer_var proxy_consumer;

  event_channel = CosEventChannelAdmin::EventChannel::_narrow(obj);
  assert(! CORBA::is_nil(event_channel));
  cerr << "Supplier: EventChannel successfully located ..." << endl;

  cerr << "Supplier: Attempting to obtain a Supplier Admin object ..." << endl;
  
  supplier_admin = event_channel->for_suppliers();
  assert(! CORBA::is_nil(supplier_admin));
  cerr << "Supplier: Successfully retrieved a Supplier Admin object ..." << endl;

  cerr << "Supplier: Attempting to obtain a Push Consumer Proxy ..." << endl;
  proxy_consumer = supplier_admin->obtain_push_consumer();
  assert(! CORBA::is_nil(proxy_consumer));
  cerr << "Supplier: Successfully obtained a Push Consumer Proxy ..." << endl;

  try
  {
    cerr << "Supplier: Attempting to connect supplier to Event Channel  ..." << endl;
    proxy_consumer->connect_push_supplier(CosEventComm::PushSupplier::_duplicate(supplier));
    cerr << "Supplier: connected ..." << endl;
  }
  catch( CosEventChannelAdmin::AlreadyConnected)
  {
  }

  cerr << "Supplier: preparing to deliver messages to Event Channel" << endl;
  
  CORBA::Any any;
  any <<=(CORBA::ULong) 555555555;
  proxy_consumer->push(any);
  cerr << "Supplier: just pushed a long value" << endl;
  sleep(1);
  any <<=(CORBA::Short) 100;
  proxy_consumer->push(any);
  cerr << "Supplier: just pushed a short value" << endl;
  any <<=(CORBA::Double) 999999.999999;
  proxy_consumer->push(any);
  cerr << "Supplier: just pushed a double value" << endl;
  CORBA::String_var s1 = CORBA::string_dup("CORBA on Linux ROCKS!");
  any <<= s1;
  proxy_consumer->push(any);
  cerr << "Supplier: just pushed a string" << endl;
  sleep(5);
  any <<=(CORBA::ULong) 13;
  proxy_consumer->push(any);
  sleep(2);
  cerr << "Supplier is ending" << endl;
}

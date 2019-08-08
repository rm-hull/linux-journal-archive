//consumer.C
#include <unistd.h>
#include <iostream.h>
#include <iomanip.h>
#include <CORBA.h>
#include <mico/naming.h>
#include <mico/CosEventComm.h>
#include <mico/CosEventChannelAdmin.h>
#include <pthread.h>
#include <signal.h>

typedef struct _Param
{
  CosEventChannelAdmin::ProxyPullSupplier_var proxy;
} thr_param;

pthread_t thread;

void ding(int sig)
{
  cerr << "Consumer got signal, attempting to cleanly cancel thread" << endl;
  pthread_cancel(thread);
  sleep(1);
  exit(0);
}

void * start(void * arg)
{
  CosEventChannelAdmin::ProxyPullSupplier_var proxy_supplier = 
    ((thr_param *) arg)->proxy;
  CORBA::Any* anyval;
  CORBA::ULong longval;
  CORBA::Short shortval;
  CORBA::Double doubleval;
  CORBA::String_var stringval;
  CORBA::Boolean has_event = 0;
  cerr << "Consumer: in start of thread, sleeping..." << endl;
  sleep(2);
  while(1)
  {
    try
    {
      while(!has_event)
      {
        sleep(1);
        cerr << "Consumer: thread calling try_pull" << endl;
        anyval = proxy_supplier->try_pull(has_event);
        cerr << "Consumer: thread returned from try_pull, testing for cancel" << endl;
        pthread_testcancel();
      }
      cerr << "Consumer: got event" << endl;
      if( *anyval >>= longval )
      {
        cerr << "Consumer: thread pulled long: " << longval << endl;
        if( longval == 13 )
        {
          cerr << "Consumer: thread received #13, exiting..." << endl;
          int * x = new int();
          *x = -1;
          pthread_exit(x);
        }
      }
      else if( *anyval >>= shortval )
      {
        cerr << "Consumer: thread pulled short: " << shortval << endl;
      }
      else if( *anyval >>= doubleval )
      {
        cerr << setiosflags(ios::fixed);
        cerr << "Consumer: thread pulled double: " << doubleval << endl;
      }
      else if( *anyval >>= stringval )
      {
        cerr << "Consumer: thread pulled string: " << stringval << endl;
      }
      has_event = 0;
    }
    catch(CosEventComm::Disconnected e)
    {
      cerr << "Consumer: Disconnected Exception was caught" << endl;
      int * x = new int();
      *x = -1;
      pthread_exit(x);
    }
  }
  cerr << "Consumer: returning from thread start" << endl;
  return(arg);
}

class ConsumerImpl : virtual public CosEventComm::PullConsumer_skel 
{
public:
  ConsumerImpl() {}
  void disconnect_pull_consumer();
};

void ConsumerImpl::disconnect_pull_consumer() 
{
  cerr << "disconnect_pull_consumer() executing" << endl;
}

int main(int argc, char** argv)
{
  signal(SIGINT,ding);

  CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "mico-local-orb");
  CORBA::BOA_var boa = orb->BOA_init(argc, argv, "mico-local-boa");

  ConsumerImpl* consumer = new ConsumerImpl();

  CORBA::Object_var nsobj =
    orb->resolve_initial_references("NameService");
  assert(! CORBA::is_nil(nsobj));
  
  CosNaming::NamingContext_var context = 
    CosNaming::NamingContext::_narrow(nsobj);
  assert(! CORBA::is_nil(context));

  CosNaming::Name name;
  name.length(1);
  name[0].id = CORBA::string_dup("EventChannelFactory");
  name[0].kind = CORBA::string_dup("factory");

  CORBA::Object_var obj;

  try {
    obj = context->resolve(name);
  }
  catch(CosNaming::NamingContext::CannotProceed e) {
    cerr << "Consumer: Exception: CosNaming::NamingContext::CannotProceed" << endl;
    return -1;
  }
  catch(CosNaming::NamingContext::NotFound e) {
    cerr << "Consumer: Exception: CosNaming::NamingContext::NotFound" << endl;
    return -1;
  }
  catch(CosNaming::NamingContext::InvalidName e) {
    cerr << "Consumer: Exception: CosNaming::NamingContext::InvalidName" << endl;
    return -1;
  }
  
  SimpleEventChannelAdmin::EventChannelFactory_var
  factory;
  CosEventChannelAdmin::EventChannel_var event_channel;

  factory = SimpleEventChannelAdmin::EventChannelFactory::
  _narrow(obj);
  assert(! CORBA::is_nil(factory));

  cerr << "Consumer: Attempting to create Event Channel from Factory ..." << endl;
  event_channel = factory->create_eventchannel();
  assert(! CORBA::is_nil(event_channel));
  cerr << "Consumer: Successfully created Event Channel from Factory ..." << endl;
  
  name.length(1);
  name[0].id = CORBA::string_dup("TestEventChannel");
  name[0].kind = CORBA::string_dup("");

  try {
    cerr << "Consumer: Attempting to bind name to new Event Channel ..." << endl;
    context->bind(name, 
	      CosEventChannelAdmin::EventChannel::_duplicate(event_channel));
  }
  catch(...) {
    cerr << "Consumer: Failed to bind name to EventChannel" << endl;
    return -1;
  }
  cerr << "Consumer: Successfully bound name to new Event Channel ..." << endl;
  
  cerr << "Consumer: Attempting to obtain a ConsumerAdmin object ..." << endl;
  CosEventChannelAdmin::ConsumerAdmin_var consumer_admin;
  consumer_admin = event_channel->for_consumers();
  assert(! CORBA::is_nil(consumer_admin));
  cerr << "Consumer: Successfully obtained a ConsumerAdmin object ..." << endl;
  
  cerr << "Consumer: Attempting to obtain a Pull Supplier Proxy ..." << endl;
  CosEventChannelAdmin::ProxyPullSupplier_var proxy_supplier;
  proxy_supplier = consumer_admin->obtain_pull_supplier();
  assert(! CORBA::is_nil(proxy_supplier));
  cerr << "Consumer: Successfully obtained Pull Supplier ..." << endl;

  try
  {
    cerr << "Consumer: Attempting to Pull Consumer to Proxy ..." << endl;
    proxy_supplier->connect_pull_consumer(CosEventComm::PullConsumer::_duplicate(consumer));
    cerr << "Consumer: Connected Pull Consumer to Proxy ..." << endl;
  }
  catch( CosEventChannelAdmin::AlreadyConnected)
  {
  }
  
  boa->impl_is_ready(CORBA::ImplementationDef::_nil());
  cerr << "Consumer has called impl_is_ready, creating thread ..." << endl;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  attr.detachstate = PTHREAD_CREATE_DETACHED;
  thr_param parm;
  parm.proxy = proxy_supplier;
  int thr = pthread_create(&thread, &attr, start, &parm);

  try
  {
    cerr << "Consumer: calling orb->run()" << endl;
    orb->run();
  }
  catch(CORBA::COMM_FAILURE e )
  {
    cerr << "Consumer: COMM_FAILURE exception caught" << endl;
    return(-1);
  }
  catch(CORBA::SystemException se)
  {
    cerr << "Consumer: CORBA::SystemException caught" << endl;
    return(-1);
  }
  cerr << "Consumer is ending" << endl;
  return(0);
}



// LJEventChannel.java
import org.omg.CosNaming.*;
import org.omg.CosEventComm.*;
import org.omg.CosEventChannelAdmin.*;
import com.visigenic.vbroker.services.CosEvent.*;
import java.io.DataInputStream;
import org.omg.CORBA.SystemException;
import java.util.*;
import logging.*;

class LogServiceImpl extends _PushSupplierImplBase implements LogServiceOperations
{
  private org.omg.CORBA.ORB orb;
  private org.omg.CORBA.BOA boa;
  private PushConsumer _pushConsumer;

  public void disconnect_push_supplier() {
    System.out.println("LogServiceImpl.disconnect_push_supplier() called");
    try {
      boa.deactivate_obj(this);
    }
    catch(org.omg.CORBA.SystemException e) {
      e.printStackTrace();
    }
  }

  LogServiceImpl()
  {
    System.out.println("in LogServiceImpl constructor, creating EventChannel");
    String[] args = null;
    try
    {
      orb = org.omg.CORBA.ORB.init(args, null);
      boa = orb.BOA_init();
      EventChannel channel = null;
      ProxyPushConsumer pushConsumer = null;
      try
      {
          String name = args[0];
          System.out.println("In LogServiceImpl:  binding to channel_server");
          channel = EventChannelHelper.bind(orb,"channel_server");
          System.out.println("Created event channel: " + channel);
          if(channel == null)
          {
            System.out.println("ERROR: Failed to bind to Event Channel ... bailing");
          }
          else
          {
            pushConsumer = channel.for_suppliers().obtain_push_consumer();
            System.out.println("Obtained push consumer: " + pushConsumer);
          }
          if(pushConsumer == null)
          {
            System.out.println("ERROR:  failed to obtain push consumer");
          }
          else
          {
            PushConsumer clone = PushConsumerHelper.narrow(pushConsumer._clone());
            _pushConsumer = clone;
            System.out.println("Connecting to Push Supplier ...");
            pushConsumer.connect_push_supplier(this);            
          }
      }
      catch(org.omg.CORBA.SystemException e)
      {
        e.printStackTrace();
      }
    }
    catch(Exception e)
    {
      e.printStackTrace();
    }
  }

  public void send(String str)
  {
    try
    {
      org.omg.CORBA.Any message = orb.create_any();
      message.insert_string(str);
      System.out.println("Supplier pushing: " + message);
      _pushConsumer.push(message);
    }
    catch(Disconnected e)
    {
      System.out.println("Disconnected");
    }
    catch(SystemException e)
    {
      e.printStackTrace();
      disconnect_push_supplier();
    }
  }
}

public class LJEventChannel
{
  public static void main(String[] args)
  {
    org.omg.CORBA.ORB orb = org.omg.CORBA.ORB.init(args,null);
    org.omg.CORBA.BOA boa = orb.BOA_init();
    // create the logging server 
    LogServiceImpl new_service = new LogServiceImpl();
    LogService service = new _tie_LogService(new_service,"LogService");
    if( service == null )
      System.out.println("service is NULL!!!!!");

    try
    {
      org.omg.CORBA.Object objRef = orb.resolve_initial_references("NameService");
      org.omg.CosNaming.NamingContext rootContext = org.omg.CosNaming.NamingContextHelper.narrow(objRef);
      NameComponent comp1 = new NameComponent("Linux Journal","");
      NameComponent [] name = {comp1};
      NamingContext ljcontext = rootContext.bind_new_context(name);
      
      NameComponent comp2 = new NameComponent("LJEventChannel","ec");
      NameComponent [] name2 = {comp2};
      ljcontext.rebind(name2,service);
    }
    catch(Exception e)
    {
      System.out.println("Exception: " + e);
    }
    // export the object reference
    boa.obj_is_ready(service);
    System.out.println(service + " is ready.");
    // wait for requests
    boa.impl_is_ready();
  }
}

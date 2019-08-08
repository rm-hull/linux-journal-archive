// PushSupplier.java
import org.omg.CosNaming.*;
import logging.*;

public class PushSupplier
{
  public static void main(String args[])
  {
    // Initiliaze the ORB.
    org.omg.CORBA.ORB orb = org.omg.CORBA.ORB.init(args,null);
    String logname;
    if( args.length > 0 )
      logname = args[0];
    else
      logname = "default supplier";
    // bind to the Log Service
    //LogService logger = LogServiceHelper.bind(orb, "LogService");
    LogService logger = null;
    
    try
    {
      org.omg.CORBA.Object objRef = orb.resolve_initial_references("NameService");
      org.omg.CosNaming.NamingContext rootContext = org.
			omg.CosNaming.NamingContextHelper.
			narrow(objRef);
      NameComponent comp1 = new NameComponent("Linux Journal","");
      NameComponent comp2 = new NameComponent("LJEventChannel","ec");
      NameComponent [] name = {comp1, comp2};
      org.omg.CORBA.Object rObjRef = rootContext.resolve(name);
      logger = LogServiceHelper.narrow(rObjRef);
    }
    catch(Exception e)
    {
      System.out.println("Exception: " + e);
    }
    
    if( logger == null )
    {
      System.out.println("logger is null");
      return;
    }
    else
      System.out.println("logger is NOT null");
    System.out.println("bound the Log Service");
    // use user-supplied args as the supplier's name, or use a default.
    System.out.println("supplier [" + logname + "] entering send loop");
    while(true)
    {
      try
      {
        System.out.println("supplier [" + logname + "] sending a string now...");
        logger.send("this is the string from supplier [" + logname + "]");
        Thread.sleep(2000);
      }
      catch( InterruptedException e )
      {
        System.out.println("Received InterruptedException: " + e);
      }
    }
  }
}




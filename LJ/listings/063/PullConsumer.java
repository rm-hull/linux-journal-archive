// PullConsumer.java

import org.omg.CosEventComm.*;
import org.omg.CosEventChannelAdmin.*;
import com.visigenic.vbroker.services.CosEvent.*;
import java.io.DataInputStream;

public class PullConsumer extends _PullConsumerImplBase
{
  public void disconnect_pull_consumer()
  {
    System.out.println("PullConsumer.disconnect_pull_consumer called");
  }

  public static void main(String[] args)
  {
    try
    {
      org.omg.CORBA.ORB orb = org.omg.CORBA.ORB.init(args, null);
      org.omg.CORBA.BOA boa = orb.BOA_init();
      EventChannel channel = null;
      PullConsumer ljPullConsumer = null;
      ProxyPullSupplier pullSupplier = null;
      DataInputStream in = new DataInputStream(System.in);

      System.out.println("about to call bind on channel_server");
      channel = EventChannelHelper.bind(orb,"channel_server");
      System.out.println("Located event channel: " + channel);

      ljPullConsumer = new PullConsumer();
      boa.obj_is_ready(ljPullConsumer);
      System.out.println("Created ljPullConsumer: " + ljPullConsumer);
      pullSupplier = channel.for_consumers().obtain_pull_supplier();
      System.out.println("Obtained pull supplier: " + pullSupplier);

      System.out.println("Connecting...");
      System.out.flush();
      pullSupplier.connect_pull_consumer(ljPullConsumer);

      System.out.println("Consumer entering Event Pull Loop...");
      org.omg.CORBA.BooleanHolder hasEvent = new org.omg.CORBA.BooleanHolder();
      org.omg.CORBA.Any result = null;
      while(true)
      {
        while(!hasEvent.value)
        {
          result = pullSupplier.try_pull(hasEvent);
        }
        System.out.println("Consumer pulled event: " + result.toString());
        hasEvent.value = false;
      }
    }
    catch(Exception e)
    {
      e.printStackTrace();
    }
  }
}

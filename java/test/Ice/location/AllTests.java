// **********************************************************************
//
// Copyright (c) 2002
// ZeroC, Inc.
// Billerica, MA, USA
//
// All Rights Reserved.
//
// Ice is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License version 2 as published by
// the Free Software Foundation.
//
// **********************************************************************

public class AllTests
{
    private static void
    test(boolean b)
    {
        if(!b)
        {
            throw new RuntimeException();
        }
    }

    public static void
    allTests(Ice.Communicator communicator)
    {
	ServerManagerPrx manager = ServerManagerPrxHelper.checkedCast(
	    communicator.stringToProxy("servermanager :default -t 10000 -p 12345"));
	test(manager != null);

	//
	// Start a server, get the port of the adapter it's listening on,
	// and add it to the configuration so that the client can locate
	// the TestAdapter adapter.
	//
	System.out.print("starting server... ");
        System.out.flush();
	manager.startServer();
	System.out.println("ok");

	System.out.print("testing stringToProxy... ");
        System.out.flush();
	Ice.ObjectPrx base = communicator.stringToProxy("test @ TestAdapter");
	Ice.ObjectPrx base2 = communicator.stringToProxy("test @ TestAdapter");
	System.out.println("ok");

	System.out.print("testing checked cast... ");
        System.out.flush();
	TestPrx obj = TestPrxHelper.checkedCast(base);
	test(obj != null);
	TestPrx obj2 = TestPrxHelper.checkedCast(base2);
	test(obj2 != null);
	System.out.println("ok");
 
	System.out.print("testing object reference from server... ");
        System.out.flush();
	HelloPrx hello = obj.getHello();
	hello.sayHello();
	System.out.println("ok");

	System.out.print("shutdown server... ");
        System.out.flush();
	obj.shutdown();
	System.out.println("ok");

	System.out.print("restarting server... ");
        System.out.flush();
	manager.startServer();
	System.out.println("ok");

	System.out.print("testing whether server is still reachable... ");
        System.out.flush();
	try
	{
	    obj2.ice_ping();
	}
	catch(Ice.LocalException ex)
	{
	    test(false);
	}
	System.out.println("ok");
    
    
	System.out.print("testing object reference from server... ");
        System.out.flush();
	hello.sayHello();
	System.out.println("ok");

	System.out.print("testing reference with unknown adapter...");
	System.out.flush();
	try
	{
	    base = communicator.stringToProxy("test @ TestAdapterUnknown");
	    base.ice_ping();
	    test(false);
	}
	catch(Ice.NoEndpointException ex)
	{
	}
	System.out.println("ok");	

	System.out.print("shutdown server... ");
        System.out.flush();
	obj.shutdown();
	System.out.println("ok");

	System.out.print("testing whether server is gone... ");
        System.out.flush();
	try
	{
	    obj2.ice_ping();
	    test(false);
	}
        catch(Ice.LocalException ex)
        {
            System.out.println("ok");
        }

	System.out.print("shutdown server manager...");
        System.out.flush();
	manager.shutdown();
	System.out.println("ok");
    }
}

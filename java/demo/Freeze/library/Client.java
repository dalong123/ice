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

public class Client extends Ice.Application
{
    public int
    run(String[] args)
    {
	return RunParser.runParser(appName(), args, communicator());
    }

    static public void
    main(String[] args)
    {
	Client app = new Client();
	app.main("demo.Freeze.library.Client", args, "config");
    }
}

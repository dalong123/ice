// **********************************************************************
//
// Copyright (c) 2003-2016 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Ice.seqMapping;

import java.util.concurrent.CompletionStage;
import java.util.concurrent.CompletableFuture;

import test.Ice.seqMapping.AMD.Test.*;
import test.Ice.seqMapping.Serialize.*;

public final class AMDMyClassI implements MyClass
{
    @Override
    public CompletionStage<Void> shutdownAsync(com.zeroc.Ice.Current current)
    {
        current.adapter.getCommunicator().shutdown();
        return CompletableFuture.completedFuture((Void)null);
    }

    @Override
    public CompletionStage<MyClass.OpSerialSmallJavaResult> opSerialSmallJavaAsync(Small i,
                                                                                   com.zeroc.Ice.Current current)
    {
        return CompletableFuture.completedFuture(new MyClass.OpSerialSmallJavaResult(i, i));
    }

    @Override
    public CompletionStage<MyClass.OpSerialLargeJavaResult> opSerialLargeJavaAsync(Large i,
                                                                                   com.zeroc.Ice.Current current)
    {
        return CompletableFuture.completedFuture(new MyClass.OpSerialLargeJavaResult(i, i));
    }

    @Override
    public CompletionStage<MyClass.OpSerialStructJavaResult> opSerialStructJavaAsync(Struct i,
                                                                                     com.zeroc.Ice.Current current)
    {
        return CompletableFuture.completedFuture(new MyClass.OpSerialStructJavaResult(i, i));
    }
}

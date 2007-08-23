// **********************************************************************
//
// Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Freeze/TransactionI.h>
#include <Freeze/ConnectionI.h>
#include <Freeze/Exception.h>
#include <Freeze/Initialize.h>
#include <Ice/LoggerUtil.h>

using namespace std;
using namespace Ice;

DbTxn*
Freeze::getTxn(const Freeze::TransactionPtr& tx)
{
    return dynamic_cast<Freeze::TransactionI*>(tx.get())->dbTxn();
}

void
Freeze::TransactionI::commit()
{
    if(_txn == 0)
    {
        throw DatabaseException(__FILE__, __LINE__, "inactive transaction");
    }
    
    long txnId = 0;
  
    try
    {
        _connection->closeAllIterators();

        if(_txTrace >= 1)
        {
            txnId = (_txn->id() & 0x7FFFFFFF) + 0x80000000L;
        }

        _txn->commit(0);

        if(_txTrace >= 1)
        {
            Trace out(_connection->communicator()->getLogger(), "Freeze.Transaction");
            out << "committed transaction " << hex << txnId << dec;
        }
    }
    catch(const ::DbDeadlockException& dx)
    {
    
        if(_txTrace >= 1)
        {
            Trace out(_connection->communicator()->getLogger(), "Freeze.Transaction");
            out << "failed to commit transaction " << hex << txnId << dec << ": " << dx.what();
        }

        postCompletion(false, true);
        DeadlockException ex(__FILE__, __LINE__);
        ex.message = dx.what();
        throw ex;
    }
    catch(const ::DbException& dx)
    {
        if(_txTrace >= 1)
        {
            Trace out(_connection->communicator()->getLogger(), "Freeze.Transaction");
            out << "failed to commit transaction " << hex << txnId << dec << ": " << dx.what();
        }

        postCompletion(false, false);
        DatabaseException ex(__FILE__, __LINE__);
        ex.message = dx.what();
        throw ex;
    }
    postCompletion(true, false);
}

void
Freeze::TransactionI::rollback()
{
    if(_txn != 0)
    {
        long txnId = 0;
        try
        {
            _connection->closeAllIterators();
            
            if(_txTrace >= 1)
            {
                txnId = (_txn->id() & 0x7FFFFFFF) + 0x80000000L;
            }
            
            _txn->abort();
            
            if(_txTrace >= 1)
            {
                Trace out(_connection->communicator()->getLogger(), "Freeze.Transaction");
                out << "rolled back transaction " << hex << txnId << dec;
            }
        }
        catch(const ::DbDeadlockException& dx)
        {
            if(_txTrace >= 1)
            {
                Trace out(_connection->communicator()->getLogger(), "Freeze.Transaction");
                out << "failed to rollback transaction " << hex << txnId << dec << ": " << dx.what();
            }
            
            postCompletion(false, true);
            throw DeadlockException(__FILE__, __LINE__, dx.what());
        }
        catch(const ::DbException& dx)
        {
            if(_txTrace >= 1)
            {
                Trace out(_connection->communicator()->getLogger(), "Freeze.Transaction");
                out << "failed to rollback transaction " << hex << txnId << dec << ": " << dx.what();
            }
            
            postCompletion(false, false);
            throw DatabaseException(__FILE__, __LINE__, dx.what());
        }
        postCompletion(true, false);
    }
}

Freeze::ConnectionPtr
Freeze::TransactionI::getConnection() const
{
    if(_txn != 0)
    {
        return _connection;
    }
    else
    {
        return 0;
    }
}

//
// External refcount operations, from code holding a Transaction[I]Ptr
//
void
Freeze::TransactionI::__incRef()
{
    Shared::__incRef();
    if(_txn != 0)
    {
         _connection->__incRef();
    }
}

void
Freeze::TransactionI::__decRef()
{
    if(_txn != 0)
    {
         _connection->__decRef();
    }
    Shared::__decRef();
}

//
// The refcount on transaction held by the associated ConnectionI object
//
void
Freeze::TransactionI::internalIncRef()
{
    Shared::__incRef();
}

void
Freeze::TransactionI::internalDecRef()
{
    Shared::__incRef();
}


void
Freeze::TransactionI::setPostCompletionCallback(const Freeze::PostCompletionCallbackPtr& cb)
{
    _postCompletionCallback = cb;
}

    
Freeze::TransactionI::TransactionI(ConnectionI* connection) :
    _connection(connection),
    _txTrace(connection->txTrace()),
    _txn(0)
{
    try
    {
        _connection->dbEnv()->getEnv()->txn_begin(0, &_txn, 0);

        if(_txTrace >= 1)
        {
            long txnId = (_txn->id() & 0x7FFFFFFF) + 0x80000000L;
            Trace out(_connection->communicator()->getLogger(), "Freeze.Transaction");
            out << "started transaction " << hex << txnId << dec;
        }
    }
    catch(const ::DbException& dx)
    {
        if(_txTrace >= 1)
        {
            Trace out(_connection->communicator()->getLogger(), "Freeze.Transaction");
            out << "failed to start transaction: " << dx.what();
        }

        DatabaseException ex(__FILE__, __LINE__);
        ex.message = dx.what();
        throw ex;
    }
}

    
Freeze::TransactionI::~TransactionI()
{
    if(_txn != 0)
    {
        try
        {
            rollback();
        }
        catch(const IceUtil::Exception& e)
        {
            Error error(_connection->communicator()->getLogger());
            error << "transaction rollback raised :" << e;
        }
    }
}

void
Freeze::TransactionI::postCompletion(bool committed, bool deadlock)
{
    _txn = 0;

    if(_postCompletionCallback != 0)
    {
        _postCompletionCallback->postCompletion(committed, deadlock);
    }

    _connection->clearTransaction(); // will release the internal refcount
}

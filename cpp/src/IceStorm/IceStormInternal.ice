// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_STORM_INTERNAL_ICE
#define ICE_STORM_INTERNAL_ICE

#include <IceStorm/IceStorm.ice>
#include <Ice/Current.ice>

module IceStorm
{

/**
 *
 * A sequence of bytes.
 *
 **/
sequence<byte> ByteSeq;

/**
 *
 * Mirror datastructure of Ice::Current::Context.
 *
 **/
dictionary<string, string> ContextData;

/**
 *
 * The event data.
 *
 **/
struct EventData
{
    /** The operation name. */
    string op;
    /** The operation mode. */
    Ice::OperationMode mode;
     /** The encoded data for the operation's input parameters. */
    ByteSeq data;
    /** The Ice::Current::Context data from the originating request. */
    ContextData context;
};

/** A sequence of EventData. */
sequence<EventData> EventDataSeq;

/**
 *
 * The TopicLink interface. This is used to forward events between
 * federated Topic instances.
 *
 * @see TopicInternal
 *
 **/
interface TopicLink
{
    /**
     *
     * Forward a sequence of events.
     *
     * @param events The events to forward.
     *
     **/
    ["ami"] void forward(EventDataSeq events);
};

/**
 *
 * The TopicUpstreamLink interface. This is used to inform an unstream
 * topic that a federated downstream topic is alive.
 *
 * @see TopicInternal
 *
 **/
interface TopicUpstreamLink
{
    /**
     *
     *  Inform the upstream topic that a federated downstream topic is alive.
     *
     **/
    void keepAlive();
};

/**
 *
 * Internal operations for a topic.
 *
 * @see Topic
 *
 **/
interface TopicInternal extends Topic
{
    /**
     *
     * Retrieve a proxy to the TopicLink interface.
     *
     * @return The TopicLink for the Topic.
     *
     **/
    idempotent TopicLink* getLinkProxy();

    /**
     *
     * Send a link notification to the downstream topic.
     *
     * @param name The topic name.
     *
     * @param upstream The upstream topic.
     * 
     **/
    void linkNotification(string name, TopicUpstreamLink* upstream);

    /**
     *
     * Send an unlink notification to the downstream topic.
     *
     * @param name The topic name.
     *
     * @param upstream The upstream topic.
     * 
     **/
    void unlinkNotification(string name, TopicUpstreamLink* upstream);
};

}; // End module IceStorm

#endif

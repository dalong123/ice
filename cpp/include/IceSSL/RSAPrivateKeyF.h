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

#ifndef ICE_SSL_RSA_PRIVATE_KEY_F_H
#define ICE_SSL_RSA_PRIVATE_KEY_F_H

#include <Ice/Handle.h>

namespace IceSSL
{

class RSAPrivateKey;
typedef IceInternal::Handle<RSAPrivateKey> RSAPrivateKeyPtr;

}

namespace IceInternal
{

void incRef(::IceSSL::RSAPrivateKey*);
void decRef(::IceSSL::RSAPrivateKey*);

}

#endif

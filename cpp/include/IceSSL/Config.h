// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_SSL_CONFIG_H
#define ICE_SSL_CONFIG_H

#include <Ice/Config.h>

//
// Automatically link IceSSL[D|++11|++11D].lib with Visual C++
//
#if !defined(ICE_BUILDING_ICE_SSL) && defined(ICE_SSL_API_EXPORTS)
#   define ICE_BUILDING_ICE_SSL
#endif

#if defined(_MSC_VER) && !defined(ICE_BUILDING_ICE_SSL)
#   pragma comment(lib, ICE_LIBNAME("IceSSL"))
#endif

#if defined(__APPLE__)
#  define ICE_USE_SECURE_TRANSPORT 1
#elif defined(_WIN32)
#  define ICE_USE_SCHANNEL 1
#else
#  define ICE_USE_OPENSSL 1
#endif

#endif

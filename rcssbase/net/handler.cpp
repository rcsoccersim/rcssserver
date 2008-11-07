// -*-c++-*-

/***************************************************************************
                          handler.cpp  - handles network startup
                             -------------------
    begin                : 14-AUG-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef HAVE_CONFIG_H
#include <config.h>
#endif

#include "handler.hpp"

namespace rcss {
namespace net {

Handler &
Handler::instance()
{
    static Handler rval;
    return rval;
}

Handler::Handler()
    : M_valid( true )
{
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(CYGWIN)
    WORD wVersionRequested;
    int err;

    wVersionRequested = MAKEWORD( 2, 2 );
    err = WSAStartup( wVersionRequested, &M_WSAData );
    if ( err != 0 )
    {
        m_valid = false;
        return;
    }
    if ( LOBYTE( M_WSAData.wVersion ) != 2
         || HIBYTE( M_WSAData.wVersion ) != 2 )
    {
				WSACleanup();
				M_valid = false;
        return;
    }
#endif
}

Handler::~Handler()
{
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(CYGWIN)
    WSACleanup();
#endif
}

}
}

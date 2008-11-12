// -*-c++-*-

/***************************************************************************
                          handler.hpp  - handles network startup
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

#ifndef RCSS_NET_HANDLER_HPP
#define RCSS_NET_HANDLER_HPP

#ifdef __CYGWIN__
// cygwin is not win32
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <winsock2.h>
#endif

namespace rcss {
namespace net {

class Handler {

public:
    static
    Handler & instance();

    bool valid() const
      {
          return M_valid;
      }

private:

    Handler();

    Handler( const Handler & ); // not used
    Handler & operator=( const Handler & ); // not used

    ~Handler();

#ifdef __CYGWIN__
// cygwin is not win32
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    WSADATA M_WSAData;
#endif
    bool M_valid;
};

}
}

#endif // RCSS_NET_HANDLER_HPP

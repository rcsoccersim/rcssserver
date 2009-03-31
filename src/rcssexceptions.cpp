// -*-c++-*-

/***************************************************************************
                               rcssexceptions.cpp
                               exceptions for rcss
                             -------------------
    begin                : 2008-08-27
    copyright            : (C) 2008 by The RoboCup Soccer Server
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rcssexceptions.h"

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

namespace rcss {
namespace util {

NullErr::NullErr( const char * file,
                  const int & line,
                  const char * msg ) throw()
{
#ifdef HAVE_SSTREAM
    std::ostringstream tmp;
    tmp << file << ": " << line << ": " << msg;
    M_msg = tmp.str();
#else
    std::ostrstream tmp;
    tmp << file << ": " << line << ": " << msg << std::ends;
    M_msg = tmp.str();
    tmp.freeze( false );
#endif
}

}
}

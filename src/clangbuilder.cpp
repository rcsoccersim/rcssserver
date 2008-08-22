// -*-c++-*-

/***************************************************************************
                               clangbuilder.cc
                      Interface for building a clang message
                             -------------------
    begin                : 25-FEB-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include "clangbuilder.h"

namespace rcss {
namespace clang {

BuilderErr::BuilderErr( const char * file,
                        const int & line,
                        const char * msg ) throw()
{
#ifdef HAVE_SSTREAM
    std::ostringstream tmp;
#else
    std::ostrstream tmp;
#endif

    tmp << file << ": " << line << ": " << msg;

#ifndef HAVE_SSTREAM
    tmp << std::ends;
#endif

    M_msg = tmp.str();
}

}
}

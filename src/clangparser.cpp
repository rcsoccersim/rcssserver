// -*-c++-*-

/***************************************************************************
                                clangparser.cpp
                           Parser for the Coach Lang
                             -------------------
    begin                : 2008-08-22
    copyright            : (C) 2008 by The RoboCup Soccer Server
                           Maintainance Group.
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

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>;
#endif

#include "clangparser.h"

namespace rcss {
namespace clang {

Parser::Parser( Builder & builder )
  : M_param( *this, builder ),
    M_parser( &RCSS_CLANG_parse )
{

}

int
Parser::parse( const char * msg )
{
#ifdef HAVE_SSTREAM
    std::istringstream strm( msg );
#else
    std::istrstream strm( msg );
#endif
    return ( rcss::Parser::parse( strm ) ? 0 : 1 );
}

}
}

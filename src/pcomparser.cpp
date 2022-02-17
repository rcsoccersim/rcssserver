// -*-c++-*-

/***************************************************************************
                                pcomparser.cpp
                          Parser for Player Commands
                             -------------------
    begin                : 2008-08-27
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

#include <sstream>

#include "pcomparser.h"

namespace rcss {
namespace pcom {

Parser::Parser( Builder & builder )
    : M_param( *this, builder ),
      M_parser( &RCSS_PCOM_parse )
{

}

int
Parser::parse( const char * msg )
{
    std::istringstream strm( msg );

    return ( rcss::Parser::parse( strm ) ? 0 : 1 );
}

}
}

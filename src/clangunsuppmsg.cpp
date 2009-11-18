// -*-c++-*-

/***************************************************************************
                                clangunsuppmsg.cc
                       Class for CLang unsupported messages
                             -------------------
    begin                : 28-MAY-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
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

#include "clangunsuppmsg.h"

#include <iostream>
#include <string>

namespace rcss {
namespace clang {

UnsuppMsg::UnsuppMsg()
    : Msg()
{

}

UnsuppMsg::~UnsuppMsg()
{

}

std::auto_ptr< Msg >
UnsuppMsg::deepCopy() const
{
    return std::auto_ptr< Msg >( new UnsuppMsg( *this ) );
}

std::ostream &
UnsuppMsg::print( std::ostream & out ) const
{
    out << "(unsupported_clang)";
    return out;
}

std::ostream &
UnsuppMsg::printPretty( std::ostream & out,
                        const std::string & line_header ) const
{
    return out << line_header << "Unsupported Clang" << std::endl;
}

}
}

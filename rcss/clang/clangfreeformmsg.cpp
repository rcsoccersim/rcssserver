// -*-c++-*-

/***************************************************************************
                                clangfreeformmsg.h
                       Class for CLang Freeform messages
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

#include "clangfreeformmsg.h"

namespace rcss {
namespace clang {

FreeformMsg::FreeformMsg()
    : Msg()
{

}

FreeformMsg::FreeformMsg( const std::string & str )
	: Msg(),
	  M_str( str )
{

}

FreeformMsg::~FreeformMsg()
{

}

std::shared_ptr< Msg >
FreeformMsg::deepCopy() const
{
    std::shared_ptr< Msg > rval( new FreeformMsg( *this ) );
    return rval;
}

std::ostream &
FreeformMsg::print( std::ostream & out ) const
{
    out << "(freeform \"" << M_str << "\")";
    return out;
}

std::ostream &
FreeformMsg::printPretty( std::ostream & out,
                          const std::string & line_header ) const
{
    return out << line_header << "Freeform" << std::endl
               << line_header << "  " << "\"" << M_str << "\"" << std::endl;
}

}
}

// -*-c++-*-

/***************************************************************************
                                clanginfomsg.cc
                       Class for CLang Info messages
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

#include "clanginfomsg.h"

namespace rcss {
namespace clang {

InfoMsg::InfoMsg()
    : Msg()
{

}

InfoMsg::InfoMsg( const Storage & tokens )
    : Msg(),
      M_tokens( tokens )
{

}

InfoMsg::~InfoMsg()
{
	M_tokens.clear();
}

std::shared_ptr< Msg >
InfoMsg::deepCopy() const
{
	Storage new_tokens;
	for ( Storage::const_reference token : M_tokens )
	{
	    new_tokens.push_back( token->deepCopy() );
	}

	std::shared_ptr< Msg > ptr( new InfoMsg( new_tokens ) );
    return ptr;
}

std::ostream &
InfoMsg::print( std::ostream & out ) const
{
    out << "(info";
    for ( Storage::const_reference token : getTokens() )
    {
        if ( ! token )
        {
            out << " (null)";
        }
        else
        {
            out << " " << *token;
        }
    }
    out << ")";
    return out;
}

std::ostream &
InfoMsg::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    out << line_header << "Info" << std::endl;
    for ( Storage::const_reference token : getTokens() )
    {
        if ( ! token )
        {
            out << line_header << " - (null)\n";
        }
        else
        {
            token->printPretty( out, line_header + " - " );
        }
    }
    return out;
}

}
}

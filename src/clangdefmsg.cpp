// -*-c++-*-

/***************************************************************************
                                clangdefmsg.cpp
                       Class for CLang Define messages
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

#include "clangdefmsg.h"
#include "types.h"

namespace rcss {
namespace clang {

DefineMsg::DefineMsg()
    : Msg()
{

}

DefineMsg::DefineMsg( const Storage & defs )
    : Msg(),
      M_defs( defs )
{

}

DefineMsg::~DefineMsg()
{
	for ( Storage::iterator i = M_defs.begin();
          i != M_defs.end();
          ++i )
	{
	    delete *i;
	}
	M_defs.clear();
}

std::auto_ptr< Msg >
DefineMsg::deepCopy() const
{
	Storage new_defs;
	for ( Storage::const_iterator i = M_defs.begin();
          i != M_defs.end();
          ++i )
    {
	    new_defs.push_back( (*i)->deepCopy().release() );
    }
	return std::auto_ptr< Msg >( new DefineMsg( new_defs ) );
}

std::ostream &
DefineMsg::print( std::ostream & out ) const
{
    out << "(define";
    for ( Storage::const_iterator token_iter = getDefs().begin();
          token_iter != getDefs().end();
          ++token_iter )
    {
        if ( *token_iter == NULL )
        {
            out << " (null)";
        }
        else
        {
            out << " " << **token_iter;
        }
    }
    out << ")";
    return out;
}

std::ostream &
DefineMsg::printPretty( std::ostream & out,
                        const std::string & line_header ) const
{
    out << line_header << "Define" << std::endl;
    for ( Storage::const_iterator token_iter = getDefs().begin();
          token_iter != getDefs().end();
          ++token_iter )
    {
        if ( *token_iter == NULL )
        {
            out << line_header << " - (null)\n";
        }
        else
        {
            (*token_iter)->printPretty( out, line_header + " - " );
        }
    }
    return out;
}

}
}

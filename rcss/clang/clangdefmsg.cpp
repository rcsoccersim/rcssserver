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
	M_defs.clear();
}

std::shared_ptr< Msg >
DefineMsg::deepCopy() const
{
	Storage new_defs;
	for ( Storage::const_reference def : M_defs )
    {
	    new_defs.push_back( def->deepCopy() );
    }

	std::shared_ptr< Msg > ptr( new DefineMsg( new_defs ) );
    return ptr;
}

std::ostream &
DefineMsg::print( std::ostream & out ) const
{
    out << "(define";
    for ( Storage::const_reference def : getDefs() )
    {
        if ( ! def )
        {
            out << " (null)";
        }
        else
        {
            out << " " << *def;
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
    for ( Storage::const_reference def : getDefs() )
    {
        if ( ! def )
        {
            out << line_header << " - (null)\n";
        }
        else
        {
            def->printPretty( out, line_header + " - " );
        }
    }
    return out;
}

}
}

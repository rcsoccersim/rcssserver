// -*-c++-*-

/***************************************************************************
                                clangadvicemsg.cc
                       Class for CLang Advice messages
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

#include "clangadvicemsg.h"

namespace rcss {
namespace clang {

AdviceMsg::AdviceMsg()
    : Msg()
{

}

AdviceMsg::AdviceMsg( const Storage & tokens )
    : Msg(),
      M_tokens( tokens )
{

}

AdviceMsg::~AdviceMsg()
{
	M_tokens.clear();
}

std::shared_ptr< Msg >
AdviceMsg::deepCopy() const
{
	Storage new_tokens;
	for( Storage::const_reference i : M_tokens )
	{
	    new_tokens.push_back( i->deepCopy() );
	}

	std::shared_ptr< Msg > rval( new AdviceMsg( new_tokens ) );
    return rval;
}

//     void
//     AdviceMsg::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     AdviceMsg::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }

std::ostream &
AdviceMsg::print( std::ostream & out ) const
{
    out << "(advice";
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
AdviceMsg::printPretty( std::ostream & out,
                        const std::string & line_header ) const
{
    out << line_header << "Advice" << std::endl;
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

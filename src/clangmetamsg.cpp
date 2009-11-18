// -*-c++-*-

/***************************************************************************
                                clangmetamsg.cc
                       Class for CLang Meta messages
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

#include "clangmetamsg.h"

#include "types.h"

#include <iostream>
#include <string>

namespace rcss {
namespace clang {

MetaToken::MetaToken()
{

}

MetaToken::~MetaToken()
{

}

MetaTokenVer::MetaTokenVer( const double & ver )
	: MetaToken(),
	  M_ver( ver )
{

}

MetaTokenVer::~MetaTokenVer()
{

}

std::auto_ptr< MetaToken >
MetaTokenVer::deepCopy() const
{
    return std::auto_ptr< MetaToken >( new MetaTokenVer( *this ) );
}

std::ostream &
MetaTokenVer::print( std::ostream & out ) const
{
    return out << "(ver " << M_ver << ")";
}

std::ostream &
MetaTokenVer::printPretty( std::ostream & out,
                           const std::string & line_header ) const
{
    return out << line_header << "version: " << M_ver << std::endl;
}




MetaMsg::MetaMsg()
    : Msg()
{

}

MetaMsg::MetaMsg( const Storage & tokens )
	: Msg(),
	  M_tokens( tokens )
{

}

MetaMsg::~MetaMsg()
{
	for ( Storage::const_iterator i = M_tokens.begin();
          i != M_tokens.end();
          ++i )
	{
	    delete *i;
	}
	M_tokens.clear();
}

std::auto_ptr< Msg >
MetaMsg::deepCopy() const
{
	Storage new_tokens;
	for ( Storage::const_iterator i = M_tokens.begin();
          i != M_tokens.end();
          ++i )
	{
	    new_tokens.push_back( (*i)->deepCopy().release() );
	}
	return std::auto_ptr< Msg >( new MetaMsg( new_tokens ) );
}

std::ostream &
MetaMsg::print( std::ostream & out ) const
{
    out << "(meta";
    for ( Storage::const_iterator token_iter = M_tokens.begin();
          token_iter != M_tokens.end();
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
MetaMsg::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    out << line_header << "Meta" << std::endl;

    for ( Storage::const_iterator token_iter = M_tokens.begin();
          token_iter != M_tokens.end();
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

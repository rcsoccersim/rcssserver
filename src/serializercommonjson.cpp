// -*-c++-*-

/*!
  \file serializercommonjson.cpp
  \brief Class for serializing data by JSON format
  \author Hidehisa Akiyama
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Server Maintenance Committee.
 Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "serializercommonjson.h"

#include <iomanip>

namespace rcss {

SerializerCommonJSON::SerializerCommonJSON()
{

}

SerializerCommonJSON::~SerializerCommonJSON()
{

}

void
SerializerCommonJSON::serializeServerParamBegin( std::ostream & strm ) const
{
    strm << "{"
         << std::quoted( "type" ) << ':' << std::quoted( "server_param" ) << ','
         << std::quoted( "params" ) << ":{";
}


void
SerializerCommonJSON::serializeServerParamEnd( std::ostream & strm ) const
{
    strm << "}}";
}


void
SerializerCommonJSON::serializePlayerParamBegin( std::ostream & strm ) const
{
    strm << "{"
         << std::quoted( "type" ) << ':' << std::quoted( "player_param" ) << ','
         << std::quoted( "params" ) << ":{";
}


void
SerializerCommonJSON::serializePlayerParamEnd( std::ostream & strm ) const
{
    strm << "}}";
}


void
SerializerCommonJSON::serializePlayerTypeBegin( std::ostream & strm,
                                                const int id ) const
{
     strm << '{'
          << std::quoted( "type" ) << ':' << std::quoted( "player_type" )
          << ','
          << std::quoted( "id" ) << ':' << id
          << ','
          << std::quoted( "params" ) << ":{";
}

void
SerializerCommonJSON::serializePlayerTypeEnd( std::ostream & strm ) const
{
    strm << "}}";
}


void
SerializerCommonJSON::serializeParam( std::ostream & strm,
                                       const std::string & name,
                                       const int param ) const
{
    strm << std::quoted( name ) << ':' << param;
}

void
SerializerCommonJSON::serializeParam( std::ostream & strm,
                                       const std::string & name,
                                       const bool param ) const
{
    strm << std::quoted( name ) << ':' << std::boolalpha << param;
}

void
SerializerCommonJSON::serializeParam( std::ostream & strm,
                                       const std::string & name,
                                       const double & param ) const
{
    strm << std::quoted( name ) << ':' << param;
}

void
SerializerCommonJSON::serializeParam( std::ostream & strm,
                                       const std::string & name,
                                       const std::string & param ) const
{
    strm << std::quoted( name ) << ':' << std::quoted( param );
}


const
SerializerCommon::Ptr
SerializerCommonJSON::create()
{
    Ptr ptr( new SerializerCommonJSON() );
    return ptr;
}


namespace {
RegHolder json = SerializerCommon::factory().autoReg( &SerializerCommonJSON::create, -1 );
}

}

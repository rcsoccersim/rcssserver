// -*-c++-*-

/***************************************************************************
                            serializercommonstdv7.cc
                  Class for serializing data to std v7 clients
                             -------------------
    begin                : 24-JAN-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
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

#include "serializercommonstdv7.h"

#include "clangmsg.h"

namespace rcss {

SerializerCommonStdv7::SerializerCommonStdv7()
{

}

SerializerCommonStdv7::~SerializerCommonStdv7()
{

}

void
SerializerCommonStdv7::serializeServerParamBegin( std::ostream & strm ) const
{
    strm << "(server_param";
}

void
SerializerCommonStdv7::serializeServerParamEnd( std::ostream & strm ) const
{
    strm << ")";
}

void
SerializerCommonStdv7::serializePlayerParamBegin( std::ostream & strm ) const
{
    strm << "(player_param";
}

void
SerializerCommonStdv7::serializePlayerParamEnd( std::ostream & strm ) const
{
    strm << ")";
}

void
SerializerCommonStdv7::serializePlayerTypeBegin( std::ostream & strm ) const
{
    strm << "(player_type";
}

void
SerializerCommonStdv7::serializePlayerTypeEnd( std::ostream & strm ) const
{
    strm << ')';
}

void
SerializerCommonStdv7::serializeParam( std::ostream & strm,
                                       const int param ) const
{
    strm << ' ' << param;
}

void
SerializerCommonStdv7::serializeParam( std::ostream & strm,
                                       const unsigned int param ) const
{
    strm << ' ' << param;
}

void
SerializerCommonStdv7::serializeParam( std::ostream & strm,
                                       const bool param ) const
{
    strm << ' ' << param;
}

void
SerializerCommonStdv7::serializeParam( std::ostream & strm,
                                       const double & param ) const
{
    strm << ' ' << param;
}

void
SerializerCommonStdv7::serializeParam( std::ostream & strm,
                                       const std::string & param ) const
{
    strm << ' ' << param;
}

const
SerializerCommon::Ptr
SerializerCommonStdv7::create()
{
    Ptr ptr( new SerializerCommonStdv7() );
    return ptr;
}

namespace {
RegHolder v7 = SerializerCommon::factory().autoReg( &SerializerCommonStdv7::create, 7 );
}

}

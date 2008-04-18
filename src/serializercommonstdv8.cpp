// -*-c++-*-

/***************************************************************************
                            serializercommonstdv8.cc
                  Class for serializing data to std v8 clients
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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include "serializercommonstdv8.h"
#include "clangmsg.h"

namespace rcss
{

SerializerCommonStdv8::SerializerCommonStdv8()
{}

SerializerCommonStdv8::~SerializerCommonStdv8()
{}

const
SerializerCommonStdv8 &
SerializerCommonStdv8::instance()
{
    static SerializerCommonStdv8 ser;
    return ser;
}

void
SerializerCommonStdv8::serializeServerParamBegin( std::ostream & strm ) const
{
    strm << "(server_param ";
}

void
SerializerCommonStdv8::serializePlayerParamBegin( std::ostream & strm ) const
{
    strm << "(player_param ";
}

void
SerializerCommonStdv8::serializePlayerTypeBegin( std::ostream & strm ) const
{
    strm << "(player_type ";
}

void
SerializerCommonStdv8::serializeParam( std::ostream & strm,
                                       const std::string & name,
                                       const int param ) const
{
    strm << "(" << name << ' ' << param << ')';
}

void
SerializerCommonStdv8::serializeParam( std::ostream & strm,
                                       const std::string & name,
                                       const bool param ) const
{
    strm << "(" << name << ' ' << param << ')';
}

void
SerializerCommonStdv8::serializeParam( std::ostream & strm,
                                       const std::string & name,
                                       const double & param ) const
{
    strm << "(" << name << ' ' << param << ')';
}

void
SerializerCommonStdv8::serializeParam( std::ostream & strm,
                                       const std::string & name,
                                       const std::string & param ) const
{
    strm << "(" << name << " \"" << param << "\")";
}

namespace {

const
SerializerCommon &
create()
{
    return SerializerCommonStdv8::instance();
}

lib::RegHolder v8 = SerializerCommon::factory().autoReg( &create, 8 );
lib::RegHolder v9 = SerializerCommon::factory().autoReg( &create, 9 );
lib::RegHolder v10 = SerializerCommon::factory().autoReg( &create, 10 );
lib::RegHolder v11 = SerializerCommon::factory().autoReg( &create, 11 );
lib::RegHolder v12 = SerializerCommon::factory().autoReg( &create, 12 );

}
}

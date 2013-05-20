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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "serializercommonstdv8.h"

#include "clangmsg.h"

namespace rcss {

SerializerCommonStdv8::SerializerCommonStdv8()
{

}

SerializerCommonStdv8::~SerializerCommonStdv8()
{

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

const
SerializerCommon::Ptr
SerializerCommonStdv8::create()
{
    Ptr ptr( new SerializerCommonStdv8() );
    return ptr;
}

namespace {
RegHolder v8 = SerializerCommon::factory().autoReg( &SerializerCommonStdv8::create, 8 );
RegHolder v9 = SerializerCommon::factory().autoReg( &SerializerCommonStdv8::create, 9 );
RegHolder v10 = SerializerCommon::factory().autoReg( &SerializerCommonStdv8::create, 10 );
RegHolder v11 = SerializerCommon::factory().autoReg( &SerializerCommonStdv8::create, 11 );
RegHolder v12 = SerializerCommon::factory().autoReg( &SerializerCommonStdv8::create, 12 );
RegHolder v13 = SerializerCommon::factory().autoReg( &SerializerCommonStdv8::create, 13 );
RegHolder v14 = SerializerCommon::factory().autoReg( &SerializerCommonStdv8::create, 14 );
RegHolder v15 = SerializerCommon::factory().autoReg( &SerializerCommonStdv8::create, 15 );
//RegHolder v16 = SerializerCommon::factory().autoReg( &SerializerCommonStdv8::create, 16 );
}

}

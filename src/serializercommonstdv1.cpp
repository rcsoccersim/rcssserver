// -*-c++-*-

/***************************************************************************
                            serializercommonstdv1.cc
                  Class for serializing data to std v1 clients
                             -------------------
    begin                : 27-JAN-2003
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

#include "serializercommonstdv1.h"
#include "clangmsg.h"

namespace rcss {

SerializerCommonStdv1::SerializerCommonStdv1()
{

}

SerializerCommonStdv1::~SerializerCommonStdv1()
{

}

const
SerializerCommonStdv1 &
SerializerCommonStdv1::instance()
{
    static SerializerCommonStdv1 ser;
    return ser;
}

void
SerializerCommonStdv1::serializeServerParamBegin( std::ostream & ) const
{

}

void
SerializerCommonStdv1::serializeServerParamEnd( std::ostream & ) const
{

}

void
SerializerCommonStdv1::serializePlayerParamBegin( std::ostream & ) const
{

}

void
SerializerCommonStdv1::serializePlayerParamEnd( std::ostream & ) const
{

}

void
SerializerCommonStdv1::serializePlayerTypeBegin( std::ostream & ) const
{

}

void
SerializerCommonStdv1::serializePlayerTypeEnd( std::ostream & ) const
{

}

void
SerializerCommonStdv1::serializeParam( std::ostream &,
                                       const int ) const
{

}

void
SerializerCommonStdv1::serializeParam( std::ostream &,
                                       const unsigned int ) const
{

}

void
SerializerCommonStdv1::serializeParam( std::ostream &,
                                       const bool ) const
{

}

void
SerializerCommonStdv1::serializeParam( std::ostream &,
                                       const double & ) const
{

}

void
SerializerCommonStdv1::serializeParam( std::ostream &,
                                       const std::string & ) const
{

}

namespace {

const
SerializerCommon &
create()
{
    return SerializerCommonStdv1::instance();
}

lib::RegHolder v1 = SerializerCommon::factory().autoReg( &create, 1 );
lib::RegHolder v2 = SerializerCommon::factory().autoReg( &create, 2 );
lib::RegHolder v3 = SerializerCommon::factory().autoReg( &create, 3 );
lib::RegHolder v4 = SerializerCommon::factory().autoReg( &create, 4 );
lib::RegHolder v5 = SerializerCommon::factory().autoReg( &create, 5 );
lib::RegHolder v6 = SerializerCommon::factory().autoReg( &create, 6 );

}
}

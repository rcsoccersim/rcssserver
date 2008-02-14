// -*-c++-*-

/***************************************************************************
                            serializeronlinecoachstdv1.cc
               Class for serializing data to std v1 online coaches
                             -------------------
    begin                : 27-MAY-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
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

#include "serializeronlinecoachstdv1.h"

namespace rcss
{

SerializerOnlineCoachStdv1::SerializerOnlineCoachStdv1( const SerializerCommon & common,
                                                        const SerializerCoach & coach )
    : SerializerOnlineCoach( common, coach )
{

}

SerializerOnlineCoachStdv1::~SerializerOnlineCoachStdv1()
{

}

const SerializerOnlineCoachStdv1*
SerializerOnlineCoachStdv1::instance()
{
    rcss::SerializerCommon::Creator cre_common;
    if ( ! rcss::SerializerCommon::factory().getCreator( cre_common, 1 ) )
        return NULL;
    rcss::SerializerCoach::Creator cre_coach;
    if ( ! rcss::SerializerCoach::factory().getCreator( cre_coach, 1 ) )
        return NULL;
    static SerializerOnlineCoachStdv1 ser( cre_common(), *cre_coach() );
    return &ser;
}

void
SerializerOnlineCoachStdv1::serializeRefAudio( std::ostream& strm,
                                               const int& time,
                                               const std::string& name,
                                               const char* msg ) const
{
    strm << "(hear " << time << " " << name
         << " " << msg << ")";
}

void
SerializerOnlineCoachStdv1::serializePlayerAudio( std::ostream& strm,
                                                  const int& time,
                                                  const std::string& name,
                                                  const char* msg ) const
{
    strm << "(hear " << time << " " << name << " "
         << msg << ")";
}

void
SerializerOnlineCoachStdv1::serializePlayerClangVer( std::ostream& ,
                                                     const std::string&,
                                                     const unsigned int&,
                                                     const unsigned int& ) const
{}

void
SerializerOnlineCoachStdv1::serializeInit( std::ostream& strm,
                                           const int ) const
{
    strm << "(init ok)";
}

void
SerializerOnlineCoachStdv1::serializeChangedPlayer( std::ostream&,
                                                    const int,
                                                    const int ) const
{}

namespace
{
const SerializerOnlineCoach*
create()
{ return SerializerOnlineCoachStdv1::instance(); }

lib::RegHolder v1 = SerializerOnlineCoach::factory().autoReg( &create, 1 );
lib::RegHolder v2 = SerializerOnlineCoach::factory().autoReg( &create, 2 );
lib::RegHolder v3 = SerializerOnlineCoach::factory().autoReg( &create, 3 );
lib::RegHolder v4 = SerializerOnlineCoach::factory().autoReg( &create, 4 );
lib::RegHolder v5 = SerializerOnlineCoach::factory().autoReg( &create, 5 );
}
}

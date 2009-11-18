// -*-c++-*-

/***************************************************************************
                            serializeronlinecoachstdv7.cc
               Class for serializing data to std v7 online coaches
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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "serializeronlinecoachstdv7.h"

namespace rcss {

SerializerOnlineCoachStdv7::SerializerOnlineCoachStdv7( const SerializerCommon::Ptr common,
                                                        const SerializerCoach::Ptr coach )
    : SerializerOnlineCoachStdv6( common, coach )
{

}

SerializerOnlineCoachStdv7::~SerializerOnlineCoachStdv7()
{

}

void
SerializerOnlineCoachStdv7::serializeRefereeAudio( std::ostream & strm,
                                                   const int time,
                                                   const std::string & name,
                                                   const char * msg ) const
{
    strm << "(hear " << time << ' ' << name
         << ' ' << msg << ')';
}

void
SerializerOnlineCoachStdv7::serializePlayerAudio( std::ostream & strm,
                                                  const int time,
                                                  const std::string & name,
                                                  const char * msg ) const
{
    strm << "(hear " << time << ' ' << name << " \""
         << msg << "\")";
}

void
SerializerOnlineCoachStdv7::serializeChangedPlayer( std::ostream & strm,
                                                    const int unum,
                                                    const int type ) const
{
    strm << "(change_player_type " << unum;
    if ( type >= 0 )
        strm << ' ' << type;
    strm << ')';
}

const
SerializerOnlineCoach::Ptr
SerializerOnlineCoachStdv7::create()
{
    SerializerCommon::Creator cre_common;
    if ( ! SerializerCommon::factory().getCreator( cre_common, 7 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerCoach::Creator cre_coach;
    if ( ! SerializerCoach::factory().getCreator( cre_coach, 7 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerOnlineCoach::Ptr ptr( new SerializerOnlineCoachStdv7( cre_common(), cre_coach() ) );
    return ptr;
}


namespace {
RegHolder v7 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv7::create, 7 );
}

}

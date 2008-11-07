// -*-c++-*-

/***************************************************************************
                            serializercoachstdv13.cpp
               Class for serializing data to std v8 offline coaches
                             -------------------
    begin                : 2008-10-28
    copyright            : (C) 2008 by The RoboCup Soccer Server
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

#include "serializercoachstdv13.h"

#include "player.h"

namespace rcss {

SerializerCoachStdv13::SerializerCoachStdv13( const SerializerCommon & common )
    : SerializerCoachStdv8( common )
{

}


SerializerCoachStdv13::~SerializerCoachStdv13()
{

}

const
SerializerCoachStdv13 *
SerializerCoachStdv13::instance()
{
    rcss::SerializerCommon::Creator cre;
    if ( ! rcss::SerializerCommon::factory().getCreator( cre, 13 ) )
    {
        return NULL;
    }

    static SerializerCoachStdv13 ser( cre() );
    return &ser;
}

void
SerializerCoachStdv13::serializeVisualPlayer( std::ostream & strm,
                                             const Player & player,
                                             const std::string & name,
                                             const PVector & pos,
                                             const PVector & vel,
                                             const int body,
                                             const int neck ) const
{
    strm << " (" << name
         << ' ' << pos.x << ' ' << pos.y
         << ' ' << vel.x << ' ' << vel.y
         << ' ' << body
         << ' ' << neck;
    if ( player.isTackling() )
    {
        strm << " t";
    }
    else if ( player.kicked() )
    {
        strm << " k";
    }
    strm << ')';
}

void
SerializerCoachStdv13::serializeVisualPlayer( std::ostream & strm,
                                             const Player & player,
                                             const std::string & name,
                                             const PVector & pos,
                                             const PVector & vel,
                                             const int body,
                                             const int neck,
                                             const int point_dir ) const
{
    strm << " (" << name
         << ' ' << pos.x << ' ' << pos.y
         << ' ' << vel.x << ' ' << vel.y
         << ' ' << body
         << ' ' << neck
         << ' ' << point_dir;
    if ( player.isTackling() )
    {
        strm << " t";
    }
    else if ( player.kicked() )
    {
        strm << " k";
    }
    strm << ')';
}


namespace {

const
SerializerCoach *
create()
{
    return SerializerCoachStdv13::instance();
}

RegHolder v13 = SerializerCoach::factory().autoReg( &create, 13 );
}

}

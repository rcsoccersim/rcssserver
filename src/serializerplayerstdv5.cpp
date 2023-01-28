// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv5.cpp
                  Class for serializing data to std v5 players
                             -------------------
    begin                : 2023-01-27
    copyright            : (C) 2023 by The RoboCup Soccer Server
                           Maintenance Group.
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

#include "serializerplayerstdv5.h"

#include "player.h"

namespace rcss {

SerializerPlayerStdv5::SerializerPlayerStdv5( const SerializerCommon::Ptr common )
    : SerializerPlayerStdv1( common )
{

}

SerializerPlayerStdv5::~SerializerPlayerStdv5()
{

}


void
SerializerPlayerStdv5::serializeBodyCounts( std::ostream & strm,
                                            const Player & self ) const
{
    SerializerPlayerStdv1::serializeBodyCounts( strm, self );

    strm << " (turn_neck " << self.turnNeckCount() << ')';
}

const
SerializerPlayer::Ptr
SerializerPlayerStdv5::create()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 5 ) )
    {
        return SerializerPlayer::Ptr();
    }

    SerializerPlayer::Ptr ptr( new SerializerPlayerStdv5( cre() ) );
    return ptr;
}

namespace {
RegHolder v5 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv5::create, 5 );
RegHolder v6 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv5::create, 6 );

}

}

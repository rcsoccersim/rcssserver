// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv14.cpp
                  Class for serializing data to std v14 players
                             -------------------
    begin                : 2009-10-26
    copyright            : (C) 2009 by The RoboCup Soccer Server
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

#include "serializerplayerstdv14.h"

#include "player.h"

namespace rcss {

SerializerPlayerStdv14::SerializerPlayerStdv14( const SerializerCommon::Ptr common )
    : SerializerPlayerStdv13( common )
{

}

SerializerPlayerStdv14::~SerializerPlayerStdv14()
{

}

void
SerializerPlayerStdv14::serializeCard( std::ostream & strm,
                                       const Player & self ) const
{
    if ( self.isRedCarded() )
    {
        strm << "(card red)";
    }
    else if ( self.isYellowCarded() )
    {
        strm << "(card yellow)";

    }
    else
    {
        strm << "(card none)";
    }
}

void
SerializerPlayerStdv14::serializeFSPlayerState( std::ostream & strm,
                                                const Player & player ) const
{
    if ( player.isTackling() )
    {
        strm << " t";
    }
    else if ( player.kicked() )
    {
        strm << " k";
    }

    if ( player.isRedCarded() )
    {
        strm << " r";
    }
    else if ( player.isYellowCarded() )
    {
        strm << " y";
    }
}


const
SerializerPlayer::Ptr
SerializerPlayerStdv14::create()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 14 ) )
    {
        return SerializerPlayer::Ptr();
    }

    SerializerPlayer::Ptr ptr( new SerializerPlayerStdv14( cre() ) );
    return ptr;
}

namespace {
RegHolder v14 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv14::create, 14 );
}

}

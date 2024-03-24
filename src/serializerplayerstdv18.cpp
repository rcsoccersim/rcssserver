// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv18.cpp
                  Class for serializing data to std v14 players
                             -------------------
    begin                : 2022-10-08
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

#include "serializerplayerstdv18.h"

#include "player.h"

namespace rcss {

SerializerPlayerStdv18::SerializerPlayerStdv18( const SerializerCommon::Ptr common )
    : SerializerPlayerStdv14( common )
{

}

SerializerPlayerStdv18::~SerializerPlayerStdv18()
{

}


void
SerializerPlayerStdv18::serializeBodyCounts( std::ostream & strm,
                                             const Player & self ) const
{
    SerializerPlayerStdv14::serializeBodyCounts( strm, self );

    strm << " (change_focus " << self.changeFocusCount() << ')';
}


void
SerializerPlayerStdv18::serializeFSCounts( std::ostream & strm,
                                           const Player & self ) const
{
    strm << " (count "
         << self.kickCount() << ' '
         << self.dashCount() << ' '
         << self.turnCount() << ' '
         << self.catchCount() << ' '
         << self.moveCount() << ' '
         << self.turnNeckCount() << ' '
         << self.changeViewCount() << ' '
         << self.sayCount() << ' '
         << self.changeFocusCount() << ')';
}


// void
// SerializerPlayerStdv18::serializeFSCounts( std::ostream & strm,
//                                           const int count_kick,
//                                           const int count_dash,
//                                           const int count_turn,
//                                           const int count_catch,
//                                           const int count_move,
//                                           const int count_turn_neck,
//                                           const int count_change_view,
//                                           const int count_say,
//                                           const int count_change_focus) const
// {
//     strm << " (count "
//          << count_kick << ' '
//          << count_dash << ' '
//          << count_turn << ' '
//          << count_catch << ' '
//          << count_move << ' '
//          << count_turn_neck << ' '
//          << count_change_view << ' '
//          << count_say << ' '
//          << count_change_focus
//          << ')';
// }

void
SerializerPlayerStdv18::serializeFocusPoint( std::ostream & strm,
                                             const Player & self ) const
{
    strm << " (focus_point "
         << self.focusDist()<< " "
         << Rad2Deg( self.focusDir() );

    strm << ')';
}

void
SerializerPlayerStdv18::serializeFSPlayerFocus( std::ostream & strm,
                                                const Player & p ) const

{
    strm << " (focus_point "
         << p.focusDist() << ' '
         << Rad2Deg( p.focusDir() )
         << ')';
}

const
SerializerPlayer::Ptr
SerializerPlayerStdv18::create()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 18 ) )
    {
        return SerializerPlayer::Ptr();
    }

    SerializerPlayer::Ptr ptr( new SerializerPlayerStdv18( cre() ) );
    return ptr;
}

namespace {
RegHolder v18 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv18::create, 18 );
RegHolder v19 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv18::create, 19 );
}

}

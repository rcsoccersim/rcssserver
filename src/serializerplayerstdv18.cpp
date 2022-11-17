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
SerializerPlayerStdv18::serializeFSCounts( std::ostream & strm,
                                          const int count_kick,
                                          const int count_dash,
                                          const int count_turn,
                                          const int count_catch,
                                          const int count_move,
                                          const int count_turn_neck,
                                          const int count_change_view,
                                          const int count_say,
                                          const int count_set_focus) const
{
    strm << " (count "
         << count_kick << ' '
         << count_dash << ' '
         << count_turn << ' '
         << count_catch << ' '
         << count_move << ' '
         << count_turn_neck << ' '
         << count_change_view << ' '
         << count_say << ' '
         << count_set_focus
         << ')';
}

void
SerializerPlayerStdv18::serializeFocusPoint( std::ostream & strm,
                                             const Player & self ) const
{
    strm << " (focus_point "
         << self.focusPointCommitted().getMag()<< " "
         << Rad2Deg( self.focusPointCommitted().getHead() );

    strm << ')';
}

void
SerializerPlayerStdv18::serializeFSPlayerBegin( std::ostream & strm,
                                                const char side,
                                                const int unum,
                                                const bool goalie,
                                                const int type,
                                                const double & x,
                                                const double & y,
                                                const double & vel_x,
                                                const double & vel_y,
                                                const double & body_dir,
                                                const double & neck_dir,
                                                const double & focus_point_dist,
                                                const double & focus_point_dir ) const

{
    strm << " ((p " << side
         << ' ' << unum;

    if ( goalie )
    {
        strm << " g";
    }

    strm << ' ' << type // hetro player type
         << ") " << x
         << ' ' << y
         << ' ' << vel_x
         << ' ' << vel_y
         << ' ' << body_dir
         << ' ' << neck_dir
         << ' ' << focus_point_dist
         << ' ' << focus_point_dir;
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
}

}

// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv8.cpp
                  Class for serializing data to std v8 players
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

#include "serializerplayerstdv8.h"

#include "player.h"

namespace rcss {

SerializerPlayerStdv8::SerializerPlayerStdv8( const SerializerCommon::Ptr common )
    : SerializerPlayerStdv7( common )
{

}

SerializerPlayerStdv8::~SerializerPlayerStdv8()
{

}

void
SerializerPlayerStdv8::serializeVisualPlayer( std::ostream & strm,
                                              const Player & player,
                                              const std::string & name,
                                              const double & dist,
                                              const int dir ) const
{
    strm << " (" << name << ' ' << dist << ' ' << dir;
    if ( player.isTackling() ) strm << " t";
    strm << ')';
}


void
SerializerPlayerStdv8::serializeVisualPlayer( std::ostream & strm,
                                              const Player & player,
                                              const std::string & name,
                                              const double & dist,
                                              const int dir,
                                              const int point_dir ) const
{
    strm << " (" << name << ' ' << dist << ' ' << dir
         << ' ' << point_dir;
    if ( player.isTackling() ) strm << " t";
    strm << ')';
}


void
SerializerPlayerStdv8::serializeVisualPlayer( std::ostream & strm,
                                              const Player & player,
                                              const std::string & name,
                                              const double & dist,
                                              const int dir,
                                              const double & dist_chg,
                                              const double & dir_chg,
                                              const int body_dir,
                                              const int head_dir ) const
{
    strm << " (" << name << ' ' << dist << ' ' << dir
         << ' ' << dist_chg << ' ' << dir_chg
         << ' ' << body_dir << ' ' << head_dir;
    if ( player.isTackling() ) strm << " t";
    strm << ')';
}


void
SerializerPlayerStdv8::serializeVisualPlayer( std::ostream & strm,
                                              const Player & player,
                                              const std::string & name,
                                              const double & dist,
                                              const int dir,
                                              const double & dist_chg,
                                              const double & dir_chg,
                                              const int body_dir,
                                              const int head_dir,
                                              const int point_dir ) const
{
    strm << " (" << name << ' ' << dist << ' ' << dir
         << ' ' << dist_chg << ' ' << dir_chg
         << ' ' << body_dir << ' ' << head_dir
         << ' ' << point_dir;
    if ( player.isTackling() ) strm << " t";
    strm << ')';
}


void
SerializerPlayerStdv8::serializeAllyAudioFull( std::ostream & strm,
                                               const int time,
                                               const double & dir,
                                               const int unum,
                                               const char* msg ) const
{
    strm << "(hear " << time << ' '
         << dir << " our " << unum
         << " \"" << msg << "\")";
}

void
SerializerPlayerStdv8::serializeOppAudioFull( std::ostream & strm,
                                              const int time,
                                              const double & dir,
                                              const char* msg ) const
{
    strm << "(hear " << time << ' '
         << dir << " opp \"" << msg << "\")";
}

void
SerializerPlayerStdv8::serializeAllyAudioShort( std::ostream & strm,
                                                const int time,
                                                const int unum ) const
{
    strm << "(hear " << time << " our " << unum << ')';
}

void
SerializerPlayerStdv8::serializeOppAudioShort( std::ostream & strm,
                                               const int time ) const
{
    strm << "(hear " << time << " opp)";
}


void
SerializerPlayerStdv8::serializeFSBall( std::ostream & strm,
                                        const double & x,
                                        const double & y,
                                        const double & vel_x,
                                        const double & vel_y ) const
{
    strm << " ((b)"
         << ' ' << x
         << ' ' << y
         << ' ' << vel_x
         << ' ' << vel_y
         << ')';
}

void
SerializerPlayerStdv8::serializeFSPlayerBegin( std::ostream & strm,
                                               const char side,
                                               const int unum,
                                               const bool goalie,
                                               const int type,
                                               const double & x,
                                               const double & y,
                                               const double & vel_x,
                                               const double & vel_y,
                                               const double & body_dir,
                                               const double & neck_dir ) const

{
    strm << " ((p " << side
         << ' ' << unum;

    if ( goalie )
    {
        strm << " g";
    }
    else
    {
        strm << ' ' << type; // hetro player type
    }

    strm << ") " << x
         << ' ' << y
         << ' ' << vel_x
         << ' ' << vel_y
         << ' ' << body_dir
         << ' ' << neck_dir;
}

void
SerializerPlayerStdv8::serializeFSPlayerArm( std::ostream & strm,
                                             const double & mag,
                                             const double & head ) const
{
    strm << ' ' << mag
         << ' ' << head;
}

void
SerializerPlayerStdv8::serializeFSPlayerStamina( std::ostream & strm,
                                                 const double & stamina,
                                                 const double & effort,
                                                 const double & recovery,
                                                 const double & /*stamina_capacity*/ ) const
{
    strm << " (stamina " << stamina
         << ' ' << effort
         << ' ' << recovery
         << ')';
}

void
SerializerPlayerStdv8::serializeFSCounts( std::ostream & strm,
                                          const int count_kick,
                                          const int count_dash,
                                          const int count_turn,
                                          const int count_catch,
                                          const int count_move,
                                          const int count_turn_neck,
                                          const int count_change_view,
                                          const int count_say ) const
{
    strm << " (count "
         << count_kick << ' '
         << count_dash << ' '
         << count_turn << ' '
         << count_catch << ' '
         << count_move << ' '
         << count_turn_neck << ' '
         << count_change_view << ' '
         << count_say << ')';
}

void
SerializerPlayerStdv8::serializeServerParamBegin( std::ostream & strm ) const
{
    strm << "(server_param ";
}

void
SerializerPlayerStdv8::serializePlayerParamBegin( std::ostream & strm ) const
{
    strm << "(player_param ";
}

void
SerializerPlayerStdv8::serializePlayerTypeBegin( std::ostream & strm ) const
{
    strm << "(player_type ";
}

void
SerializerPlayerStdv8::serializeParam( std::ostream & strm,
                                       const std::string & name,
                                       const int param ) const
{
    strm << "(" << name << ' ' << param << ')';
}

void
SerializerPlayerStdv8::serializeParam( std::ostream & strm,
                                       const std::string & name,
                                       const bool param ) const
{
    strm << "(" << name << ' ' << param << ')';
}

void
SerializerPlayerStdv8::serializeParam( std::ostream & strm,
                                       const std::string & name,
                                       const double & param ) const
{
    strm << "(" << name << ' ' << param << ')';
}

void
SerializerPlayerStdv8::serializeParam( std::ostream & strm,
                                       const std::string & name,
                                       const std::string & param ) const
{
    strm << "(" << name << " \"" << param << "\")";
}

const
SerializerPlayer::Ptr
SerializerPlayerStdv8::create()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 8 ) )
    {
        return SerializerPlayer::Ptr();
    }

    SerializerPlayer::Ptr ptr( new SerializerPlayerStdv8( cre() ) );
    return ptr;
}


namespace {

RegHolder v8 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv8::create, 8 );
RegHolder v9 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv8::create, 9 );
RegHolder v10 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv8::create, 10 );
RegHolder v11 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv8::create, 11 );
RegHolder v12 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv8::create, 12 );
}

}

// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv8.cc
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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include "serializerplayerstdv8.h"

namespace rcss
{

SerializerPlayerStdv8::SerializerPlayerStdv8( const SerializerCommon& common )
    : SerializerPlayerStdv7( common )
{}

SerializerPlayerStdv8::~SerializerPlayerStdv8()
{}

const SerializerPlayerStdv8*
SerializerPlayerStdv8::instance()
{
    rcss::SerializerCommon::Creator cre;
    if( !rcss::SerializerCommon::factory().getCreator( cre, 8 ) )
        return NULL;
    static SerializerPlayerStdv8 ser( cre() );
    return &ser;
}

void
SerializerPlayerStdv8::serializeAllyAudioFull( std::ostream& strm,
                                               const int time,
                                               const double & dir,
                                               const int unum,
                                               const char* msg ) const
{
    strm << "(hear " << time << " "
         << dir << " our " << unum
         << " \"" << msg << "\")";
}

void
SerializerPlayerStdv8::serializeOppAudioFull( std::ostream& strm,
                                              const int time,
                                              const double & dir,
                                              const char* msg ) const
{
    strm << "(hear " << time << " "
         << dir << " opp \"" << msg << "\")";
}

void
SerializerPlayerStdv8::serializeAllyAudioShort( std::ostream& strm,
                                                const int time,
                                                const int unum ) const
{ strm << "(hear " << time << " our " << unum << ")"; }

void
SerializerPlayerStdv8::serializeOppAudioShort( std::ostream& strm,
                                               const int time ) const
{ strm << "(hear " << time << " opp)"; }


void
SerializerPlayerStdv8::serializeFSBall( std::ostream& strm,
                                        const double & x,
                                        const double & y,
                                        const double & vel_x,
                                        const double & vel_y ) const
{
    strm << " ((b)"
         << " " << x
         << " " << y
         << " " << vel_x
         << " " << vel_y
         << ")";
}

void
SerializerPlayerStdv8::serializeFSPlayerBegin( std::ostream& strm,
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
         << " " << unum;
    if( goalie )
        strm << " g";
    else
        strm << " " << type; // hetro player type
    strm << ") " << x
         << " " << y
         << " " << vel_x
         << " " << vel_y
         << " " << body_dir
         << " " << neck_dir;
}

void
SerializerPlayerStdv8::serializeFSPlayerArm( std::ostream& strm,
                                             const double & mag,
                                             const double & head ) const
{
    strm << " " << mag
         << " " << head;
}

void
SerializerPlayerStdv8::serializeFSPlayerEnd( std::ostream& strm,
                                             const double & stamina,
                                             const double & effort,
                                             const double & recovery ) const
{
    strm << " (stamina " << stamina
         << " " << effort
         << " " << recovery
         << "))";
}

void
SerializerPlayerStdv8::serializeFSCounts( std::ostream& strm,
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
         << count_kick << " "
         << count_dash << " "
         << count_turn << " "
         << count_catch << " "
         << count_move << " "
         << count_turn_neck << " "
         << count_change_view << " "
         << count_say << ")";
}

void
SerializerPlayerStdv8::serializeServerParamBegin( std::ostream& strm ) const
{
    strm << "(server_param ";
}

void
SerializerPlayerStdv8::serializePlayerParamBegin( std::ostream& strm ) const
{
    strm << "(player_param ";
}

void
SerializerPlayerStdv8::serializePlayerTypeBegin( std::ostream& strm ) const
{
    strm << "(player_type ";
}

void
SerializerPlayerStdv8::serializeParam( std::ostream& strm,
                                       const std::string& name,
                                       const int param ) const
{
    strm << "(" << name << " " << param << ")";
}

void
SerializerPlayerStdv8::serializeParam( std::ostream& strm,
                                       const std::string& name,
                                       const bool param ) const
{
    strm << "(" << name << " " << param << ")";
}

void
SerializerPlayerStdv8::serializeParam( std::ostream& strm,
                                       const std::string& name,
                                       const double & param ) const
{
    strm << "(" << name << " " << param << ")";
}

void
SerializerPlayerStdv8::serializeParam( std::ostream& strm,
                                       const std::string& name,
                                       const std::string& param ) const
{
    strm << "(" << name << " \"" << param << "\")";
}

namespace
{
const SerializerPlayer*
create()
{ return SerializerPlayerStdv8::instance(); }

lib::RegHolder v8 = SerializerPlayer::factory().autoReg( &create, 8 );
lib::RegHolder v9 = SerializerPlayer::factory().autoReg( &create, 9 );
lib::RegHolder v10 = SerializerPlayer::factory().autoReg( &create, 10 );
lib::RegHolder v11 = SerializerPlayer::factory().autoReg( &create, 11 );
lib::RegHolder v12 = SerializerPlayer::factory().autoReg( &create, 12 );
}
}

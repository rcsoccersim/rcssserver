// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv1.cc
                  Class for serializing data to std v1 players
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

#include "serializerplayerstdv1.h"
#include "param.h"
#include "clangmsg.h"


static char * playmode_strings[] = PLAYMODE_STRINGS;

namespace rcss {

SerializerPlayerStdv1::SerializerPlayerStdv1( const SerializerCommon & common )
    : SerializerPlayer( common )
{

}

SerializerPlayerStdv1::~SerializerPlayerStdv1()
{

}

const
SerializerPlayerStdv1 *
SerializerPlayerStdv1::instance()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 1 ) )
    {
        return NULL;
    }

    static SerializerPlayerStdv1 ser( cre() );
    return &ser;
}

void
SerializerPlayerStdv1::serializeRefAudio( std::ostream & strm,
                                          const int & time,
                                          const char * msg ) const
{
    strm << "(hear " << time << " "
         << REFEREE_NAME << " " << msg << ")";
}

void
SerializerPlayerStdv1::serializeCoachAudio( std::ostream & strm,
                                            const int & time,
                                            const std::string & name,
                                            const char * msg ) const
{
    strm << "(hear " << time << " "
         << name << " " << msg << ")";
}

void
SerializerPlayerStdv1::serializeCoachStdAudio( std::ostream & strm,
                                               const int & time,
                                               const std::string & name,
                                               const rcss::clang::Msg & msg ) const
{
    strm << "(hear " << time << " "
         << name << " " << msg << ")";
}

void
SerializerPlayerStdv1::serializeSelfAudio( std::ostream & strm,
                                           const int & time,
                                           const char * msg ) const
{
    strm << "(hear " << time << " self "
         << msg << ")";
}

void
SerializerPlayerStdv1::serializePlayerAudio( std::ostream & strm,
                                             const int & time,
                                             const double & dir,
                                             const char * msg ) const
{
    strm << "(hear " << time << " "
         << dir << " " << msg << ")";
}

void
SerializerPlayerStdv1::serializeVisualBegin( std::ostream & strm,
                                             const int time ) const
{
    strm << "(see " << time;
}

void
SerializerPlayerStdv1::serializeVisualEnd( std::ostream & strm ) const
{
    strm << ")";
}

// void
// SerializerPlayerStdv1::serializeVisualObject( std::ostream & strm,
//                                               const std::string & name,
//                                               const int dir ) const
// {
//     strm << " (" << name << " " << dir << ")";
// }

// void
// SerializerPlayerStdv1::serializeVisualObject( std::ostream & strm,
//                                               const std::string & name,
//                                               const double & dist,
//                                               const int dir ) const
// {
//     strm << " (" << name << " " << dist << " " << dir << ")";
// }

// void
// SerializerPlayerStdv1::serializeVisualObject( std::ostream & strm,
//                                               const std::string & name,
//                                               const double & dist,
//                                               const int dir,
//                                               const double & dist_chg,
//                                               const double & dir_chg ) const
// {
//     strm << " (" << name << " " << dist << " " << dir
//          << " " << dist_chg << " " << dir_chg
//          << ")";
// }

// void
// SerializerPlayerStdv1::serializeVisualObject( std::ostream & strm,
//                                               const std::string & name,
//                                               const double & dist,
//                                               const int dir,
//                                               const double & dist_chg,
//                                               const double & dir_chg,
//                                               const double & body_dir ) const
// {
//     strm << " (" << name << " " << dist << " " << dir
//          << " " << dist_chg << " " << dir_chg
//          << " " << body_dir
//          << ")";
// }

// void
// SerializerPlayerStdv1::serializeVisualObject( std::ostream & strm,
//                                               const std::string & name,
//                                               const double & dist,
//                                               const int dir,
//                                               const double & dist_chg,
//                                               const double & dir_chg,
//                                               const double & body_dir,
//                                               const double & head_dir ) const
// {
//     strm << " (" << name << " " << dist << " " << dir
//          << " " << dist_chg << " " << dir_chg
//          << " " << body_dir << " " << head_dir
//          << ")";
// }

// void
// SerializerPlayerStdv1::serializeVisualObject( std::ostream & strm,
//                                               const std::string & name,
//                                               const double & dist,
//                                               const int dir,
//                                               const bool tackling ) const
// {
//     strm << " (" << name << " " << dist << " " << dir;
//     if( tackling )
//         strm << " t";
//     strm << ")";
// }

// void
// SerializerPlayerStdv1::serializeVisualObject( std::ostream & strm,
//                                               const std::string & name,
//                                               const double & dist,
//                                               const int dir,
//                                               const int point_dir,
//                                               const bool tackling ) const
// {
//     strm << " (" << name << " " << dist << " " << dir
//          << " " << point_dir;
//     if( tackling )
//         strm << " t";
//     strm << ")";
// }

// void
// SerializerPlayerStdv1::serializeVisualObject( std::ostream & strm,
//                                               const std::string & name,
//                                               const double & dist,
//                                               const int dir,
//                                               const double & dist_chg,
//                                               const double & dir_chg,
//                                               const int body_dir,
//                                               const int head_dir,
//                                               const bool tackling ) const
// {
//     strm << " (" << name << " " << dist << " " << dir
//          << " " << dist_chg << " " << dir_chg
//          << " " << body_dir << " " << head_dir;
//     if( tackling )
//         strm << " t";
//     strm << ")";
// }

// void
// SerializerPlayerStdv1::serializeVisualObject( std::ostream & strm,
//                                               const std::string & name,
//                                               const double & dist,
//                                               const int dir,
//                                               const double & dist_chg,
//                                               const double & dir_chg,
//                                               const int body_dir,
//                                               const int head_dir,
//                                               const int point_dir,
//                                               const bool tackling ) const
// {
//     strm << " (" << name << " " << dist << " " << dir
//          << " " << dist_chg << " " << dir_chg
//          << " " << body_dir << " " << head_dir
//          << " " << point_dir;
//     if( tackling )
//         strm << " t";
//     strm << ")";
// }

void
SerializerPlayerStdv1::serializeBodyBegin( std::ostream & strm,
                                           const int time ) const
{
    strm << "(sense_body " << time;
}

void
SerializerPlayerStdv1::serializeBodyEnd( std::ostream & strm ) const
{
    strm << ")";
}

void
SerializerPlayerStdv1::serializeBodyViewMode( std::ostream & strm,
                                              const char* qual,
                                              const char* width ) const
{
    strm << " (view_mode " << qual << " " << width << ")";
}

void
SerializerPlayerStdv1::serializeBodyStamina( std::ostream & strm,
                                             const double & stamina,
                                             const double & effort ) const
{
    strm << " (stamina " << stamina << " " << effort << ")";
}

void
SerializerPlayerStdv1::serializeBodyVelocity( std::ostream & strm,
                                              const double & mag ) const
{
    strm << " (speed " << mag << ")";
}

void
SerializerPlayerStdv1::serializeBodyVelocity( std::ostream & strm,
                                              const double & mag,
                                              const int head ) const
{
    strm << " (speed " << mag << " " << head << ")";
}

void
SerializerPlayerStdv1::serializeBodyCounts( std::ostream & strm,
                                            const int count_kick,
                                            const int count_dash,
                                            const int count_turn,
                                            const int count_say ) const
{
    strm << " (kick " << count_kick << ")"
         << " (dash " << count_dash << ")"
         << " (turn " << count_turn << ")"
         << " (say " << count_say << ")";
}

void
SerializerPlayerStdv1::serializeBodyCounts( std::ostream & strm,
                                            const int count_catch,
                                            const int count_move,
                                            const int count_change_view ) const
{
    strm << " (catch " << count_catch << ")"
         << " (move " << count_move << ")"
         << " (change_view " << count_change_view << ")";
}

void
SerializerPlayerStdv1::serializeNeckAngle( std::ostream & strm,
                                           const int ang ) const
{
    strm << " (head_angle " << ang << ")";
}

void
SerializerPlayerStdv1::serializeNeckCount( std::ostream & strm,
                                           const int count_turn_neck ) const
{
    strm << " (turn_neck " << count_turn_neck << ")";
}

void
SerializerPlayerStdv1::serializeArm( std::ostream & strm,
                                     const int movable_cycles,
                                     const int expires_cycles,
                                     const double & dist,
                                     const int head,
                                     const int count ) const
{
    strm << " (arm"
         << " (movable " << movable_cycles << ")"
         << " (expires " << expires_cycles << ")"
         << " (target " << dist << " " << head << ")"
         << " (count " << count << ")"
         << ")";
}

void
SerializerPlayerStdv1::serializeFocus( std::ostream & strm,
                                       const char* name,
                                       const int count ) const
{
    strm << " (focus"
         << " (target " << name << ")"
         << " (count " << count << ")"
         << ")";
}

void
SerializerPlayerStdv1::serializeFocus( std::ostream & strm,
                                       const char* team,
                                       const int unum,
                                       const int count ) const
{
    strm << " (focus"
         << " (target " << team << " " << unum << ")"
         << " (count " << count << ")"
         << ")";
}


void
SerializerPlayerStdv1::serializeTackle( std::ostream & strm,
                                        const int cycles,
                                        const int count ) const
{
    strm << " (tackle"
         << " (expires " << cycles << ")"
         << " (count " << count << ")"
         << ")";
}

void
SerializerPlayerStdv1::serializeCollision( std::ostream & strm,
                                           const bool ball_collide,
                                           const bool player_collide,
                                           const bool post_collide ) const
{
    bool collide = false;

    strm << " (collision";

    if ( ball_collide )
    {
        collide = true;
        strm << " (ball)";
    }

    if ( player_collide )
    {
        collide = true;
        strm<< " (player)";
    }

    if ( post_collide )
    {
        collide = true;
        strm<< " (post)";
    }

    if ( ! collide )
    {
        strm << " none";
    }

    strm << ")";
}

void
SerializerPlayerStdv1::serializeFSBegin( std::ostream & strm,
                                         const int time ) const
{
    strm.precision( 6 );
    strm << "(fullstate " << time;
}

void
SerializerPlayerStdv1::serializeFSEnd( std::ostream & strm) const
{
    strm << ")";
}

void
SerializerPlayerStdv1::serializeFSViewMode( std::ostream & strm,
                                            const char* qual,
                                            const char* width ) const
{
    strm << " (vmode " << qual << " " << width << ")";
}

void
SerializerPlayerStdv1::serializeFSPlayMode( std::ostream & strm,
                                            const char* mode ) const
{
    strm << " (pmode " << mode << ")";
}

// caution: in version 5 left is the points for the left team
// and right is the points for the right team.  In version 8
// and later left is the points of the players team and right
// is the points for the opp
void
SerializerPlayerStdv1::serializeFSScore( std::ostream & strm,
                                         const int left,
                                         const int right ) const
{
    strm << " (score " << left << " " << right << ")";
}


void
SerializerPlayerStdv1::serializeFSBall( std::ostream & strm,
                                        const double & x,
                                        const double & y,
                                        const double & vel_x,
                                        const double & vel_y ) const
{
    strm << " (ball"
         << " " << x
         << " " << y
         << " " << vel_x
         << " " << vel_y
         << ")";
}

void
SerializerPlayerStdv1::serializeFSPlayerBegin( std::ostream & strm,
                                               const char side,
                                               const int unum,
                                               const bool,
                                               const int,
                                               const double & x,
                                               const double & y,
                                               const double & vel_x,
                                               const double & vel_y,
                                               const double & body_dir,
                                               const double & neck_dir ) const

{
    strm << " (" << side << "_" << unum
         << " " << x
         << " " << y
         << " " << vel_x
         << " " << vel_y
         << " " << body_dir
         << " " << neck_dir;
}

void
SerializerPlayerStdv1::serializeFSPlayerEnd( std::ostream & strm,
                                             const double & stamina,
                                             const double & effort,
                                             const double & recovery ) const
{
    strm << " " << stamina
         << " " << effort
         << " " << recovery
         << ")";
}


void
SerializerPlayerStdv1::serializeInit( std::ostream & strm,
                                      const char * side,
                                      const int unum,
                                      const PlayMode & mode ) const
{
    strm << "(init " << side << " " << unum << " "
         << playmode_strings[ mode ] << ")";
}

void
SerializerPlayerStdv1::serializeReconnect( std::ostream & strm,
                                           const char * side,
                                           const PlayMode & mode ) const
{
    strm << "(reconnect " << side << " "
         << playmode_strings[ mode ] << ")";
}

void
SerializerPlayerStdv1::serializeOKClang( std::ostream & strm,
                                         const int min,
                                         const int max ) const
{
    strm << "(ok clang (ver " << min << " " << max << "))";
}

void
SerializerPlayerStdv1::serializeErrorNoTeamName( std::ostream & strm,
                                                 const std::string & team_name ) const
{
    strm << "(error no team with name " << team_name << ")";
}

void
SerializerPlayerStdv1::serializeScore( std::ostream & strm,
                                       const int time,
                                       const int our,
                                       const int opp ) const
{
    strm << "(score " << time << " " << our << " " << opp << ")";
}

namespace {

const
SerializerPlayer *
create()
{
    return SerializerPlayerStdv1::instance();
}

lib::RegHolder v1 = SerializerPlayer::factory().autoReg( &create, 1 );
lib::RegHolder v2 = SerializerPlayer::factory().autoReg( &create, 2 );
lib::RegHolder v3 = SerializerPlayer::factory().autoReg( &create, 3 );
lib::RegHolder v4 = SerializerPlayer::factory().autoReg( &create, 4 );
lib::RegHolder v5 = SerializerPlayer::factory().autoReg( &create, 5 );
lib::RegHolder v6 = SerializerPlayer::factory().autoReg( &create, 6 );

}
}

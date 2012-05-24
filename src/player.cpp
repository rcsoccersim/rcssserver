// -*-c++-*-

/***************************************************************************
                             player.cpp
               A class for field players and goalies
                          -------------------
    begin                : 26-NOV-2001
    copyright            : (C) 2001, 2002 by The RoboCup Soccer Server
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

#include "player.h"

#include "coach.h"
#include "stadium.h"
#include "team.h"
#include "types.h"
#include "random.h"
#include "referee.h"
#include "heteroplayer.h"
#include "serverparam.h"
#include "playerparam.h"
#include "utility.h"

#include "serializer.h"
#include "initsenderplayer.h"
#include "bodysender.h"
#include "fullstatesender.h"
#include "visualsenderplayer.h"

#include <algorithm>
#include <cassert>
#include <cctype>

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

namespace {

inline
double
NormalizeDashPower( const double & p )
{
    return rcss::bound( ServerParam::instance().minDashPower(),
                        p,
                        ServerParam::instance().maxDashPower() );
}

inline
double
NormalizeDashAngle( const double & d )
{
    return rcss::bound( ServerParam::instance().minDashAngle(),
                        d,
                        ServerParam::instance().maxDashAngle() );
}

inline
double
NormalizeKickPower( const double & p )
{
    return rcss::bound( ServerParam::instance().minPower(),
                        p,
                        ServerParam::instance().maxPower() );
}

// For v11 or older version
inline
double
NormalizeTacklePower( const double & p )
{
    return rcss::bound( - ServerParam::instance().maxBackTacklePower(),
                        p,
                        ServerParam::instance().maxTacklePower() );
}

inline
double
NormalizeMoment( const double & p )
{
    return Deg2Rad( rcss::bound( ServerParam::instance().minMoment(),
                                 p,
                                 ServerParam::instance().maxMoment() ) );
}

inline
double
NormalizeNeckMoment( const double & p )
{
    return Deg2Rad( rcss::bound( ServerParam::instance().minNeckMoment(),
                                 p,
                                 ServerParam::instance().maxNeckMoment() ) );
}

inline
double
NormalizeNeckAngle( const double & p )
{
    return rcss::bound( Deg2Rad( ServerParam::instance().minNeckAngle() ),
                        p,
                        Deg2Rad( ServerParam::instance().maxNeckAngle() ) );
}

} // end of no-name namespace


Player::Player( Stadium & stadium,
                Team * team,
                int number )
    : MPObject( stadium,
                "", "",
                O_TYPE_PLAYER_NAME, O_TYPE_PLAYER_NAME_SHORT ),
      VISIBLE_DISTANCE( ServerParam::instance().visibleDistance() ),
      VISIBLE_DISTANCE2( VISIBLE_DISTANCE * VISIBLE_DISTANCE ),
      //
      M_init_observer( new rcss::InitObserverPlayer ),
      M_observer( new rcss::ObserverPlayer ),
      M_body_observer( new rcss::BodyObserverPlayer ),
      M_fullstate_observer( new rcss::FullStateObserver ),
      M_parser( *this ),
      //
      M_version( 3.0 ),
      M_team( team ),
      M_side( team->side() ),
      M_unum( number ),
      M_goalie( false ),
      //
      M_unum_far_length( 20.0 ),
      M_unum_too_far_length( 40.0 ),
      M_team_far_length( 40.0 ),
      M_team_too_far_length( 60.0 ),
      //
      M_clang_min_ver( 0 ),
      M_clang_max_ver( 0 ),
      //
      M_player_type( NULL ),
      M_player_type_id( 0 ),
      M_substituted( false ),
      M_kick_rand( ServerParam::instance().kickRand() ), // pfr 8/14/00: for RC2000 evaluation
      //
      M_state( DISABLE ),
      M_card_count( 0 ),
      //
      M_synch_see( false ),
      M_visual_send_interval( 4 ),
      M_high_quality( true ),
      M_visible_angle( Deg2Rad( ServerParam::instance().visibleAngleDegree() ) ),
      M_view_width( rcss::pcom::NORMAL ),
      //
      M_hear_capacity_from_teammate( ServerParam::instance().hearMax() ),
      M_hear_capacity_from_opponent( ServerParam::instance().hearMax() ),
      //
      M_stamina( ServerParam::instance().staminaMax() ),
      M_recovery( ServerParam::instance().recoverInit() ),
      M_effort( ServerParam::instance().effortInit() ),
      M_stamina_capacity( ServerParam::instance().staminaCapacity() ),
      M_consumed_stamina( 0.0 ),
      //
      M_angle_body( 0.0 ),
      M_angle_body_committed( 0.0 ),
      M_angle_neck( 0.0 ),
      M_angle_neck_committed( 0.0 ),
      //
      M_ball_collide( false ),
      M_player_collide( false ),
      M_post_collide( false ),
      //
      M_command_done( false ),
      M_turn_neck_done( false ),
      M_done_received( false ),
      //
      M_goalie_catch_ban( 0 ),
      M_goalie_moves_since_catch( 0 ),
      M_kick_cycles( 0 ),
      M_dash_cycles( 0 ),
      //
      M_kick_count( 0 ),
      M_dash_count( 0 ),
      M_turn_count( 0 ),
      M_catch_count( 0 ),
      M_move_count( 0 ),
      M_turn_neck_count( 0 ),
      M_change_view_count( 0 ),
      M_say_count( 0 ),
      M_arm( ServerParam::instance().pointToBan(),
             ServerParam::instance().pointToDuration() ),
      M_attentionto_count( 0 ),
      M_tackle_cycles( 0 ),
      M_tackle_count( 0 ),
      M_foul_cycles( 0 ),
      M_foul_count( 0 )
{
    assert( team );

    M_enable = false;

    M_weight = ServerParam::instance().playerWeight();
    M_max_speed = ServerParam::instance().playerSpeedMax();
    M_max_accel = ServerParam::instance().playerAccelMax();

    M_pos.x = -( unum() * 3 * team->side() );
    M_pos.y = - ServerParam::PITCH_WIDTH/2.0 - 3.0;

    setPlayerType( 0 );
    recoverAll();
}

Player::~Player()
{
#if 0
    if ( ! isGoalie() )
    {
        std::cerr << M_team->name() << ','
                  << unum() << ','
                  << M_player_type->staminaIncMax() << ','
                  << M_player_type->staminaIncMax() * 3000 << ','
                  << M_consumed_stamina;
        //               << " dash_count = " << M_dash_count;
        //     if ( M_dash_count > 0 )
        //     {
        //         std::cerr << " consumed_stamina/dash =  " << M_consumed_stamina / M_dash_count;
        //     }
        std::cerr << std::endl;
    }
#endif

    if ( M_init_observer )
    {
        delete M_init_observer;
        M_init_observer = static_cast< rcss::InitObserverPlayer * >( 0 );
    }

    if ( M_observer )
    {
        delete M_observer;
        M_observer = static_cast< rcss::ObserverPlayer * >( 0 );
    }

    if ( M_body_observer )
    {
        delete M_body_observer;
        M_body_observer = static_cast< rcss::BodyObserverPlayer * >( 0 );
    }

    if ( M_fullstate_observer )
    {
        delete M_fullstate_observer;
        M_fullstate_observer = static_cast< rcss::FullStateObserver * >( 0 );
    }
}

bool
Player::init( const double ver,
              const bool goalie )
{
    M_version = ver;
    M_goalie = goalie;

    setEnable();

    M_goalie_catch_ban = 0;
    M_goalie_moves_since_catch = 0;

    {
        char lname[128], sname[128];
        snprintf( lname, 128, PLAYER_NAME_FORMAT, team()->name().c_str(), unum() );
        snprintf( sname, 128, PLAYER_NAME_FORMAT_SHORT, team()->name().c_str(), unum(),
                  isGoalie() ? GOALIE_VISUAL_STRING : "" );
        setName( lname, sname );
    }

    char buf[128];
    snprintf( buf, 128, PLAYER_NAME_FAR_FORMAT, team()->name().c_str() );
    M_name_far = buf;
    snprintf( buf, 128, PLAYER_NAME_TOOFAR_FORMAT );
    M_name_toofar = buf;

    snprintf( buf, 128, PLAYER_NAME_FAR_FORMAT_SHORT, team()->name().c_str() );
    M_short_name_far = buf;
    snprintf( buf, 128, PLAYER_NAME_TOOFAR_FORMAT_SHORT );
    M_short_name_toofar = buf;

    M_angle_body_committed = SideDirection( side() );

    // pfr 8/14/00: for RC2000 evaluation
    M_randp = ServerParam::instance().playerRand();
    if ( ServerParam::instance().teamActuatorNoise() )
    {
        M_randp *= team()->prandFactorTeam();
        M_kick_rand *= team()->kickRandFactorTeam();
    }

#ifdef NEW_QSTEP
    dist_qstep_player = team->distQstepTeam();
    land_qstep_player = team->landQstepTeam();
    dir_qstep_player  = team->dirQstepTeam();
#endif

    if ( ! setSenders() )
    {
        disable();
        std::cerr << "Error: Could not find serializer or sender for version "
                  << version() << std::endl;
        return false;
    }

    return true;
}

void
Player::setPlayerType( const int id )
{
    const HeteroPlayer * type = M_stadium.playerType( id );
    if ( ! type )
    {
        return;
    }

    M_player_type_id = id;
    M_player_type = type;
    M_max_speed = M_player_type->playerSpeedMax();
    M_decay = M_player_type->playerDecay();
    M_size = M_player_type->playerSize();
    M_kick_rand = M_player_type->kickRand();

    if ( ServerParam::instance().teamActuatorNoise() )
    {
        M_kick_rand *= team()->kickRandFactorTeam();
    }
}

void
Player::substitute( const int type )
{
    setPlayerType( type );
    recoverAll();

    M_state &= ( STAND | GOALIE | DISCARD );
    M_card_count = 0;
    M_substituted = true;
}

void
Player::setEnable()
{
    M_state = STAND;
    M_enable = true;
    if ( isGoalie() )
    {
        M_state |= GOALIE;
    }
}


void
Player::resetState()
{
    int state = ( STAND | GOALIE | DISCARD
                  | YELLOW_CARD | RED_CARD );

    if ( M_kick_cycles > 0 )
    {
        state |= ( KICK | KICK_FAULT );
    }

    if ( tackleCycles() > 0 )
    {
        state |= ( TACKLE | TACKLE_FAULT );
    }

    if ( foulCycles() > 0 )
    {
        state |= FOUL_CHARGED;
    }

    M_state &= state;
}

void
Player::disable()
{
    if ( M_enable )
    {
        std::cout << "A player disconnected : ("
                  << team()->name() << " " << unum()
                  << ")\n";
    }

    if ( isGoalie()
         && this == M_stadium.ballCatcher() )
    {
        M_stadium.clearBallCatcher();
    }

    M_enable = false;

    int card = M_state;
    card &= ( YELLOW_CARD | RED_CARD );
    M_state = DISABLE;
    M_state |= card;

    M_pos.x = -( unum() * 3 * side() );
    M_pos.y = - ServerParam::PITCH_WIDTH/2.0 - 3.0;
    M_vel.x = 0.0;
    M_vel.y = 0.0;
    M_accel.x = 0.0;
    M_accel.y = 0.0;

    if ( connected() )
    {
        RemoteClient::close();
    }
}

void
Player::discard()
{
    if ( M_state & STAND )
    {
        disable();
        if ( ! ( M_state & DISCARD ) )
        {
            M_state |= DISCARD;
        }
        else
        {
            M_state &= ~DISCARD;
        }
    }
}

void
Player::changeToGoalie()
{
    M_goalie = true;

    setEnable();

    {
        char lname[128], sname[128];
        snprintf( lname, 128, PLAYER_NAME_FORMAT, team()->name().c_str(), unum() );
        snprintf( sname, 128, PLAYER_NAME_FORMAT_SHORT, team()->name().c_str(), unum(),
                  isGoalie() ? GOALIE_VISUAL_STRING : "" );
        setName( lname, sname );
    }
}

void
Player::parseMsg( char * msg,
                  const size_t & len )
{
    char * command = msg;
    if ( command[ len - 1 ] != 0 )
    {
        if ( version() >= 8.0 )
        {
            send( "(warning message_not_null_terminated)" );
        }
        command[ len ] = 0;
    }
    M_stadium.logger().writePlayerLog( *this, command, RECV );

    /** Call the PlayerCommandParser */
    if (
#ifndef __CYGWIN__
        M_parser.parse( command ) != 0
#else
        // Cygwin's flex/bison may cause problems.
        ! parseCommand( command )
#endif
        )
    {
        send( "(error illegal_command_form)" );
        std::cerr << "Error parsing >" << command << "<\n";
    }
}

bool
Player::parseCommand( const char * command )
{
    const char * buf = command;
    int n_read = 0;

    int count = 0;
    while ( *buf != '\0' )
    {
        //while ( *buf == ' ' || *buf == '\t' || *buf == '\n' ) ++buf;
        while ( std::isspace( *buf ) ) ++buf;
        if ( *buf == '\0' ) break;

        if ( ! std::strncmp( buf, "(turn ", 6 ) )
        {
            double moment = 0.0;
            if ( std::sscanf( buf, " ( turn %lf ) %n ",
                              &moment, &n_read ) != 1 )
            {
                std::cerr << "Error parsing >" << buf << "<\n";
                return false;
            }
            buf += n_read;

            turn( moment );
        }
        else if ( ! std::strncmp( buf, "(dash ", 6 ) )
        {
            double power = 0.0, dir = 0.0;
            if ( std::sscanf( buf, " ( dash %lf %lf ) %n ", &power, &dir, &n_read ) != 2
                 && std::sscanf( buf, " ( dash %lf ) %n ", &power, &n_read ) != 1 )
            {
                std::cerr << "Error parsing >" << buf << "<\n";
                return false;
            }

            buf += n_read;

            dash( power, dir );
        }
        else if ( ! std::strncmp( buf, "(turn_neck ", 11 ) )
        {
            double moment = 0.0;
            if ( std::sscanf( buf, " ( turn_neck %lf ) %n ",
                              &moment, &n_read ) != 1 )
            {
                std::cerr << "Error parsing >" << buf << "<\n";
                return false;
            }
            buf += n_read;

            turn_neck( moment );
        }
        else if ( ! std::strncmp( buf, "(say ", 5 ) )
        {
            buf += 5;
            char msg[MaxMesg];
            if ( *buf == '"'
                 && std::sscanf( buf, " \"%[-0-9a-zA-Z ().+*/?<>_]\" ) %n ",
                                 msg, &n_read ) == 1 )
            {
                buf += n_read;
                say( msg );
                return true;
            }
            else if ( std::sscanf( buf, "%[-0-9a-zA-Z ().+*/?<>_]",
                                   msg ) == 1 )
            {
                int l = std::strlen( msg );
                if ( msg[l-1] == ')' )
                {
                    msg[l-1] = '\0';
                }
                else
                {
                    std::cerr << "Error parsing >" << buf << "<\n";
                    return false;
                }

                say( msg );

                // the say command consumes the rest of the
                // command string so no point trying to parse
                // subsequent commands.
                return true;
            }

            std::cerr << "Error parsing >" << buf << "<\n";
            return false;
        }
        else if ( ! std::strncmp( buf, "(attentionto ", 13 ) )
        {
            if ( ! std::strncmp( buf, "(attentionto off)", 17 ) )
            {
                buf += 17;
                attentionto( false, rcss::pcom::UNKNOWN_TEAM, std::string(), 0 );
            }
            else
            {
                bool on = true;
                rcss::pcom::TEAM at_side = rcss::pcom::UNKNOWN_TEAM;
                char at_team_name[256];
                int at_unum = 0;
                if ( std::sscanf( buf, " ( attentionto %s %d ) %n ",
                                  at_team_name, &at_unum, &n_read ) != 2 )
                {
                    std::cerr << "Error parsing attentionto >" << buf << "<\n";
                    return false;
                }
                buf += n_read;

                if ( ! std::strcmp( at_team_name, "our" ) )
                {
                    at_side = rcss::pcom::OUR;
                }
                else if ( ! std::strcmp( at_team_name, "opp" ) )
                {
                    at_side = rcss::pcom::OPP;
                }
                else if ( ! std::strcmp( at_team_name, "l" ) )
                {
                    at_side = rcss::pcom::LEFT_SIDE;
                }
                else if ( ! std::strcmp( at_team_name, "r" ) )
                {
                    at_side = rcss::pcom::RIGHT_SIDE;
                }

                attentionto( on, at_side, std::string( at_team_name ), at_unum );
            }
        }
        else if ( ! std::strncmp( buf, "(kick ", 6 ) )
        {
            double power = 0.0;
            double dir = 0.0;
            if ( std::sscanf( buf, " ( kick %lf %lf ) %n ",
                              &power, &dir, &n_read ) != 2 )
            {
                std::cerr << "Error parsing >" << buf << "<\n";
                return false;
            }
            buf += n_read;

            kick( power, dir );
        }
        else if ( ! std::strncmp( buf, "(long_kick ", 11 ) )
        {
            double power = 0.0;
            double dir = 0.0;
            if ( std::sscanf( buf, " ( long_kick %lf %lf ) %n ",
                              &power, &dir, &n_read ) != 2 )
            {
                std::cerr << "Error parsing >" << buf << "<\n";
                return false;
            }
            buf += n_read;

            long_kick( power, dir );
        }
        else if ( ! std::strncmp( buf, "(tackle ", 8 ) )
        {
            double power_or_dir = 0.0;
            char foul_str[16];
            std::memset( foul_str, 0, 16 );
            if ( std::sscanf( buf, " ( tackle %lf %[^)] ) %n ",
                              &power_or_dir, foul_str, &n_read ) != 2
                 && std::sscanf( buf, " ( tackle %lf ) %n ",
                                 &power_or_dir, &n_read ) != 1 )
            {
                std::cerr << "Error parsing >" << buf << "<\n";
                return false;
            }

            if ( std::strlen( foul_str ) > 0 )
            {
                if ( ! std::strcmp( foul_str, "on" )
                     || ! std::strcmp( foul_str, "true" ) )
                {
                    tackle( power_or_dir, true );
                }
                else if ( ! std::strcmp( foul_str, "off" )
                          || ! std::strcmp( foul_str, "false" ) )
                {
                    tackle( power_or_dir, false );
                }
                else
                {
                    std::cerr << "Error parsing >" << buf << "<\n";
                }
            }
            else
            {
                tackle( power_or_dir );
            }

            buf += n_read;
        }
        else if ( ! std::strncmp( buf, "(pointto ", 9 ) )
        {
            if ( ! std::strncmp( buf, "(pointto off)", 13 ) )
            {
                buf += 13;
                pointto( false, 0.0, 0.0 );
            }
            else
            {
                double dist, head;
                if ( std::sscanf( buf, " ( pointto %lf %lf ) %n ",
                                  &dist, &head, &n_read ) != 2 )
                {
                    std::cerr << "Error parsing >" << buf << "<\n";
                    return false;
                }
                buf += n_read;

                pointto( true, dist, head );
            }
        }
        else if ( ! std::strncmp( buf, "(catch ", 7 ) )
        {
            double dir = 0.0;
            if ( std::sscanf( buf, " ( catch %lf ) %n ",
                              &dir, &n_read ) != 1 )
            {
                std::cerr << "Error parsing >" << buf << "<\n";
                return false;
            }
            buf += n_read;

            goalieCatch( dir );
        }
        else if ( ! std::strncmp( buf, "(move ", 6 ) )
        {
            double x, y;
            if ( std::sscanf( buf, " ( move %lf %lf ) %n ",
                              &x, &y, &n_read ) != 2
                 && std::sscanf( buf, " ( move ( %lf %lf ) ) %n ",
                                 &x, &y, &n_read ) != 2 )
            {
                std::cerr << "Error parsing >" << buf << "<\n";
                return false;
            }
            buf += n_read;

            move( x, y );
        }
        else if ( ! std::strncmp( buf, "(change_view ", 13 ) )
        {
            rcss::pcom::VIEW_WIDTH vwidth = rcss::pcom::NORMAL;
            rcss::pcom::VIEW_QUALITY vquality = rcss::pcom::HIGH;

            char width[16], quality[16];
            std::memset( width, 0, 16 );
            std::memset( quality, 0, 16 );

            if ( std::sscanf( buf, " ( change_view %15[^ )] %15[^ )] ) %n ",
                              width, quality, &n_read ) != 2
                 && std::sscanf( buf, " ( change_view %15[^ )] ) %n ",
                                 width, &n_read ) != 1 )
            {
                std::cerr << "Error parsing >" << buf << "<\n";
                return false;
            }
            buf += n_read;

            if ( ! std::strcmp( width, "narrow" ) ) vwidth = rcss::pcom::NARROW;
            else if ( ! std::strcmp( width, "normal" ) ) vwidth = rcss::pcom::NORMAL;
            else if ( ! std::strcmp( width, "wide" ) ) vwidth = rcss::pcom::WIDE;
            else
            {
                std::cerr << "Error parsing view width >" << buf << "< width=" << width << "\n";
                return false;
            }

            if ( ! std::strcmp( quality, "low" ) ) vquality = rcss::pcom::LOW;
            else if ( ! std::strcmp( quality, "high" ) ) vquality = rcss::pcom::HIGH;
            else if ( std::strlen( quality ) == 0 ) vquality = rcss::pcom::HIGH;
            else
            {
                std::cerr << "Error parsing view quality >" << buf << "<\n";
                return false;
            }

            change_view( vwidth, vquality );
        }
        else if ( ! std::strncmp( buf, "(compression ", 13 ) )
        {
            int level = 0;
            if ( std::sscanf( buf, " ( compression %d ) %n ",
                              &level, &n_read ) != 1 )
            {
                std::cerr << "Error parsing >" << buf << "<\n";
                return false;
            }
            buf += n_read;

            compression( level );
        }
        else if ( ! std::strncmp( buf, "(sense_body)", 12 ) )
        {
            buf += 12;
            sense_body();
        }
        else if ( ! std::strncmp( buf, "(score)", 7 ) )
        {
            buf += 7;
            score();
        }
        else if ( ! std::strncmp( buf, "(bye)", 5 ) )
        {
            buf += 5;
            bye();
        }
        else if ( ! std::strncmp( buf, "(done)", 6 ) )
        {
            buf += 6;
            done();
        }
        else if ( ! std::strncmp( buf, "(clang ", 7 ) )
        {
            int min_ver, max_ver;
            if ( std::sscanf( buf, " ( clang ( ver %d %d ) ) %n ",
                              &min_ver, &max_ver, &n_read ) != 2 )
            {
                std::cerr << "Error parsing >" << buf << "<\n";
                return false;
            }
            buf += n_read;

            clang( min_ver, max_ver );
        }
        else if ( ! std::strncmp( buf, "(ear ", 5 ) )
        {
            int n = parseEar( buf );
            if ( n == 0 )
            {
                std::cerr << "Error parsing ear >" << buf << "<\n";
                return false;
            }
            buf += n;
        }
        else if ( ! std::strncmp( buf, "(synch_see)", 11 ) )
        {
            buf += 11;
            synch_see();
        }
        else
        {
            std::cerr << "Error parsing >" << buf << "<\n";
            return false;
        }

        ++count;
    }


    return ( count > 0 );
}

int
Player::parseEar( const char * command )
{
    int n_read = 0;

    char onoff[16];
    char team_str[256];
    char mode_str[16];

    rcss::pcom::TEAM team_side = rcss::pcom::UNKNOWN_TEAM;
    rcss::pcom::EAR_MODE mode = rcss::pcom::UNKNOWN_EAR_MODE;

    if ( std::sscanf( command, " ( ear ( %15[^ )] %255[^ )] %15[^ )] ) ) %n ",
                      onoff, team_str, mode_str, &n_read ) == 3 )
    {
        bool on = false;
        if( ! std::strcmp( onoff, "on" ) ) on = true;
        else if( ! std::strcmp( onoff, "off" ) ) on = false;
        else return 0;

        if ( ! std::strcmp( team_str, "our" ) )
        {
            team_side = rcss::pcom::OUR;
        }
        else if ( ! std::strcmp( team_str, "opp" ) )
        {
            team_side = rcss::pcom::OPP;
        }
        else if( ! std::strcmp( team_str, "left" ) || ! std::strcmp( team_str, "l" ) )
        {
            team_side = rcss::pcom::LEFT_SIDE;
        }
        else if( ! std::strcmp( team_str, "right" ) || ! std::strcmp( team_str, "r" ) )
        {
            team_side = rcss::pcom::RIGHT_SIDE;
        }

        if ( ! std::strcmp( mode_str, "partial" ) || ! std::strcmp( mode_str, "p" ) )
        {
            mode = rcss::pcom::PARTIAL;
        }
        else if( ! std::strcmp( mode_str, "complete" ) || ! std::strcmp( mode_str, "c" ) )
        {
            mode = rcss::pcom::COMPLETE;
        }
        else
        {
            return 0;
        }

        ear( on, team_side, team_str, mode );
        return n_read;
    }
    else if ( std::sscanf( command, " ( ear ( %[^ )] %[^ )] ) ) %n ",
                           onoff, team_str, &n_read ) == 2 )
    {
        bool on;
        if ( ! std::strcmp( onoff, "on" ) ) on = true;
        else if( ! std::strcmp( onoff, "off" ) ) on = false;
        else return 0;

        if ( ! std::strcmp( team_str, "our" ) )
        {
            team_side = rcss::pcom::OUR;
        }
        else if ( ! std::strcmp( team_str, "opp" ) )
        {
            team_side = rcss::pcom::OPP;
        }
        else if ( ! std::strcmp( team_str, "left" ) || ! std::strcmp( team_str, "l" ) )
        {
            team_side = rcss::pcom::LEFT_SIDE;
        }
        else if ( ! std::strcmp( team_str, "right" ) || ! std::strcmp( team_str, "r" ) )
        {
            team_side = rcss::pcom::RIGHT_SIDE;
        }
        else if ( ! std::strcmp( mode_str, "partial" ) || ! std::strcmp( mode_str, "p" ) )
        {
            mode = rcss::pcom::PARTIAL;
        }
        else if( ! std::strcmp( mode_str, "complete" ) || ! std::strcmp( mode_str, "c" ) )
        {
            mode = rcss::pcom::COMPLETE;
        }

        ear( on, team_side, team_str, mode );
        return n_read;
    }
    else if ( std::sscanf( command, " ( ear ( %[^ )] ) ) %n ",
                           onoff, &n_read ) == 1 )
    {
        bool on;
        if ( ! std::strcmp( onoff, "on" ) ) on = true;
        else if( ! std::strcmp( onoff, "off" ) ) on = false;
        else return 0;

        ear( on, rcss::pcom::UNKNOWN_TEAM, std::string(), rcss::pcom::UNKNOWN_EAR_MODE );
        return n_read;
    }

    return 0;
}


void
Player::send( const char * msg )
{
    if ( RemoteClient::send( msg, std::strlen( msg ) + 1 ) != -1 )
    {
        M_stadium.logger().writePlayerLog( *this, msg, SEND );
    }
}


void
Player::dash( double power )
{
    dash( power, 0.0 );

    //     if ( ! M_command_done )
    //     {
    //         // calculate stamina
    //         double power_need = ( power < 0.0
    //                               ? NormalizeDashPower( power ) * -2.0
    //                               : NormalizeDashPower( power ) );
    //         if ( power_need > stamina() + M_player_type->extraStamina() )
    //         {
    //             power_need = stamina() + M_player_type->extraStamina();
    //         }
    //         M_stamina -= power_need;
    //         if ( stamina() < 0.0 )
    //         {
    //             M_stamina = 0.0;
    //         }

    //         if ( M_stadium.playmode() != PM_BeforeKickOff
    //              && M_stadium.playmode() != PM_TimeOver )
    //         {
    //             M_consumed_stamina += power_need;
    //         }

    //         power = ( power < 0.0
    //                   ? power_need / -2.0
    //                   : power_need );
    //         double effective_dash_power
    //             = effort()
    //             * power
    //             * M_player_type->dashPowerRate();
    //         if ( pos().y < 0.0 )
    //         {
    //             effective_dash_power /= ( side() == LEFT
    //                                       ? ServerParam::instance().slownessOnTopForLeft()
    //                                       : ServerParam::instance().slownessOnTopForRight() );
    //         }

    //         push( PVector::fromPolar( effective_dash_power,
    //                                   angleBodyCommitted() ) );
    //         ++M_dash_count;
    //         M_command_done = true;
    //     }
}

void
Player::dash( double power,
              double dir )
{
    if ( ! M_command_done )
    {
        const ServerParam & param = ServerParam::instance();

        power = NormalizeDashPower( power );
        dir = NormalizeDashAngle( dir );

        if ( param.dashAngleStep() < EPS )
        {
            // players can dash in any direction.
        }
        else
        {
            // The dash direction is discretized by server::dash_angle_step
            dir = param.dashAngleStep() * rint( dir / param.dashAngleStep() );
        }

        bool back_dash = power < 0.0;

        double power_need = ( back_dash
                              ? power * -2.0
                              : power );
        power_need = std::min( power_need, stamina() + M_player_type->extraStamina() );
        M_stamina = std::max( 0.0, stamina() - power_need );

        power = ( back_dash
                  ? power_need / -2.0
                  : power_need );

        double dir_rate = ( std::fabs( dir ) > 90.0
                            ? param.backDashRate() - ( ( param.backDashRate() - param.sideDashRate() )
                                                       * ( 1.0 - ( std::fabs( dir ) - 90.0 ) / 90.0 ) )
                            : param.sideDashRate() + ( ( 1.0 - param.sideDashRate() )
                                                       * ( 1.0 - std::fabs( dir ) / 90.0 ) )
                            );
        dir_rate = rcss::bound( 0.0, dir_rate, 1.0 );

        double effective_dash_power = std::fabs( effort()
                                                 * power
                                                 * dir_rate
                                                 * M_player_type->dashPowerRate() );
        if ( pos().y < 0.0 )
        {
            effective_dash_power /= ( side() == LEFT
                                      ? param.slownessOnTopForLeft()
                                      : param.slownessOnTopForRight() );
        }

        if ( back_dash )
        {
            dir += 180.0;
        }

        push( PVector::fromPolar( effective_dash_power,
                                  normalize_angle( angleBodyCommitted() + Deg2Rad( dir ) ) ) );

        M_dash_cycles = 1;
        ++M_dash_count;
        M_command_done = true;
    }
}


void
Player::turn( double moment )
{
    if ( ! M_command_done )
    {
        M_angle_body = normalize_angle( angleBodyCommitted()
                                        + ( 1.0 + drand( -M_randp, M_randp ) )
                                        * NormalizeMoment( moment )
                                        / ( 1.0 + M_player_type->inertiaMoment() * vel().r() ) );
        ++M_turn_count;
        M_command_done = true;
    }
}

void
Player::turn_neck( double moment )
{
    if ( ! M_turn_neck_done )
    {
        M_angle_neck = NormalizeNeckAngle( angleNeckCommitted()
                                           + NormalizeNeckMoment( moment ) );
        ++M_turn_neck_count;
        M_turn_neck_done = true;
    }
}

void
Player::kick( double power,
              double dir )
{
    if ( M_command_done )
    {
        return;
    }

    M_command_done = true;
    M_kick_cycles = 1;

    power = NormalizeKickPower( power );
    dir = NormalizeMoment( dir );

    M_state |= KICK;

    if ( M_stadium.playmode() == PM_BeforeKickOff ||
         M_stadium.playmode() == PM_AfterGoal_Left ||
         M_stadium.playmode() == PM_AfterGoal_Right ||
         M_stadium.playmode() == PM_OffSide_Left ||
         M_stadium.playmode() == PM_OffSide_Right ||
         M_stadium.playmode() == PM_Foul_Charge_Left ||
         M_stadium.playmode() == PM_Foul_Charge_Right ||
         M_stadium.playmode() == PM_Foul_Push_Left ||
         M_stadium.playmode() == PM_Foul_Push_Right ||
         M_stadium.playmode() == PM_Back_Pass_Left ||
         M_stadium.playmode() == PM_Back_Pass_Right ||
         M_stadium.playmode() == PM_Free_Kick_Fault_Left ||
         M_stadium.playmode() == PM_Free_Kick_Fault_Right ||
         M_stadium.playmode() == PM_CatchFault_Left ||
         M_stadium.playmode() == PM_CatchFault_Right ||
         M_stadium.playmode() == PM_TimeOver )
    {
        M_state |= KICK_FAULT;
        return;
    }

    if ( ! ballKickable() )
    {
        M_state |= KICK_FAULT;
        M_stadium.failedKickTaken( *this );
        return;
    }

    double dir_diff = std::fabs( angleFromBody( M_stadium.ball() ) );
    PVector rpos = M_stadium.ball().pos() - this->pos();
    double dist_ball
        = rpos.r()
        - M_player_type->playerSize()
        - ServerParam::instance().ballSize();

    double eff_power
        = power
        * M_player_type->kickPowerRate()
        * (1.0 - 0.25*dir_diff/M_PI - 0.25*dist_ball/M_player_type->kickableMargin());

    PVector accel = PVector::fromPolar( eff_power,
                                        dir + angleBodyCommitted() );

    //         // pfr 8/14/00: for RC2000 evaluation
    //         // add noise to kick
    //         {
    //             double maxrnd = M_kick_rand * power / ServerParam::instance().maxPower();
    //             PVector kick_noise( drand( -maxrnd, maxrnd ),
    //                                 drand( -maxrnd, maxrnd ) );
    //             //std::cout << "Kick noise (" << power << "): " << kick_noise << std::endl;
    //             accel += kick_noise;
    //         }

    // akiyama 2008-01-30
    // new kick noise

    // noise = kick_rand
    //         * power/max_power
    //         * (   ( 0.5 + 0.25*( dist_rate + dir_rate ) )
    //             + ( 0.5 + 0.5*ball_speed/(ball_speed_max*ball_decay) ) )

    // [0.5, 1.0]
    double pos_rate
        = 0.5
        + 0.25 * ( dir_diff/M_PI + dist_ball/M_player_type->kickableMargin() );
    // [0.5, 1.0]
    double speed_rate
        = 0.5
        + 0.5 * ( M_stadium.ball().vel().r()
                  / ( ServerParam::instance().ballSpeedMax()
                      * ServerParam::instance().ballDecay() ) );
    // [0, 2*kick_rand]
    double max_rand
        = M_kick_rand
        * ( power / ServerParam::instance().maxPower() )
        * ( pos_rate + speed_rate );
    PVector kick_noise = PVector::fromPolar( drand( 0.0, max_rand ),
                                             drand( -M_PI, M_PI ) );
    accel += kick_noise;

    //             std::cout << M_stadium.time()
    //                       << " Kick "
    //                       << M_kick_rand
    //                       << " " << power
    //                       << " " << pos_rate
    //                       << " " << speed_rate
    //                       << " " << kick_noise
    //                       << " " << kick_noise.r()
    //                       << " " << max_rand
    //                       << std::endl;

    M_stadium.kickTaken( *this, accel );

    ++M_kick_count;
}

// 2011-05-14 akiyama
// added for testing purpose
void
Player::long_kick( double power,
                   double dir )
{
    return;

    if ( M_command_done )
    {
        return;
    }

    M_command_done = true;
    M_kick_cycles = ServerParam::instance().longKickDelay() + 1;

    M_long_kick_power = NormalizeKickPower( power );
    M_long_kick_dir = NormalizeMoment( dir );

    M_state |= KICK;
}

void
Player::doLongKick()
{
    if ( ! isEnabled() )
    {
        return;
    }

    if ( M_kick_cycles != 1 )
    {
        return;
    }

    if ( M_stadium.playmode() == PM_BeforeKickOff ||
         M_stadium.playmode() == PM_AfterGoal_Left ||
         M_stadium.playmode() == PM_AfterGoal_Right ||
         M_stadium.playmode() == PM_OffSide_Left ||
         M_stadium.playmode() == PM_OffSide_Right ||
         M_stadium.playmode() == PM_Foul_Charge_Left ||
         M_stadium.playmode() == PM_Foul_Charge_Right ||
         M_stadium.playmode() == PM_Foul_Push_Left ||
         M_stadium.playmode() == PM_Foul_Push_Right ||
         M_stadium.playmode() == PM_Back_Pass_Left ||
         M_stadium.playmode() == PM_Back_Pass_Right ||
         M_stadium.playmode() == PM_Free_Kick_Fault_Left ||
         M_stadium.playmode() == PM_Free_Kick_Fault_Right ||
         M_stadium.playmode() == PM_CatchFault_Left ||
         M_stadium.playmode() == PM_CatchFault_Right ||
         M_stadium.playmode() == PM_TimeOver )
    {
        M_state |= KICK_FAULT;
        return;
    }

    if ( ! ballKickable() )
    {
        M_state |= KICK_FAULT;
        M_stadium.failedKickTaken( *this );
        return;
    }

    double dir_diff = std::fabs( angleFromBody( M_stadium.ball() ) );
    PVector rpos = M_stadium.ball().pos() - this->pos();
    double dist_ball
        = rpos.r()
        - M_player_type->playerSize()
        - ServerParam::instance().ballSize();

    double eff_power = M_long_kick_power
        * ( M_player_type->kickPowerRate() * ServerParam::instance().longKickPowerFactor() )
        * (1.0 - 0.25*dir_diff/M_PI - 0.25*dist_ball/M_player_type->kickableMargin());

    PVector accel = PVector::fromPolar( eff_power,
                                        M_long_kick_dir + angleBodyCommitted() );

    // [0.5, 1.0]
    double pos_rate
        = 0.5
        + 0.25 * ( dir_diff/M_PI + dist_ball/M_player_type->kickableMargin() );
    // [0.5, 1.0]
    double speed_rate
        = 0.5
        + 0.5 * ( M_stadium.ball().vel().r()
                  / ( ServerParam::instance().ballSpeedMax()
                      * ServerParam::instance().ballDecay() ) );
    // [0, 2*kick_rand]
    double max_rand
        = M_kick_rand
        * ( M_long_kick_power / ServerParam::instance().maxPower() )
        * ( pos_rate + speed_rate );
    PVector kick_noise = PVector::fromPolar( drand( 0.0, max_rand ),
                                             drand( -M_PI, M_PI ) );
    accel += kick_noise;

    M_stadium.kickTaken( *this, accel );

    ++M_kick_count;
}

void
Player::goalieCatch( double dir )
{
    if ( M_command_done )
    {
        return;
    }

    M_command_done = true;
    M_state |= CATCH;

    //pfr: we should only be able to catch in PlayOn mode
    //tom: actually the goalie can catch the ball in any playmode, but
    //infringements should be awarded.  Maybe later.
    if ( ! this->isGoalie()
         || M_goalie_catch_ban > 0
         || ( M_stadium.playmode() != PM_PlayOn
              && ! Referee::isPenaltyShootOut( M_stadium.playmode() ) )
         )
    {
        M_state |= CATCH_FAULT;
        return;
    }

    // Tom: Catches can now occur an any position, but the ref will award and infringement if the ball is outside of the penalty area
    //      static RArea p_l( PVector( -PITCH_LENGTH/2+PENALTY_AREA_LENGTH/2.0, 0.0 ),
    //                        PVector( PENALTY_AREA_LENGTH, PENALTY_AREA_WIDTH )) ;
    //      static RArea p_r( PVector( +PITCH_LENGTH/2-PENALTY_AREA_LENGTH/2.0, 0.0 ),
    //                        PVector( PENALTY_AREA_LENGTH, PENALTY_AREA_WIDTH ) ) ;
    //      switch (this->team->side)
    //      {
    //        case LEFT:
    //          if ( !(p_l.inArea(this->pos)) ){
    //            alive |= CATCH_FAULT;
    //            return;
    //          }
    //        break;
    //        case RIGHT:
    //          if ( !(p_r.inArea(this->pos)) ){
    //            alive |= CATCH_FAULT;
    //            return;
    //          }
    //        break ;
    //      }

#if 0
    RArea catchable( PVector( ServerParam::instance().catchAreaLength()*0.5,
                              0.0 ),
                     PVector( ServerParam::instance().catchAreaLength(),
                              ServerParam::instance().catchAreaWidth() ) );

    PVector	rotated_pos = M_stadium.ball().pos() - this->pos();
    rotated_pos.rotate( -( angleBodyCommitted() + NormalizeMoment( dir ) ) );

    if ( ! catchable.inArea( rotated_pos )
         || drand( 0, 1 ) >= ServerParam::instance().catchProb() )
    {
        M_state |= CATCH_FAULT;
        return;
    }

    M_goalie_catch_ban = ServerParam::instance().catchBanCycle();

    {
        PVector new_pos = M_stadium.ball().pos() - this->pos();
        double mag = new_pos.r();
        // I would much prefer to cache the message of the catch command
        // to the end of the cycle and then do all the movements and
        // playmode changes there, but I feel that would be too much of a
        // depature from the current behaviour.
        mag -= ServerParam::instance().ballSize() + M_player_type->playerSize();
        new_pos.normalize( mag );
        M_pos += new_pos;
        M_angle_body = new_pos.th();
        M_vel = PVector();
    }

    M_goalie_moves_since_catch = 0; // reset the number of times the goalie moved

    M_stadium.ballCaught( *this );
#endif

    // 2009-10-28 akiyama
    // heterogeneous goalie model proposed by Thomas Gabel
    const ServerParam & SP = ServerParam::instance();

    M_goalie_catch_ban = SP.catchBanCycle();

    const double this_catch_area_delta = SP.catchAreaLength() * ( M_player_type->catchAreaLengthStretch() - 1.0 );
    const double this_catch_area_l_max = SP.catchAreaLength() + this_catch_area_delta;
    const double this_catch_area_l_min = SP.catchAreaLength() - this_catch_area_delta;

    //const RArea default_catchable( PVector( SP.catchAreaLength()*0.5, 0.0 ),
    //                               PVector( SP.catchAreaLength(), SP.catchAreaWidth() ) );
    const RArea max_catchable( PVector( this_catch_area_l_max*0.5, 0.0 ),
                               PVector( this_catch_area_l_max, SP.catchAreaWidth() ) );
    const RArea min_catchable( PVector( this_catch_area_l_min*0.5, 0.0 ),
                               PVector( this_catch_area_l_min, SP.catchAreaWidth() ) );
    const double catch_angle = angleBodyCommitted() + NormalizeMoment( dir );

    PVector	rotated_pos = M_stadium.ball().pos() - this->pos();
    rotated_pos.rotate( -catch_angle );

    if ( ! max_catchable.inArea( rotated_pos ) )
    {
        M_state |= CATCH_FAULT;
        return;
    }

    bool success = false;
    if ( min_catchable.inArea( rotated_pos ) )
    {
        //success = ( drand( 0, 1 ) <= SP.catchProb() );
        boost::bernoulli_distribution<> rng( SP.catchProbability() );
        boost::variate_generator< rcss::random::DefaultRNG &,
            boost::bernoulli_distribution<> >
            dst( rcss::random::DefaultRNG::instance(), rng );
        success = dst();
        //std::cerr << M_stadium.time()
        //          << ": goalieCatch min_catchable ok" << std::endl;
    }
    else
    {
        double catch_prob
            = SP.catchProbability()
            - SP.catchProbability() * ( ( rotated_pos.x - this_catch_area_l_min )
                                        / ( this_catch_area_l_max - this_catch_area_l_min ) );
        catch_prob = std::min( std::max( 0.0, catch_prob ), 1.0 );

        //success = ( drand( 0, 1 ) <= catch_prob );
        boost::bernoulli_distribution<> rng( catch_prob );
        boost::variate_generator< rcss::random::DefaultRNG &,
            boost::bernoulli_distribution<> >
            dst( rcss::random::DefaultRNG::instance(), rng );
        success = dst();
        //std::cerr << M_stadium.time()
        //          << ": goalieCatch "
        //          << " dir=" << Rad2Deg( normalize_angle( angleBodyCommitted() + NormalizeMoment( dir ) ) )
        //          << " x=" << rotated_pos.x
        //          << " min_l=" << this_catch_area_l_min
        //          << " max_l=" << this_catch_area_l_max
        //          << " catch_prob=" << catch_prob << std::endl;
    }

    if ( success )
    {
        PVector new_pos = M_stadium.ball().pos() - this->pos();
        double mag = new_pos.r();
        // I would much prefer to cache the message of the catch command
        // to the end of the cycle and then do all the movements and
        // playmode changes there, but I feel that would be too much of a
        // depature from the current behaviour.
        mag -= SP.ballSize() + M_player_type->playerSize();
        new_pos.normalize( mag );
        M_pos += new_pos;
        M_angle_body = new_pos.th();
        M_vel = PVector();

        M_stadium.ballCaught( *this );
    }
    else
    {
        M_state |= CATCH_FAULT;

        PVector vel = M_stadium.ball().vel();
        vel.rotate( -catch_angle );
        vel.x = -vel.x;
        vel.rotate( catch_angle );

        PVector accel = vel - M_stadium.ball().vel();

        std::cerr << M_stadium.time()
                  << ": goalieCatch probabilistic failure."
                  << " vel=" << M_stadium.ball().vel()
                  << " new_vel=" << vel
                  << " accel=" << accel
                  << std::endl;

        M_stadium.ballPunched( *this, accel );
    }

    ++M_catch_count;
    M_goalie_moves_since_catch = 0; // reset the number of times the goalie moved
}

void
Player::say( std::string message )
{
    if ( message.length() > ServerParam::instance().sayMsgSize() )
    {
        return;
    }

    M_stadium.sendPlayerAudio( *this, message.c_str() );
    ++M_say_count;
}

void
Player::sense_body()
{
    M_body_observer->sendBody();
}

void
Player::score()
{
    M_init_observer->sendScore();
}

void
Player::move( double x,
              double y )
{
    if ( M_command_done )
    {
        return;
    }

    if ( M_stadium.playmode() == PM_BeforeKickOff ||
         M_stadium.playmode() == PM_AfterGoal_Right ||
         M_stadium.playmode() == PM_AfterGoal_Left
         //|| M_stadium.playmode() == PM_PenaltySetup_Left
         //|| M_stadium.playmode() == PM_PenaltySetup_Right
         )
    {
        M_pos.x = x * side();
        M_pos.y = y * side();
        M_stadium.collisions();
    }
    else if ( ( M_stadium.playmode() == PM_FreeKick_Left
                || M_stadium.playmode() == PM_FreeKick_Right )
              && M_stadium.ballCatcher() == this )
    {
        if ( ServerParam::instance().goalieMaxMoves() < 0
             || M_goalie_moves_since_catch < ServerParam::instance().goalieMaxMoves() )
        {
            M_pos.x = x * side();
            M_pos.y = y * side();
            ++M_goalie_moves_since_catch;
        }
        else
        {
            send( "(error too_many_moves)" );
        }
    }
    else
    {
        return;
    }

    M_command_done = true;
    ++M_move_count;
}

void
Player::change_view( rcss::pcom::VIEW_WIDTH viewWidth,
                     rcss::pcom::VIEW_QUALITY viewQuality )
{
    if ( M_synch_see
         && viewQuality != rcss::pcom::HIGH  )
    {
        return;
    }

    const double DEFAULT_VISIBLE_ANGLE = Deg2Rad( ServerParam::instance().visibleAngleDegree() );

    if ( viewWidth == rcss::pcom::NARROW )
    {
        if ( ! M_synch_see )
        {
            M_visible_angle = DEFAULT_VISIBLE_ANGLE / 2.0;
            M_visual_send_interval = 2;
        }
        else
        {
            M_visible_angle = DEFAULT_VISIBLE_ANGLE * ( 2.0 / 3.0 ); // == sim_step / send_step
            M_visual_send_interval = 1;
        }
    }
    else if ( viewWidth == rcss::pcom::NORMAL )
    {
        if ( ! M_synch_see )
        {
            M_visible_angle = DEFAULT_VISIBLE_ANGLE;
            M_visual_send_interval = 4;
        }
        else
        {
            M_visible_angle = DEFAULT_VISIBLE_ANGLE * ( 4.0 / 3.0 ); // == 2 * sim_step / send_step
            M_visual_send_interval = 2;
        }
    }
    else if ( viewWidth == rcss::pcom::WIDE )
    {
        if ( ! M_synch_see )
        {
            M_visible_angle = DEFAULT_VISIBLE_ANGLE * 2.0;
            M_visual_send_interval = 8;
        }
        else
        {
            M_visible_angle = DEFAULT_VISIBLE_ANGLE * ( 6.0 / 3.0 ); // == 3 * sim_step / send_step
            M_visual_send_interval = 3;
        }
    }
    else
    {
        return;
    }

    M_view_width = viewWidth;

    if ( viewQuality == rcss::pcom::HIGH )
    {
        M_high_quality = true;
    }
    else if ( viewQuality == rcss::pcom::LOW )
    {
        if ( ! M_synch_see )
        {
            M_visual_send_interval /= 2;
            M_high_quality = false;
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }

    ++M_change_view_count;
}


void
Player::change_view( rcss::pcom::VIEW_WIDTH viewWidth )
{
    const double DEFAULT_VISIBLE_ANGLE = Deg2Rad( ServerParam::instance().visibleAngleDegree() );

    if ( viewWidth == rcss::pcom::NARROW )
    {
        if ( ! M_synch_see )
        {
            M_visible_angle = DEFAULT_VISIBLE_ANGLE / 2.0;
            M_visual_send_interval = 2;
        }
        else
        {
            M_visible_angle = DEFAULT_VISIBLE_ANGLE * ( 2.0 / 3.0 ); // == sim_step / send_step
            M_visual_send_interval = 1;
        }
    }
    else if ( viewWidth == rcss::pcom::NORMAL )
    {
        if ( ! M_synch_see )
        {
            M_visible_angle = DEFAULT_VISIBLE_ANGLE;
            M_visual_send_interval = 4;
        }
        else
        {
            M_visible_angle = DEFAULT_VISIBLE_ANGLE * ( 4.0 / 3.0 ); // == 2 * sim_step / send_step
            M_visual_send_interval = 2;
        }
    }
    else if ( viewWidth == rcss::pcom::WIDE )
    {
        if ( ! M_synch_see )
        {
            M_visible_angle = DEFAULT_VISIBLE_ANGLE * 2.0;
            M_visual_send_interval = 8;
        }
        else
        {
            M_visible_angle = DEFAULT_VISIBLE_ANGLE * ( 6.0 / 3.0 ); // == 3 * sim_step / send_step
            M_visual_send_interval = 3;
        }
    }
    else
    {
        return;
    }

    M_view_width = viewWidth;
    M_high_quality = true;

    ++M_change_view_count;
}

void
Player::compression( int level )
{
#ifdef HAVE_LIBZ
    if ( level > 9 )
    {
        return;
    }

#ifdef HAVE_SSTREAM
    std::ostringstream reply;
    reply << "(ok compression " << level << ")";
    send( reply.str().c_str() );
#else
    std::ostrstream reply;
    reply << "(ok compression " << level << ")" << std::ends;
    send( reply.str() );
    reply.freeze( false );
#endif

    setCompressionLevel( level );
#else
    send( "(warning compression_unsupported)" );
#endif
}

void
Player::bye()
{
    disable();
    M_command_done = true;
}

void
Player::done()
{
    M_done_received = true;
}

void
Player::pointto( bool on, double dist, double head )
{
    if ( on == false )
    {
        M_arm.stopPointing();
    }
    else
    {
        M_arm.pointTo( rcss::geom::Vector2D( pos().x, pos().y ),
                       angleBodyCommitted() + angleNeckCommitted(),
                       rcss::geom::polarVector2D( dist, Deg2Rad( head ) ) );
    }
}

void
Player::attentionto( bool on,
                     rcss::pcom::TEAM team_side,
                     std::string team_name,
                     int at_unum )
{
    if ( on == false )
    {
        // turn attention to off
        focusOff();
        ++M_attentionto_count;
    }
    else
    {
        const Team * at_team = NULL;

        if ( team_side == rcss::pcom::OUR )
        {
            at_team = team();
        }
        else if ( team_side == rcss::pcom::OPP )
        {
            if ( side() == LEFT )
            {
                at_team = &( M_stadium.teamRight() );
            }
            else
            {
                at_team = &( M_stadium.teamLeft() );
            }
        }
        else if ( team_side == rcss::pcom::LEFT_SIDE )
        {
            at_team = &( M_stadium.teamLeft() );
        }
        else if ( team_side == rcss::pcom::RIGHT_SIDE )
        {
            at_team = &( M_stadium.teamRight() );
        }
        else if ( team_name == M_stadium.teamLeft().name() )
        {
            at_team = &( M_stadium.teamLeft() );
        }
        else if ( team_name == M_stadium.teamRight().name() )
        {
            at_team = &( M_stadium.teamRight() );
        }
        else
        {
            return;
        }

        if ( ! at_team->enabled() )
        {
            return;
        }

        if ( at_unum < 1 || at_unum > at_team->size() )
        {
            return;
        }

        // make sure we aren't trying to focus on ourselves.
        if ( at_team == team() && at_unum == unum() )
        {
            return;
        }

        for ( int i = 0; i < at_team->size(); ++i )
        {
            const Player * p = at_team->player( i );
            if ( p && at_unum == p->unum() )
            {
                // turn attention to on
                focusOn( *( at_team->player( at_unum - 1  ) ) );
                ++M_attentionto_count;
                break;
            }
        }
    }
}


void
Player::tackle( double power_or_angle )
{
    tackle( power_or_angle, false );
}

void
Player::tackle( double power_or_angle,
                bool foul )
{
    if ( M_command_done )
    {
        return;
    }

    M_command_done = true;
    M_tackle_cycles = ServerParam::instance().tackleCycles();
    ++M_tackle_count;

    PVector player_2_ball = M_stadium.ball().pos() - pos();
    player_2_ball.rotate( - angleBodyCommitted() );

    double tackle_dist = ( player_2_ball.x > 0.0
                           ? ServerParam::instance().tackleDist()
                           : ServerParam::instance().tackleBackDist() );

    if ( std::fabs( tackle_dist ) <= 1.0e-5 )
    {
        M_state |= TACKLE_FAULT;
        return;
    }

    double exponent = ServerParam::instance().tackleExponent();

    // 2009-10-22 akiyama: foul option
    if ( foul )
    {
        foul = false;

        const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
        for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            if ( (*p)->isEnabled()
                 && (*p)->side() != this->side()
                 && (*p)->ballKickable() )
            {
                foul = true;
                exponent = ServerParam::instance().foulExponent();
                break;
            }
        }
    }

    // tackle failure probability
    double prob = ( std::pow( std::fabs( player_2_ball.x ) / tackle_dist,
                              exponent )
                    + std::pow( std::fabs( player_2_ball.y ) / ServerParam::instance().tackleWidth(),
                                exponent ) );

    if ( prob < 1.0 )
    {
        boost::bernoulli_distribution<> rng( 1 - prob );
        boost::variate_generator< rcss::random::DefaultRNG &,
            boost::bernoulli_distribution<> >
            dst( rcss::random::DefaultRNG::instance(), rng );

        if ( dst() )
        {
            M_state |= TACKLE;

            if ( M_stadium.playmode() == PM_BeforeKickOff ||
                 M_stadium.playmode() == PM_AfterGoal_Left ||
                 M_stadium.playmode() == PM_AfterGoal_Right  ||
                 M_stadium.playmode() == PM_OffSide_Left ||
                 M_stadium.playmode() == PM_OffSide_Right ||
                 M_stadium.playmode() == PM_Foul_Charge_Left ||
                 M_stadium.playmode() == PM_Foul_Charge_Right ||
                 M_stadium.playmode() == PM_Foul_Push_Left ||
                 M_stadium.playmode() == PM_Foul_Push_Right ||
                 M_stadium.playmode() == PM_Back_Pass_Left ||
                 M_stadium.playmode() == PM_Back_Pass_Right ||
                 M_stadium.playmode() == PM_Free_Kick_Fault_Left ||
                 M_stadium.playmode() == PM_Free_Kick_Fault_Right ||
                 M_stadium.playmode() == PM_TimeOver )
            {
                return;
            }

            if ( foul )
            {
                M_tackle_cycles = 0;
            }

            double power_rate = 1.0;
            PVector accel( 0.0, 0.0 );

            // 2008-02-07 akiyama
            // new tackle model based on the Thomas Gabel's proposal
            if ( version() >= 12.0 )
            {
                double angle = NormalizeMoment( power_or_angle );
                double eff_power
                    = ( ServerParam::instance().maxBackTacklePower()
                        + ( ( ServerParam::instance().maxTacklePower()
                              - ServerParam::instance().maxBackTacklePower() )
                            * ( 1.0 - ( std::fabs( angle ) / M_PI ) )
                            //* ( 1.0 - std::pow( std::fabs( angle ) / M_PI, 2.0 ) )
                            )
                        )
                    * ServerParam::instance().tacklePowerRate();

                eff_power *= 1.0 - 0.5*( std::fabs( player_2_ball.th() ) / M_PI );

                accel = PVector::fromPolar( eff_power,
                                            angle + angleBodyCommitted() );

                //                     std::cerr << M_stadium.time()
                //                               << ": v12 tackle arg=" << power_or_angle
                //                               << " angle=" << angle
                //                               << " angl_diff=" << Rad2Deg( player_2_ball.th() )
                //                               << " eff_power=" << eff_power
                //                               << std::endl;
            }
            else
            {
                // Calculate the accleration that will be applied to the ball
                double power = NormalizeTacklePower( power_or_angle );
                double eff_power = power * ServerParam::instance().tacklePowerRate();

                if ( power >= 0.0 )
                {
                    power_rate = power / ServerParam::instance().maxTacklePower();
                }
                else if ( std::fabs( power ) < 1.0e-10 )
                {
                    power_rate = 0.0;
                }
                else if ( std::fabs( ServerParam::instance().maxBackTacklePower() ) > 1.0e-10 )
                {
                    power_rate = -power / ServerParam::instance().maxBackTacklePower();
                }

                // Reduce the acceleration by the probability of kicking it.
                // For instance when the ball is on the boundry of the tackle
                // area, the acceleration is very low as one only barely touches
                // the ball.
                //eff_power *= 1 - prob;

                eff_power *= 1.0 - 0.5*( std::fabs( player_2_ball.th() ) / M_PI );

                accel = PVector::fromPolar( eff_power,
                                            angleBodyCommitted() );

                //                     std::cerr << M_stadium.time()
                //                               << ": v11 tackle arg=" << power_or_angle
                //                               << " power=" << power
                //                               << " angl_diff=" << Rad2Deg( player_2_ball.th() )
                //                               << " eff_power=" << eff_power
                //                               << std::endl;
            }

            //                 // pfr 8/14/00: for RC2000 evaluation
            //                 // add noise to kick
            //                 {
            //                     double maxrnd = ( M_kick_rand * power * ( 1 - prob )
            //                                       / ServerParam::instance().maxPower() );
            //                     PVector kick_noise( drand( -maxrnd, maxrnd ),
            //                                         drand( -maxrnd, maxrnd ) );
            //                     accel += kick_noise;
            //                 }

            // akiyama 2008-01-30
            // new kick noise

            // [0.5, 1]
            double pos_rate = 0.5 + 0.5*( 1.0 - prob );
            // [0.5, 1]
            double speed_rate
                = 0.5
                + 0.5 * ( M_stadium.ball().vel().r()
                          / ( ServerParam::instance().ballSpeedMax()
                              * ServerParam::instance().ballDecay() ) );
            // [0, 2*tackle_rand]
            // tackle_rand = kick_rand * server::tackle_rand_factor
            double max_rand = M_kick_rand * ServerParam::instance().tackleRandFactor()
                * power_rate
                * ( pos_rate + speed_rate );
            PVector tackle_noise = PVector::fromPolar( drand( 0.0, max_rand ),
                                                       drand( -M_PI, M_PI ) );
            accel += tackle_noise;

            M_stadium.tackleTaken( *this, accel, foul );
        }
        else
        {
            M_stadium.failedTackleTaken( *this, foul );
            M_state |= ( TACKLE | TACKLE_FAULT );
        }
    }
    else
    {
        M_stadium.failedTackleTaken( *this, foul );
        M_state |= TACKLE_FAULT;
    }
}

void
Player::clang( int min, int max )
{
    //    std::ostrstream resp;
    //    resp << "(ok clang (ver " << min << " " << max << "))" << std::ends;
    //    send( resp.str() );
    //    resp.freeze( false );

    M_clang_min_ver = min;
    M_clang_max_ver = max;

    sendOKClang();

    if( M_team != NULL
        && team()->olcoach() != NULL
        && team()->olcoach()->assigned() )
    {
        M_team->olcoach()->sendPlayerClangVer( *this );
    }
}

void
Player::ear( bool on,
             rcss::pcom::TEAM team_side,
             std::string team_name,
             rcss::pcom::EAR_MODE mode )
{
    Side s = NEUTRAL;
    if ( team_side == rcss::pcom::OUR )
    {
        s = side();
    }
    else if ( team_side == rcss::pcom::OPP )
    {
        s = static_cast< Side >( - side() );
    }
    else if ( team_side == rcss::pcom::LEFT_SIDE )
    {
        s = LEFT;
    }
    else if ( team_side == rcss::pcom::RIGHT_SIDE )
    {
        s = RIGHT;
    }
    else if ( team_name.length() > 0 )
    {
        if ( team_name == M_stadium.teamLeft().name() )
        {
            s = M_stadium.teamLeft().side();
        }
        else if ( team_name == M_stadium.teamRight().name() )
        {
            s = M_stadium.teamRight().side();
        }
        else
        {
            sendErrorNoTeamName( team_name );
            //        std::ostrstream ostr;
            //        ostr << "(error no team with name " << team_name << ")" << std::ends;
            //        send( ostr.str() );
            //        ostr.freeze( false );
            return;
        }
    }

    //std::cerr << "Ear:\n";
    //std::cerr << "\tSide: " << ( side == LEFT ? "left" : "right" ) << std::endl;

    bool partial = true;
    bool complete = true;
    if ( mode == rcss::pcom::PARTIAL )
    {
        complete = false;
    }
    else if ( mode == rcss::pcom::COMPLETE )
    {
        partial = false;
    }

    //std::cerr << "\tOn: " << on << std::endl;
    //std::cerr << "\t\tPartial: " << partial << std::endl;
    //std::cerr << "\t\tComplete: " << complete << std::endl;

    setEar( on, s, complete, partial );
}

// 2008-02-09 akiyama
// command to change the see message timer
void
Player::synch_see()
{
    const double DEFAULT_VISIBLE_ANGLE = Deg2Rad( ServerParam::instance().visibleAngleDegree() );

    switch ( M_view_width ) {
    case rcss::pcom::NARROW:
        M_visible_angle = DEFAULT_VISIBLE_ANGLE * ( 2.0 / 3.0 ); // == sim_step / send_step
        M_visual_send_interval = 1;
        break;
    case rcss::pcom::NORMAL:
        M_visible_angle = DEFAULT_VISIBLE_ANGLE * ( 4.0 / 3.0 ); // == 2 * sim_step / send_step
        M_visual_send_interval = 2;
        break;
    case rcss::pcom::WIDE:
        M_visible_angle = DEFAULT_VISIBLE_ANGLE * ( 6.0 / 3.0 ); // == 3 * sim_step / send_step
        M_visual_send_interval = 3;
        break;
    default:
        return;
        break;
    }

    M_synch_see = true;
    M_high_quality = true;

    // TODO: move to the Observer
    send( "(ok synch_see)" );
}

void
Player::sendInit()
{
    M_init_observer->sendInit();
    M_init_observer->sendServerParams();
    M_init_observer->sendPlayerParams();
    M_init_observer->sendPlayerTypes();
    M_init_observer->sendChangedPlayers();
}

void
Player::sendReconnect()
{
    M_init_observer->sendReconnect();
    M_init_observer->sendServerParams();
    M_init_observer->sendPlayerParams();
    M_init_observer->sendPlayerTypes();
    M_init_observer->sendChangedPlayers();
}

void
Player::sendVisual()
{
    if ( ! M_synch_see )
    {
        M_observer->sendVisual();
    }
}

void
Player::sendSynchVisual()
{
    if ( M_synch_see )
    {
        M_observer->sendVisual();
    }
}

/* contributed by Artur Merke */
void
Player::sendFullstate()
{
    M_fullstate_observer->sendFullState();
}

bool
Player::setSenders()
{
    rcss::SerializerPlayer::Creator ser_cre;
    if ( ! rcss::SerializerPlayer::factory().getCreator( ser_cre,
                                                         (int)version() ) )
    {
        std::cerr << "No SerializerPlayer::Creator v" << version() << std::endl;
        return false;
    }

    const rcss::SerializerPlayer::Ptr ser = ser_cre();
    if ( ! ser )
    {
        std::cerr << "No SerializerPlayer v" << version() << std::endl;
        return false;
    }

    rcss::BodySenderPlayer::Params body_params( getTransport(),
                                                *this,
                                                ser );
    rcss::BodySenderPlayer::Creator body_cre;
    if ( ! rcss::BodySenderPlayer::factory().getCreator( body_cre,
                                                         (int)version() ) )
    {
        std::cerr << "No BodySenderPlayer::Creator v" << version() << std::endl;
        return false;
    }
    M_body_observer->setBodySender( body_cre( body_params ) );

    rcss::VisualSenderPlayer::Params visual_params( getTransport(),
                                                    *this,
                                                    ser,
                                                    M_stadium );
    rcss::VisualSenderPlayer::Creator vis_cre;
    if ( ! rcss::VisualSenderPlayer::factory().getCreator( vis_cre,
                                                           (int)version() ) )
    {
        std::cerr << "No VisualSenderPlayer::Creator v" << version() << std::endl;
        return false;
    }
    M_observer->setVisualSender( vis_cre( visual_params ) );


    rcss::InitSenderPlayer::Params init_params( getTransport(),
                                                *this,
                                                ser,
                                                M_stadium );
    rcss::InitSenderPlayer::Creator init_cre;
    if ( ! rcss::InitSenderPlayer::factory().getCreator( init_cre,
                                                         (int)version() ) )
    {
        std::cerr << "No InitSenderPlayer::Creator v" << version() << std::endl;
        return false;
    }
    M_init_observer->setInitSender( init_cre( init_params ) );


    rcss::FullStateSenderPlayer::Params fs_params( getTransport(),
                                                   *this,
                                                   ser,
                                                   M_stadium );
    rcss::FullStateSenderPlayer::Creator full_cre;
    if ( ! rcss::FullStateSenderPlayer::factory().getCreator( full_cre,
                                                              (int)version() ) )
    {
        std::cerr << "No FullSenderPlayer::Creator v" << version() << std::endl;
        return false;
    }
    M_fullstate_observer->setFullStateSender( full_cre( fs_params ) );


    rcss::AudioSenderPlayer::Params audio_params( getTransport(),
                                                  *this,
                                                  ser,
                                                  M_stadium );
    rcss::AudioSenderPlayer::Creator audio_cre;
    if ( ! rcss::AudioSenderPlayer::factory().getCreator( audio_cre,
                                                          (int)version() ) )
    {
        std::cerr << "No AudioSenderPlayer::Creator v" << version() << std::endl;
        return false;
    }
    setAudioSender( audio_cre( audio_params ) );

    return true;
}

void
Player::turnImpl()
{
    M_angle_body_committed = this->M_angle_body;
    M_angle_neck_committed = this->M_angle_neck;
    M_vel.assign( 0.0, 0.0 );
    M_accel.assign( 0.0, 0.0 );
}

void
Player::updateAngle()
{
    M_angle_body_committed = this->M_angle_body;
    M_angle_neck_committed = this->M_angle_neck;
}

void
Player::collidedWithPost()
{
    addState( POST_COLLIDE );
    M_post_collide = true;
}

double
Player::maxAccel() const
{
    if ( pos().y < 0.0 )
    {
        return M_max_accel * ( M_team->side() == LEFT
                               ? ServerParam::instance().slownessOnTopForLeft()
                               : ServerParam::instance().slownessOnTopForRight() );
    }
    return M_max_accel;
}

double
Player::maxSpeed() const
{
    if ( pos().y < 0.0 )
    {
        return M_max_speed * ( M_team->side() == LEFT
                               ? ServerParam::instance().slownessOnTopForLeft()
                               : ServerParam::instance().slownessOnTopForRight() );
    }
    return M_max_speed;
}


void
Player::decrementHearCapacity( const Player & sender )
{
    if ( team() == sender.team() )
    {
        M_hear_capacity_from_teammate -= ServerParam::instance().hearDecay();
    }
    else
    {
        M_hear_capacity_from_opponent -= ServerParam::instance().hearDecay();
    }
}

bool
Player::canHearFullFrom( const Player & sender ) const
{
    if ( team() == sender.team() )
    {
        return M_hear_capacity_from_teammate >= (int)ServerParam::instance().hearDecay();
    }
    else
    {
        return M_hear_capacity_from_opponent >= (int)ServerParam::instance().hearDecay();
    }
}


void
Player::recoverAll()
{
    M_stamina = ServerParam::instance().staminaMax();
    M_recovery = 1.0;
    M_effort = M_player_type->effortMax();

    recoverStaminaCapacity();

    M_consumed_stamina = 0.0;

    M_hear_capacity_from_teammate = ServerParam::instance().hearMax();
    M_hear_capacity_from_opponent = ServerParam::instance().hearMax();
}

void
Player::recoverStaminaCapacity()
{
    M_stamina_capacity = ServerParam::instance().staminaCapacity();
}

void
Player::updateStamina()
{
    const ServerParam & param = ServerParam::instance();

    if ( M_stamina <= param.recoverDecThr() * param.staminaMax() )
    {
        if ( M_recovery > param.recoverMin() )
        {
            M_recovery -= param.recoverDec();
        }

        if ( M_recovery < param.recoverMin() )
        {
            M_recovery = param.recoverMin();
        }
    }

    if ( M_stamina <= param.effortDecThr() * param.staminaMax() )
    {
        if ( M_effort > M_player_type->effortMin() )
        {
            M_effort -= param.effortDec();
        }

        if ( M_effort < M_player_type->effortMin() )
        {
            M_effort = M_player_type->effortMin();
        }
    }

    if ( M_stamina >= param.effortIncThr() * param.staminaMax() )
    {
        if ( M_effort < M_player_type->effortMax() )
        {
            M_effort += param.effortInc();
            if ( M_effort > M_player_type->effortMax() )
            {
                M_effort = M_player_type->effortMax();
            }
        }
    }

    //M_stamina += ( M_recovery * M_player_type->staminaIncMax() );
    //if ( M_stamina > param.staminaMax() )
    //{
    //    M_stamina = param.staminaMax();
    //}

    double stamina_inc = std::min( M_recovery * M_player_type->staminaIncMax(),
                                   param.staminaMax() - M_stamina );

    if ( param.staminaCapacity() >= 0.0 )
    {
        if ( stamina_inc > M_stamina_capacity )
        {
            stamina_inc = M_stamina_capacity;
        }
    }

    M_stamina += stamina_inc;
    if ( M_stamina > param.staminaMax() )
    {
        M_stamina = param.staminaMax();
    }

    if ( param.staminaCapacity() >= 0.0 )
    {
        M_stamina_capacity -= stamina_inc;
        if ( M_stamina_capacity < 0.0 )
        {
            M_stamina_capacity = 0.0;
        }
    }
}

void
Player::updateCapacity()
{
    M_hear_capacity_from_teammate += ServerParam::instance().hearInc();
    if( M_hear_capacity_from_teammate > (int)ServerParam::instance().hearMax() )
    {
        M_hear_capacity_from_teammate = ServerParam::instance().hearMax();
    }

    M_hear_capacity_from_opponent += ServerParam::instance().hearInc();
    if ( M_hear_capacity_from_opponent > (int)ServerParam::instance().hearMax() )
    {
        M_hear_capacity_from_opponent = ServerParam::instance().hearMax();
    }

    if ( M_goalie_catch_ban > 0 )
    {
        --M_goalie_catch_ban;
    }
}

double
Player::kickableArea() const
{
    return M_player_type->playerSize()
        + ServerParam::instance().ballSize() //M_stadium.ball().size()
        + M_player_type->kickableMargin();
}

bool
Player::ballKickable() const
{
    return pos().distance2( M_stadium.ball().pos() ) <= std::pow( kickableArea(), 2 );
}

double
Player::foulDetectProbability() const
{
    return M_player_type->foulDetectProbability();
}

void
Player::resetCollisionFlags()
{
    M_ball_collide = false;
    M_player_collide = false;
    M_post_collide = false;
}

void
Player::resetCommandFlags()
{
    if ( M_kick_cycles >= 0 )
    {
        --M_kick_cycles;
    }

    if ( M_dash_cycles >= 0 )
    {
        --M_dash_cycles;
    }

    if ( tackleCycles() > 0 )
    {
        --M_tackle_cycles;
    }

    if ( foulCycles() > 0 )
    {
        --M_foul_cycles;
    }

    if ( M_kick_cycles <= 0
         && tackleCycles() == 0
         && foulCycles() == 0 )
    {
        M_command_done = false;
    }

    M_turn_neck_done = false;

    M_done_received = false;
}

void
Player::setFoulCharged()
{
    M_foul_cycles = ServerParam::instance().foulCycles();
    M_command_done = true;
    M_state |= FOUL_CHARGED;
}

void
Player::incFoulCount()
{
    ++M_foul_count;
}

void
Player::yellowCard()
{
    if ( hasYellowCard() )
    {
        redCard();
    }
    else
    {
        M_state |= YELLOW_CARD;
        ++M_card_count;
    }
}

void
Player::redCard()
{
    discard();
    M_state &= ~YELLOW_CARD;
    M_state |= RED_CARD;

    M_card_count = 2;
}

void
Player::place( const PVector & location )
{
    M_pos = location;
    M_vel.assign( 0.0, 0.0 );
    M_accel.assign( 0.0, 0.0 );
}

void
Player::place( const PVector & pos,
               const double & angle,
               const PVector & vel,
               const PVector & accel )
{
    M_pos = pos;
    M_angle_body = angle;
    M_angle_body_committed = angle;
    M_vel = vel;
    M_accel = accel;
}

// -*-c++-*-

/***************************************************************************
                          player.cc  -  A class for field players and
                                        goalies
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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "player.h"

#include "coach.h"
#include "field.h"
#include "types.h"
#include "random.h"
#include "referee.h"
#include "serverparam.h"
#include "playerparam.h"
#include "utility.h"

#include "initsender.h"
#include "bodysender.h"
#include "fullstatesender.h"
#include "visual.h"

#include <algorithm>
#include <cassert>

namespace {

inline
double
NormalizeDashPower( const double & p )
{
//     return std::max( std::min( p,
//                                ServerParam::instance().maxPower() ),
//                      ServerParam::instance().minPower() );
    return rcss::bound( ServerParam::instance().minPower(),
                        p,
                        ServerParam::instance().maxPower() );
}

inline
double
NormalizeKickPower( const double & p )
{
//     return std::max( std::min( p,
//                                ServerParam::instance().maxPower() ),
//                      ServerParam::instance().minPower() );
    return rcss::bound( ServerParam::instance().minPower(),
                        p,
                        ServerParam::instance().maxPower() );
}

inline
double
NormalizeTacklePower( const double & p )
{
//     return std::max( std::min( p,
//                                ServerParam::instance().maxTacklePower() ),
//                      ServerParam::instance().minTacklePower() );
    return rcss::bound( ServerParam::instance().minTacklePower(),
                        p,
                        ServerParam::instance().maxTacklePower() );
}

inline
double
NormalizeMoment( const double & p )
{
//     return Deg2Rad( std::max( std::min( p,
//                                         ServerParam::instance().maxMoment() ),
//                               ServerParam::instance().minMoment() ) );
    return Deg2Rad( rcss::bound( ServerParam::instance().minMoment(),
                                 p,
                                 ServerParam::instance().maxMoment() ) );
}

inline
double
NormalizeNeckMoment( const double & p )
{
//     return Deg2Rad( std::max( std::min( p,
//                                         ServerParam::instance().maxNeckMoment() ),
//                               ServerParam::instance().minNeckMoment() ) );
    return Deg2Rad( rcss::bound( ServerParam::instance().minNeckMoment(),
                                 p,
                                 ServerParam::instance().maxNeckMoment() ) );
}

inline
double
NormalizeNeckAngle( const double & p )
{
//     return std::max( std::min( p,
//                                Deg2Rad( ServerParam::instance().maxNeckAngle() ) ),
//                      Deg2Rad( ServerParam::instance().minNeckAngle() ) );
    return rcss::bound( Deg2Rad( ServerParam::instance().minNeckAngle() ),
                        p,
                        Deg2Rad( ServerParam::instance().maxNeckAngle() ) );
}

} // end of no-name namespace


Player::Player( Stadium & stadium,
                Team * team,
                int number )
    : MPObject( stadium,
                PObject::OT_PLAYER,
                "", "",
                O_TYPE_PLAYER_NAME, O_TYPE_PLAYER_NAME_SHORT ),
      M_init_observer( new rcss::InitObserverPlayer ),
      M_observer( new rcss::ObserverPlayer ),
      M_body_observer( new rcss::BodyObserverPlayer ),
      M_fullstate_observer( new rcss::FullStateObserver ),
      M_team( team ),
      M_unum( number ),
      M_stamina( ServerParam::instance().staminaMax() ),
      M_recovery( ServerParam::instance().recoverInit() ),
      M_consumed_stamina( 0.0 ),
      M_vis_angle( ServerParam::instance().visAngle() ),
      defangle( ServerParam::instance().visAngle() ),
      vis_distance( ServerParam::instance().visibleDistance() ),
      vis_distance2( vis_distance * vis_distance ),
      M_version( 3.0 ),
      M_unum_far_length( 20.0 ),
      M_unum_too_far_length( 40.0 ),
      M_team_far_length( 40.0 ),
      M_team_too_far_length( 60.0 ),
      M_angle_body( 0.0 ),
      M_angle_body_committed( 0.0 ),
      M_angle_neck( 0.0 ),
      M_angle_neck_committed( 0.0 ),
      M_vis_send( 4 ),
      M_highquality( true ),
      M_state( DISABLE ),
      M_ball_collide( false ),
      M_player_collide( false ),
      M_post_collide( false ),
      M_command_done( false ),
      M_turn_neck_done( false ),
      M_done_received( false ),
      M_hear_capacity_from_teammate( ServerParam::instance().hearMax() ),
      M_hear_capacity_from_opponent( ServerParam::instance().hearMax() ),
      M_goalie( false ),
      M_goalie_catch_ban( 0 ),
      M_goalie_moves_since_catch( 0 ),
      M_arm( ServerParam::instance().pointToBan (),
             ServerParam::instance().pointToDuration () ),
      M_tackle_cycles( 0 ),
      M_tackle_count( 0 ),
      M_clang_min_ver( 0 ),
      M_clang_max_ver( 0 ),
      M_offside_mark( false ),
      M_parser( *this )
{
    assert( team );

    M_enable = false;

    M_weight = ServerParam::instance().playerWeight();
    M_max_speed = ServerParam::instance().playerSpeedMax();
    M_max_accel = ServerParam::instance().playerAccelMax();

    M_pos.x = -( unum() * 3 * team->side() );
    M_pos.y = - ServerParam::PITCH_WIDTH/2.0 - 3.0;

    // pfr 8/14/00: for RC2000 evaluation
    M_kick_rand = ServerParam::instance().kickRand();

    M_kick_count
        = M_dash_count
        = M_turn_count
        = M_catch_count
        = M_move_count
        = M_turn_neck_count
        = M_change_view_count
        = M_say_count = 0;

    setPlayerType( 0 );

    M_effort = M_player_type->effortMax();
}

Player::~Player()
{
#if 0
    std::cerr << shortName() << " consumed stamina = "
              << M_consumed_stamina
              << " dash_count = " << M_dash_count
              << " ave stam =  " << M_consumed_stamina / M_dash_count
              << std::endl;
#endif
    delete M_init_observer;
    M_init_observer = NULL;
    delete M_observer;
    M_observer = NULL;
    delete M_body_observer;
    M_body_observer = NULL;
    delete M_fullstate_observer;
    M_fullstate_observer = NULL;
}

bool
Player::init( const double & ver,
              const bool goalie )
{
    M_version = ver;
    M_goalie = goalie;

    setEnable();

    M_goalie_catch_ban = 0;
    M_goalie_moves_since_catch = 0;

    {
        char lname[128], sname[128];
        std::snprintf( lname, 128, PLAYER_NAME_FORMAT, team()->name().c_str(), unum() );
        std::snprintf( sname, 128, PLAYER_NAME_FORMAT_SHORT, team()->name().c_str(), unum(),
                       isGoalie() ? GOALIE_VISUAL_STRING : "" );
        setName( lname, sname );
    }

    char buf[128];
    std::snprintf( buf, 128, PLAYER_NAME_FAR_FORMAT, team()->name().c_str() );
    M_name_far = buf;
    std::snprintf( buf, 128, PLAYER_NAME_TOOFAR_FORMAT );
    M_name_toofar = buf;

    std::snprintf( buf, 128, PLAYER_NAME_FAR_FORMAT_SHORT, team()->name().c_str() );
    M_short_name_far = buf;
    std::snprintf( buf, 128, PLAYER_NAME_TOOFAR_FORMAT_SHORT );
    M_short_name_toofar = buf;

    M_angle_body_committed = SideDirection( team()->side() );

    // pfr 8/14/00: for RC2000 evaluation
    double my_prand = ServerParam::instance().playerRand();
    if ( ServerParam::instance().teamActuatorNoise() )
    {
        my_prand *= team()->prandFactorTeam();
        M_kick_rand *= team()->kickRandFactorTeam();
    }

    M_randp = my_prand;

#ifdef NEW_QSTEP
    dist_qstep_player = team->distQstepTeam();
    land_qstep_player = team->landQstepTeam();
    dir_qstep_player  = team->dirQstepTeam();
#endif

    if ( ver >= 12.0 )
    {
        M_vis_send = 1;

        //M_unum_far_length = 30.0;
        //M_unum_too_far_length = 60.0;
        //M_team_far_length = 60.0;
        //M_team_too_far_length = 100.0;
    }

    if ( ! setSenders() )
    {
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
    M_inertia_moment = M_player_type->inertiaMoment();
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

    // reset stamina etc.
    M_stamina = ServerParam::instance().staminaMax();
    M_recovery = 1.0;
    M_effort = M_player_type->effortMax();
    M_consumed_stamina = 0.0;

    M_hear_capacity_from_teammate = ServerParam::instance().hearMax();
    M_hear_capacity_from_opponent = ServerParam::instance().hearMax();
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
    if ( tackleCycles() > 0 )
    {
        M_state &= ( STAND | GOALIE | DISCARD | TACKLE | TACKLE_FAULT );
    }
    else
    {
        M_state &= ( STAND | GOALIE | DISCARD );
    }
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
    M_state = DISABLE;
    M_pos.x = -( unum() * 3 * team()->side() );
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
Player::dash( double power )
{
    if ( ! M_command_done )
    {
        // calculate stamina
        double power_need = ( power < 0.0
                              ? NormalizeDashPower( power ) * -2.0
                              : NormalizeDashPower( power ) );
        if ( power_need > stamina() + M_player_type->extraStamina() )
        {
            power_need = stamina() + M_player_type->extraStamina();
        }
        M_stamina -= power_need;
        if ( stamina() < 0.0 )
        {
            M_stamina = 0.0;
        }
        M_consumed_stamina += power_need;

        power = (power < 0) ? power_need / -2.0 : power_need;
        double effective_dash_power
            = effort()
            * power
            * M_player_type->dashPowerRate();
        if ( pos().y < 0.0 )
        {
            effective_dash_power /= ( team()->side() == LEFT
                                      ? ServerParam::instance().slownessOnTopForLeft()
                                      : ServerParam::instance().slownessOnTopForRight() );
        }

        push( PVector::fromPolar( effective_dash_power,
                                  angleBodyCommitted() ) );
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
                                        / (1.0 + M_inertia_moment * vel().r()) );
        ++M_turn_count;
        M_command_done = true;
    }
}

void
Player::turn_neck( double moment)
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
Player::kick( double power, double dir )
{
    if ( ! M_command_done )
    {
        power = NormalizeKickPower( power );
        dir = NormalizeMoment( dir );

        PVector tmp;
        double dir_diff;
        double dist_ball;

        M_state |= KICK;

        if ( M_stadium.playmode() == PM_BeforeKickOff ||
             M_stadium.playmode() == PM_AfterGoal_Left ||
             M_stadium.playmode() == PM_AfterGoal_Right  ||
             M_stadium.playmode() == PM_OffSide_Left ||
             M_stadium.playmode() == PM_OffSide_Right ||
             M_stadium.playmode() == PM_Back_Pass_Left ||
             M_stadium.playmode() == PM_Back_Pass_Right ||
             M_stadium.playmode() == PM_Free_Kick_Fault_Left ||
             M_stadium.playmode() == PM_Free_Kick_Fault_Right ||
             M_stadium.playmode() == PM_CatchFault_Left ||
             M_stadium.playmode() == PM_CatchFault_Right ||
             M_stadium.playmode() == PM_TimeOver )
        {
            return;
        }

        if ( pos().distance( M_stadium.ball().pos() )
             > ( M_player_type->playerSize()
                 + M_stadium.ball().size() + M_player_type->kickableMargin()) )
        {
            M_state |= KICK_FAULT;
            return;
        }

        dir_diff = std::fabs( angleFromBody( M_stadium.ball() ) );
        tmp = M_stadium.ball().pos() - this->pos();
        dist_ball = ( tmp.r() - M_player_type->playerSize()
                      - ServerParam::instance().ballSize() );

        double eff_power = power * ServerParam::instance().kickPowerRate()
            * (1.0 - 0.25*dir_diff/M_PI - 0.25*dist_ball/M_player_type->kickableMargin());

        PVector accel = PVector::fromPolar( eff_power,
                                            dir + angleBodyCommitted() );
#if 0
        // pfr 8/14/00: for RC2000 evaluation
        // add noise to kick
        {
            double maxrnd = M_kick_rand * power / ServerParam::instance().maxPower();
            PVector kick_noise( drand( -maxrnd, maxrnd ),
                                drand( -maxrnd, maxrnd ) );
            //std::cout << "Kick noise (" << power << "): " << kick_noise << std::endl;

            accel += kick_noise;
        }
#else
        // akiyama 2007-11-14
        // new kick noise
        {
            double power_rand = M_kick_rand * eff_power * power / ServerParam::instance().maxPower();
            double speed_rand = M_kick_rand * M_stadium.ball().vel().r();
            double max_rand = power_rand + speed_rand;
            PVector kick_noise = PVector::fromPolar( drand( 0.0, max_rand ),
                                                     drand( -M_PI, M_PI ) );

            accel += kick_noise;

//             std::cout << M_stadium.time()
//                       << " Kick:"
//                       << " power = " << power
//                       << " power_rand = " << power_rand
//                       << " speed_rand = " << speed_rand
//                       << " max_rand = " << max_rand
//                       << " kick_noise = " << kick_noise
//                       << " mag = " << kick_noise.r()
//                       << std::endl;

        }
#endif

        M_stadium.kickTaken( *this, accel );

        ++M_kick_count;
        M_command_done = true;
    }
}

void
Player::goalieCatch( double dir )
{
    if ( ! M_command_done )
    {
//         static RArea p_l( PVector( -PITCH_LENGTH/2+PENALTY_AREA_LENGTH/2.0, 0.0 ),
//                           PVector( PENALTY_AREA_LENGTH, PENALTY_AREA_WIDTH )) ;
//         static RArea p_r( PVector( +PITCH_LENGTH/2-PENALTY_AREA_LENGTH/2.0, 0.0 ),
//                           PVector( PENALTY_AREA_LENGTH, PENALTY_AREA_WIDTH ) ) ;

        M_state |= CATCH;

        //pfr: we should only be able to catch in PlayOn mode
        //tom: actually the goalie can catch the ball in any playmode, but
        //infringements should be awarded.  Maybe later.
        if ( ! this->isGoalie()
             || M_goalie_catch_ban > 0
             || ( M_stadium.playmode() != PM_PlayOn
                  && ! Referee::isPenaltyShootOut( M_stadium.playmode() ) )
             )
            /*
              M_stadium.playmode() == PM_BeforeKickOff ||
              M_stadium.playmode() == PM_AfterGoal_Left ||
              M_stadium.playmode() == PM_AfterGoal_Right ||
              M_stadium.playmode() == PM_OffSide_Left ||
              M_stadium.playmode() == PM_OffSide_Right  ||
              M_stadium.playmode() == PM_Back_Pass_Left ||
              M_stadium.playmode() == PM_Back_Pass_Right ||
              M_stadium.playmode() == PM_Free_Kick_Fault_Left ||
              M_stadium.playmode() == PM_Free_Kick_Fault_Right )
            */
        {
            M_state |= CATCH_FAULT;
            return;
        }

        // Tom: Catches can now occur an any position, but the ref will award and infringement if the ball is outside of the penalty area
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

        ++M_catch_count;
        M_command_done = true;
    }
}

void
Player::say( std::string message )
{
    if ( message.size() > ServerParam::instance().sayMsgSize() )
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
    if ( ! M_command_done )
    {
        if ( M_stadium.playmode() == PM_BeforeKickOff ||
             M_stadium.playmode() == PM_AfterGoal_Right ||
             M_stadium.playmode() == PM_AfterGoal_Left
             )
        {
            M_pos.x = x * team()->side();
            M_pos.y = y * team()->side();
            M_stadium.collisions();
        }
        else if ( ( M_stadium.playmode() == PM_FreeKick_Left
                    || M_stadium.playmode() == PM_FreeKick_Right )
                  && M_stadium.ballCatcher() == this )
        {
            if ( ServerParam::instance().goalieMaxMoves() < 0
                 || M_goalie_moves_since_catch < ServerParam::instance().goalieMaxMoves() )
            {
                M_pos.x = x * team()->side();
                M_pos.y = y * team()->side();
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
}

void
Player::change_view( rcss::pcom::VIEW_WIDTH viewWidth,
                     rcss::pcom::VIEW_QUALITY viewQuality )
{
    if ( version() >= 12.0
         && ( viewWidth == rcss::pcom::NARROW
              || viewQuality != rcss::pcom::HIGH  )
         )
    {
        return;
    }

    if ( viewWidth == rcss::pcom::NARROW )
    {
        if ( version() < 12.0 )
        {
            M_vis_angle = defangle / 2.0;
            M_vis_send = 2;
        }
        else
        {
            return;
        }
    }
    else if ( viewWidth == rcss::pcom::NORMAL )
    {
        if ( version() < 12.0 )
        {
            M_vis_angle = defangle;
            M_vis_send = 4;
        }
        else
        {
            M_vis_angle = defangle;
            M_vis_send = 1;
        }
    }
    else if ( viewWidth == rcss::pcom::WIDE )
    {
        if ( version() < 12.0 )
        {
            M_vis_angle = defangle * 2.0;
            M_vis_send = 8;
        }
        else
        {
            M_vis_angle = defangle * 2.0;
            M_vis_send = 2;
        }
    }
    else
    {
        return;
    }

    if ( viewQuality == rcss::pcom::HIGH )
    {
        M_highquality = true;
    }
    else if ( viewQuality == rcss::pcom::LOW )
    {
        if ( version() < 12.0 )
        {
            M_vis_send /= 2;
            M_highquality = false;
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
    if ( viewWidth == rcss::pcom::NARROW )
    {
        if ( version() < 12.0 )
        {
            M_vis_angle = defangle / 2.0;
            M_vis_send = 2;
        }
        else
        {
            return;
        }
    }
    else if ( viewWidth == rcss::pcom::NORMAL )
    {
        if ( version() < 12.0 )
        {
            M_vis_angle = defangle;
            M_vis_send = 4;
        }
        else
        {
            M_vis_angle = defangle;
            M_vis_send = 1;
        }
    }
    else if ( viewWidth == rcss::pcom::WIDE )
    {
        if ( version() < 12.0 )
        {
            M_vis_angle = defangle * 2.0;
            M_vis_send = 8;
        }
        else
        {
            M_vis_angle = defangle * 2.0;
            M_vis_send = 2;
        }
    }
    else
    {
        return;
    }

    M_highquality = true;

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
            if ( team()->side() == LEFT )
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
        if( at_team == team() && at_unum == unum() )
        {
            return;
        }

        for ( int i = 0; i < at_team->size(); ++i )
        {
            const Player * p = at_team->player( i );
            if ( p && at_unum == p->unum() )
            {
                // turn attention to on
                //focusOn( *( at_team->M_player[ at_unum - 1 ] ) );
                focusOn( *( at_team->player( at_unum - 1  ) ) );
                break;
            }
        }
    }
}

void
Player::tackle( double power )
{
    if ( ! M_command_done && ! isTackling() )
    {
        M_command_done = true;
        M_tackle_cycles = ServerParam::instance().tackleCycles();
        ++M_tackle_count;

        PVector player_2_ball = M_stadium.ball().pos() - pos();
        player_2_ball.rotate( - angleBodyCommitted() );

        double tackle_dist;
        if ( player_2_ball.x > 0.0 ){ //cant tackle backwards.
            tackle_dist = ServerParam::instance().tackleDist();
        }
        else{
            tackle_dist = ServerParam::instance().tackleBackDist();
        }

        double prob = ( std::pow( fabs( player_2_ball.x ) / tackle_dist,
                                  ServerParam::instance().tackleExponent() )
                       + std::pow( fabs( player_2_ball.y )
                                   / ServerParam::instance().tackleWidth(),
                                   ServerParam::instance().tackleExponent() ) );

        if ( prob < 1.0 )
        {
            boost::bernoulli_distribution<> rng( 1 - prob );
            boost::variate_generator< rcss::random::DefaultRNG &,
                boost::bernoulli_distribution<> >
                gen( rcss::random::DefaultRNG::instance(), rng );

            if ( gen() )
            {
                M_state |= TACKLE;

                if ( M_stadium.playmode() == PM_BeforeKickOff ||
                     M_stadium.playmode() == PM_AfterGoal_Left ||
                     M_stadium.playmode() == PM_AfterGoal_Right  ||
                     M_stadium.playmode() == PM_OffSide_Left ||
                     M_stadium.playmode() == PM_OffSide_Right ||
                     M_stadium.playmode() == PM_Back_Pass_Left ||
                     M_stadium.playmode() == PM_Back_Pass_Right ||
                     M_stadium.playmode() == PM_Free_Kick_Fault_Left ||
                     M_stadium.playmode() == PM_Free_Kick_Fault_Right ||
                     M_stadium.playmode() == PM_TimeOver )
                {
                    return;
                }

                // Calculate the accleration that will be applied to the ball
                // akym: TODO? tackle pawoer should be the positive value.
                double eff_power = ( ServerParam::instance().tacklePowerRate()
                                     * NormalizeTacklePower( power ) );

                // Reduce the acceleration by the probability of kicking it.
                // For instance when the ball is on the boundry of the tackle
                // area, the acceleration is very low as one only barely touches
                // the ball.
                // eff_power *= 1 - prob;

                PVector accel = PVector::fromPolar( eff_power,
                                                    angleBodyCommitted() );
#if 0
                // pfr 8/14/00: for RC2000 evaluation
                // add noise to kick
                {
                    double maxrnd = ( M_kick_rand * power * ( 1 - prob )
                                      / ServerParam::instance().maxPower() );
                    PVector kick_noise( drand( -maxrnd, maxrnd ),
                                        drand( -maxrnd, maxrnd ) );

                    accel += kick_noise;
                }
#else
                // akiyama 2007-11-14
                // new kick noise
                {
                    double power_rand = M_kick_rand * eff_power * power / ServerParam::instance().maxPower();
                    double speed_rand = M_kick_rand * M_stadium.ball().vel().r();
                    double max_rand = power_rand + speed_rand;
                    PVector kick_noise = PVector::fromPolar( drand( 0.0, max_rand ),
                                                             drand( -M_PI, M_PI ) );

                    accel += kick_noise;

//                     std::cout << M_stadium.time()
//                               << " Tackle:"
//                               << " power = " << power
//                               << " power_rand = " << power_rand
//                               << " speed_rand = " << speed_rand
//                               << " max_rand = " << max_rand
//                               << " kick_noise = " << kick_noise
//                               << " mag = " << kick_noise.r()
//                               << std::endl;
                }
#endif

                M_stadium.kickTaken( *this, accel );
            }
            else
            {
                M_state |= ( TACKLE | TACKLE_FAULT );
            }
        }
        else
        {
            M_state |= TACKLE_FAULT;
        }
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
    Side side = NEUTRAL;
    if ( team_side == rcss::pcom::OUR )
    {
        side = team()->side();
    }
    else if ( team_side == rcss::pcom::OPP )
    {
        side = (Side)( - team()->side() );
    }
    else if ( team_side == rcss::pcom::LEFT_SIDE )
    {
        side = LEFT;
    }
    else if ( team_side == rcss::pcom::RIGHT_SIDE )
    {
        side = RIGHT;
    }
    else if ( team_name.length() > 0 )
    {
        if ( team_name == M_stadium.teamLeft().name() )
        {
            side = M_stadium.teamLeft().side();
        }
        else if ( team_name == M_stadium.teamRight().name() )
        {
            side = M_stadium.teamRight().side();
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
    if ( mode == rcss::pcom::PARTIAL ){
        complete = false;
    }
    else if ( mode == rcss::pcom::COMPLETE ) {
        partial = false;
    }

    //std::cerr << "\tOn: " << on << std::endl;
    //std::cerr << "\t\tPartial: " << partial << std::endl;
    //std::cerr << "\t\tComplete: " << complete << std::endl;

    setEar( on, side, complete, partial );
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
    if ( version() < 12.0 )
    {
        M_observer->sendVisual();
    }
}

void
Player::sendSynchVisual()
{
    if ( version() >= 12.0 )
    {
        M_observer->sendVisual();
    }
}

/* contributed by Artur Merke */
void
Player::send_fullstate_information()
{
    M_fullstate_observer->sendFullState();
}

bool
Player::setSenders()
{
    rcss::SerializerPlayer::Creator ser_cre;
    if ( ! rcss::SerializerPlayer::factory().getCreator( ser_cre,
                                                         (int)version() ) )
        return false;

    const rcss::SerializerPlayer* ser = ser_cre();
    if ( ! ser )
        return false;

    rcss::BodySenderPlayer::Params body_params( getTransport(),
                                                *this,
                                                *ser );
    rcss::BodySenderPlayer::Creator body_cre;
    if( ! rcss::BodySenderPlayer::factory().getCreator( body_cre,
                                                        (int)version() ) )
        return false;
    M_body_observer->setBodySender( body_cre( body_params ) );

    rcss::VisualSenderPlayer::Params visual_params( getTransport(),
                                                    *this,
                                                    *ser,
                                                    M_stadium );
    rcss::VisualSenderPlayer::Creator vis_cre;
    if( ! rcss::VisualSenderPlayer::factory().getCreator( vis_cre,
                                                          (int)version() ) )
        return false;
    M_observer->setVisualSender( vis_cre( visual_params ) );


    rcss::InitSenderPlayer::Params init_params( getTransport(),
                                                *this,
                                                *ser,
                                                M_stadium );

    rcss::InitSenderPlayer::Creator init_cre;
    if( ! rcss::InitSenderPlayer::factory().getCreator( init_cre,
                                                        (int)version() ) )
        return false;

    M_init_observer->setInitSender( init_cre( init_params ) );


    rcss::FullStateSenderPlayer::Params fs_params( getTransport(),
                                                   *this,
                                                   *ser,
                                                   M_stadium );

    rcss::FullStateSenderPlayer::Creator full_cre;
    if( ! rcss::FullStateSenderPlayer::factory().getCreator( full_cre,
                                                             (int)version() ) )
        return false;
    M_fullstate_observer->setFullStateSender( full_cre( fs_params ) );


    rcss::AudioSenderPlayer::Params audio_params( getTransport(),
                                                  *this,
                                                  *ser,
                                                  M_stadium );

    rcss::AudioSenderPlayer::Creator audio_cre;
    if( ! rcss::AudioSenderPlayer::factory().getCreator( audio_cre,
                                                         (int)version() ) )
        return false;

    setAudioSender( audio_cre( audio_params ) );
    return true;
}

void
Player::recoverAll()
{
    M_stamina = ServerParam::instance().staminaMax();
    M_recovery = 1.0;
    M_effort = M_player_type->effortMax();
    M_consumed_stamina = 0.0;

    M_hear_capacity_from_teammate = ServerParam::instance().hearMax();
    M_hear_capacity_from_opponent = ServerParam::instance().hearMax();
}

void
Player::updateStamina()
{
    if ( M_stamina <= ( ServerParam::instance().recoverDecThr()
                        * ServerParam::instance().staminaMax() ) )
    {
        if ( M_recovery > ServerParam::instance().recoverMin() )
            M_recovery -= ServerParam::instance().recoverDec();
        if ( M_recovery < ServerParam::instance().recoverMin() )
            M_recovery = ServerParam::instance().recoverMin();
    }

    if ( M_stamina <= ( ServerParam::instance().effortDecThr()
                        * ServerParam::instance().staminaMax() ) )
    {
        if ( M_effort > M_player_type->effortMin() )
            M_effort -= ServerParam::instance().effortDec();
        if ( M_effort < M_player_type->effortMin() )
            M_effort = M_player_type->effortMin();
    }

    if ( M_stamina >= ( ServerParam::instance().effortIncThr()
                        * ServerParam::instance().staminaMax() ) )
    {
        if ( M_effort < M_player_type->effortMax() )
        {
            M_effort += ServerParam::instance().effortInc();
            if ( M_effort > M_player_type->effortMax() )
                M_effort = M_player_type->effortMax();
        }
    }

    M_stamina += ( M_recovery * M_player_type->staminaIncMax() );
    if ( M_stamina > ServerParam::instance().staminaMax() )
        M_stamina = ServerParam::instance().staminaMax();
}

void
Player::updateCapacity()
{
    M_hear_capacity_from_teammate += ServerParam::instance().hearInc();
    if( M_hear_capacity_from_teammate > (int)ServerParam::instance().hearMax() )
        M_hear_capacity_from_teammate = ServerParam::instance().hearMax();

    M_hear_capacity_from_opponent += ServerParam::instance().hearInc();
    if ( M_hear_capacity_from_opponent > (int)ServerParam::instance().hearMax() )
        M_hear_capacity_from_opponent = ServerParam::instance().hearMax();

    if ( M_goalie_catch_ban > 0 )
        --M_goalie_catch_ban;
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
    if ( M_tackle_cycles > 0 )
    {
        --M_tackle_cycles;
    }

    if ( tackleCycles() == 0 )
    {
        M_command_done = false;
    }

    M_turn_neck_done = false;

    M_done_received = false;
}

void
Player::clearOffsideMark()
{
    M_offside_mark = false;
}

void
Player::setOffsideMark( const double & offside_line )
{
    M_offside_mark = true;
    M_offside_pos.x = offside_line;
    M_offside_pos.y = pos().y;
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
    M_angle_body_committed = angle;
    M_vel = vel;
    M_accel = accel;
}

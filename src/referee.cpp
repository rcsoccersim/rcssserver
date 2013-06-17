// -*-c++-*-

/***************************************************************************
                                   referee.h
                              Refereeing module
                             -------------------
    begin                : 16-May-2002
    copyright            : (C) 2001 by The RoboCup Soccer Server
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

#include "referee.h"

#include "stadium.h"
#include "player.h"
#include "team.h"
#include "random.h"

#include <ctime>
#include <limits>

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

namespace {
const int CLEAR_PLAYER_TIME = 5;
}

const char * KeepawayRef::trainingMsg = "training Keepaway 1";
const int KeepawayRef::TURNOVER_TIME = 4;

PVector
Referee::truncateToPitch( PVector ball_pos )
{
    ball_pos.x = std::min( ball_pos.x, + ServerParam::PITCH_LENGTH * 0.5 );
    ball_pos.x = std::max( ball_pos.x, - ServerParam::PITCH_LENGTH * 0.5 );
    ball_pos.y = std::min( ball_pos.y, + ServerParam::PITCH_WIDTH * 0.5 );
    ball_pos.y = std::max( ball_pos.y, - ServerParam::PITCH_WIDTH * 0.5 );

    return ball_pos;
}

PVector
Referee::moveOutOfPenalty( const Side side,
                           PVector ball_pos )
{
    if ( side != RIGHT )
    {
        if ( ball_pos.x <= ( - ServerParam::PITCH_LENGTH * 0.5
                             + ServerParam::PENALTY_AREA_LENGTH )
             && std::fabs( ball_pos.y ) <= ServerParam::PENALTY_AREA_WIDTH * 0.5 )
        {
            ball_pos.x
                = - ServerParam::PITCH_LENGTH * 0.5
                + ServerParam::PENALTY_AREA_LENGTH + EPS;
            if ( ball_pos.y > 0 )
            {
                ball_pos.y = +ServerParam::PENALTY_AREA_WIDTH * 0.5 + EPS;
            }
            else
            {
                ball_pos.y = -ServerParam::PENALTY_AREA_WIDTH * 0.5 - EPS;
            }
        }
    }

    if ( side != LEFT )
    {
        if ( ball_pos.x >= ( ServerParam::PITCH_LENGTH * 0.5
                             - ServerParam::PENALTY_AREA_LENGTH )
             && std::fabs( ball_pos.y ) <= ServerParam::PENALTY_AREA_WIDTH * 0.5 )
        {
            ball_pos.x
                = ServerParam::PITCH_LENGTH * 0.5
                - ServerParam::PENALTY_AREA_LENGTH - EPS;
            if( ball_pos.y > 0 )
            {
                ball_pos.y = +ServerParam::PENALTY_AREA_WIDTH * 0.5 + EPS;
            }
            else
            {
                ball_pos.y = -ServerParam::PENALTY_AREA_WIDTH * 0.5 - EPS;
            }
        }
    }

    return ball_pos;
}

PVector
Referee::moveOutOfGoalArea( const Side side,
                            PVector ball_pos )
{
    if ( side != RIGHT )
    {
        if ( ball_pos.x <= ( - ServerParam::PITCH_LENGTH*0.5
                             + ServerParam::GOAL_AREA_LENGTH )
             && std::fabs( ball_pos.y ) <= ServerParam::GOAL_AREA_WIDTH *0.5 )
        {
            ball_pos.x
                = - ServerParam::PITCH_LENGTH*0.5
                + ServerParam::GOAL_AREA_LENGTH + EPS;
        }
    }

    if ( side != LEFT )
    {
        if ( ball_pos.x >= ( ServerParam::PITCH_LENGTH*0.5
                             - ServerParam::GOAL_AREA_LENGTH )
             && std::fabs( ball_pos.y ) <= ServerParam::GOAL_AREA_WIDTH *0.5 )
        {
            ball_pos.x
                = ServerParam::PITCH_LENGTH*0.5
                - ServerParam::GOAL_AREA_LENGTH - EPS;
        }
    }

    return ball_pos;
}

PVector
Referee::moveIntoPenalty( Side side, PVector pos )
{
    if ( side != RIGHT )
    {
        if ( pos.x > ( -ServerParam::PITCH_LENGTH * 0.5
                       + ServerParam::PENALTY_AREA_LENGTH
                       + ServerParam::instance().ballSize() ) )
        {
            pos.x
                = -ServerParam::PITCH_LENGTH * 0.5
                + ServerParam::PENALTY_AREA_LENGTH
                + ServerParam::instance().ballSize();
        }

        if ( std::fabs( pos.y ) > ( ServerParam::PENALTY_AREA_WIDTH * 0.5
                                    + ServerParam::instance().ballSize() ) )
        {
            if ( pos.y > 0 )
            {
                pos.y
                    = ServerParam::PENALTY_AREA_WIDTH * 0.5
                    + ServerParam::instance().ballSize();
            }
            else
            {
                pos.y
                    = -ServerParam::PENALTY_AREA_WIDTH * 0.5
                    - ServerParam::instance().ballSize();
            }
        }
    }
    if ( side != LEFT )
    {
        if ( pos.x < ( ServerParam::PITCH_LENGTH * 0.5
                       - ServerParam::PENALTY_AREA_LENGTH
                       - ServerParam::instance().ballSize() ) )
        {
            pos.x
                = ServerParam::PITCH_LENGTH * 0.5
                - ServerParam::PENALTY_AREA_LENGTH
                - ServerParam::instance().ballSize();
        }

        if ( std::fabs( pos.y ) > ( ServerParam::PENALTY_AREA_WIDTH * 0.5
                                    + ServerParam::instance().ballSize() ) )
        {
            if ( pos.y > 0 )
            {
                pos.y
                    = ServerParam::PENALTY_AREA_WIDTH * 0.5
                    + ServerParam::instance().ballSize();
            }
            else
            {
                pos.y
                    = -ServerParam::PENALTY_AREA_WIDTH * 0.5
                    - ServerParam::instance().ballSize();
            }
        }
    }

    return pos;
}



void
Referee::awardFreeKick( const Side side,
                        PVector pos )
{
    pos = truncateToPitch( pos );
    pos = moveOutOfPenalty( (Side)(-side), pos );

    if( side == LEFT )
    {
        M_stadium.placeBall( PM_FreeKick_Left, LEFT, pos );
    }
    else if( side == RIGHT )
    {
        M_stadium.placeBall( PM_FreeKick_Right, RIGHT, pos );
    }
}

void
Referee::awardGoalKick( const Side side,
                        PVector pos )
{
    if ( pos.y > 0 )
    {
        pos.y = ServerParam::GOAL_AREA_WIDTH * 0.5;
    }
    else
    {
        pos.y = -ServerParam::GOAL_AREA_WIDTH * 0.5;
    }

    M_stadium.clearBallCatcher();
    if ( side == LEFT )
    {
        pos.x = - ServerParam::PITCH_LENGTH * 0.5 + ServerParam::GOAL_AREA_LENGTH;
        M_stadium.placeBall( PM_GoalKick_Left, LEFT, pos );
    }
    else
    {
        pos.x = ServerParam::PITCH_LENGTH * 0.5 - ServerParam::GOAL_AREA_LENGTH;
        M_stadium.placeBall( PM_GoalKick_Right, RIGHT, pos );
    }
}

void
Referee::awardDropBall( PVector pos )
{
    M_stadium.clearBallCatcher();

    pos = truncateToPitch( pos );
    pos = moveOutOfPenalty( NEUTRAL, pos );

    M_stadium.placeBall( PM_Drop_Ball, NEUTRAL, pos );
    M_stadium.placePlayersInField();

    if( ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        M_stadium.changePlayMode( PM_PlayOn );
    }
}

void
Referee::awardKickIn( const Side side,
                      PVector pos )
{
    M_stadium.clearBallCatcher();

    pos = truncateToPitch( pos );

    if ( side == LEFT )
    {
        M_stadium.placeBall( PM_KickIn_Left, LEFT, pos );
    }
    else
    {
        M_stadium.placeBall( PM_KickIn_Right, RIGHT, pos );
    }
}


void
Referee::awardCornerKick( const Side side,
                          PVector pos )
{
    M_stadium.clearBallCatcher();

    if ( pos.y > 0 )
    {
        pos.y
            = ServerParam::PITCH_WIDTH * 0.5
            - ServerParam::instance().cornerKickMargin();
    }
    else
    {
        pos.y
            = -ServerParam::PITCH_WIDTH * 0.5
            + ServerParam::instance().cornerKickMargin();
    }

    if ( side == LEFT )
    {
        pos.x
            = ServerParam::PITCH_LENGTH * 0.5
            - ServerParam::instance().cornerKickMargin();
        M_stadium.placeBall( PM_CornerKick_Left, LEFT, pos );
    }
    else
    {
        pos.x
            = -ServerParam::PITCH_LENGTH * 0.5
            + ServerParam::instance().cornerKickMargin();
        M_stadium.placeBall( PM_CornerKick_Right, RIGHT, pos );
    }
}


bool
Referee::inPenaltyArea( const Side side,
                        const PVector & pos )
{
    if ( side != RIGHT )
    {
        // according to FIFA the ball is catchable if it is at
        // least partly within the penalty area, thus we add ball size
        static RArea pen_area( PVector( - ServerParam::PITCH_LENGTH/2
                                        + ServerParam::PENALTY_AREA_LENGTH/2.0,
                                        0.0 ),
                               PVector( ServerParam::PENALTY_AREA_LENGTH
                                        + ServerParam::instance().ballSize() * 2,
                                        ServerParam::PENALTY_AREA_WIDTH
                                        + ServerParam::instance().ballSize() * 2 ) ) ;
        if ( pen_area.inArea( pos ) )
        {
            return true;
        }
    }

    if ( side != LEFT )
    {
        // according to FIFA the ball is catchable if it is at
        // least partly within the penalty area, thus we add ball size
        static RArea pen_area( PVector( + ServerParam::PITCH_LENGTH/2
                                        - ServerParam::PENALTY_AREA_LENGTH/2.0,
                                        0.0 ),
                               PVector( ServerParam::PENALTY_AREA_LENGTH
                                        + ServerParam::instance().ballSize() * 2,
                                        ServerParam::PENALTY_AREA_WIDTH
                                        + ServerParam::instance().ballSize() * 2 ) ) ;
        if ( pen_area.inArea( pos ) )
        {
            return true;
        }
    }

    return false;
}


bool
Referee::isPenaltyShootOut( const PlayMode pm,
                            const Side side )
{
    bool bLeft = false, bRight = true;
    switch( pm ) {
    case PM_PenaltySetup_Left:
    case PM_PenaltyReady_Left:
    case PM_PenaltyTaken_Left:
    case PM_PenaltyMiss_Left:
    case PM_PenaltyScore_Left:
        bLeft = true;
        break;
    case PM_PenaltySetup_Right:
    case PM_PenaltyReady_Right:
    case PM_PenaltyTaken_Right:
    case PM_PenaltyMiss_Right:
    case PM_PenaltyScore_Right:
        bRight = true;
        break;
    default:
        return false;
    }

    if ( side == NEUTRAL && ( bLeft == true || bRight == true ) )
    {
        return true;
    }
    else if ( side == LEFT && bLeft == true )
    {
        return true;
    }
    else if ( side == RIGHT && bRight == true )
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool
Referee::crossGoalLine( const Side side,
                        const PVector & prev_ball_pos )
{
    if ( prev_ball_pos.x == M_stadium.ball().pos().x )
    {
        // ball cannot have crossed gline
        //          std::cout << time << ": vertcal movement\n";
        return false;
    }

    if ( std::fabs( M_stadium.ball().pos().x )
         <= ServerParam::PITCH_LENGTH*0.5 + ServerParam::instance().ballSize() )
    {
        // ball hasn't crossed gline
        //          std::cout << time << ": hasn't crossed\n";
        return false;
    }

    if ( std::fabs( prev_ball_pos.x )
         > ServerParam::PITCH_LENGTH*0.5 + ServerParam::instance().ballSize() )
    {
        // ball already over the gline
        //          std::cout << time << ": already crossed\n";
        return false;
    }

    if ( ( side * M_stadium.ball().pos().x ) >= 0 )
    {
        //ball in wrong half
        //          std::cout << time << ": wrong_half\n";
        return false;
    }

    if ( std::fabs( prev_ball_pos.y ) > ( ServerParam::instance().goalWidth()*0.5
                                          + ServerParam::instance().goalPostRadius() )
         && std::fabs( prev_ball_pos.x ) > ServerParam::PITCH_LENGTH*0.5 )
    {
        // then the only goal that could have been scored would be
        // from going behind the goal post.  I'm pretty sure that
        // isn't possible anyway, but just in case this function acts
        // as a double check
        //          std::cout << time << ": behind_half\n";
        return false;
    }

    double delta_x = M_stadium.ball().pos().x - prev_ball_pos.x;
    double delta_y = M_stadium.ball().pos().y - prev_ball_pos.y;

    // we already checked above that ball->pos.x != prev_ball_pos.x, so delta_x cannot be zero.
    double gradient = delta_y / delta_x;
    double offset = prev_ball_pos.y - gradient * prev_ball_pos.x;

    // determine y for x = ServerParam::PITCH_LENGTH*0.5 + ServerParam::instance().ballSize() * -side
    double x = ( ServerParam::PITCH_LENGTH*0.5 + ServerParam::instance().ballSize() ) * -side;
    double y_intercept = gradient * x + offset;

    //      std::cout << time << ": prev = " << prev_ball_pos << std::endl;
    //      std::cout << time << ": curr = " << ball->pos << std::endl;
    //      std::cout << time << ": delta_x = " << delta_x << std::endl;
    //      std::cout << time << ": delta_y = " << delta_y << std::endl;
    //      std::cout << time << ": grad = " << gradient << std::endl;
    //      std::cout << time << ": off = " << offset << std::endl;
    //      std::cout << time << ": x = " << x << std::endl;
    //      std::cout << time << ": y_inter = " << y_intercept << std::endl;


    return std::fabs( y_intercept ) <= ( ServerParam::instance().goalWidth()*0.5
                                         + ServerParam::instance().goalPostRadius() );
}

//**********
// TimeRef
//**********

void
TimeRef::analyse()
{
    static int s_half_time_count = 0;

    const PlayMode pm = M_stadium.playmode();
    if ( pm == PM_BeforeKickOff
         || pm == PM_TimeOver
         || pm == PM_AfterGoal_Right
         || pm == PM_AfterGoal_Left
         || pm == PM_OffSide_Right
         || pm == PM_OffSide_Left
         || pm == PM_Foul_Charge_Right
         || pm == PM_Foul_Charge_Left
         || pm == PM_Foul_Push_Right
         || pm == PM_Foul_Push_Left
         || pm == PM_Back_Pass_Right
         || pm == PM_Back_Pass_Left
         || pm == PM_Free_Kick_Fault_Right
         || pm == PM_Free_Kick_Fault_Left
         || pm == PM_CatchFault_Right
         || pm == PM_CatchFault_Left )
    {
        return;
    }

    const ServerParam & param = ServerParam::instance();

    /* if a value of half_time is negative, then ignore time. */
    if ( param.halfTime() > 0 )
    {
        int normal_time = param.halfTime() * param.nrNormalHalfs();
        int maximum_time
            = normal_time
            + param.extraHalfTime() * param.nrExtraHalfs();

        /* check for penalty shoot-outs, half_time and extra_time. */
        if ( M_stadium.time() >= maximum_time )
        {
            if ( param.penaltyShootOuts()
                 && M_stadium.teamLeft().point() == M_stadium.teamRight().point() )
            {
                return; // handled by PenaltyRef
            }
            else
            {
                M_stadium.sendRefereeAudio( "time_up" );
                M_stadium.changePlayMode( PM_TimeOver );
                return;
            }
        }
        // overtime
        else if ( M_stadium.time() >= normal_time )
        {
            int extra_count = ( s_half_time_count + 1 ) - param.nrNormalHalfs();

            if ( ! M_stadium.teamLeft().enabled()
                 || ! M_stadium.teamRight().enabled() )
            {
                M_stadium.sendRefereeAudio( "time_up_without_a_team" );
                M_stadium.changePlayMode( PM_TimeOver );
                return;
            }
            // when golden_goal is on,
            //    referee always checks the score difference.
            //    if score is different, the game is finished immediately.
            else if ( param.goldenGoal()
                      && M_stadium.teamLeft().point() != M_stadium.teamRight().point() )
            {
                M_stadium.sendRefereeAudio( "time_up" );
                M_stadium.changePlayMode( PM_TimeOver );
                return;
            }
            // check half time in overtime
            else if ( M_stadium.time() >= ( normal_time
                                            + ( param.extraHalfTime() * extra_count ) ) )
            {
                // when normal halves have just finished (i.e. extra_count==0),
                // referee always check the score difference.
                if ( extra_count == 0
                     && M_stadium.teamLeft().point() != M_stadium.teamRight().point() )
                {
                    M_stadium.sendRefereeAudio( "time_up" );
                    M_stadium.changePlayMode( PM_TimeOver );
                }
                // otherwise, the game is go into the overtime.
                else
                {
                    ++s_half_time_count;
                    M_stadium.sendRefereeAudio( "time_extended" );
                    Side kick_off_side = ( s_half_time_count % 2 == 0
                                           ? LEFT
                                           : RIGHT );
                    M_stadium.callHalfTime( kick_off_side, s_half_time_count );
                    placePlayersInTheirField();
                }

                return;
            }
        }
        // if not in overtime, check whether halfTime() cycles have been passed
        else if ( M_stadium.time() >= param.halfTime() * ( s_half_time_count + 1 ) )
        {
            ++s_half_time_count;
            Side kick_off_side = ( s_half_time_count % 2 == 0
                                   ? LEFT
                                   : RIGHT );
            M_stadium.sendRefereeAudio( "half_time" );
            M_stadium.callHalfTime( kick_off_side, s_half_time_count );
            placePlayersInTheirField();
            return;
        }
    }

    //     if ( ServerParam::instance().CoachMode && M_stadium.check_ball_pos() != BPI_InField )
    //     {
    //         // freeze the ball
    //         M_stadium.ball->moveTo( M_stadium.ball->pos(),
    //                                 0.0,
    //                                 PVector( 0.0, 0.0 ),
    //                                 PVector( 0.0, 0.0 ) );
    //     }
}


void
Referee::placePlayersInTheirField()
{
    static const RArea fld_l( PVector( - ServerParam::PITCH_LENGTH/4, 0.0 ),
                              PVector( ServerParam::PITCH_LENGTH/2,
                                       ServerParam::PITCH_WIDTH ) );
    static const RArea fld_r( PVector( ServerParam::PITCH_LENGTH/4, 0.0),
                              PVector( ServerParam::PITCH_LENGTH/2,
                                       ServerParam::PITCH_WIDTH ) );

    const bool kick_off_offside ( ServerParam::instance().kickOffOffside()
                                  && ( M_stadium.playmode() == PM_KickOff_Left
                                       || M_stadium.playmode() == PM_KickOff_Right ) );

    const Stadium::PlayerCont::iterator end = M_stadium.remotePlayers().end();
    for ( Stadium::PlayerCont::iterator it = M_stadium.remotePlayers().begin();
          it != end;
          ++it )
    {
        if ( ! (*it)->isEnabled() ) continue;

        switch ( (*it)->side() ) {
        case LEFT:
            if ( (*it)->pos().x > 0 )
            {
                if ( kick_off_offside )
                {
                    (*it)->moveTo( PVector( -(*it)->size(), (*it)->pos().y ) );
                }
                else
                {
                    (*it)->moveTo( fld_l.randomize() );
                }
            }
            break;
        case RIGHT:
            if ( (*it)->pos().x < 0 )
            {
                if ( kick_off_offside )
                {
                    (*it)->moveTo( PVector( (*it)->size(), (*it)->pos().y ) );
                }
                else
                {
                    (*it)->moveTo( fld_r.randomize() );
                }
            }
            break;
        case NEUTRAL:
        default:
            break;
        }

        if ( (*it)->side() != M_stadium.kickOffSide() )
        {
            CArea expand_c( PVector( 0.0, 0.0 ),
                            ServerParam::KICK_OFF_CLEAR_DISTANCE + (*it)->size() );

            if ( expand_c.inArea( (*it)->pos() ) )
            {
                (*it)->moveTo( expand_c.nearestEdge( (*it)->pos() ) );
            }
        }
    }
}


void
Referee::clearPlayersFromBall( const Side side )
{
    // I would really prefer if we did not teleport players around the field. In
    // my mind players should be given time to move away from the ball and given
    // yellow cards if they repeatedly fail to stay clear.  Two yellows and your
    // out of the game.

    const PlayMode pm = M_stadium.playmode();

    const double clear_dist = ( ( pm == PM_Back_Pass_Left
                                  || pm == PM_Back_Pass_Right
                                  || ( ( pm == PM_Foul_Charge_Left
                                         || pm == PM_Foul_Push_Left )
                                       && inPenaltyArea( LEFT, M_stadium.ball().pos() ) )
                                  || ( ( pm == PM_Foul_Charge_Right
                                         || pm == PM_Foul_Push_Right )
                                       && inPenaltyArea( RIGHT, M_stadium.ball().pos() ) ) )
                                ? ServerParam::GOAL_AREA_LENGTH
                                : ServerParam::KICK_OFF_CLEAR_DISTANCE );
    const bool indirect = ( pm == PM_Back_Pass_Left
                            || pm == PM_Back_Pass_Right
                            || pm == PM_Foul_Charge_Left
                            || pm == PM_Foul_Charge_Right
                            || pm == PM_Foul_Push_Left
                            || pm == PM_Foul_Push_Right
                            || pm == PM_IndFreeKick_Left
                            || pm == PM_IndFreeKick_Right );
    //const double goal_half_width = ServerParam::instance().goalWidth()*0.5;

    const double max_x = ServerParam::PITCH_LENGTH*0.5 + ServerParam::PITCH_MARGIN;
    const double max_y = ServerParam::PITCH_WIDTH*0.5 + ServerParam::PITCH_MARGIN;

    const bool ball_at_corner
        = ( std::fabs( M_stadium.ball().pos().x ) > max_x - clear_dist
            && std::fabs( M_stadium.ball().pos().y ) > max_y - clear_dist );

    const Stadium::PlayerCont::iterator end = M_stadium.players().end();
    for ( int loop = 0; loop < 10; ++loop )
    {
        bool exist = false;
        for ( Stadium::PlayerCont::iterator it = M_stadium.players().begin();
              it != end;
              ++it )
        {
            if ( ! (*it)->isEnabled() ) continue;

            if ( side == NEUTRAL
                 || (*it)->side() == side )
            {
                if ( indirect
                     && std::fabs( (*it)->pos().x ) >= ServerParam::PITCH_LENGTH*0.5 )
                {
                    // defender is allowed to stand on the goal line.
                    continue;
                }

                CArea clear_area( M_stadium.ball().pos(),
                                  clear_dist + (*it)->size() );
                if ( clear_area.inArea( (*it)->pos() ) )
                {
                    CArea expand_clear_area( M_stadium.ball().pos(),
                                             clear_dist + (*it)->size() + 1.0e-5 );
                    PVector new_pos = expand_clear_area.nearestEdge( (*it)->pos() );

                    if ( ball_at_corner
                         && std::fabs( new_pos.x ) > ServerParam::PITCH_LENGTH*0.5
                         && std::fabs( new_pos.y ) > ServerParam::PITCH_WIDTH*0.5 )
                    {
                        new_pos -= M_stadium.ball().pos();
                        new_pos.rotate( M_PI );
                        new_pos += M_stadium.ball().pos();
                    }

                    if ( indirect
                         && std::fabs( new_pos.x ) > ServerParam::PITCH_LENGTH*0.5 )
                    {
                        double tangent
                            = ( new_pos.y - M_stadium.ball().pos().y )
                            / ( new_pos.x - M_stadium.ball().pos().x );
                        new_pos.x = ServerParam::PITCH_LENGTH*0.5
                            * ( new_pos.x > 0.0 ? 1.0 : -1.0 );
                        new_pos.y = M_stadium.ball().pos().y
                            + tangent * ( new_pos.x - M_stadium.ball().pos().x );
                    }

                    if ( std::fabs( new_pos.x ) > max_x )
                    {
                        double r = clear_dist + (*it)->size();
                        double theta = std::acos( ( max_x - std::fabs( M_stadium.ball().pos().x ) ) / r );
                        double tmp_y = std::fabs( r * std::sin( theta ) );
                        new_pos.x = ( new_pos.x < 0.0 ? -max_x : +max_x );
                        new_pos.y = ( M_stadium.ball().pos().y
                                      + ( new_pos.y < M_stadium.ball().pos().y
                                          ? - tmp_y - 1.0e-5
                                          : + tmp_y + 1.0e-5 ) );
                    }

                    if ( std::fabs( new_pos.y ) > max_y )
                    {
                        double r = clear_dist + (*it)->size();
                        double theta = std::acos( ( max_y - std::fabs( M_stadium.ball().pos().y ) ) / r );
                        double tmp_x = std::fabs( r * std::sin( theta ) );
                        new_pos.x = ( M_stadium.ball().pos().x
                                      + ( new_pos.x < M_stadium.ball().pos().x
                                          ? - tmp_x - 1.0e-5
                                          : + tmp_x + 1.0e-5 ) );
                        new_pos.y = ( new_pos.y < 0.0 ? -max_y : +max_y );
                    }

                    if ( std::fabs( new_pos.x ) > max_x
                         || std::fabs( new_pos.y ) > max_y )
                    {
                        new_pos -= M_stadium.ball().pos();
                        new_pos.rotate( M_PI );
                        new_pos += M_stadium.ball().pos();
                    }

                    (*it)->place( new_pos );
                    exist = true;
                }
            }
        }

        if ( exist )
        {
            M_stadium.collisions();
        }
        else
        {
            break;
        }
    }
}

void
Referee::checkFoul( const Player & tackler,
                    const bool foul,
                    bool * detect_charge,
                    bool * detect_yellow,
                    bool * detect_red )
{
    bool foul_charge = false;
    bool yellow_card = false;
    bool red_card = false;

    boost::bernoulli_distribution<> rng( tackler.foulDetectProbability() );
    boost::variate_generator< rcss::random::DefaultRNG &, boost::bernoulli_distribution<> >
        dst( rcss::random::DefaultRNG::instance(), rng );

    // 2011-05-14 akiyama
    // added red card probability
    boost::bernoulli_distribution<> red_rng( ServerParam::instance().redCardProbability() );
    boost::variate_generator< rcss::random::DefaultRNG &, boost::bernoulli_distribution<> >
        red_dst( rcss::random::DefaultRNG::instance(), red_rng );

    const double ball_dist2 = tackler.pos().distance2( M_stadium.ball().pos() );
    const double ball_angle = ( M_stadium.ball().pos() - tackler.pos() ).th();

    //std::cerr << M_stadium.time() << " (tackleTaken) "
    //          << " (tackler " << SideStr( tackler.side() ) << ' ' << tackler.unum() << ")"
    //          << std::endl;

    const Stadium::PlayerCont::iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( ! (*p)->isEnabled() ) continue;
        if ( (*p)->side() == tackler.side() ) continue;

        if ( ! (*p)->ballKickable() ) continue; // no kickable

        bool pre_check = false;
        if ( foul )
        {
            (*p)->setFoulCharged();

            //std::cerr << "---->" << (*p)->unum() << " intentional foul. prob=" << rng.p() << std::endl;
            if ( dst() )
            {
                //std::cerr << "----> " << (*p)->unum() << " detected intentional foul." << std::endl;
                pre_check = true;
                foul_charge = true;
            }
        }

        if ( ! (*p)->dashed() )
        {
            //std::cerr << "----> " << (*p)->unum() << " no dash." << std::endl;
            continue; // no dashing
        }

        PVector player_rel = (*p)->pos() - tackler.pos();

        if ( player_rel.r2() > ball_dist2 )
        {
            //std::cerr << "----> " << (*p)->unum() << " ball near." << std::endl;
            continue; // further than ball
        }

        //std::cerr << "--> (player " << SideStr( (*p)->side() ) << ' ' << (*p)->unum() << ")\n";

        player_rel.rotate( -ball_angle );

        if ( player_rel.x < 0.0
             || std::fabs( player_rel.y ) > (*p)->size() + tackler.size() )
        {
            //std::cerr << "----> " << (*p)->unum() << " behind or big y_diff. rel=" << player_rel
            //          << std::endl;
            continue;
        }

        double body_diff = std::fabs( normalize_angle( (*p)->angleBodyCommitted() - ball_angle ) );
        if ( body_diff > M_PI*0.5 )
        {
            //std::cerr << "----> " << (*p)->unum() << " over body angle. angle=" << body_diff / M_PI * 180.0
            //          << std::endl;
            continue;
        }

        if ( foul )
        {
            if ( pre_check )
            {
                //std::cerr << "----> " << (*p)->unum() << " detected yellow_card." << std::endl;
                yellow_card = true;
                if ( red_dst() )
                {
                    yellow_card = false;
                    red_card = true;
                }
            }
        }
        else
        {
            if ( dst() )
            {
                //std::cerr << "----> " << (*p)->unum() << " detected foul. prob=" << rng.p() << std::endl;
                foul_charge = true;
                if ( red_dst() )
                {
                    yellow_card = true;
                }
            }
        }
        //std::cerr << "----> not detected foul. prob=" << rng.p()<< std::endl;
    }

    *detect_charge = foul_charge;
    *detect_yellow = yellow_card;
    *detect_red = red_card;
}


//**********
// BallStuckRef
//**********

void
BallStuckRef::analyse()
{
    if ( ServerParam::instance().ballStuckArea() <= 0.0
         || ServerParam::instance().dropTime() <= 0 )
    {
        return;
    }

    if ( M_stadium.playmode() != PM_PlayOn )
    {
        M_last_ball_pos = M_stadium.ball().pos();
        M_counter = 0;
        return;
    }

    if ( M_stadium.ball().pos().distance2( M_last_ball_pos )
         <= std::pow( ServerParam::instance().ballStuckArea(), 2 ) )
    {
        ++M_counter;

        if ( M_counter >= ServerParam::instance().dropTime() )
        {
            M_last_ball_pos = M_stadium.ball().pos();
            M_counter = 0;

            awardDropBall( M_stadium.ball().pos() );
        }
    }
    else
    {
        M_last_ball_pos = M_stadium.ball().pos();
        M_counter = 0;
    }

}


//**********
// OffsideRef
//**********

const int OffsideRef::AFTER_OFFSIDE_WAIT = 30;

void
OffsideRef::kickTaken( const Player & kicker )
{
    setOffsideMark( kicker );
}

void
OffsideRef::failedKickTaken( const Player & kicker )
{
    checkIntentionalAction( kicker );
}

void
OffsideRef::tackleTaken( const Player & tackler,
                         const bool )
{
    setOffsideMark( tackler );
}

void
OffsideRef::failedTackleTaken( const Player & tackler )
{
    checkIntentionalAction( tackler );
}

// 2011-05-14 akiyama
// added offside checking based on intentional actione based
void
OffsideRef::checkIntentionalAction( const Player & kicker )
{
    if ( ! ServerParam::instance().useOffside() )
    {
        return;
    }

    for ( std::vector< Candidate >::iterator it = M_offside_candidates.begin(),
              end = M_offside_candidates.end();
          it != end;
          ++it )
    {
        if ( it->player_ == &kicker
             && it->player_->pos().distance2( M_stadium.ball().pos() )
             < std::pow( ServerParam::instance().offsideActiveArea(), 2 ) )
        {
            M_offside_pos = it->pos_;
            callOffside();
        }
    }
}

void
OffsideRef::ballTouched( const Player & player )
{
    setOffsideMark( player );
}

void
OffsideRef::analyse()
{
    if ( ! ServerParam::instance().useOffside() )
    {
        return;
    }

    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( M_stadium.playmode() == PM_BeforeKickOff
         || M_stadium.playmode() == PM_KickOff_Left
         || M_stadium.playmode() == PM_KickOff_Right )
    {
        if ( ServerParam::instance().kickOffOffside() )
        {
            placePlayersInTheirField();
        }
        return;
    }

    if ( M_stadium.playmode() == PM_OffSide_Left )
    {
        ++M_after_offside_time;

        //clearPlayersFromBall( LEFT );
        if ( M_after_offside_time > AFTER_OFFSIDE_WAIT - CLEAR_PLAYER_TIME )
        {
            clearPlayersFromBall( LEFT );
        }

        checkPlayerAfterOffside();

        if ( M_after_offside_time > AFTER_OFFSIDE_WAIT )
        {
            M_stadium.changePlayMode( PM_FreeKick_Right );
        }
        return;
    }

    if ( M_stadium.playmode() == PM_OffSide_Right )
    {
        ++M_after_offside_time;

        //clearPlayersFromBall( RIGHT );
        if ( M_after_offside_time > AFTER_OFFSIDE_WAIT - CLEAR_PLAYER_TIME )
        {
            clearPlayersFromBall( RIGHT );
        }

        checkPlayerAfterOffside();

        if ( M_after_offside_time > AFTER_OFFSIDE_WAIT )
        {
            M_stadium.changePlayMode( PM_FreeKick_Left );
        }
        return;
    }

    if ( M_stadium.playmode() != PM_PlayOn )
    {
        M_offside_candidates.clear();
        return;
    }

#if 0
    // 2011-05-14 akiyama
    // disabled distance based offside checking
    bool found = false;
    {
        double dist2 = std::pow( ServerParam::instance().offsideActiveArea(), 2 );

        for ( std::vector< Candidate >::const_iterator it = M_offside_candidates.begin(),
                  end = M_offside_candidates.end();
              it != end;
              ++it )
        {
            if ( ! it->player_->isEnabled() ) continue;

            double tmp = it->player_->pos().distance2( M_stadium.ball().pos() );
            if ( tmp < dist2 )
            {
                found = true;
                dist2 = tmp;
                M_offside_pos = it->pos_;
            }
        }
    }

    if ( found )
    {
        callOffside();
    }
#endif
}

void
OffsideRef::playModeChange( PlayMode pm )
{
    if ( pm != PM_PlayOn )
    {
        M_offside_candidates.clear();
    }
}

void
OffsideRef::setOffsideMark( const Player & kicker )
{
    if ( ! ServerParam::instance().useOffside() )
    {
        return;
    }

    if ( M_last_kick_time == M_stadium.time()
         && M_last_kick_stoppage_time == M_stadium.stoppageTime() )
    {
        M_last_kicker_side = NEUTRAL;
        M_offside_candidates.clear();
        return;
    }

    M_last_kick_time = M_stadium.time();
    M_last_kick_stoppage_time = M_stadium.stoppageTime();
    M_last_kicker_side = kicker.side();


    for ( std::vector< Candidate >::iterator it = M_offside_candidates.begin(),
              end = M_offside_candidates.end();
          it != end;
          ++it )
    {
        if ( it->player_ == &kicker )
        {
            M_offside_pos = it->pos_;
            callOffside();
            return;
        }
    }

    M_offside_candidates.clear();

    if ( M_stadium.playmode() == PM_GoalKick_Left
         || M_stadium.playmode() == PM_GoalKick_Right
         || M_stadium.playmode() == PM_KickIn_Left
         || M_stadium.playmode() == PM_KickIn_Right
         || M_stadium.playmode() == PM_CornerKick_Left
         || M_stadium.playmode() == PM_CornerKick_Right )
    {
        return;
    }

    const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();

    double first = 0.0;
    double second = 0.0;
    double offside_line = 0.0;

    switch ( kicker.side() ) {
    case LEFT:
        for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            if ( ! (*p)->isEnabled() ) continue;

            if ( (*p)->side() == RIGHT )
            {
                if ( (*p)->pos().x > second )
                {
                    second = (*p)->pos().x;
                    if ( second > first )
                    {
                        std::swap( first, second );
                    }
                }
            }
        }

        if ( second > M_stadium.ball().pos().x )
        {
            offside_line = second;
        }
        else
        {
            offside_line = M_stadium.ball().pos().x;
        }

        for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            if ( ! (*p)->isEnabled() ) continue;

            if ( (*p)->side() == LEFT
                 && (*p)->pos().x > offside_line
                 && (*p)->unum() != kicker.unum() )
            {
                M_offside_candidates.push_back( Candidate( *p, offside_line, (*p)->pos().y ) );
            }
        }
        break;

    case RIGHT:
        for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            if ( ! (*p)->isEnabled() ) continue;

            if ( (*p)->side() == LEFT )
            {
                if ( (*p)->pos().x < second )
                {
                    second = (*p)->pos().x;
                    if ( second < first )
                    {
                        std::swap( first, second );
                    }
                }
            }
        }

        if ( second < M_stadium.ball().pos().x )
        {
            offside_line = second;
        }
        else
        {
            offside_line = M_stadium.ball().pos().x;
        }

        for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            if ( ! (*p)->isEnabled() ) continue;

            if ( (*p)->side() == RIGHT
                 && (*p)->pos().x < offside_line
                 && (*p)->unum() != kicker.unum() )
            {
                M_offside_candidates.push_back( Candidate( *p, offside_line, (*p)->pos().y ) );
            }
        }
        break;
    case NEUTRAL:
    default:
        break;
    }
}

void
OffsideRef::callOffside()
{
    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( M_last_kicker_side == NEUTRAL )
    {
        return;
    }

    static const RArea pt( PVector( 0.0, 0.0 ),
                           PVector( ServerParam::PITCH_LENGTH
                                    - 2.0*ServerParam::instance().cornerKickMargin(),
                                    ServerParam::PITCH_WIDTH
                                    - 2.0*ServerParam::instance().cornerKickMargin() ) );
    static const RArea g_l( PVector( - ServerParam::PITCH_LENGTH/2.0
                                     + ServerParam::GOAL_AREA_LENGTH/2.0,
                                     0.0 ),
                            PVector( ServerParam::GOAL_AREA_LENGTH,
                                     ServerParam::GOAL_AREA_WIDTH ) );
    static const RArea g_r( PVector( + ServerParam::PITCH_LENGTH/2.0
                                     - ServerParam::GOAL_AREA_LENGTH/2.0,
                                     0.0 ),
                            PVector( ServerParam::GOAL_AREA_LENGTH,
                                     ServerParam::GOAL_AREA_WIDTH ) );

    PVector pos;

    if ( M_offside_pos.x > ServerParam::PITCH_LENGTH/2.0
         || g_r.inArea( M_offside_pos ) )
    {
        pos.x = + ServerParam::PITCH_LENGTH/2.0 - ServerParam::GOAL_AREA_LENGTH;
        pos.y = ( M_offside_pos.y > 0 ? 1 : -1 ) * ServerParam::GOAL_AREA_WIDTH/2.0;
    }
    else if ( M_offside_pos.x < - ServerParam::PITCH_LENGTH/2.0
              || g_l.inArea( M_offside_pos ) )
    {
        pos.x = - ServerParam::PITCH_LENGTH/2.0 + ServerParam::GOAL_AREA_LENGTH;
        pos.y = ( M_offside_pos.y > 0 ? 1 : -1 ) * ServerParam::GOAL_AREA_WIDTH/2.0;
    }
    else if ( ! pt.inArea( M_offside_pos ) )
    {
        pos = pt.nearestEdge( M_offside_pos );
    }
    else
    {
        pos = M_offside_pos;
    }

    if ( M_last_kicker_side == LEFT )
    {
        M_stadium.placeBall( PM_OffSide_Left, RIGHT, pos );
    }
    else
    {
        M_stadium.placeBall( PM_OffSide_Right, LEFT, pos );
    }

    M_stadium.clearBallCatcher();
    M_offside_candidates.clear();

    M_stadium.placePlayersInField();
    //clearPlayersFromBall( M_last_kicker_side );

    M_after_offside_time = 0;
}


void
OffsideRef::checkPlayerAfterOffside()
{
    Side offsideside = NEUTRAL;
    PVector center( 0.0, 0.0 ), size( 0.0, 0.0 );

    if ( M_stadium.playmode() == PM_OffSide_Right )
    {
        center.assign( +ServerParam::PITCH_LENGTH/4 + M_offside_pos.x/2,
                       0.0 );
        size.assign( ServerParam::PITCH_LENGTH/2 - M_offside_pos.x,
                     ServerParam::PITCH_WIDTH );
        offsideside = RIGHT;
    }
    else if ( M_stadium.playmode() == PM_OffSide_Left )
    {
        center.assign( -ServerParam::PITCH_LENGTH/4 + M_offside_pos.x/2,
                       0.0 );
        size.assign( ServerParam::PITCH_LENGTH/2 + M_offside_pos.x,
                     ServerParam::PITCH_WIDTH );
        offsideside = LEFT;
    }
    else
    {
        return;
    }

    //const CArea c( M_ofefside_pos, ServerParam::instance().offsideActiveArea() );
    const CArea c( M_offside_pos, 2.5 );
    const RArea fld( center, size );

    for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin(),
              end = M_stadium.players().end();
          p != end;
          ++p )
    {
        if ( ! (*p)->isEnabled() ) continue;
        if ( (*p)->side() != offsideside ) continue;

        if ( c.inArea( (*p)->pos() ) )
        {
            (*p)->moveTo( c.nearestEdge( (*p)->pos() ) );
        }

        if ( ! fld.inArea( (*p)->pos() ))
        {
            if ( M_stadium.playmode() == PM_OffSide_Right )
            {
                (*p)->moveTo( fld.nearestVEdge( (*p)->pos() )
                              + PVector( ServerParam::instance().offsideKickMargin(), 0 ) );
            }
            else
            {
                (*p)->moveTo( fld.nearestVEdge( (*p)->pos() )
                              - PVector( ServerParam::instance().offsideKickMargin(), 0 ) );
            }
        }
    }
}

//**********
// FreeKickRef
//**********

const int FreeKickRef::AFTER_FREE_KICK_FAULT_WAIT = 30;

void
FreeKickRef::kickTaken( const Player & kicker )
{
    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( goalKick( M_stadium.playmode() ) )
    {
        if ( ( M_stadium.playmode() == PM_GoalKick_Left
               && kicker.side() != LEFT )
             || ( M_stadium.playmode() == PM_GoalKick_Right
                  && kicker.side() != RIGHT )
             )
        {
            // opponent player kicks tha ball while ball is in penalty areas.
            awardGoalKick( (Side)( - kicker.side() ), M_stadium.ball().pos() );
            M_goal_kick_count = -1;
            M_kick_taken = false;
            return;
        }

        if ( M_kick_taken )
        {
            // ball was not kicked directly into play (i.e. out of penalty area
            // without touching another player
            if ( &kicker != M_kick_taker )
            {
                if ( ServerParam::instance().properGoalKicks() )
                {
                    awardGoalKick( M_kick_taker->side(),
                                   M_stadium.ball().pos() );
                }
            }
            else if ( M_kick_taker_dashes != M_kick_taker->dashCount() )
            {
                if ( ServerParam::instance().freeKickFaults() )
                {
                    M_stadium.setPlayerState( M_kick_taker->side(),
                                              M_kick_taker->unum(),
                                              FREE_KICK_FAULT );
                    callFreeKickFault( kicker.side(),
                                       M_stadium.ball().pos() );
                }
                else if ( ServerParam::instance().properGoalKicks() )
                {
                    awardGoalKick( M_kick_taker->side(),
                                   M_stadium.ball().pos() );
                }
            }
            // else it's part of a compound kick
        }
        //else
        //{
        M_kick_taken = true;
        M_kick_taker = &kicker;
        M_kick_taker_dashes = M_kick_taker->dashCount();
        //}
    }
    else if ( M_stadium.playmode() == PM_OffSide_Left
              || M_stadium.playmode() == PM_OffSide_Right )
    {
        // do nothing
    }
    else if ( M_stadium.playmode() != PM_PlayOn )
    {
        M_kick_taken = true;
        M_kick_taker = &kicker;
        M_kick_taker_dashes = M_kick_taker->dashCount();
        M_stadium.changePlayMode( PM_PlayOn );
    }
    else // PM_PlayOn
    {
        if ( M_kick_taken )
        {
            if ( M_kick_taker == &kicker
                 && ServerParam::instance().freeKickFaults() )
            {
                if ( M_kick_taker->dashCount() > M_kick_taker_dashes )
                {
                    M_stadium.setPlayerState( M_kick_taker->side(),
                                              M_kick_taker->unum(),
                                              FREE_KICK_FAULT );
                    callFreeKickFault( M_kick_taker->side(),
                                       M_stadium.ball().pos() );
                }
            }
            else
            {
                M_kick_taken = false;
            }
        }
    }
}

void
FreeKickRef::tackleTaken( const Player & tackler,
                          const bool )
{
    kickTaken( tackler );
}

void
FreeKickRef::ballTouched( const Player & player )
{
    if ( ( M_stadium.playmode() == PM_GoalKick_Left
           && player.side() != LEFT )
         || ( M_stadium.playmode() == PM_GoalKick_Right
              && player.side() != RIGHT )
         )
    {
        // opponent player kicks tha ball while ball is in penalty area.
        awardGoalKick( (Side)( - player.side() ), M_stadium.ball().pos() );
        M_goal_kick_count = -1;
        M_kick_taken = false;
        return;
    }

    if ( M_kick_taken )
    {
        if ( M_kick_taker == &player
             && ServerParam::instance().freeKickFaults() )
        {
            if ( M_kick_taker->dashCount() > M_kick_taker_dashes )
            {
                M_stadium.setPlayerState( M_kick_taker->side(),
                                          M_kick_taker->unum(),
                                          FREE_KICK_FAULT );
                callFreeKickFault( M_kick_taker->side(),
                                   M_stadium.ball().pos() );
            }
            /// else do nothing yet as the player just colided with the ball instead of dashing into it
        }
        else
        {
            M_kick_taken = false;
        }
    }
}

void
FreeKickRef::analyse()
{
    const PlayMode pm = M_stadium.playmode();

    if ( isPenaltyShootOut( pm ) )
    {
        return;
    }

    if ( pm == PM_Free_Kick_Fault_Left )
    {
        ++M_after_free_kick_fault_time;

        //clearPlayersFromBall( LEFT );
        if ( M_after_free_kick_fault_time > AFTER_FREE_KICK_FAULT_WAIT - CLEAR_PLAYER_TIME )
        {
            clearPlayersFromBall( LEFT );
        }

        if ( M_after_free_kick_fault_time > AFTER_FREE_KICK_FAULT_WAIT )
        {
            M_stadium.changePlayMode( PM_FreeKick_Right );
        }
        return;
    }

    if ( pm == PM_Free_Kick_Fault_Right )
    {
        ++M_after_free_kick_fault_time;

        //clearPlayersFromBall( RIGHT );
        if ( M_after_free_kick_fault_time > AFTER_FREE_KICK_FAULT_WAIT - CLEAR_PLAYER_TIME )
        {
            clearPlayersFromBall( RIGHT );
        }

        if ( M_after_free_kick_fault_time > AFTER_FREE_KICK_FAULT_WAIT )
        {
            M_stadium.changePlayMode( PM_FreeKick_Left );
        }
        return;
    }

    if ( pm == PM_Back_Pass_Left
         || pm == PM_Back_Pass_Right
         || pm == PM_CatchFault_Left
         || pm == PM_CatchFault_Right )
    {
        // analyzed by CatchRef
        return;
    }

    if ( goalKick( pm ) )
    {
        placePlayersForGoalkick();
        M_stadium.placePlayersInField();

        if ( ! inPenaltyArea( NEUTRAL, M_stadium.ball().pos() ) )
        {
            M_stadium.changePlayMode( PM_PlayOn );
        }
        else
        {
            if ( M_kick_taken
                 && ServerParam::instance().properGoalKicks() )
            {
                if ( ballStopped() )
                {
                    if ( tooManyGoalKicks() )
                    {
                        awardFreeKick( (Side)( -M_kick_taker->side() ),
                                       M_stadium.ball().pos() );
                    }
                    else
                    {
                        awardGoalKick( M_kick_taker->side(),
                                       M_stadium.ball().pos() );
                    }
                }
            }
            else
            {
                if ( M_timer > -1 )
                {
                    M_timer--;
                }

                if ( M_timer == 0 )
                {
                    awardDropBall( M_stadium.ball().pos() );
                }
            }
        }

        return;
    }

    if ( pm != PM_PlayOn
         && pm != PM_BeforeKickOff
         && pm != PM_TimeOver
         && pm != PM_AfterGoal_Right
         && pm != PM_AfterGoal_Left
         && pm != PM_OffSide_Right
         && pm != PM_OffSide_Left
         && pm != PM_Foul_Charge_Right
         && pm != PM_Foul_Charge_Left
         && pm != PM_Foul_Push_Right
         && pm != PM_Foul_Push_Left
         && pm != PM_Back_Pass_Right
         && pm != PM_Back_Pass_Left
         && pm != PM_Free_Kick_Fault_Right
         && pm != PM_Free_Kick_Fault_Left
         && pm != PM_CatchFault_Right
         && pm != PM_CatchFault_Left )
    {
        if ( M_stadium.ball().vel().x != 0.0
             || M_stadium.ball().vel().y != 0.0 )
        {
            M_stadium.changePlayMode( PM_PlayOn );
        }
    }

    M_stadium.placePlayersInField();

    if ( pm != PM_PlayOn
         && pm != PM_TimeOver
         && pm != PM_GoalKick_Left
         && pm != PM_GoalKick_Right
         && pm != PM_OffSide_Left
         && pm != PM_OffSide_Right
         && pm != PM_Foul_Charge_Left
         && pm != PM_Foul_Charge_Right
         && pm != PM_Foul_Push_Right
         && pm != PM_Foul_Push_Left
         && pm != PM_Back_Pass_Left
         && pm != PM_Back_Pass_Right
         && pm != PM_Free_Kick_Fault_Left
         && pm != PM_Free_Kick_Fault_Right
         && pm != PM_CatchFault_Left
         && pm != PM_CatchFault_Right )
    {
        clearPlayersFromBall( (Side)(-M_stadium.kickOffSide() ) );
    }

    if ( freeKick( pm ) )
    {
        if ( M_timer > -1 )
        {
            M_timer--;
        }

        if ( M_timer == 0 )
        {
            awardDropBall( M_stadium.ball().pos() );
        }
    }
}


void
FreeKickRef::playModeChange( PlayMode pm )
{
    if ( pm != PM_PlayOn )
    {
        M_kick_taken = false;
    }

    if ( pm == PM_KickOff_Left
         || pm == PM_KickIn_Left
         || pm == PM_FreeKick_Left
         || pm == PM_IndFreeKick_Left
         || pm == PM_CornerKick_Left
         || pm == PM_GoalKick_Left )
    {
        clearPlayersFromBall( RIGHT );
    }
    else if ( pm == PM_KickOff_Right
              || pm == PM_KickIn_Right
              || pm == PM_FreeKick_Right
              || pm == PM_IndFreeKick_Right
              || pm == PM_CornerKick_Right
              || pm == PM_GoalKick_Right )
    {
        clearPlayersFromBall( LEFT );
    }
    else if ( pm == PM_Drop_Ball )
    {
        clearPlayersFromBall( NEUTRAL );
    }

    if ( goalKick( pm ) )
    {
        M_timer = ServerParam::instance().dropTime();

        if ( ! goalKick( M_stadium.playmode() ) )
        {
            M_goal_kick_count = 0;
        }
        else
        {
            M_goal_kick_count++;
        }
    }
    else
    {
        M_goal_kick_count = 0;
    }

    if ( freeKick( pm ) )
    {
        M_timer = ServerParam::instance().dropTime();
    }

    if ( ! freeKick( pm ) && ! goalKick( pm ) )
    {
        M_timer = -1;
    }

    if ( pm == PM_Free_Kick_Fault_Left
         || pm == PM_Free_Kick_Fault_Right )
    {
        M_stadium.clearBallCatcher();
        M_after_free_kick_fault_time = 0;
    }
}


void
FreeKickRef::callFreeKickFault( Side side, PVector pos )
{
    pos = truncateToPitch( pos );
    pos = moveOutOfPenalty( side, pos );

    M_stadium.clearBallCatcher();

    if ( side == LEFT )
    {
        M_stadium.placeBall( PM_Free_Kick_Fault_Left, RIGHT, pos );
    }
    else if ( side == RIGHT )
    {
        M_stadium.placeBall( PM_Free_Kick_Fault_Right, LEFT, pos );
    }

    M_after_free_kick_fault_time = 0;
}

bool
FreeKickRef::goalKick( PlayMode pm )
{
    return ( pm == PM_GoalKick_Right
             || pm == PM_GoalKick_Left );
}


bool
FreeKickRef::freeKick( PlayMode pm )
{
    switch( pm )
    {
    case PM_KickOff_Right:
    case PM_KickIn_Right:
    case PM_FreeKick_Right:
    case PM_CornerKick_Right:
    case PM_IndFreeKick_Right:
    case PM_KickOff_Left:
    case PM_KickIn_Left:
    case PM_FreeKick_Left:
    case PM_CornerKick_Left:
    case PM_IndFreeKick_Left:
        return true;
    default:
        return false;
    }
}

bool
FreeKickRef::ballStopped()
{
    return M_stadium.ball().vel().r() < ServerParam::instance().stoppedBallVel();
}

bool
FreeKickRef::tooManyGoalKicks()
{
    return M_goal_kick_count >= ServerParam::instance().maxGoalKicks();
}


void
FreeKickRef::placePlayersForGoalkick()
{
    static const RArea p_l( PVector( - ServerParam::PITCH_LENGTH/2
                                     + ServerParam::PENALTY_AREA_LENGTH/2.0,
                                     0.0 ),
                            PVector( ServerParam::PENALTY_AREA_LENGTH,
                                     ServerParam::PENALTY_AREA_WIDTH ) );
    static const RArea p_r( PVector( + ServerParam::PITCH_LENGTH/2
                                     - ServerParam::PENALTY_AREA_LENGTH/2.0,
                                     0.0 ),
                            PVector( ServerParam::PENALTY_AREA_LENGTH,
                                     ServerParam::PENALTY_AREA_WIDTH ) );
    const RArea * p_area;

    int oppside;

    if ( M_stadium.playmode() == PM_GoalKick_Left )
    {
        oppside = RIGHT;
        p_area = &p_l;
    }
    else
    {
        oppside = LEFT;
        p_area = &p_r;
    }

    const Stadium::PlayerCont::iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( ! (*p)->isEnabled() ) continue;

        if ( (*p)->side() == oppside )
        {
            const double size = (*p)->size();
            RArea expand_area( p_area->left() - size,
                               p_area->right() + size,
                               p_area->top() - size,
                               p_area->bottom() + size );

            if ( expand_area.inArea( (*p)->pos() ) )
            {
                PVector new_pos = expand_area.nearestEdge( (*p)->pos() );
                if ( new_pos.x * oppside >= ServerParam::PITCH_LENGTH/2 )
                {
                    new_pos.x
                        = ( ServerParam::PITCH_LENGTH/2
                            - ServerParam::PENALTY_AREA_LENGTH
                            - size )
                        * oppside;
                }

                (*p)->moveTo( new_pos );
            }
        }
    }
}


//**********
// TouchRef
//**********

const int TouchRef::AFTER_GOAL_WAIT = 50;

void
TouchRef::analyse()
{
    analyseImpl();

    M_prev_ball_pos = M_stadium.ball().pos();
}

void
TouchRef::analyseImpl()
{
    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( M_stadium.playmode() == PM_AfterGoal_Left )
    {
        if ( ++M_after_goal_time > AFTER_GOAL_WAIT )
        {
            M_stadium.placeBall( PM_KickOff_Right, RIGHT, PVector(0.0,0.0) );
            placePlayersInTheirField();
        }
        return;
    }

    if ( M_stadium.playmode() == PM_AfterGoal_Right )
    {
        if ( ++M_after_goal_time > AFTER_GOAL_WAIT )
        {
            M_stadium.placeBall( PM_KickOff_Left, LEFT, PVector(0.0,0.0) );
            placePlayersInTheirField();
        }
        return;
    }

    if ( checkGoal() )
    {
        return;
    }

    if ( M_stadium.playmode() != PM_AfterGoal_Left
         && M_stadium.playmode() != PM_AfterGoal_Right
         && M_stadium.playmode() != PM_TimeOver )
    {
        if ( std::fabs( M_stadium.ball().pos().x )
             > ServerParam::PITCH_LENGTH * 0.5
             + ServerParam::instance().ballSize() )
        {
            // check for goal kick or corner kick
            Side side = NEUTRAL;
            if ( M_last_touched != NULL )
            {
                side = M_last_touched->side();
            }

            if ( M_stadium.ball().pos().x
                 > ServerParam::PITCH_LENGTH * 0.5
                 + ServerParam::instance().ballSize() )
            {
                if ( side == RIGHT )
                {
                    awardCornerKick( LEFT, M_stadium.ball().pos() );
                }
                else if ( M_stadium.ballCatcher() == NULL )
                {
                    awardGoalKick( RIGHT, M_stadium.ball().pos() );
                }
                else
                {
                    // the ball is caught and the goalie must have
                    // moved to a position beyond the opponents goal
                    // line.  Let the catch ref clean up the mess
                    return;
                }
            }
            else if ( M_stadium.ball().pos().x
                      < ServerParam::PITCH_LENGTH * 0.5
                      - ServerParam::instance().ballSize() )
            {
                if ( side == LEFT )
                {
                    awardCornerKick( RIGHT, M_stadium.ball().pos() );
                }
                else if ( M_stadium.ballCatcher() == NULL )
                {
                    awardGoalKick( LEFT, M_stadium.ball().pos() );
                }
                else
                {
                    // the ball is caught and the goalie must have
                    // moved to a position beyond the opponents goal
                    // line.  Let the catch ref clean up the mess
                    return;
                }
            }
        }
        else if ( std::fabs( M_stadium.ball().pos().y )
                  > ServerParam::PITCH_WIDTH * 0.5
                  + ServerParam::instance().ballSize() )
        {
            // check for kick in.

            Side side = NEUTRAL;
            if ( M_last_touched != NULL )
            {
                side = M_last_touched->side();
            }

            if ( side == NEUTRAL )
            {
                // somethings gone wrong but give a drop ball
                awardDropBall( M_stadium.ball().pos() );
            }
            else
            {
                awardKickIn( (Side)( -side ), M_stadium.ball().pos() );
            }
        }
    }
}

void
TouchRef::kickTaken( const Player & kicker )
{
    if ( std::fabs( M_stadium.ball().pos().x )
         <= ServerParam::PITCH_LENGTH * 0.5 + ServerParam::instance().ballSize() )
    {
        if ( M_stadium.playmode() == PM_PlayOn
             && M_last_indirect_kicker
             && M_last_indirect_kicker != &kicker )
        {
            M_last_indirect_kicker = NULL;
            M_indirect_mode = false;
        }

        if ( M_indirect_mode )
        {
            M_last_indirect_kicker = &kicker;
        }

        M_last_touched = &kicker;
    }
}


void
TouchRef::tackleTaken( const Player & tackler,
                       const bool )
{
    kickTaken( tackler );
}

void
TouchRef::ballTouched( const Player & kicker )
{
    if ( std::fabs( M_stadium.ball().pos().x )
         <= ServerParam::PITCH_LENGTH * 0.5 + ServerParam::instance().ballSize() )
    {
        if ( M_stadium.playmode() == PM_PlayOn
             && M_last_indirect_kicker
             && M_last_indirect_kicker != &kicker )
        {
            M_last_indirect_kicker = NULL;
            M_indirect_mode = false;
        }

        if ( M_indirect_mode )
        {
            M_last_indirect_kicker = &kicker;
        }

        M_last_touched = &kicker;
    }
}


void
TouchRef::playModeChange( PlayMode pm )
{
    if ( pm != PM_PlayOn )
    {
        M_last_touched = NULL;
    }

    if ( indirectFreeKick( pm ) )
    {
        M_last_indirect_kicker = NULL;
        M_indirect_mode = true;
    }
    else if ( pm != PM_PlayOn && pm != PM_Drop_Ball )
    {
        M_last_indirect_kicker = NULL;
        M_indirect_mode = false;
    }
}


bool
TouchRef::checkGoal()
{
    if ( M_stadium.playmode() == PM_AfterGoal_Left
         || M_stadium.playmode() == PM_AfterGoal_Right
         || M_stadium.playmode() == PM_TimeOver )
    {
        return false;
    }

    if ( M_indirect_mode )
    {
        return false;
    }

    const ServerParam & param = ServerParam::instance();

    // FIFA rules:  Ball has to be completely outside of the pitch to be considered out
    //    static RArea pt( PVector(0.0,0.0),
    //                       PVector( ServerParam::PITCH_LENGTH
    //                                + ServerParam::instance().ballSize() * 2,
    //                                ServerParam::PITCH_WIDTH
    //                                + ServerParam::instance().ballSize() * 2 ) );

    if ( std::fabs( M_stadium.ball().pos().x )
         <= ServerParam::PITCH_LENGTH * 0.5 + param.ballSize() )
    {
        return false;
    }


    if ( ( ! M_stadium.ballCatcher()
           || M_stadium.ballCatcher()->side() == LEFT )
         && crossGoalLine( LEFT, M_prev_ball_pos )
         && ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        M_stadium.score( RIGHT );
        announceGoal( M_stadium.teamRight() );
        M_after_goal_time = 0;
        M_stadium.placeBall( LEFT, M_stadium.ball().pos() );
        if ( param.halfTime() >= 0
             && param.goldenGoal()
             && M_stadium.time() >= param.halfTime() * param.nrNormalHalfs() )
        {
            M_stadium.changePlayMode( PM_TimeOver );
        }
        else
        {
            M_stadium.changePlayMode( PM_AfterGoal_Right );
        }
        return true;
    }
    else if ( ( ! M_stadium.ballCatcher()
                || M_stadium.ballCatcher()->side() == RIGHT )
              && crossGoalLine( RIGHT, M_prev_ball_pos )
              && ! isPenaltyShootOut( M_stadium.playmode() )  )
    {
        M_stadium.score( LEFT );
        announceGoal( M_stadium.teamLeft() );
        M_after_goal_time = 0;
        M_stadium.placeBall( RIGHT, M_stadium.ball().pos() );
        if ( param.halfTime() >= 0
             && param.goldenGoal()
             && M_stadium.time() >= param.halfTime() * param.nrNormalHalfs() )
        {
            M_stadium.changePlayMode( PM_TimeOver );
        }
        else
        {
            M_stadium.changePlayMode( PM_AfterGoal_Left );
        }
        return true;
    }

    return false;
}


void
TouchRef::announceGoal( const Team & team )
{
#ifdef HAVE_SSTREAM
    std::ostringstream msg;
    msg << "goal_" << SideStr( team.side() ) << "_" << team.point();
    M_stadium.sendRefereeAudio( msg.str().c_str() );
#else
    std::ostrstream msg;
    msg << "goal_" << SideStr( team.side() ) << "_" << team.point() << std::ends;
    M_stadium.say( msg.str() );
    msg.freeze( false );
#endif
}


bool
TouchRef::indirectFreeKick( const PlayMode pm )
{
    switch( pm ) {
    case PM_IndFreeKick_Right:
    case PM_IndFreeKick_Left:
        return true;
    default:
        return false;
    }
}

//************
// CatchRef
//************

const int CatchRef::AFTER_BACKPASS_WAIT = 30;
const int CatchRef::AFTER_CATCH_FAULT_WAIT = 30;

void
CatchRef::kickTaken( const Player & kicker )
{
    if ( kicker.side() == LEFT )
    {
        M_team_l_touched = true;
    }
    else if ( kicker.side() == RIGHT )
    {
        M_team_r_touched = true;
    }

    if ( M_team_l_touched && M_team_r_touched )
    {
        M_last_back_passer = NULL;
        M_before_last_back_passer = NULL;
        return;
    }

    //! check if a different player kicked the ball
    if ( M_last_back_passer != &kicker )
    {
        M_before_last_back_passer = M_last_back_passer;
		M_last_back_passer = &kicker;
		M_last_back_passer_time = M_stadium.time();
    }
    else
    {
        //! same player is kicking the ball again, update last kick time
		M_last_back_passer_time = M_stadium.time();
    }
}

void
CatchRef::tackleTaken( const Player & tackler,
                       const bool )
{
    kickTaken( tackler );
}

void
CatchRef::ballTouched( const Player & player )
{
    // If ball is not kicked, back pass violation is never taken.

    if ( player.side() == LEFT )
    {
        M_team_l_touched = true;
    }
    else if ( player.side() == RIGHT )
    {
        M_team_r_touched = true;
    }

    if ( M_team_l_touched && M_team_r_touched )
    {
        M_before_last_back_passer = NULL;
        M_last_back_passer = NULL;
    }
}

void
CatchRef::ballCaught( const Player & catcher )
{
    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    // check handling violation
    if ( M_stadium.playmode() != PM_AfterGoal_Left
         && M_stadium.playmode() != PM_AfterGoal_Right
         && M_stadium.playmode() != PM_TimeOver
         && ! inPenaltyArea( catcher.side(), M_stadium.ball().pos() ) )
    {
        callCatchFault( catcher.side(), M_stadium.ball().pos() );
        return;
    }

    // check backpass violation
    if ( ServerParam::instance().backPasses()
         && M_stadium.playmode() != PM_AfterGoal_Left
         && M_stadium.playmode() != PM_AfterGoal_Right
         && M_stadium.playmode() != PM_TimeOver
         && M_last_back_passer != NULL
         && M_last_back_passer->team() == catcher.team() )
    {
        if ( M_last_back_passer == &catcher
             && M_before_last_back_passer != NULL
             && M_before_last_back_passer->team() != catcher.team() )
        {
            // no backpass violation, if last kicker is goalie itself and before kicker is opponent
        }
        else if ( M_stadium.time() == M_last_back_passer_time
                  && ( M_before_last_back_passer == NULL
                       || M_before_last_back_passer->team() != catcher.team() ) )
        {
            // no backpass violation. kick and catch are taken simultaneously
        }
        else
        {
            M_stadium.setPlayerState( M_last_back_passer->side(),
                                      M_last_back_passer->unum(),
                                      BACK_PASS );
            callBackPass( catcher.side() );

            return;
        }
    }

    M_last_back_passer = NULL;
    M_before_last_back_passer = NULL;

    awardFreeKick( catcher.side(), M_stadium.ball().pos() );
}


void
CatchRef::ballPunched( const Player & catcher )
{
    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    // check handling violation
    if ( M_stadium.playmode() != PM_AfterGoal_Left
         && M_stadium.playmode() != PM_AfterGoal_Right
         && M_stadium.playmode() != PM_TimeOver
         && ! inPenaltyArea( catcher.side(), M_stadium.ball().pos() ) )
    {
        callCatchFault( catcher.side(), M_stadium.ball().pos() );
        return;
    }

    // check backpass violation
    if ( M_stadium.playmode() != PM_AfterGoal_Left
         && M_stadium.playmode() != PM_AfterGoal_Right
         && M_stadium.playmode() != PM_TimeOver
         && M_stadium.time() != M_last_back_passer_time
         && M_last_back_passer != NULL
         && M_last_back_passer->team() == catcher.team()
         && ServerParam::instance().backPasses() )
    {
        if ( M_last_back_passer == &catcher
             && M_before_last_back_passer
             && M_before_last_back_passer->team() != catcher.team() )
        {
            // no backpass violatoin, if last kicker is goalie itself and before kicker is opponent
        }
        else
        {
            M_stadium.setPlayerState( M_last_back_passer->side(),
                                      M_last_back_passer->unum(),
                                      BACK_PASS );
            callBackPass( catcher.side() );
        }

        return;
    }

    M_last_back_passer = NULL;
    M_before_last_back_passer = NULL;
}


void
CatchRef::analyse()
{
    const PlayMode pm = M_stadium.playmode();

    M_team_l_touched = false;
    M_team_r_touched = false;

    if ( isPenaltyShootOut( pm ) )
    {
        return;
    }

    if ( pm == PM_Back_Pass_Left )
    {
        ++M_after_back_pass_time;

        //clearPlayersFromBall( LEFT );
        if ( M_after_back_pass_time > AFTER_BACKPASS_WAIT - CLEAR_PLAYER_TIME )
        {
            clearPlayersFromBall( LEFT );
        }

        if ( M_after_back_pass_time > AFTER_BACKPASS_WAIT )
        {
            //M_stadium.changePlayMode( PM_FreeKick_Right );
            M_stadium.changePlayMode( PM_IndFreeKick_Right );
        }
        return;
    }

    if ( pm == PM_Back_Pass_Right )
    {
        ++M_after_back_pass_time;

        //clearPlayersFromBall( RIGHT );
        if ( M_after_back_pass_time > AFTER_BACKPASS_WAIT - CLEAR_PLAYER_TIME )
        {
            clearPlayersFromBall( RIGHT );
        }

        if ( M_after_back_pass_time > AFTER_BACKPASS_WAIT )
        {
            //M_stadium.changePlayMode( PM_FreeKick_Left );
            M_stadium.changePlayMode( PM_IndFreeKick_Left );
        }
        return;
    }

    if ( pm == PM_CatchFault_Left )
    {
        ++M_after_catch_fault_time;

        //clearPlayersFromBall( LEFT );
        if ( M_after_catch_fault_time > AFTER_CATCH_FAULT_WAIT - CLEAR_PLAYER_TIME )
        {
            clearPlayersFromBall( LEFT );
        }

        if ( M_after_catch_fault_time > AFTER_CATCH_FAULT_WAIT )
        {
            //M_stadium.changePlayMode( PM_IndFreeKick_Right );
            M_stadium.changePlayMode( PM_FreeKick_Right );
        }
        return;
    }

    if ( pm == PM_CatchFault_Right )
    {
        ++M_after_catch_fault_time;

        //clearPlayersFromBall( RIGHT );
        if ( M_after_catch_fault_time > AFTER_CATCH_FAULT_WAIT - CLEAR_PLAYER_TIME )
        {
            clearPlayersFromBall( RIGHT );
        }

        if ( M_after_catch_fault_time > AFTER_CATCH_FAULT_WAIT )
        {
            //M_stadium.changePlayMode( PM_IndFreeKick_Left );
            M_stadium.changePlayMode( PM_FreeKick_Left );
        }
        return;
    }


    if ( M_stadium.ballCatcher()
         && pm != PM_AfterGoal_Left
         && pm != PM_AfterGoal_Right
         && pm != PM_TimeOver
         && ! inPenaltyArea( M_stadium.ballCatcher()->side(),
                             M_stadium.ball().pos() ) )
    {
        callCatchFault( M_stadium.ballCatcher()->side(),
                        M_stadium.ball().pos() );
    }

}

void
CatchRef::playModeChange( PlayMode pmode )
{
    if ( pmode != PM_PlayOn )
    {
        M_before_last_back_passer = NULL;
        M_last_back_passer = NULL;
    }

    if ( pmode == PM_Back_Pass_Left
         || pmode == PM_Back_Pass_Left )
    {
        M_stadium.clearBallCatcher();
        M_after_back_pass_time = 0;
    }
    else if ( pmode == PM_CatchFault_Left
              || pmode == PM_CatchFault_Left )
    {
        M_stadium.clearBallCatcher();
        M_after_catch_fault_time = 0;
    }
}

void
CatchRef::callBackPass( const Side side )
{
    PVector pos = truncateToPitch( M_stadium.ball().pos() );
    //pos = moveOutOfPenalty( side, pos );
    pos = moveOutOfGoalArea( side, pos );

    M_stadium.clearBallCatcher();

    if ( side == LEFT )
    {
        M_stadium.placeBall( PM_Back_Pass_Left, RIGHT, pos );
    }
    else
    {
        M_stadium.placeBall( PM_Back_Pass_Right, LEFT, pos );
    }

    M_last_back_passer = NULL;
    M_after_back_pass_time = 0;
}

void
CatchRef::callCatchFault( Side side, PVector pos )
{
    pos = truncateToPitch( pos );
    //pos = moveIntoPenalty( side, pos );
    pos = moveOutOfPenalty( side, pos );

    M_stadium.clearBallCatcher();

    if ( side == LEFT )
    {
        M_stadium.placeBall( PM_CatchFault_Left, RIGHT, pos );
    }
    else if ( side == RIGHT )
    {
        M_stadium.placeBall( PM_CatchFault_Right, LEFT, pos );
    }

    M_after_catch_fault_time = 0;
}

//************
// FoulRef
//************

const int FoulRef::AFTER_FOUL_WAIT = 30;

void
FoulRef::tackleTaken( const Player & tackler,
                      const bool foul )
{
    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    bool detect_charge = false;
    bool detect_yellow = false;
    bool detect_red = false;

    checkFoul( tackler, foul, &detect_charge, &detect_yellow, &detect_red );

    if ( detect_red )
    {
        callRedCard( tackler );
    }
    else if ( detect_yellow )
    {
        callYellowCard( tackler );
    }
    else if ( detect_charge )
    {
        callFoul( tackler );
    }
}

void
FoulRef::analyse()
{
    const PlayMode pm = M_stadium.playmode();

    if ( isPenaltyShootOut( pm ) )
    {
        return;
    }

    if ( pm == PM_Foul_Charge_Left
         || pm == PM_Foul_Push_Left )
    {
        ++M_after_foul_time;

        //clearPlayersFromBall( LEFT );
        if ( M_after_foul_time > AFTER_FOUL_WAIT - CLEAR_PLAYER_TIME )
        {
            clearPlayersFromBall( LEFT );
        }

        if ( M_after_foul_time > AFTER_FOUL_WAIT )
        {
            if ( Referee::inPenaltyArea( LEFT, M_stadium.ball().pos() ) )
            {
                M_stadium.changePlayMode( PM_IndFreeKick_Right );
            }
            else
            {
                M_stadium.changePlayMode( PM_FreeKick_Right );
            }
        }
        return;
    }

    if ( pm == PM_Foul_Charge_Right
         || pm == PM_Foul_Push_Right )
    {
        ++M_after_foul_time;

        //clearPlayersFromBall( RIGHT );
        if ( M_after_foul_time > AFTER_FOUL_WAIT - CLEAR_PLAYER_TIME )
        {
            clearPlayersFromBall( RIGHT );
        }

        if ( M_after_foul_time > AFTER_FOUL_WAIT )
        {
            if ( Referee::inPenaltyArea( RIGHT, M_stadium.ball().pos() ) )
            {
                M_stadium.changePlayMode( PM_IndFreeKick_Left );
            }
            else
            {
                M_stadium.changePlayMode( PM_FreeKick_Left );
            }
        }
        return;
    }
}

void
FoulRef::callFoul( const Player & tackler )
{
    PVector pos = truncateToPitch( tackler.pos() );
    pos = moveOutOfGoalArea( NEUTRAL, pos );

    M_stadium.clearBallCatcher();

    if ( tackler.side() == LEFT )
    {
        pos = moveOutOfPenalty( RIGHT, pos );
        M_stadium.placeBall( PM_Foul_Charge_Left,
                             RIGHT, pos );
    }
    else if ( tackler.side() == RIGHT )
    {
        pos = moveOutOfPenalty( LEFT, pos );
        M_stadium.placeBall( PM_Foul_Charge_Right,
                             LEFT, pos );
    }

    M_stadium.punishFoulPlay( tackler.side(),
                              tackler.unum() );
    M_after_foul_time = 0;
}

void
FoulRef::callYellowCard( const Player & tackler )
{
    callFoul( tackler );

    M_stadium.yellowCard( tackler.side(),
                          tackler.unum() );
}

void
FoulRef::callRedCard( const Player & tackler )
{
    callFoul( tackler );

    M_stadium.redCard( tackler.side(),
                       tackler.unum() );
}

void
FoulRef::playModeChange( PlayMode pm )
{
    if ( pm == PM_Foul_Charge_Left
         || pm == PM_Foul_Charge_Right
         || pm == PM_Foul_Push_Left
         || pm == PM_Foul_Push_Right )
    {
        M_after_foul_time = 0;
    }
}

//************
// KeepawayRef
//************

KeepawayRef::KeepawayRef( Stadium & stadium )
    : Referee( stadium ),
      M_episode( 0 ),
      M_keepers( 0 ),
      M_takers( 0 ),
      M_time( 0 ),
      M_take_time( 0 )
{

}

void
KeepawayRef::analyse()
{
    if ( ! ServerParam::instance().keepAwayMode() )
    {
        return;
    }

    static time_t s_start_time = std::time( NULL );

    if ( M_stadium.playmode() == PM_PlayOn )
    {
        if ( ! ballInKeepawayArea() )
        {
            logEpisode( "o" );
            M_stadium.sendRefereeAudio( trainingMsg );
            resetField();
        }
        else if ( M_take_time >= TURNOVER_TIME )
        {
            logEpisode( "t" );
            M_stadium.sendRefereeAudio( trainingMsg );
            resetField();
        }
        else
        {
            bool keeper_poss = false;

            const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
            for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
                  p != end;
                  ++p )
            {
                if ( ! (*p)->isEnabled() ) continue;

                PVector ppos = (*p)->pos();

                if ( ppos.distance2( M_stadium.ball().pos() )
                     < std::pow( ServerParam::instance().kickableArea(), 2 ) )
                {
                    if ( (*p)->side() == LEFT )
                    {
                        keeper_poss = true;
                    }
                    else if ( (*p)->side() == RIGHT )
                    {
                        keeper_poss = false;
                        ++M_take_time;
                        break;
                    }
                }
            }

            if ( keeper_poss )
            {
                M_take_time = 0;
            }
        }
    }
    else if ( ServerParam::instance().kawayStart() >= 0 )
    {
        if ( difftime( std::time( NULL ), s_start_time ) > ServerParam::instance().kawayStart() )
        {
            M_stadium.changePlayMode( PM_PlayOn );
        }
    }
}

void
KeepawayRef::playModeChange( PlayMode pm )
{
    if ( ServerParam::instance().keepAwayMode() )
    {
        if ( pm == PM_PlayOn && M_episode == 0 )
        {
            M_episode = 1;

            const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
            for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
                  p != end;
                  ++p )
            {
                if ( ! (*p)->isEnabled() ) continue;

                if ( (*p)->side() == LEFT )
                {
                    ++M_keepers;
                }
                else if ( (*p)->side() == RIGHT )
                {
                    ++M_takers;
                }
            }

            logHeader();
            resetField();
        }
    }
}

bool
KeepawayRef::ballInKeepawayArea()
{
    PVector ball_pos = M_stadium.ball().pos();
    return ( std::fabs( ball_pos.x ) < ServerParam::instance().keepAwayLength() * 0.5
             && std::fabs( ball_pos.y ) < ServerParam::instance().keepAwayWidth() * 0.5 );
}

void
KeepawayRef::logHeader()
{
    if ( M_stadium.logger().kawayLog() )
    {
        M_stadium.logger().kawayLog() << "# Keepers: " << M_keepers << '\n'
                                      << "# Takers:  " << M_takers << '\n'
                                      << "# Region:  " << ServerParam::instance().keepAwayLength()
                                      << " x " << ServerParam::instance().keepAwayWidth()
                                      << '\n'
                                      << "#\n"
                                      << "# Description of Fields:\n"
                                      << "# 1) Episode number\n"
                                      << "# 2) Start time in simulator steps (100ms)\n"
                                      << "# 3) End time in simulator steps (100ms)\n"
                                      << "# 4) Duration in simulator steps (100ms)\n"
                                      << "# 5) (o)ut of bounds / (t)aken away\n"
                                      << "#\n"
                                      << std::flush;
    }
}

void
KeepawayRef::logEpisode( const char * endCond )
{
    if ( M_stadium.logger().kawayLog() )
    {
        M_stadium.logger().kawayLog() << M_episode++ << "\t"
                                      << M_time << "\t"
                                      << M_stadium.time() << "\t"
                                      << M_stadium.time() - M_time << "\t"
                                      << endCond
                                      << std::endl;
    }

    M_time = M_stadium.time();
}

void
KeepawayRef::resetField()
{
    int keeper_pos = irand( M_keepers );
    //int keeper_pos = boost::uniform_smallint<>( 0, M_keepers - 1 )( rcss::random::DefaultRNG::instance() );

    const Stadium::PlayerCont::iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( ! (*p)->isEnabled() ) continue;

        double x, y;
        if ( (*p)->side() == LEFT )
        {
            switch( keeper_pos ) {
            case 0:
                x = -ServerParam::instance().keepAwayLength() * 0.5 + drand( 0, 3 );
                y = -ServerParam::instance().keepAwayWidth() * 0.5 + drand( 0, 3 );
                break;
            case 1:
                x = ServerParam::instance().keepAwayLength() * 0.5 - drand( 0, 3 );
                y = -ServerParam::instance().keepAwayWidth() * 0.5 + drand( 0, 3 );
                break;
            case 2:
                x = ServerParam::instance().keepAwayLength() * 0.5 - drand( 0, 3 );
                y = ServerParam::instance().keepAwayWidth() * 0.5 - drand( 0, 3 );
                break;
            default:
                x = drand( -1, 1 ); y = drand( -1, 1 );
                break;
            }

            (*p)->place( PVector( x, y ) );

            keeper_pos = ( keeper_pos + 1 ) % M_keepers;
        }
        else if ( (*p)->side() == RIGHT )
        {
            x = -ServerParam::instance().keepAwayLength() * 0.5 + drand( 0, 3 );
            y = ServerParam::instance().keepAwayWidth() * 0.5 - drand( 0, 3 );

            (*p)->place( PVector( x, y ) );
        }
    }

    M_stadium.placeBall( NEUTRAL,
                         PVector( -ServerParam::instance().keepAwayLength() * 0.5 + 4.0,
                                  -ServerParam::instance().keepAwayWidth() * 0.5 + 4.0 ) );
    M_stadium.recoveryPlayers();

    M_take_time = 0;
}


//************
// PenaltyRef
//************

PenaltyRef::PenaltyRef( Stadium& stadium )
    : Referee( stadium ),
      M_timer( -1 ),
      M_pen_nr_taken( 0 ),
      M_bDebug( false ),
      M_cur_pen_taker( NEUTRAL ),
      M_last_taker( NULL ),
      M_prev_ball_pos( 0.0, 0.0 ),
      M_timeover( false )
{

}

void
PenaltyRef::analyse()
{
    startPenaltyShootout();
    analyseImpl();

    M_prev_ball_pos = M_stadium.ball().pos();
}

void
PenaltyRef::startPenaltyShootout()
{
    static bool first_time = true;

    const ServerParam & param = ServerParam::instance();

    // if normal and extra time are over -> start the penalty procedure or quit
    if ( first_time
         && param.penaltyShootOuts()
         && M_stadium.playmode() != PM_BeforeKickOff
         && M_stadium.teamLeft().point() == M_stadium.teamRight().point()
         && ( ( param.halfTime() < 0
                && param.nrNormalHalfs() + param.nrExtraHalfs() == 0 )
              || ( param.halfTime() >= 0
                   && ( M_stadium.time() >=
                        ( param.halfTime() * param.nrNormalHalfs()
                          + param.extraHalfTime() * param.nrExtraHalfs() ) ) )
              )
         )
    {
        if ( drand( 0, 1 ) < 0.5 )       // choose random side of the playfield
        {
            M_pen_side = LEFT;            // and inform players
        }
        else
        {
            M_pen_side = RIGHT;
        }

        M_stadium.clearBallCatcher();
        M_stadium.sendRefereeAudio( M_pen_side == LEFT
                                    ? "penalty_onfield_l"
                                    : "penalty_onfield_r" );
        // choose at random who starts (note that in penalty_init, actually the
        // opposite player is chosen since there the playMode changes)
        M_cur_pen_taker = ( drand( 0, 1 ) < 0.5 ) ? LEFT : RIGHT;

        // place the goalkeeper of the opposite field close to the penalty goal
        // otherwise it is hard to get there before pen_setup_wait cycles
        Side side = ( M_pen_side == LEFT ) ? RIGHT : LEFT;
        const Stadium::PlayerCont::iterator end = M_stadium.players().end();
        for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            if ( (*p)->isEnabled()
                 && (*p)->side() == side
                 && (*p)->isGoalie() )
            {
                (*p)->moveTo( PVector( -M_pen_side
                                       * (ServerParam::PITCH_LENGTH/2-10),
                                       10 ) );
            }
        }

        penalty_init();
        first_time = false;
    }
}

void
PenaltyRef::analyseImpl()
{
    static const char * playmode_strings[] = PLAYMODE_STRINGS;

    if ( ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( M_timeover )
    {
        M_stadium.changePlayMode( PM_TimeOver );
        return;
    }

    const PlayMode pm = M_stadium.playmode();

    bool bCheckLeft  = penalty_check_players( LEFT );
    bool bCheckRight = penalty_check_players( RIGHT );

    if ( M_bDebug )
    {
        std::cerr << "timer " << playmode_strings[M_stadium.playmode()] << " "
                  << M_stadium.time() << " " << M_timer << " " << bCheckLeft
                  << " " << bCheckRight << std::endl;
    }

    // if ready or taken make sure all players keep well-positioned
    if ( ServerParam::instance().penCoachMovesPlayers()
         && ( pm == PM_PenaltyReady_Left || pm == PM_PenaltyReady_Right
              || pm == PM_PenaltyTaken_Left || pm == PM_PenaltyTaken_Right )
         )
    {
        if ( ! bCheckLeft )
        {
            penalty_place_all_players( LEFT );
        }

        if ( ! bCheckRight )
        {
            penalty_place_all_players( RIGHT );
        }

        bCheckLeft = bCheckRight = true;
    }

    if ( M_bDebug )
        std::cerr << "after placement" << std::endl;

    if ( M_timer < 0 )
    {
        std::cerr << "(PenaltyRef::analyse) timer cannot be negative?" << std::endl;
    }
    else if ( M_timer == 0 )
    {
        handleTimeout( bCheckLeft, bCheckRight );
    }
    else // M_timer > 0
    {
        handleTimer( bCheckLeft, bCheckRight );
    }
}

void
PenaltyRef::handleTimeout( bool left_move_check,
                           bool right_move_check )
{
    const PlayMode pm = M_stadium.playmode();

    if ( M_bDebug )
        std::cerr << M_stadium.time() << " timer reached zero" << std::endl;

    // when setup has finished and still players are positioned incorrectly
    // replace them and go to ready mode.
    if ( ServerParam::instance().penCoachMovesPlayers()
         && ( pm == PM_PenaltySetup_Left
              || pm == PM_PenaltySetup_Right )
         )
    {
        if ( ! left_move_check )
        {
            penalty_place_all_players( LEFT );
        }

        if ( ! right_move_check )
        {
            penalty_place_all_players( RIGHT );
        }

        left_move_check = right_move_check = true;
    }


    if ( pm == PM_PenaltyMiss_Left
         || pm == PM_PenaltyScore_Left
         || pm == PM_PenaltyMiss_Right
         || pm == PM_PenaltyScore_Right )
    {
        penalty_init();
    }
    else if ( left_move_check
              && right_move_check )
    {
        if ( pm == PM_PenaltySetup_Left )
        {
            M_stadium.changePlayMode( PM_PenaltyReady_Left );
        }
        else if ( pm == PM_PenaltySetup_Right )
        {
            M_stadium.changePlayMode( PM_PenaltyReady_Right );
        }
        // time elapsed -> missed goal
        else if ( pm == PM_PenaltyTaken_Left
                  || pm == PM_PenaltyReady_Left )
        {
            penalty_miss( LEFT );
        }
        else if ( pm == PM_PenaltyTaken_Right
                  || pm == PM_PenaltyReady_Right )
        {
            penalty_miss( RIGHT );
        }
    }
    // if incorrect positioned , place them correctly
    else if ( M_cur_pen_taker == LEFT )
    {
        penalty_foul( ( left_move_check == false ) ? LEFT : RIGHT );
    }
    else if ( M_cur_pen_taker == RIGHT )
    {
        penalty_foul( ( right_move_check == false ) ? RIGHT : LEFT );
    }
}

void
PenaltyRef::handleTimer( const bool left_move_check,
                         const bool right_move_check )
{
    const PlayMode pm = M_stadium.playmode();

    --M_timer;

    if ( pm == PM_PenaltyScore_Left
         || pm == PM_PenaltyScore_Right
         || pm == PM_PenaltyMiss_Left
         || pm == PM_PenaltyMiss_Right )
    {
        // freeze the ball
        //         M_stadium.ball().moveTo( M_stadium.ball().pos(),
        //                                 //0.0,
        //                                 PVector( 0.0, 0.0 ),
        //                                 PVector( 0.0, 0.0 ) );
        M_stadium.placeBall( M_cur_pen_taker,
                             M_stadium.ball().pos() );

        return;
    }

    if ( left_move_check
         && right_move_check )
    {
        // if ball crossed goalline, process goal and set ball on goalline
        if ( crossGoalLine( M_pen_side, M_prev_ball_pos ) )
        {
            if ( pm == PM_PenaltyTaken_Left )
            {
                penalty_score( LEFT );
            }
            else if ( pm == PM_PenaltyTaken_Right )
            {
                penalty_score( RIGHT );
            }
            // freeze the ball at the current position.
            M_stadium.placeBall( M_pen_side, M_stadium.ball().pos() );
        }
        else if ( std::fabs( M_stadium.ball().pos().x )
                  > ServerParam::PITCH_LENGTH * 0.5
                  + ServerParam::instance().ballSize()
                  || std::fabs( M_stadium.ball().pos().y )
                  > ServerParam::PITCH_WIDTH * 0.5
                  + ServerParam::instance().ballSize() )
        {
            if ( M_bDebug )
                std::cerr << "MISSED" << std::endl;

            M_stadium.placeBall( M_pen_side, M_stadium.ball().pos() );
            if ( pm == PM_PenaltyTaken_Left )
            {
                penalty_miss( LEFT );
            }
            else if ( pm == PM_PenaltyTaken_Right )
            {
                penalty_miss( RIGHT );
            }
        }
    }
    // if someone makes foul and we are not in setup -> replace the players
    else if ( pm == PM_PenaltyReady_Left
              || pm == PM_PenaltyReady_Right
              || pm == PM_PenaltyTaken_Left
              || pm == PM_PenaltyTaken_Right )
    {
        if ( ServerParam::instance().penCoachMovesPlayers() )
        {
            if ( left_move_check == false )
            {
                penalty_place_all_players( LEFT );
            }
            if ( right_move_check == false )
            {
                penalty_place_all_players( RIGHT );
            }
        }
        else if ( M_cur_pen_taker == LEFT )
        {
            penalty_foul( ( left_move_check == false ) ? LEFT : RIGHT );
        }
        else if ( M_cur_pen_taker == RIGHT )
        {
            penalty_foul( ( right_move_check == false ) ? RIGHT : LEFT );
        }
    }
}



void
PenaltyRef::ballCaught( const Player & catcher )
{
    if ( ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( M_bDebug )
    {
        std::cerr << "GOALIE CATCH" << std::endl;
    }

    if ( M_stadium.playmode() == PM_PenaltyTaken_Left
         || M_stadium.playmode() == PM_PenaltyTaken_Right )
    {
        if ( catcher.side() == M_cur_pen_taker )
        {
            std::cerr << "catch by taker side goalie. foul "
                      << -M_cur_pen_taker
                      << std::endl;
            // taker team's goalie catches the ball
            penalty_foul( (Side)( -M_cur_pen_taker ) );
        }
        else if ( ! inPenaltyArea( M_pen_side, M_stadium.ball().pos() ) )
        {
            std::cerr << "outside the penalty area. foul  "
                      << -M_cur_pen_taker
                      << std::endl;
            // goalie catches the ball outside the penalty area
            penalty_foul( (Side)( -M_cur_pen_taker ) );
        }
        else
        {
            std::cerr << "legal catch. miss of the side " << M_cur_pen_taker
                      << std::endl;
            // legal catch
            penalty_miss( M_cur_pen_taker );
        }
    }
    else if ( M_stadium.playmode() == PM_PenaltyReady_Left
              || M_stadium.playmode() == PM_PenaltyReady_Right )
    {
        std::cerr << "ball caught in ready mode. foul " << -M_cur_pen_taker
                  << std::endl;
        penalty_foul( catcher.side() );
    }

    // freeze the ball
    //     M_stadium.ball().moveTo( M_stadium.ball().pos(),
    //                             //0.0,
    //                             PVector( 0.0, 0.0 ),
    //                             PVector( 0.0, 0.0 ) );
    M_stadium.placeBall( NEUTRAL, M_stadium.ball().pos() );

    //M_stadium.ball().vel.x = M_stadium.ball().vel.y = 0;
}


void
PenaltyRef::ballPunched( const Player & catcher )
{
    ballCaught( catcher );
}


void
PenaltyRef::kickTaken( const Player & kicker )
{
    if ( ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( M_stadium.ballCatcher() != NULL )
    {
        std::cerr << "player kicked and goalie catched at the same time" << std::endl;
    }
    // if in setup it is not allowed to kick the ball
    else if ( M_stadium.playmode() == PM_PenaltySetup_Left
              || M_stadium.playmode() == PM_PenaltySetup_Right )
    {
        penalty_foul( kicker.side() );
    }
    // cannot kick second time after penalty was taken
    else if ( ! ServerParam::instance().penAllowMultiKicks()
              && ( M_stadium.playmode() == PM_PenaltyTaken_Left
                   || M_stadium.playmode() == PM_PenaltyTaken_Right )
              && kicker.side() == M_cur_pen_taker )
    {
        penalty_foul( M_cur_pen_taker );
    }
    else if ( M_stadium.playmode() == PM_PenaltyReady_Left
              || M_stadium.playmode() == PM_PenaltyTaken_Left
              || M_stadium.playmode() == PM_PenaltyReady_Right
              || M_stadium.playmode() == PM_PenaltyTaken_Right )
    {
        if ( ( M_stadium.playmode() == PM_PenaltyReady_Left
               || M_stadium.playmode() == PM_PenaltyReady_Right )
             && kicker.side() == M_cur_pen_taker
             && ( ( LEFT == M_cur_pen_taker
                    && M_sLeftPenTaken.find( kicker.unum() ) != M_sLeftPenTaken.end() )
                  || ( RIGHT == M_cur_pen_taker
                       && M_sRightPenTaken.find( kicker.unum() ) != M_sRightPenTaken.end() )
                  )
             )
        {
            // this kicker has already taken the kick
            penalty_foul( M_cur_pen_taker );
        }
        else if ( M_last_taker
                  && M_last_taker->side() == M_cur_pen_taker
                  && M_last_taker != &kicker )
        {
            // not a taker player in the same team must not kick the ball.
            penalty_foul( M_cur_pen_taker );
        }
        else if ( kicker.side() != M_cur_pen_taker
                  && ! kicker.isGoalie() )
        {
            // field player in the defending team must not kick the ball.
            penalty_foul( static_cast< Side >( -M_cur_pen_taker ) );
        }
        else
        {
            M_last_taker = &kicker;
        }
    }

    // if we were ready for penalty -> change play mode
    if ( M_stadium.playmode() == PM_PenaltyReady_Left )
    {
        // when penalty is taken, add player, multiple copies are deleted
        M_sLeftPenTaken.insert( kicker.unum() );
        if ( M_sLeftPenTaken.size() == MAX_PLAYER )
        {
            M_sLeftPenTaken.clear();
        }
        M_stadium.changePlayMode( PM_PenaltyTaken_Left );
    }
    else if ( M_stadium.playmode() == PM_PenaltyReady_Right )
    {
        M_sRightPenTaken.insert( kicker.unum() );
        if ( M_sRightPenTaken.size() == MAX_PLAYER )
        {
            M_sRightPenTaken.clear();
        }
        M_stadium.changePlayMode( PM_PenaltyTaken_Right );
    }
    // if it was not allowed to kick, don't move ball
    else if ( M_stadium.playmode() != PM_PenaltyTaken_Left
              && M_stadium.playmode() != PM_PenaltyTaken_Right )
    {
        M_stadium.placeBall( M_pen_side, M_stadium.ball().pos() );
    }

}


void
PenaltyRef::tackleTaken( const Player & tackler,
                         const bool foul )
{
    if ( ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    bool detect_charge = false;
    bool detect_yellow = false;
    bool detect_red = false;

    checkFoul( tackler, foul, &detect_charge, &detect_yellow, &detect_red );

    if ( detect_charge
         || detect_yellow
         || detect_red )
    {
        if ( tackler.side() == M_cur_pen_taker )
        {
            penalty_foul( M_cur_pen_taker );
        }
        else
        {
            penalty_foul( static_cast< Side >( -M_cur_pen_taker ) );
        }
    }
    else
    {
        kickTaken( tackler );
    }
}


void
PenaltyRef::playModeChange( PlayMode pm )
{
    // if mode changes, reset the timer
    if ( ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( pm == PM_PenaltySetup_Left || pm == PM_PenaltySetup_Right )
    {
        M_last_taker = NULL;
        M_timer = ServerParam::instance().penSetupWait();
    }
    else if ( pm == PM_PenaltyReady_Left || pm == PM_PenaltyReady_Right )
    {
        M_last_taker = NULL;
        M_timer = ServerParam::instance().penReadyWait();
    }
    else if ( pm == PM_PenaltyTaken_Left || pm == PM_PenaltyTaken_Right )
    {
        M_timer = ServerParam::instance().penTakenWait();
    }
    else if ( pm == PM_PenaltyMiss_Left  || pm == PM_PenaltyMiss_Right ||
              pm == PM_PenaltyScore_Left || pm == PM_PenaltyScore_Right )
    {
        M_last_taker = NULL;
        M_timer = ServerParam::instance().penBeforeSetupWait();
    }
    else
    {
        M_last_taker = NULL;
    }
}


void
PenaltyRef::penalty_init()
{
    if ( M_bDebug )
        std::cerr << "init penalty" << std::endl;

    // change the play mode such that the other side can take the penalty
    // and place the ball at the penalty spot
    M_cur_pen_taker = ( M_cur_pen_taker == LEFT
                        ? RIGHT
                        : LEFT );
    PlayMode pm = ( M_cur_pen_taker == LEFT
                    ? PM_PenaltySetup_Left
                    : PM_PenaltySetup_Right );
    M_stadium.placeBall( pm,
                         NEUTRAL,
                         PVector( - M_pen_side
                                  * ( ServerParam::PITCH_LENGTH/2
                                      - ServerParam::instance().penDistX() ),
                                  0.0 ) );
}

void
PenaltyRef::penalty_score( Side side )
{
    M_stadium.changePlayMode( side == RIGHT
                              ? PM_PenaltyScore_Right
                              : PM_PenaltyScore_Left );

    if ( side == RIGHT )
    {
        M_stadium.penaltyScore( RIGHT, true );
    }
    else
    {
        M_stadium.penaltyScore( LEFT, true );
    }
    M_pen_nr_taken++;
    penalty_check_score();
}

void
PenaltyRef::penalty_miss( Side side )
{
    M_stadium.changePlayMode( side == LEFT
                              ? PM_PenaltyMiss_Left
                              : PM_PenaltyMiss_Right );
    M_pen_nr_taken++;

    if ( side == RIGHT )
    {
        M_stadium.penaltyScore( RIGHT, false );
    }
    else
    {
        M_stadium.penaltyScore( LEFT, false );
    }

    penalty_check_score();
}

void
PenaltyRef::penalty_foul( const Side side )
{
    M_stadium.sendRefereeAudio( ( side == LEFT
                                  ?  "penalty_foul_l"
                                  : "penalty_foul_r" ) );

    // if team takes penalty and makes mistake -> miss, otherwise -> score
    if ( side == LEFT && M_cur_pen_taker == LEFT )
    {
        penalty_miss( LEFT );
    }
    else if ( side == RIGHT && M_cur_pen_taker == RIGHT )
    {
        penalty_miss( RIGHT );
    }
    else if ( side == LEFT )
    {
        penalty_score( RIGHT );
    }
    else
    {
        penalty_score( LEFT );
    }
}

void
PenaltyRef::penalty_check_score()
{
    if ( M_bDebug ) std::cerr << "penalty_check_score" << std::endl;

    std::cerr << "Score: " << ( (M_cur_pen_taker == LEFT) ? "*" : " " )
              << M_stadium.teamLeft().penaltyPoint() << "-"
              << M_stadium.teamRight().penaltyPoint()
              << ((M_cur_pen_taker == RIGHT) ? "*" : " ") << " after "
              << M_pen_nr_taken << " penalties." << std::endl;

    // if both players have taken more than nr_kicks penalties -> check for winner
    if ( M_pen_nr_taken > 2 * ServerParam::instance().penNrKicks() )
    {
        if( M_pen_nr_taken % 2 == 0
            && ( M_stadium.teamLeft().penaltyPoint()
                 != M_stadium.teamRight().penaltyPoint() ) )
        {
            std::cerr << "Final score: "
                      << M_stadium.teamLeft().penaltyPoint() << "-"
                      << M_stadium.teamRight().penaltyPoint() << std::endl;
            if ( M_stadium.teamLeft().penaltyPoint()
                 > M_stadium.teamRight().penaltyPoint() )
            {
                M_stadium.sendRefereeAudio( "penalty_winner_l" );
            }
            else
            {
                M_stadium.sendRefereeAudio( "penalty_winner_r" );
            }
            //M_stadium.changePlayMode( PM_TimeOver );
            M_timeover = true;
        }
    }
    // if both players have taken nr_kicks and max_extra_kicks penalties -> quit
    else if ( M_pen_nr_taken > 2 * ( ServerParam::instance().penMaxExtraKicks()
                                     + ServerParam::instance().penNrKicks() )
              )
    {
        std::cerr << "Final score: "
                  << M_stadium.teamLeft().penaltyPoint() << "-"
                  << M_stadium.teamRight().penaltyPoint() << std::endl;
        if ( ServerParam::instance().penRandomWinner() )
        {
            if ( drand( 0, 1 ) < 0.5 )
            {
                M_stadium.sendRefereeAudio( "penalty_winner_l" );
                M_stadium.penaltyWinner( LEFT );
                std::cerr << "Left team has won the coin toss!" << std::endl;
            }
            else
            {
                M_stadium.sendRefereeAudio( "penalty_winner_r" );
                M_stadium.penaltyWinner( RIGHT );
                std::cerr << "Right team has won the coin toss!" << std::endl;
            }
        }
        else
        {
            M_stadium.sendRefereeAudio( "penalty_draw" );
        }
        //M_stadium.changePlayMode( PM_TimeOver );
        M_timeover = true;
    }
    // during normal kicks, check whether one team cannot win anymore
    else
    {
        // first calculate how many penalty kick sessions are left
        // and add this to the current number of points of both teams
        // finally, subtract 1 point from the team that has already shot this turn
        int iPenLeft = ServerParam::instance().penNrKicks() - M_pen_nr_taken/2;
        int iMaxExtraLeft  = M_stadium.teamLeft().penaltyPoint() + iPenLeft;
        int iMaxExtraRight = M_stadium.teamRight().penaltyPoint() + iPenLeft;
        if ( M_pen_nr_taken % 2 == 1 )
        {
            if ( M_cur_pen_taker == LEFT )
            {
                iMaxExtraLeft--;
            }
            else if ( M_cur_pen_taker == RIGHT )
            {
                iMaxExtraRight--;
            }
        }

        if ( iMaxExtraLeft < M_stadium.teamRight().penaltyPoint() )
        {
            std::cerr << "Final score: "
                      << M_stadium.teamLeft().penaltyPoint() << "-"
                      << M_stadium.teamRight().penaltyPoint() << std::endl;
            M_stadium.sendRefereeAudio( "penalty_winner_r" );
            M_stadium.changePlayMode( PM_TimeOver );
        }
        else if ( iMaxExtraRight < M_stadium.teamLeft().penaltyPoint() )
        {
            std::cerr << "Final score: "
                      << M_stadium.teamLeft().penaltyPoint() << "-"
                      << M_stadium.teamRight().penaltyPoint() << std::endl;
            M_stadium.sendRefereeAudio( "penalty_winner_l" );
            //M_stadium.changePlayMode( PM_TimeOver );
            M_timeover = true;
        }
    }
}

bool
PenaltyRef::penalty_check_players( const Side side )
{
    PlayMode pm = M_stadium.playmode();
    int     iOutsideCircle = 0;
    bool    bCheck         = true;
    PVector posGoalie;
    //int     iPlayerOutside = -1, iGoalieNr=-1;
    const Player * outside_player = NULL;
    const Player * goalie = NULL;

    if ( pm == PM_PenaltyMiss_Left  || pm == PM_PenaltyMiss_Right
         || pm == PM_PenaltyScore_Left || pm == PM_PenaltyScore_Right )
    {
        return true;
    }

    // for all players from side 'side' get the goalie pos and count how many
    // players are outside the center circle.
    const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( ! (*p)->isEnabled() ) continue;

        if ( (*p)->side() == side )
        {
            if ( (*p)->isGoalie() )
            {
                goalie = *p;
                posGoalie = (*p)->pos();
                continue;
            }

            CArea c( PVector( 0.0, 0.0 ),
                     ServerParam::KICK_OFF_CLEAR_DISTANCE
                     - (*p)->size() );
            if ( ! c.inArea( (*p)->pos() ) )
            {
                iOutsideCircle++;
                outside_player = *p;
            }
        }
    }

    if ( ! goalie )
    {
        return false;
    }

    // if the 'side' equals the one that takes the penalty shoot out
    if ( side == M_cur_pen_taker )
    {
        // in case that goalie takes penalty kick
        // or taker goes into the center circle
        if ( iOutsideCircle == 0 )
        {
            if ( pm == PM_PenaltySetup_Left || pm == PM_PenaltySetup_Right )
            {
                if ( goalie->pos().distance( M_stadium.ball().pos() ) > 2.0 )
                {
                    // bCheck = false;
                }
                else
                {
                    outside_player = goalie;
                }
            }
        }
        // if goalie not outside field, check fails
        else if ( std::fabs( posGoalie.x ) < ServerParam::PITCH_LENGTH/2.0 - 1.5
                  || std::fabs( posGoalie.y ) < ServerParam::PENALTY_AREA_WIDTH/2.0 - 1.5 )
        {
            if ( M_bDebug )
                std::cerr << "side " << side << " goalie not correct "
                          <<  posGoalie.x << " " << posGoalie.y << std::endl;
            bCheck = false;
        }
        // only one should be outside the circle -> player that takes penalty
        else if ( iOutsideCircle > 1 )
        {
            if ( M_bDebug )
                std::cerr << "side " << side << " not 1 player outside " << std::endl;
            bCheck = false;
        }
        // in setup, player outside circle should be close to ball
        else if ( ( pm == PM_PenaltySetup_Left || pm == PM_PenaltySetup_Right )
                  && iOutsideCircle == 1 )
        {
            if ( outside_player
                 && outside_player->pos().distance( M_stadium.ball().pos() ) > 2.0 )
            {
                if ( M_bDebug )
                    std::cerr << "side " << side << " attacker not correct " << std::endl;
                bCheck = false;
            }
        }
    }
    else //other team
    {
        // goalie does not stand in front of goal line
        if ( M_stadium.playmode() != PM_PenaltyTaken_Left
             && M_stadium.playmode() != PM_PenaltyTaken_Right )
        {
            if ( std::fabs( posGoalie.x )
                 < ServerParam::PITCH_LENGTH/2.0 - ServerParam::instance().penMaxGoalieDistX()
                 || std::fabs( posGoalie.y )
                 > ServerParam::instance().goalWidth()*0.5 )
            {
                if ( M_bDebug )
                    std::cerr << "side " << side << " goalie to catch not correct "
                              << posGoalie << std::endl;
                bCheck = false;
            }
        }
        // when receiving the penalty every player should be in center circle
        if ( iOutsideCircle != 0 )
        {
            if ( M_bDebug )
                std::cerr << "side " << side << " not everyone in circle " << std::endl;
            bCheck = false;
        }
    }

    if ( bCheck
         && outside_player )
    {
        // if in setup and already in set -> check fails
        if( ( side == LEFT
              && M_stadium.playmode() == PM_PenaltySetup_Left
              && M_sLeftPenTaken.find( outside_player->unum() )
              != M_sLeftPenTaken.end() )
            || ( side == RIGHT
                 && M_stadium.playmode() == PM_PenaltySetup_Right
                 && M_sRightPenTaken.find( outside_player->unum() )
                 != M_sRightPenTaken.end() )
            )
        {
            if ( M_bDebug )
                std::cerr << "side " << side << " same player" << std::endl;
            bCheck = false;
        }
    }

    return bCheck;
}

void
PenaltyRef::penalty_place_all_players( const Side side )
{
    if ( side == M_cur_pen_taker )
    {
        placeTakerTeamPlayers();
    }
    else // other team
    {
        placeOtherTeamPlayers();
    }
}

void
PenaltyRef::placeTakerTeamPlayers()
{
    const bool bPenTaken = ( M_stadium.playmode() == PM_PenaltyTaken_Right
                             || M_stadium.playmode() == PM_PenaltyTaken_Left );

    const Player * taker = ( M_last_taker
                             ? M_last_taker
                             : getCandidateTaker() );

    const PVector goalie_wait_pos_b( -M_pen_side * ( ServerParam::PITCH_LENGTH / 2 + 2.0 ), +25.0 );
    const PVector goalie_wait_pos_t( -M_pen_side * ( ServerParam::PITCH_LENGTH / 2 + 2.0 ), -25.0 );

    // then replace the players from the specified side
    const Stadium::PlayerCont::iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( ! (*p)->isEnabled() ) continue;

        if ( (*p)->side() != M_cur_pen_taker ) continue;

        if ( (*p) == taker )
        {
            if ( ! bPenTaken
                 && taker->pos().distance( M_stadium.ball().pos() ) > 2.0 )
            {
                //PVector new_pos( -M_pen_side * ( ServerParam::PITCH_LENGTH/2 - ServerParam::instance().pen_dist_x - 2.0 ),
                //0.0 );
                CArea c( M_stadium.ball().pos(), 2.0 );
                //(*p)->moveTo( new_pos );
                (*p)->moveTo( c.nearestEdge( taker->pos() ) );
            }
        }
        else
        {
            if ( (*p)->isGoalie() )
            {
                CArea c( ( (*p)->pos().y > 0.0 ? goalie_wait_pos_b : goalie_wait_pos_t ),
                         2.0 );
                if ( ! c.inArea( (*p)->pos() ) )
                {
                    (*p)->moveTo( c.nearestEdge( (*p)->pos() ) );
                }
            }
            else // not goalie
            {
                CArea center( PVector( 0.0, 0.0 ),
                              ServerParam::KICK_OFF_CLEAR_DISTANCE
                              - (*p)->size()
                              //- ServerParam::instance().pspeed_max
                              );
                if ( ! center.inArea( (*p)->pos() ) )
                {
                    //(*p)->moveTo( PVector::fromPolar( 6.5, Deg2Rad( i*15 ) ) );
                    (*p)->moveTo( center.nearestEdge( (*p)->pos() ) );
                }
            }
        }
    }
}

void
PenaltyRef::placeOtherTeamPlayers()
{
    const bool bPenTaken = ( M_stadium.playmode() == PM_PenaltyTaken_Right
                             || M_stadium.playmode() == PM_PenaltyTaken_Left );
    const double goalie_line
        = ( M_pen_side == LEFT
            ? - ServerParam::PITCH_LENGTH/2.0 + ServerParam::instance().penMaxGoalieDistX()
            : + ServerParam::PITCH_LENGTH/2.0 - ServerParam::instance().penMaxGoalieDistX() );

    const Stadium::PlayerCont::iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( ! (*p)->isEnabled() ) continue;

        if ( (*p)->side() == M_cur_pen_taker ) continue;

        // only move goalie in case the penalty has not been started yet.
        if ( (*p)->isGoalie() )
        {
            if ( ! bPenTaken )
            {
                if ( M_pen_side == LEFT )
                {
                    if ( (*p)->pos().x - goalie_line > 0.0 )
                    {
                        (*p)->moveTo( PVector( goalie_line - 1.5, 0.0 ) );
                    }
                }
                else
                {
                    if ( (*p)->pos().x - goalie_line < 0.0 )
                    {
                        (*p)->moveTo( PVector( goalie_line + 1.5, 0.0 ) );
                    }
                }
            }
        }
        else // not goalie
        {
            CArea center( PVector( 0.0, 0.0 ),
                          ServerParam::KICK_OFF_CLEAR_DISTANCE
                          - (*p)->size()
                          //- ServerParam::instance().pspeed_max
                          );
            if ( ! center.inArea( (*p)->pos() ) )
            {
                // place other players in circle in penalty area
                //(*p)->moveTo( PVector::fromPolar( 6.5, Deg2Rad( i*15 ) ) );
                (*p)->moveTo( center.nearestEdge( (*p)->pos() ) );
            }
        }
    }
}

const Player *
PenaltyRef::getCandidateTaker()
{
    const std::set< int > & sPenTaken = ( M_cur_pen_taker == LEFT
                                          ? M_sLeftPenTaken
                                          : M_sRightPenTaken );

    const Player * candidate = NULL;
    const Player * goalie = NULL;
    double min_dist2 = std::numeric_limits< double >::max();

    // first find the closest player to the ball
    const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( ! (*p)->isEnabled() ) continue;

        if ( (*p)->side() != M_cur_pen_taker ) continue;

        if ( sPenTaken.find( (*p)->unum() )
             != sPenTaken.end() )
        {
            // players that have already taken a kick cannot be
            // counted as a potential kicker.
            continue;
        }

        if ( (*p)->isGoalie() )
        {
            goalie = (*p);
            continue;
        }

        double d2 = (*p)->pos().distance2( M_stadium.ball().pos() );
        if( d2 < min_dist2 )
        {
            min_dist2 = d2;
            candidate = (*p);
        }
    }

    if ( candidate == NULL )
    {
        return goalie;
    }

    return candidate;
}

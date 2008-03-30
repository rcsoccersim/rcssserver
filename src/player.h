// -*-c++-*-

/***************************************************************************
                          player.h  -  A class for field players and goalies
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


#ifndef RCSSSERVER_PLAYER_H
#define RCSSSERVER_PLAYER_H

#include "arm.h"
#include "audio.h"
#include "object.h"
#include "pcombuilder.h"
#include "pcomparser.h"
#include "remoteclient.h"
#include "serverparam.h"

#include <string>

class Stadium;
class HeteroPlayer;
class Team;

namespace rcss {
class InitObserverPlayer;
class ObserverPlayer;
class BodyObserverPlayer;
class FullStateObserver;
}

class Player
    : public MPObject,
      public RemoteClient,
      public rcss::Listener,
      public rcss::pcom::Builder
{
private:

    rcss::InitObserverPlayer * M_init_observer;
    rcss::ObserverPlayer * M_observer;
    rcss::BodyObserverPlayer * M_body_observer;
    rcss::FullStateObserver * M_fullstate_observer;

    Team * M_team;
    const Side M_side;
    const int M_unum;  /* uniform number */

    std::string M_name_far;
    std::string M_name_toofar;
    /* th 19.05.00 */
    std::string M_short_name_far;
    std::string M_short_name_toofar;

    double M_stamina;
    double M_recovery;
    double M_effort;

    double M_consumed_stamina;

    double M_vis_angle;
    rcss::pcom::VIEW_WIDTH M_view_width;
public:
    const double defangle;
    const double vis_distance;
    const double vis_distance2;
private:
    double M_inertia_moment;

    double M_version; //!< client protocol version

    double M_unum_far_length;
    double M_unum_too_far_length;
    double M_team_far_length;
    double M_team_too_far_length;

#ifdef NEW_QSTEP
    double dist_qstep_player; /* each Player quantize step of distance */
    double land_qstep_player; /* each Player quantize step of distance for landmark */
    double dir_qstep_player; /* each Player quantize step of direction */
#endif

    double M_angle_body; //!< temporal body angle
    double M_angle_body_committed;
    double M_angle_neck; //!< temporal neck angle
    double M_angle_neck_committed;

    // pfr 8/14/00: for RC2000 evaluation
    double M_kick_rand;

    bool M_synch_see;
    int M_vis_send;
    bool M_highquality;

    Int32 M_state;

    bool M_ball_collide;
    bool M_player_collide;
    bool M_post_collide;

    bool M_command_done;
    bool M_turn_neck_done;
    bool M_done_received; //pfr:SYNCH

    int M_hear_capacity_from_teammate;
    int M_hear_capacity_from_opponent;

    bool M_goalie;
    int M_goalie_catch_ban;
    int M_goalie_moves_since_catch;

    int M_kick_count;
    int M_dash_count;
    int M_turn_count;
    int M_catch_count;
    int M_move_count;
    int M_turn_neck_count;
    int M_change_view_count;
    int M_say_count;

    Arm M_arm;

    int M_attentionto_count;

    int M_tackle_cycles;
    int M_tackle_count;

    int M_clang_min_ver;
    int M_clang_max_ver;

    bool M_offside_mark;
    PVector M_offside_pos;

    const HeteroPlayer * M_player_type;
    int M_player_type_id;

    rcss::pcom::Parser M_parser;

private:
    // not used
    Player();
    const Player & operator=( const Player & );

public:
    Player( Stadium & stadium,
            Team * team,
            int number );
    ~Player();

    bool init( const double & ver,
               const bool goalie );

    void setEnable();
    void disable();
    void discard();

    Int32 state() const
      {
          return M_state;
      }
    void addState( const Int32 state )
      {
          M_state |= state;
      }
    void resetState();

    void sendInit();
    void sendReconnect();

    void parseMsg( const char * msg,
                   const size_t & len );

    void send( const char * msg );

    void sendBody()
      {
          sense_body();
      }
    void sendVisual();
    void sendSynchVisual();
    void sendFullstate(); /* contributed by Artur Merke */

    void incArmAge()
      {
          M_arm.incAge();
      }
    const
    Arm & arm() const
      {
          return M_arm;
      }

    int attentiontoCount() const { return M_attentionto_count; }

    int tackleCycles() const { return M_tackle_cycles; }
    bool isTackling() const { return M_tackle_cycles > 0; }
    int tackleCount() const { return M_tackle_count; }

    int clangMinVer() const { return M_clang_min_ver; }
    int clangMaxVer() const { return M_clang_max_ver; }

    //void setBackPasser() { M_alive |= BACK_PASS; }
    //void setFreeKickFaulter() { M_alive |= FREE_KICK_FAULT; }

    void place( const PVector & location );
    void place( const PVector & pos,
                const double & angle,
                const PVector & vel,
                const PVector & accel );

    const
    Team * team() const
      {
          return M_team;
      }

    Side side() const
      {
          return M_side;
      }
    int unum() const
      {
          return M_unum;
      }

    bool isGoalie() const
      {
          return M_goalie;
      }

    const
    std::string & nameFar() const
      {
          return M_name_far;
      }

    const
    std::string & nameTooFar() const
      {
          return M_name_toofar;
      }

    const
    std::string & shortNameFar() const
      {
          return M_short_name_far;
      }

    const
    std::string & shortNameTooFar() const
      {
          return M_short_name_toofar;
      }

    const
    double & stamina() const
      {
          return M_stamina;
      }
    const
    double & recovery() const
      {
          return M_recovery;
      }
    const
    double & effort() const
      {
          return M_effort;
      }

    const
    double & visibleAngle() const
      {
          return M_vis_angle;
      }

    rcss::pcom::VIEW_WIDTH viewWidth() const
      {
          return M_view_width;
      }

    const
    double & version() const
      {
          return M_version;
      }

    const
    double & unumFarLength() const
      {
          return M_unum_far_length;
      }

    const
    double & unumTooFarLength() const
      {
          return M_unum_too_far_length;
      }

    const
    double & teamFarLength() const
      {
          return M_team_far_length;
      }

    const
    double & teamTooFarLength() const
      {
          return M_team_too_far_length;
      }

    double distQStep() const
      {
#ifndef NEW_QSTEP
          return ServerParam::instance().quantizeStep();
#else
          return dist_qstep_player;
#endif
      }

    double landDistQStep() const
      {
#ifndef NEW_QSTEP
          return ServerParam::instance().landmarkQuantizeStep();
#else
          return land_qstep_player;
#endif
      }

    double dirQStep() const
      {
#ifndef NEW_QSTEP
          return 0.1;
#else
          return dir_qstep_player;
#endif
      }

    const
    double & angleBodyCommitted() const
      {
          return M_angle_body_committed;
      }

    const
    double & angleNeckCommitted() const
      {
          return M_angle_neck_committed;
      }

    int visSend() const
      {
          return M_vis_send;
      }
    bool highquality() const
      {
          return M_highquality;
      }

    bool doneReceived() const
      {
          return M_done_received;
      }

    void decrementHearCapacity( const Player & sender );
    bool canHearFullFrom( const Player & sender ) const;

    int kickCount() const { return M_kick_count; }
    int dashCount() const { return M_dash_count; }
    int turnCount() const { return M_turn_count; }
    int catchCount() const { return M_catch_count; }
    int moveCount() const { return M_move_count; }
    int turnNeckCount() const { return M_turn_neck_count; }
    int changeViewCount() const { return M_change_view_count; }
    int sayCount() const { return M_say_count; }

    double angleFromBody( const PObject & obj ) const
      {
          //return M_pos.vangle( obj.pos(), M_angle_body_committed );
          return normalize_angle( ( obj.pos() - this->pos() ).angle()
                                  - M_angle_body_committed );
      }

    void setPlayerType( const int );
    void substitute( const int );

    int playerTypeId() const
      {
          return M_player_type_id;
      }


    bool setSenders();

    void recoverAll();
    void updateStamina();
    void updateCapacity();

    bool ballCollide() const
      {
          return M_ball_collide;
      }
    bool playerCollide() const
      {
          return M_player_collide;
      }
    bool postCollide() const
      {
          return M_post_collide;
      }
    void collidedWithBall()
      {
          addState( BALL_TO_PLAYER | BALL_COLLIDE );
          M_ball_collide = true;
      }
    void collidedWithPlayer()
      {
          addState( PLAYER_COLLIDE );
          M_player_collide = true;
      }
    void resetCollisionFlags();

    void resetCommandFlags();

    void clearOffsideMark();
    void setOffsideMark( const double & offside_line );

    bool hasOffsideMark() const
      {
          return M_offside_mark;
      }
    const
    PVector & offsidePos() const
      {
          return M_offside_pos;
      }

protected:

    virtual
    void turnImpl();
    virtual
    void updateAngle();
    virtual
    void collidedWithPost();
    virtual
    double maxAccel() const;
    virtual
    double maxSpeed() const;

private:
    /** PlayerCommands */
    void dash( double power );
    void turn( double moment );
    void turn_neck( double moment );
    void kick( double power, double dir );
    void goalieCatch( double dir );
    void say( std::string message );
    /*! This function is called in the begin of each cycle
      and in case a player sends a sense_body command. */
    void sense_body();
    void score();
    void move( double x, double y );
    void change_view( rcss::pcom::VIEW_WIDTH viewWidth, rcss::pcom::VIEW_QUALITY viewQuality );
    void change_view( rcss::pcom::VIEW_WIDTH viewWidth );
    void compression( int level );
    void bye();
    void done();
    void pointto( bool on, double dist, double head );
    void attentionto( bool on, rcss::pcom::TEAM team_side, std::string team_name, int at_unum );
    void tackle( double power_or_angle );
    void clang( int min, int max);
    void ear( bool on, rcss::pcom::TEAM team_side, std::string team_name, rcss::pcom::EAR_MODE mode );
    void synch_see();

};

#endif

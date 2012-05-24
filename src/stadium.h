// -*-c++-*-

/***************************************************************************
                                 stadium.h
                              Class for virtual stadium
                             -------------------
    begin                : 2009-11-18
    copyright            : (C) 2009 by The RoboCup Soccer Server
                           Maintainance Group.
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

#ifndef RCSSSERVER_STADIUM_H
#define RCSSSERVER_STADIUM_H

#include "timeable.h"


#include "object.h"
#include "field.h"
#include "weather.h"
#include "logger.h"

#include <rcssbase/gzip/gzfstream.hpp>
#include <rcssbase/net/udpsocket.hpp>

#include <cstdio>
#include <ctime>
#include <string>
#include <vector>
#include <list>
#include <memory>

class HeteroPlayer;
class XPMHolder;

class Monitor;
class Player;
class Coach;
class OnlineCoach;
class Team;

class Referee;
class ResultSaver;

struct timeval;

namespace rcss {
class Listener;
namespace clang {
class Msg;
}
}


/*
 *===================================================================
 *Part: Stadium Class
 *===================================================================
 */
class Stadium
    : public virtual Timeable {
public:
    // These are vectors and not lists, because we need to shuffle them
    typedef std::vector< Player * >  PlayerCont;
    typedef std::vector< Coach * >  OfflineCoachCont;
    typedef std::vector< OnlineCoach * > OnlineCoachCont;
    typedef std::vector< Monitor * > MonitorCont;
    typedef std::vector< rcss::Listener * > ListenerCont;
    typedef std::vector< MPObject * > MPObjectCont;
protected:
    // definitions of different timeable methods
    void doRecvFromClients( );
    void doNewSimulatorStep();
    void doSendSenseBody();
    void doSendVisuals();
    void doSendSynchVisuals();
    void doSendCoachMessages();
    bool doSendThink();
    void doQuit();

protected:
    bool M_alive;

    rcss::net::UDPSocket M_player_socket;
    rcss::net::UDPSocket M_offline_coach_socket;
    rcss::net::UDPSocket M_online_coach_socket;

    Field M_field;
    Weather M_weather;

    Logger M_logger;

    PlayerCont  M_remote_players; //!< connected players
    OfflineCoachCont M_remote_offline_coaches; //!< connected trainers
    OnlineCoachCont M_remote_online_coaches; //!< connected coaches
    MonitorCont M_monitors; //!< connected monitors

    ListenerCont M_listeners;

    MPObjectCont M_movable_objects;

    Ball * M_ball;
    PlayerCont M_players; //!< player instance container
    PlayerCont M_shuffle_players; //!< reference player container
    Coach * M_coach;
    OnlineCoachCont M_olcoaches;
    Team * M_team_l;
    Team * M_team_r;

    PlayMode M_playmode;

    int M_time;
    int M_stoppage_time;

    std::vector< HeteroPlayer * > M_player_types;


    const Player * M_ball_catcher; /* goalie who has caught ball */

    Side M_kick_off_side;

    std::list< Referee * > M_referees;

    int M_last_playon_start;

    int M_kick_off_wait;
    int M_connect_wait;
    int M_game_over_wait;

    int M_left_child;
    int M_right_child;

    tm m_real_time;

    std::list< ResultSaver * > M_savers;

public:

    Stadium();

    virtual
    ~Stadium();

    bool init();

    void finalize( const std::string & msg );

    virtual
    bool isAlive()
      {
          return M_alive;
      }

    Logger & logger()
      {
          return M_logger;
      }

    PlayMode playmode() const
      {
          return M_playmode;
      }

    int time() const
      {
          return M_time;
      }

    int stoppageTime() const
      {
          return M_stoppage_time;
      }

    const
    tm & realTime() const
      {
          return m_real_time;
      }

    const
    Weather & weather() const
      {
          return M_weather;
      }

    const
    Field & field() const
      {
          return M_field;
      }

    const
    Ball & ball() const
      {
          return *M_ball;
      }

    const
    Team & teamLeft() const
      {
          return *M_team_l;
      }

    const
    Team & teamRight() const
      {
          return *M_team_r;
      }

    OnlineCoachCont & onlineCoaches()
      {
          return  M_remote_online_coaches;
      }

    PlayerCont & remotePlayers()
      {
          return M_remote_players;
      }
    const
    PlayerCont & remotePlayers() const
      {
          return M_remote_players;
      }

    PlayerCont & players()
      {
          return M_players;
      }
    const
    PlayerCont & players() const
      {
          return M_players;
      }

    MonitorCont & monitors()
      {
          return M_monitors;
      }
    OfflineCoachCont & offlineCoaches()
      {
          return  M_remote_offline_coaches;
      }
    ListenerCont& listeners()
      {
          return  M_listeners;
      }

    void removeListener( const rcss::Listener * listener );
    void addListener( rcss::Listener * listener )
      {
          M_listeners.push_back( listener );
      }

    void addOfflineCoach( Coach * coach )
      {
          M_remote_offline_coaches.push_back( coach );
      }

    const
    HeteroPlayer * playerType( int id ) const;


    const
    Player * ballCatcher() const
      {
          return M_ball_catcher;
      }

    void clearBallCatcher()
      {
          M_ball_catcher = static_cast< Player * >( 0 );
      }

    Side kickOffSide() const
      {
          return M_kick_off_side;
      }

    int lastPlayOnStart() const
      {
          return M_last_playon_start;
      }

private:
    void createObjects();

    void udp_recv_message();
    void udp_recv_from_coach();
    void udp_recv_from_online_coach();

    void parsePlayerInit( const char * message,
                          const rcss::net::Addr & cli_addr );
    bool parseMonitorInit( const char * message,
                           const rcss::net::Addr & cli_addr );
    bool parseCoachInit( const char * message,
                         const rcss::net::Addr & cli_addr );
    void parseOnlineCoachInit( const char * message,
                               const rcss::net::Addr & cli_addr );
    Player * initPlayer( const char * teamname,
                         const double & version,
                         const bool goalie,
                         const rcss::net::Addr & addr );
    Player * reconnectPlayer( const char * teamname,
                              const int unum,
                              const rcss::net::Addr & addr );
    Coach * initCoach( const double & version,
                       const rcss::net::Addr & );
    OnlineCoach * initOnlineCoach( const char * teamname,
                                   const char * coachname,
                                   const double & version,
                                   const rcss::net::Addr & addr );

    void removeDisconnectedClients();

    void step();

    void turnMovableObjects();
    void incMovableObjects();

    void sendDisp();

    Player * getPlayer( const Side side,
                        const int unum );

public:

    //
    // referee interfaces
    //

    void kickOff();
    void callHalfTime( const Side kick_off_side,
                       const int half_time_count );

    void callFoul( const Side side,
                   PVector pos );
    void dropBall( PVector pos );
    void discardPlayer( const Side side,
                        const int unum );

    void punishFoulPlay( const Side side,
                         const int unum );
    void yellowCard( const Side side,
                     const int unum );
    void redCard( const Side side,
                  const int unum );

    void score( const Side side );
    void penaltyScore( const Side side,
                       const bool scored );
    void penaltyWinner( const Side side );

    void placeBall( const Side kick_off_side,
                    const PVector & pos );
    /*!
      The side parameter is redundant, but it save us from having to look it up.
      Later I would like to make playmodes objects that have side as a member.
    */
    void placeBall( const PlayMode pm,
                    const Side kick_off_side,
                    const PVector & pos );

    void moveBall( const PVector & pos,
                   const PVector & vel );
    bool movePlayer( const Side side,
                     const int unum,
                     const PVector & pos,
                     const double * ang = NULL,
                     const PVector * vel = NULL );

    void changePlayMode( const PlayMode pm );

    void placePlayersInField();

    void recoveryPlayers();

private:
    //! diretcly send message to player client that has cli_addr
    void sendToPlayer( const char *msg,
                       const rcss::net::Addr & cli_addr );
    //! diretcly send message to offline coach client that has cli_addr
    void sendToCoach( const char *msg,
                      const rcss::net::Addr & cli_addr );
    //! diretcly send message to online coach client that has cli_addr
    void sendToOnlineCoach( const char *msg,
                            const rcss::net::Addr & cli_addr );
public:
    void sendRefereeAudio( const char * msg );
    void sendPlayerAudio( const Player & player,
                          const char * msg );
    void sendCoachAudio( const Coach & coach,
                         const char * msg );
    void sendCoachStdAudio( const OnlineCoach &,
                            const rcss::clang::Msg & msg );

    void sendTeamGraphic( const Side side,
                          const unsigned int x,
                          const unsigned int y );

    BallPosInfo ballPosInfo();

private:

    void assignPlayerTypes();
public:
    void substitute( const Player * player,
                     const int player_type_id );
    void broadcastSubstitution( const int side,
                                const int unum,
                                const int player_type );

    bool changePlayerToGoalie( const Player * player );
    void broadcastChangePlayerToGoalie( const Player * player );


    void setPlayerState( const Side side,
                         const int unum,
                         const int state );

    void kickTaken( const Player & kicker,
                    const PVector & accel );
    void failedKickTaken( const Player & kicker );
    void tackleTaken( const Player & tackler,
                      const PVector & accel,
                      const bool foul );
    void failedTackleTaken( const Player & tackler,
                            const bool foul );
    void ballCaught( const Player & catcher );
    void ballPunched( const Player & catcher,
                      const PVector & accel );

    void collisions();

private:

    void calcBallCollisionPos( Player * p );
    void calcCollisionPos( MPObject * a,
                           MPObject * b );

    void checkAutoMode();
    void startTeams();
    int startTeam( const std::string & start );

    bool teamConnected( const Side side );

    void killTeams();

    void saveResults();

    void disable();

};

#endif

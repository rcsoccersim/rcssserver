// -*-c++-*-
/*
 *Header:
 *File: field.h (for C++)
 *Author: Noda Itsuki
 *Date: 1996/02/16
 *EndHeader:
 */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000,2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler
 Copyright (C) 2002- RoboCup Soccer Simulator Maintainance Group.

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */


#ifndef RCSSSERVER_FIELD_H
#define RCSSSERVER_FIELD_H

#include "timeable.h"

#include "object.h"
#include "serverparam.h"
#include "logger.h"

#include <rcssbase/gzip/gzfstream.hpp>

#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
#include <cstdio>

class HeteroPlayer;

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

class Field {
private:
    const Field & operator=( const Field & );

    std::vector< const PObject * > M_goals;
    std::vector< PObject * > M_landmarks;
public:
    const PObject line_l;
    const PObject line_r;
    const PObject line_t;
    const PObject line_b;

    Field();
    ~Field();

    const
    std::vector< const PObject * > & goals() const
      {
          return M_goals;
      }

    const
    std::vector< PObject * > & landmarks() const
      {
          return M_landmarks;
      }

    void addLandmark( PObject * new_obj );
};

class Weather {
private:
    const Weather & operator=( const Weather & );
public:
    PVector	wind_vector;
    double wind_rand;

    void init();
} ;


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
    Logger M_logger;

    PlayerCont  M_remote_players; //!< connected players
    OfflineCoachCont M_remote_offline_coaches; //!< connected trainers
    OnlineCoachCont M_remote_online_coaches; //!< connected coaches
    MonitorCont M_monitors; //!< connected monitors

    ListenerCont M_listeners;

    MPObjectCont M_movable_objects;

    Ball * M_ball;
    PlayerCont M_players; //!< player instance container
    Coach * M_coach;
    OnlineCoachCont M_olcoaches;
    Team * M_team_l;
    Team * M_team_r;

    Weather	M_weather;

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

    void removeListener( const rcss::Listener* listener )
      {
          M_listeners.erase( remove( M_listeners.begin(),
                                     M_listeners.end(),
                                     listener ),
                             M_listeners.end() );
      }
    void addListener( rcss::Listener* listener )
      {
          M_listeners.push_back( listener );
      }

    void addOfflineCoach( Coach* coach )
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
          M_ball_catcher = NULL;
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
    void initObjects();

    template < class T >
    void recv( std::vector< T >& clients );

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

    Player * initPlayer( const char * init_message,
                         const rcss::net::Addr & );
    Player * newPlayer( const char * teamname,
                        const double & version,
                        const bool goalie_flag,
                        const rcss::net::Addr & );
    Player * reconnectPlayer( const char * init_message,
                              const rcss::net::Addr & );
    Coach * initCoach( const char * init_message,
                       const rcss::net::Addr & );
    OnlineCoach * initOnlineCoach( const char * init_message,
                                   const rcss::net::Addr & );

    void removeDisconnectedClients();

    void step();


    void turnMovableObjects();
    void incMovableObjects();

    void sendDisp();

    void move_caught_ball(); // [2000.07.21: I.Noda]

public:
    void referee_get_foul( const double & x, const double & y,
                           const Side side );
    void referee_drop_ball( const double & x, const double & y,
                            const Side side );
    void discard_player( const Side side,
                         const int unum );

    void score( const Side side );
    void penaltyScore( const Side side,
                       const bool scored );
    void penaltyWinner( const Side side );

    void setHalfTime( const Side kick_off_side,
                      const int half_time_count );

    void set_ball( const Side kick_off_side,
                   const PVector & pos,
                   const PVector & vel = PVector(0.0,0.0) );
    /*!
      The side parameter is redundant, but it save us from having to look it up.
      Later I would like to make playmodes objects that have side as a member.
    */
    void placeBall( const PlayMode pm,
                    const Side side,
                    PVector location );

    bool movePlayer( const Side side,
                     const int unum,
                     const PVector & pos,
                     const double * ang = NULL,
                     const PVector * vel = NULL );

    void placePlayersInField();

    void recoveryPlayers();

    void change_play_mode( const PlayMode pm );

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

    void addTeamGraphic( const Side side,
                         const unsigned int x,
                         const unsigned int y,
                         std::auto_ptr< XPMHolder > holder );
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

    void setPlayerState( const Side side,
                         const int unum,
                         const int state );

    void kickTaken( const Player & kicker,
                    const PVector & accel );

    void ballCaught( const Player & catcher );

    void ballCatchFailed();

    void collisions();

private:

    void calcBallCollPos( Player * p );
    void calcCollPos( MPObject * a,
                      MPObject * b );

    void checkAutoMode();
    void startTeams();
    int startTeam( const std::string & start );

    bool teamConnected( const Side side );

    void killTeams();

    void saveResults();

    void disable();

public:
    static
    void _Start( Stadium & stad );

};

#endif

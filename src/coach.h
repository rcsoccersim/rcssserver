// -*-c++-*-

/***************************************************************************
                              coach.h
                  A classes for trainer and coaches.
                             -------------------
    begin                : 2007-04-18
    copyright            : (C) 20072 by The RoboCup Soccer Server
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

#ifndef RCSSSERVER_COACH_H
#define RCSSSERVER_COACH_H

#include "audio.h"
#include "stadium.h"
#include "remoteclient.h"

class Team;

namespace rcss {
class InitObserverOfflineCoach;
class InitObserverOnlineCoach;
class ObserverCoach;
}

/*!
  \class Coach
  \brief trainer class
 */
class Coach
    : public RemoteClient,
      public rcss::Listener
{
private:

    rcss::InitObserverOfflineCoach * M_init_observer_coach;

protected:

    rcss::ObserverCoach * M_observer;

    Stadium & M_stadium;

    bool M_assigned;
    bool M_eye;
    bool M_hear;
    double M_version;

    bool M_done_received; //pfr:SYNCH

private:

    // not used
    Coach();
    Coach( const Coach & );
    Coach & operator=( const Coach & );

public:

    explicit
    Coach( Stadium & stadim );

    virtual
    ~Coach();

    void disable();

    virtual
    bool setSenders( const double & client_version );

    virtual
    void sendInit();

    virtual
    void send( const char * msg );

    virtual
    void parseMsg( char * msg,
                   const size_t & len );

    virtual
    void parse_command( const char * command );

    virtual
    Side side() const
      {
          return NEUTRAL;
      }

    bool assigned() const
      {
          return M_assigned;
      }

    bool isEyeOn() const
      {
          return M_eye;
      }
    bool isEarOn() const
      {
          return M_hear;
      }

    double version() const
      {
          return M_version;
      }

    bool doneReceived() const
      {
          return M_done_received;
      }

private:
    int parse_change_mode( const char * command );
    int parse_move( const char * command );

    void change_mode( std::string mode );

    void ear( std::string mode );

    void recover();
    void change_player_type( const std::string & team_name,
                             int unum,
                             int player_type );

    void change_player_type_goalie( const std::string & team_name,
                                    int unum );

protected:
    void check_ball();
    void look();
    void team_names();
    void eye( std::string mode );
    bool change_player_type_goalie_impl( const Team * team,
                                         int unum );
    void compression( int level );

    void sendOKEye();

public:
    void send_visual_info();

    void resetCommandFlags()
      {
          M_done_received = false;
      }

    void sendExternalMsg();
};

/*!
  \class OnlineCoach
  \brief online coach class
 */
class OnlineCoach
    :	public Coach {
private:

    rcss::InitObserverOnlineCoach * M_init_observer_olcoach;

    Team & M_team;
    Side M_side;

    int M_freeform_messages_said;
    int M_freeform_messages_allowed;

    std::deque< rcss::clang::Msg * > M_message_queue;

    int M_define_messages_left;
    int M_advice_messages_left;
    int M_info_messages_left;
    int M_meta_messages_left;
    int M_del_messages_left;
    int M_rule_messages_left;

    int M_msg_left_update_time;

    std::string M_coach_name;

    // not used
    OnlineCoach();
    OnlineCoach( const OnlineCoach & );
    OnlineCoach & operator=( const OnlineCoach & );

public:

    OnlineCoach( Stadium & stadium,
                 Team & team );
    ~OnlineCoach();

    void disable();

    virtual
    bool setSenders( const double & client_version );

    virtual
    void sendInit();

    virtual
    void send( const char * msg );

    virtual
    void parseMsg( char * msg,
                   const size_t & len );

    virtual
    void parse_command( const char * command );

    virtual
    Side side() const
      {
          return M_side;
      }

    void setName( const std::string & name )
      {
          M_coach_name = name;
      }
    const std::string & name() const
      {
          return M_coach_name;
      }

    void awardFreeformMessageCount();
    bool canSendFreeform() const;

    void say( const char * message );
    void say( const rcss::clang::Msg & message );

private:
    void change_player_types( const char * command );

    void change_player_type( int unum,
                             int player_type );

    void change_player_type_goalie( int unum );

    void team_graphic( const char * command );

public:
    void sendPlayerClangVer();
    void sendPlayerClangVer( const Player & player );

    /* this deques and says any messages which should come out
       returns the number of messages se nt out */
    int check_message_queue( int time );

    //returns whether it updated anything
    bool update_messages_left( int time );

};

#endif

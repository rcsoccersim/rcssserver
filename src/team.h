// -*-c++-*-

/***************************************************************************
                                   team.h
                             Team object class
                             -------------------
    begin                : 2007-11-09
    copyright            : (C) 2007 by The RoboCup Soccer Server
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

#ifndef RCSSSERVER_TEAM_H
#define RCSSSERVER_TEAM_H

#include "types.h"

#include <string>
#include <map>
#include <memory>

class Stadium;
class Player;
class OnlineCoach;
class XPMHolder;

class Team {
public:
    typedef std::pair< unsigned int, unsigned int > GraphKey;
    typedef std::map< GraphKey, XPMHolder * > GraphCont;

private:
    Stadium * M_stadium;

    std::string M_name;
    const Side M_side;

    Player * M_players[MAX_PLAYER];
    int M_size; //! the number of connected players

    int M_point;

    int M_pen_taken;        /* goals attempted in penalty_mode */
    int M_pen_point;        /* goals made in penalty_mode */
    bool M_pen_won;


#ifdef	NEW_QSTEP
    double	M_dist_qstep_team;
    double	M_land_qstep_team;
    double	M_dir_qstep_team;
#endif

    // pfr 8/14/00: for RC2000 evaluation
    double M_prand_factor_team; //factor to multiply prand by
    double M_kick_rand_factor_team; //factor to multiple kick_rand by

    int M_subs_count;
    std::map< int, int > M_ptype_count;
    std::map< int, int > M_ptype_used_count;

    OnlineCoach * M_olcoach;

    GraphCont M_graphics;


    // not used
    Team();
    const Team & operator=( const Team & );

public:

    Team( Stadium *stad, const Side side );
    ~Team();

    Player* newPlayer( const double & version,
                       const bool goalie_flag );

    void assignPlayer( int n, Player * player );
    void assignCoach( OnlineCoach * coach );

    void assignPlayerTypes();

    void setName( const char * name )
      {
          M_name = name;
      }
    const
    std::string & name() const
      {
          return M_name;
      }

    bool enabled() const
      {
          return ! M_name.empty();
      }

    Side side() const
      {
          return M_side;
      }

    int size() const
      {
          return M_size;
      }

    const
    Player * player( const int i ) const
      {
          if ( i < 0 || M_size <= i ) return NULL;
          return M_players[i];
      }

    void setPoint( int point )
      {
          M_point = point;
      }
    int incPoint()
      {
          return ++M_point;
      }
    int point() const
      {
          return M_point;
      }

    void setPenaltyScore()
      {
          ++M_pen_point;
          ++M_pen_taken;
      }
    void setPenaltyMiss()
      {
          ++M_pen_taken;
      }
    int penaltyTaken() const
      {
          return M_pen_taken;
      }
    int penaltyPoint() const
      {
          return M_pen_point;
      }

    void setPenaltyWinner()
      {
          M_pen_won = true;
      }
    bool penaltyWon() const
      {
          return M_pen_won;
      }
#ifdef NEW_QSTEP
    const
    double & distQstepTeam() const
      {
          return M_dist_qstep_team;
      }
    const
    double & landQstepTeam() const
      {
          return M_land_qstep_team;
      }
    const
    double & dirQstepTeam() const
      {
          return M_dir_qstep_team;
      }
#endif
    const
    double & prandFactorTeam() const
      {
          return M_prand_factor_team;
      }
    const
    double & kickRandFactorTeam() const
      {
          return M_kick_rand_factor_team;
      }

    int subsCount() const
      {
          return M_subs_count;
      }

    const std::map< int, int > & ptypeCount() const
      {
          return M_ptype_count;
      }

    const std::map< int, int > & ptypeUsedCount() const
      {
          return M_ptype_used_count;
      }

    int ptypeCount( const int player_type ) const;

    int ptypeUsedCount( const int player_type ) const;

    void substitute( const Player * player,
                     const int player_type );

    bool changePlayerToGoalie( const Player * player );

    OnlineCoach * olcoach()
      {
          return M_olcoach;
      }

    const
    OnlineCoach * olcoach() const
      {
          return M_olcoach;
      }

    void addTeamGraphic( const unsigned int x,
                         const unsigned int y,
                         std::auto_ptr< XPMHolder > holder );

    const
    XPMHolder * teamGraphic( const unsigned int x,
                             const unsigned int y ) const;

    const
    GraphCont & teamGraphics() const
      {
          return M_graphics;
      }
};

#endif // _H_OBJECT

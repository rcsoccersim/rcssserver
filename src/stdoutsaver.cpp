// -*-c++-*-

/***************************************************************************
                                stdoutsaver.cpp
                    Class for saving results to a mysql database
                             -------------------
    begin                : 13-MAY-2003
    copyright            : (C) 2003 by The RoboCup Soccer Simulator
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


#include "resultsaver.hpp"

#include <rcssbase/lib/loader.hpp>

#include <iostream>
#include <ctime>

class STDOutSaver
    : public rcss::ResultSaver {
public:
    STDOutSaver()
        : rcss::ResultSaver()
        , M_left_coin( false )
        , M_right_coin( false )
      {
          for ( int i = 0; i < TEAM_RIGHT + 1; ++i )
          {
              M_score[ i ] = 0;
              M_pen_taken[ i ] = 0;
              M_pen_scored[ i ] = 0;
          }
      }

    virtual
    ~STDOutSaver()
      {
          //std::cerr << "delete STDOutSaver" << std::endl;
      }

private:
    virtual
    bool doEnabled() const
      {
          return true;
      }

    virtual
    void doSaveStart()
      {
          std::cout << "\nGame Results:\n";
      }

    virtual
    void doSaveTime( const tm & time )
      {
          M_time = time;
      }

    virtual
    void doSaveTeamName( team_id id,
                         const std::string & name )
      {
          M_team_name[ id ] = name;
      }

    virtual
    void doSaveCoachName( team_id id,
                          const std::string & name )
      {
          M_coach_name[ id ] = name;
      }

    virtual
    void doSaveScore( team_id id,
                      unsigned int score )
      {
          M_score[ id ] = score;
      }

    virtual
    void doSavePenTaken( team_id id,
                         unsigned int taken )
      {
          M_pen_taken[ id ] = taken;
      }

    virtual
    void doSavePenScored( team_id id,
                          unsigned int scored )
      {
          M_pen_scored[ id ] = scored;
      }

    virtual
    void doSaveCoinTossWinner( team_id id )
      {
          switch ( id ) {
          case TEAM_LEFT:
              M_left_coin = true;
              M_right_coin = false;
              break;
          case TEAM_RIGHT:
              M_left_coin = false;
              M_right_coin = true;
              break;
          default:
              M_left_coin = false;
              M_right_coin = false;
              break;
          }
      }

    virtual
    bool doSaveComplete()
      {
          char time_str[256];
          std::strftime( time_str, 256, "%Y-%m-%d %H:%M:%S", &M_time );

          std::cout << "\t" << time_str << "\n\t";

          if ( M_team_name[ TEAM_LEFT ].empty() )
          {
              std::cout << "null vs ";
          }
          else
          {
              std::cout << "'" << M_team_name[ TEAM_LEFT ] << "' vs ";
          }

          if ( M_team_name[ TEAM_RIGHT ].empty() )
          {
              std::cout << "null\n";
          }
          else
          {
              std::cout << "'" << M_team_name[ TEAM_RIGHT ] << "'\n";
          }

          if ( ! M_coach_name[ TEAM_LEFT ].empty() )
          {
              std::cout << "\tLeft Coach: '" << M_coach_name[ TEAM_LEFT ] << "'\n";
          }

          if ( ! M_coach_name[ TEAM_RIGHT ].empty() )
          {
              std::cout << "\tRight Coach: '" << M_coach_name[ TEAM_RIGHT ] << "'\n";
          }

          std::cout << "\tScore: " << M_score[ TEAM_LEFT ] << " - " << M_score[ TEAM_RIGHT ] << "\n ";

          if ( M_pen_taken[ TEAM_LEFT ] || M_pen_taken[ TEAM_RIGHT ] )
          {
              std::cout << "\tPenalties: " << M_pen_scored[ TEAM_LEFT ]
                        << " (" << M_pen_taken[ TEAM_LEFT ] << ") - "
                        << M_pen_scored[ TEAM_RIGHT ]
                        << " (" << M_pen_taken[ TEAM_RIGHT ] << ")\n";

          }

          if ( M_left_coin )
          {
              std::cout << "\tCoin Toss Winner: Left Team\n";
          }
          else if ( M_right_coin )
          {
              std::cout << "\tCoin Toss Winner: Right Team\n";
          }
          std::cout << std::endl;
          return true;
      }

    virtual
    const char * doGetName() const
      {
          return "STDOutSaver";
      }



    tm M_time;
    std::string M_team_name[ 2 ];
    std::string M_coach_name[ 2 ];
    unsigned int M_score[ 2 ];
    unsigned int M_pen_taken[ 2 ];
    unsigned int M_pen_scored[ 2 ];
    bool M_left_coin;
    bool M_right_coin;

public:

    static
    void destroy( STDOutSaver * c )
      {
          delete c;
      }

    static
    Ptr create()
      {
          return rcss::ResultSaver::Ptr( new STDOutSaver(),
                                         &destroy,
                                         rcss::lib::Loader::loadFromCache( "libstdoutsaver" ) );
      }

};


RCSSLIB_INIT( libstdoutsaver )
{
    STDOutSaver::factory().reg( &STDOutSaver::create,
                                RCSS_MODULE_NAME );
    return true;
}

RCSSLIB_FIN( libstdoutsaver )
{
    STDOutSaver::factory().dereg( RCSS_MODULE_NAME );
}

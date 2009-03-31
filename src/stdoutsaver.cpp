// -*-c++-*-

/***************************************************************************
                                stdoutsaver.cpp
                    Class for saving results to the standard output stream
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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stdoutsaver.h"

#include <iostream>
#include <ctime>

const std::string STDOutSaver::NAME = "STDOutSaver";

STDOutSaver::STDOutSaver()
    : ResultSaver(),
      M_left_coin( false ),
      M_right_coin( false )
{
    for ( int i = 0; i < TEAM_RIGHT + 1; ++i )
    {
        M_score[ i ] = 0;
        M_pen_taken[ i ] = 0;
        M_pen_scored[ i ] = 0;
    }
}

STDOutSaver::~STDOutSaver()
{
    //std::cerr << "delete STDOutSaver" << std::endl;
}

bool
STDOutSaver::doEnabled() const
{
    return true;
}

void
STDOutSaver::doSaveStart()
{
    std::cout << "\nGame Results:\n";
}

void
STDOutSaver::doSaveTime( const tm & time )
{
    M_time = time;
}

void
STDOutSaver::doSaveTeamName( team_id id,
                             const std::string & name )
{
    M_team_name[ id ] = name;
}

void
STDOutSaver::doSaveCoachName( team_id id,
                              const std::string & name )
{
    M_coach_name[ id ] = name;
}

void
STDOutSaver::doSaveScore( team_id id,
                          unsigned int score )
{
    M_score[ id ] = score;
}

void
STDOutSaver::doSavePenTaken( team_id id,
                             unsigned int taken )
{
    M_pen_taken[ id ] = taken;
}

void
STDOutSaver::doSavePenScored( team_id id,
                              unsigned int scored )
{
    M_pen_scored[ id ] = scored;
}

void
STDOutSaver::doSaveCoinTossWinner( team_id id )
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

bool
STDOutSaver::doSaveComplete()
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

const char *
STDOutSaver::doGetName() const
{
    //return "STDOutSaver";
    return NAME.c_str();
}


ResultSaver::Ptr
STDOutSaver::create()
{
    return ResultSaver::Ptr( new STDOutSaver() );
}

namespace {
// template< typename Saver >
// ResultSaver::Ptr
// create()
// {
//     return ResultSaver::Ptr( new Saver() );
// }
//rcss::RegHolder s = ResultSaver::factory().autoReg( &create< STDOutSaver >, "STDOutSaver" );

rcss::RegHolder s = ResultSaver::factory().autoReg( &STDOutSaver::create, STDOutSaver::NAME );
}

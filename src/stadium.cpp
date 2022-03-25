// -*-c++-*-

/***************************************************************************
                                 stadium.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stadium.h"

#include "audio.h"
#include "coach.h"
#include "landmarkreader.h"
#include "logger.h"
#include "monitor.h"
#include "object.h"
#include "param.h"
#include "player.h"
#include "heteroplayer.h"
#include "random.h"
#include "referee.h"
#include "resultsaver.hpp"
#include "serverparam.h"
#include "playerparam.h"
#include "team.h"
#include "types.h"
#include "utility.h"
#include "xpmholder.h"

#include <rcss/clang/clangmsg.h>

#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cctype>
#include <cerrno>


Stadium::Stadium()
    : M_alive( true ),
      M_ball( nullptr ),
      M_players( MAX_PLAYER*2, static_cast< Player * >( 0 ) ),
      M_coach( nullptr ),
      M_olcoaches( 2, static_cast< OnlineCoach * >( 0 ) ),
      M_team_l( nullptr ),
      M_team_r( nullptr ),
      M_playmode( PM_BeforeKickOff ),
      M_time( 0 ),
      M_stoppage_time( 0 ),
      M_ball_catcher( static_cast< const Player * >( 0 ) ),
      M_kick_off_side( LEFT ),
      M_last_playon_start( 0 ),
      M_game_over_wait( 0 ),
      M_left_child( 0 ),
      M_right_child( 0 )
{
    // !!! registration order is very important !!!
    // TODO: fix dependencies among referees.
    M_referees.push_back( new TimeRef( *this ) );
    M_referees.push_back( new BallStuckRef( *this ) );
    M_referees.push_back( new OffsideRef( *this ) );
    M_referees.push_back( new FreeKickRef( *this ) );
    M_referees.push_back( new TouchRef( *this ) );
    M_referees.push_back( new CatchRef( *this ) );
    M_referees.push_back( new FoulRef( *this ) );
    M_referees.push_back( new IllegalDefenseRef( *this ) );
    M_referees.push_back( new KeepawayRef( *this ) );
    M_referees.push_back( new PenaltyRef( *this ) );

    M_movable_objects.reserve( MAX_PLAYER*2 + 1 );
}

Stadium::~Stadium()
{
    M_savers.clear();

    for ( std::list< Referee * >::iterator i = M_referees.begin();
          i != M_referees.end();
          ++i )
    {
        if ( *i )
        {
            delete *i;
            *i = static_cast< Referee * >( 0 );
        }
    }
    M_referees.clear();

    for ( MonitorCont::iterator i = M_monitors.begin();
          i != M_monitors.end();
          ++i )
    {
        //if ( !(*i)->connected() )
        if ( *i )
        {
            delete *i;
            *i = static_cast< Monitor * >( 0 );
        }
    }
    M_monitors.clear();

    for ( OnlineCoachCont::iterator it = M_olcoaches.begin();
          it != M_olcoaches.end();
          ++it )
    {
        if ( *it )
        {
            delete *it;
            *it = static_cast< OnlineCoach * >( 0 );
        }
    }
    M_olcoaches.clear();

    for ( PlayerCont::iterator it = M_players.begin();
          it != M_players.end();
          ++it )
    {
        if ( *it )
        {
            delete *it;
            *it = static_cast< Player * >( 0 );
        }
    }
    M_players.clear();

    for ( std::vector< HeteroPlayer * >::iterator it = M_player_types.begin();
          it != M_player_types.end();
          ++it )
    {
        if ( *it )
        {
            delete *it;
        }
    }
    M_player_types.clear();

    delete M_team_l; M_team_l = nullptr;
    delete M_team_r; M_team_r = nullptr;

    delete M_coach; M_coach = nullptr;
    delete M_ball; M_ball = nullptr;
}



/*
 *===================================================================
 *Part: Create Stadium Window
 *===================================================================
 */
bool
Stadium::init()
{
    M_start_time = std::time( 0 );

    if ( ServerParam::instance().randomSeed() >= 0 )
    {
        int seed = ServerParam::instance().randomSeed();
        std::cout << "Using given Simulator Random Seed: " << seed << std::endl;
        srand( seed );
        srandom( seed );
        DefaultRNG::seed( seed );
    }
    else
    {
        int seed = static_cast< int >( M_start_time );
        std::cout << "Simulator Random Seed: " << seed << std::endl;
        ServerParam::instance().setRandomSeed( seed );
        srand( seed );
        srandom( seed );
        DefaultRNG::seed( seed );
    }

    //std::cout << "Simulator Random Seed: " << ServerParam::instance().randomSeed() << std::endl;

    M_game_over_wait = ServerParam::instance().gameOverWait();

    // we create the result savers now, so that if there are any
    // errors creating them, it will be reported before
    // the game starts, not after it has finished.
    std::list< ResultSaver::FactoryHolder::Index > savers = ResultSaver::factory().list();
    for ( const auto & idx : savers )
    {
        ResultSaver::Creator creator;
        if ( ResultSaver::factory().getCreator( creator, idx ) )
        {
            ResultSaver::Ptr saver = creator();
            std::cout << saver->getName() << ": Ready\n";
            M_savers.push_back( saver );
        }
        else
        {
            std::cerr << idx << ": error loading" << std::endl;
        }
    }

    if ( ServerParam::instance().coachMode()
         && ! ServerParam::instance().coachWithRefereeMode() )
    {
        for ( std::list< Referee * >::iterator i = M_referees.begin();
              i != M_referees.end();
              ++i )
        {
            delete *i;
        }
        M_referees.clear();
    }

    {
        LandmarkReader * reader = new LandmarkReader( M_field, ServerParam::instance().landmarkFile() );
        if ( ! reader )
        {
            perror( "Can not create landmark reader" );
            disable();
            return false;
        }
        delete reader;
    }

    M_player_types.push_back( new HeteroPlayer( 0 ) );
    //std::cout << *(M_player_types[ 0 ]) << std::endl;
    for ( int i = 1; i < PlayerParam::instance().playerTypes(); i++ )
    {
        M_player_types.push_back( new HeteroPlayer() );
        //std::cout << *(M_player_types[i]) << std::endl;
    }

    if ( ! M_player_socket.bind( rcss::net::Addr( ServerParam::instance().playerPort() )  ) )
    {
        std::cerr << "Error initializing sockets: port=" << ServerParam::instance().playerPort()
                  << ". " << strerror( errno ) << std::endl;
        disable();
        return false;
    }

    if ( ! M_offline_coach_socket.bind( rcss::net::Addr( ServerParam::instance().offlineCoachPort() ) ) )
    {
        std::cerr << "Error initializing sockets: port=" << ServerParam::instance().offlineCoachPort()
                  << ". " << strerror( errno ) << std::endl;
        disable();
        return false;
    }

    if ( ! M_online_coach_socket.bind( rcss::net::Addr( ServerParam::instance().onlineCoachPort() ) ) )
    {
        std::cerr << "Error initializing sockets: port=" << ServerParam::instance().onlineCoachPort()
                  << ". " << strerror( errno ) << std::endl;
        disable();
        return false;
    }

    if ( M_player_socket.setNonBlocking() == -1
         || M_offline_coach_socket.setNonBlocking() == -1
         || M_online_coach_socket.setNonBlocking() == -1 )
    {
        std::cerr << "Error setting sockets non-blocking: "
                  << strerror( errno ) << std::endl;
        disable();
        return false;
    }

    M_weather.init();

    createObjects();

    changePlayMode( PM_BeforeKickOff );

    M_kick_off_wait = std::max( 0, ServerParam::instance().kickOffWait() );
    M_connect_wait = std::max( 0, ServerParam::instance().connectWait() );


    if ( ! Logger::instance().open( *this ) )
    {
        disable();
        return false;
    }

    return true;
}

void
Stadium::checkAutoMode()
{
    if ( ! ServerParam::instance().autoMode() )
    {
        return;
    }

    //
    // check kick off for the auto mode
    //
    if ( playmode() == PM_BeforeKickOff
         && time() < ( ( ServerParam::instance().halfTime()
                         * ServerParam::instance().nrNormalHalfs() )
                       + ( ServerParam::instance().extraHalfTime()
                           * ServerParam::instance().nrExtraHalfs() ) )
         )
    {
        if ( M_remote_players.size() == MAX_PLAYER*2 || time() > 0 )
        {
            if ( M_kick_off_wait == ServerParam::instance().kickOffWait() )
            {
                std::cout << "Waiting to kick off\n";
            }

            if ( M_kick_off_wait > 0 )
            {
                --M_kick_off_wait;
            }
        }
        else
        {
            if ( M_connect_wait == ServerParam::instance().connectWait() )
            {
                std::cout << "Waiting for players to connect\n";
            }

            if ( M_connect_wait > 0 )
            {
                --M_connect_wait;
            }
        }

        if ( M_kick_off_wait <= 0 || M_connect_wait <= 0 )
        {
            kickOff();
        }
    }
    else
    {
        M_kick_off_wait = std::max( 0, ServerParam::instance().kickOffWait() );
        M_connect_wait = std::max( 0, ServerParam::instance().connectWait() );
    }

    //
    // check time over for the auto mode
    //
    if ( ServerParam::instance().autoMode() )
    {
        if ( playmode() == PM_TimeOver )
        {
            if( M_game_over_wait == ServerParam::instance().gameOverWait() )
            {
                std::cout << "Waiting after end of match\n";
            }

            if ( M_game_over_wait > 0 )
            {
                --M_game_over_wait;
            }

            if ( M_game_over_wait <= 0 )
            {
                finalize( "Game Over. Exiting..." );
                return;
            }
        }
        else
        {
            M_game_over_wait = ServerParam::instance().gameOverWait();
        }
    }

}

void
Stadium::startTeams()
{
    if ( playmode() != PM_PlayOn )
    {
        if ( M_left_child == 0
             && ! ServerParam::instance().teamLeftStart().empty() )
        {
            M_left_child = startTeam( ServerParam::instance().teamLeftStart() );
        }

        if ( M_right_child == 0
             && ! ServerParam::instance().teamRightStart().empty()
             && teamConnected( LEFT ) )
        {
            // the right team cannot connect
            // until the left team has
            M_right_child = startTeam( ServerParam::instance().teamRightStart() );
        }
    }
}


int
Stadium::startTeam( const std::string & start )
{
    if ( start.empty() )
    {
        return 0;
    }

    std::cout << "Starting \"/bin/sh -c "
              << start.c_str() << "\"\n";

    int pid = fork();
    if ( pid == -1 )
    {
        std::cerr << PACKAGE << "-" << VERSION
                  << ": Error: Could not fork to start team: "
                  << strerror( errno ) << std::endl;
        //this->exit( EXIT_FAILURE );
        disable();
        return 0;
    }

    if ( pid == 0 )
    {
        execlp( "/bin/sh", "sh", "-c", start.c_str(), (char *)nullptr );
        std::cerr << PACKAGE << "-" << VERSION
                  << ": Error: Could not execute \"/bin/sh -c "
                  << start.c_str() << "\": "
                  << strerror( errno ) << std::endl;
        //this->exit( EXIT_FAILURE );
        disable();
        return 0;
    }

    return pid;
}

bool
Stadium::teamConnected( const Side side )
{
    if ( side == LEFT )
    {
        return M_team_l->enabled();
    }
    else if ( side == RIGHT )
    {
        return M_team_r->enabled();
    }
    else
    {
        return M_team_l->enabled() && M_team_r->enabled();
    }
}

void
Stadium::killTeams()
{
    if ( M_left_child > 0 )
    {
        kill( M_left_child, SIGINT );
        std::cout << "Killing " << M_left_child << std::endl;
        sleep( 1 );
    }

    if ( M_right_child > 0 )
    {
        kill( M_right_child, SIGINT );
        std::cout << "Killing " << M_right_child << std::endl;
        sleep( 1 );
    }
}

void
Stadium::removeListener( const rcss::Listener * listener )
{
    M_listeners.erase( std::remove( M_listeners.begin(),
                                    M_listeners.end(),
                                    listener ),
                       M_listeners.end() );
}

const
HeteroPlayer *
Stadium::playerType( int id ) const
{
    try
    {
        return M_player_types.at( id );
    }
    catch ( std::exception & e )
    {
        std::cerr << __FILE__ << ':' << __LINE__
                  << " Exception caught! " << e.what()
                  << std::endl;
        return nullptr;
    }
}

void
Stadium::createObjects()
{
    M_ball = new Ball( *this );

    //M_motable.append( M_ball );
    M_movable_objects.push_back( M_ball );

    M_ball->moveTo( PVector( 0.0, 0.0 ),
                    PVector( 0.0, 0.0 ), // vel
                    PVector( 0.0, 0.0 ) // accel
                    );

    M_ball->setConstant( ServerParam::instance().ballSize(),
                         ServerParam::instance().ballDecay(),
                         ServerParam::instance().ballRand(),
                         ServerParam::instance().ballWeight(),
                         ServerParam::instance().ballSpeedMax(),
                         ServerParam::instance().ballAccelMax() );

    M_team_l = new Team( this, LEFT );
    M_team_r = new Team( this, RIGHT );

    M_team_l->setPoint( ServerParam::instance().startGoalLeft() );
    M_team_r->setPoint( ServerParam::instance().startGoalRight() );

    for ( int i = 0; i < MAX_PLAYER; ++i )
    {
        Player * p_l = new Player( *this, M_team_l, i+1 );
        Player * p_r = new Player( *this, M_team_r, i+1 );
        M_team_l->assignPlayer( i, p_l );
        M_team_r->assignPlayer( i, p_r );
        M_players[i] = p_l;
        M_players[i+MAX_PLAYER] = p_r;
    }

    M_shuffle_players = M_players;

    M_coach = new Coach( *this );

    M_olcoaches[0] = new OnlineCoach( *this, *M_team_l );
    M_team_l->assignCoach( M_olcoaches[0] );

    M_olcoaches[1] = new OnlineCoach( *this, *M_team_r );
    M_team_r->assignCoach( M_olcoaches[1] );
}


Player *
Stadium::initPlayer( const char * teamname,
                     const double & version,
                     const bool goalie,
                     const rcss::net::Addr & addr )
{
    Team * team = static_cast< Team * >( 0 );

    if ( ServerParam::instance().fixedTeamNameLeft() == teamname
         || ServerParam::instance().fixedTeamNameRight() == teamname )
    {
        sendToPlayer( "(error conflict_with_fixed_teamname)", addr );
        return static_cast< Player * >( 0 );
    }

    if ( M_team_l->name().empty() )
    {
        team = M_team_l;
        team->setName( teamname );
    }
    else if ( M_team_l->name() == teamname )
    {
        team = M_team_l;
    }
    else if ( M_team_r->name().empty() )
    {
        team = M_team_r;
        team->setName( teamname );
    }
    else if ( M_team_r->name() == teamname )
    {
        team = M_team_r;
    }
    else
    {
        if ( ServerParam::instance().verboseMode() )
        {
            std::cerr << "Warning:Too many teams. [teamname = '"
                      << teamname << "']" << std::endl;
        }
        sendToPlayer( "(error no_more_team)", addr );
        return static_cast< Player * >( 0 );
    }

    if ( ! team )
    {
        return static_cast< Player * >( 0 );
    }

    Player * player = team->newPlayer( version, goalie );

    if ( ! player )
    {
        sendToPlayer( "(error no_more_player_or_goalie_or_illegal_client_version)", addr );
        return static_cast< Player * >( 0 );
    }

    if ( ! player->connect( addr ) )
    {
        sendToPlayer( "(error connection_failed)", addr );
        player->disable();
        return static_cast< Player * >( 0 );
    }

    addListener( player );
    M_remote_players.push_back( player );
    M_movable_objects.push_back( player );

    player->setEnforceDedicatedPort( version >= 8.0 );
    player->sendInit();

    return player;
}


Player *
Stadium::reconnectPlayer( const char * teamname,
                          const int unum,
                          const rcss::net::Addr & addr )

{
    int r = 0;
    for ( r = 0; r < MAX_PLAYER * 2; ++r )
    {
        if ( M_players[r]->team()->enabled()
             && M_players[r]->team()->name() == teamname
             && M_players[r]->unum() == unum )
        {
            /* It's same. */
            if ( M_players[r]->hasRedCard() )
            {
                sendToPlayer( "(error red_carded_player)", addr );
                return static_cast< Player * >( 0 );
            }

            if ( M_players[r]->open() != 0 )
            {
                sendToPlayer( "(error socket_open_failed)", addr );
                return static_cast< Player * >( 0 );
            }
            if ( ! M_players[r]->connect( addr ) )
            {
                sendToPlayer( "(error connection_failed)", addr );
                return static_cast< Player * >( 0 );
            }
            if ( ! M_players[r]->setSenders() )
            {
                sendToPlayer( "(error illegal_client_version)", addr );
                return static_cast< Player * >( 0 );
            }

            addListener( M_players[r] );
            M_remote_players.push_back( M_players[r] );

            M_players[r]->setEnforceDedicatedPort( M_players[r]->version() >= 8.0 );
            M_players[r]->setEnable();
            M_players[r]->sendReconnect();
            return M_players[r];
        }
    }

    sendToPlayer( "(error no_such_team_or_player)", addr );
    return static_cast< Player * >( 0 );
}

Coach *
Stadium::initCoach( const double & version,
                    const rcss::net::Addr & addr )
{
    if ( M_coach->open() != 0 )
    {
        sendToCoach( "(error socket_open_failed)", addr );
        return nullptr;
    }

    if ( ! M_coach->connect( addr ) )
    {
        sendToCoach( "(error connection_failed)", addr );
        return nullptr;
    }

    if ( ! M_coach->setSenders( version ) )
    {
        std::cerr << "Error: Could not find serializer or sender for version"
                  << version << std::endl;
        sendToCoach( "(error illegal_client_version)", addr );
        return nullptr;
    }

    addOfflineCoach( M_coach );
    addListener( M_coach );
    M_coach->setEnforceDedicatedPort( version >= 8.0 );
    M_coach->sendInit();

    return M_coach;
}


OnlineCoach *
Stadium::initOnlineCoach( const char * teamname,
                          const char * coachname,
                          const double & version,
                          const rcss::net::Addr & addr )
{

    OnlineCoach * olc = static_cast< OnlineCoach * >( 0 );

    if ( ! M_team_l->name().empty()
         && M_team_l->name() == teamname )
    {
        olc = M_team_l->olcoach();
    }
    else if ( ! M_team_r->name().empty()
              && M_team_r->name() == teamname )
    {
        olc = M_team_r->olcoach();
    }
    else
    {
        sendToOnlineCoach( "(error no_such_team)", addr );
        return static_cast< OnlineCoach * >( 0 );
    }

    if ( olc->assigned() )
    {
        sendToOnlineCoach( "(error already_have_coach)", addr );
        return static_cast< OnlineCoach * >( 0 );
    }

    if ( ! olc->connect( addr ) )
    {
        sendToOnlineCoach( "(error connection_failed)", addr );
        return static_cast< OnlineCoach * >( 0 );
    }

    if ( ! olc->setSenders( version ) )
    {
        std::cerr << "Error: Could not find serializer or sender for version "
                  << (int)olc->version() << std::endl;
        sendToOnlineCoach( "(error illegal_client_version)", addr );
        return static_cast< OnlineCoach * >( 0 );
    }

    olc->setEnforceDedicatedPort( version >= 8.0 );

    addListener( olc );

    M_remote_online_coaches.push_back( olc );

    olc->sendInit();

    if ( olc->version() >= 8.0 )
    {
        olc->sendPlayerClangVer();
    }

    if ( olc->version() >= 11.0 )
    {
        olc->sendExternalMsg();
    }

    olc->setName( coachname );

    return olc;
}


void
Stadium::step()
{
    //
    // reset command flags
    //
    for ( PlayerCont::reference p : M_players )
    {
        p->resetCommandFlags();
        p->incArmAge();
    }

    for ( int i = 0; i < 2; ++i )
    {
        M_olcoaches[i]->resetCommandFlags();
        M_olcoaches[i]->check_message_queue( time() );
        M_olcoaches[i]->update_messages_left( time() );
    }

    M_coach->resetCommandFlags();

    //
    // update objects & referees analyze state
    //
    if ( playmode() == PM_BeforeKickOff )
    {
        turnMovableObjects();
        ++M_stoppage_time;
        //for_each( M_referees.begin(), M_referees.end(), &Referee::doAnalyse );
        for_each( M_referees.begin(), M_referees.end(), []( Referee * ref ) { ref->analyse(); } );
    }
    else if ( playmode() == PM_AfterGoal_Right
              || playmode() == PM_AfterGoal_Left
              || playmode() == PM_OffSide_Right
              || playmode() == PM_OffSide_Left
              || playmode() == PM_Illegal_Defense_Left
              || playmode() == PM_Illegal_Defense_Right
              || playmode() == PM_Foul_Charge_Right
              || playmode() == PM_Foul_Charge_Left
              || playmode() == PM_Foul_Push_Right
              || playmode() == PM_Foul_Push_Left
              || playmode() == PM_Back_Pass_Right
              || playmode() == PM_Back_Pass_Left
              || playmode() == PM_Free_Kick_Fault_Right
              || playmode() == PM_Free_Kick_Fault_Left
              || playmode() == PM_CatchFault_Right
              || playmode() == PM_CatchFault_Left )
    {
        PlayMode pm = playmode();
        clearBallCatcher();
        incMovableObjects();
        ++M_stoppage_time;
        //for_each( M_referees.begin(), M_referees.end(), &Referee::doAnalyse );
        for_each( M_referees.begin(), M_referees.end(), []( Referee * ref ) { ref->analyse(); } );
        if ( pm != playmode() )
        {
            ++M_time;
            M_stoppage_time = 0;
        }
    }
    else if ( playmode() != PM_BeforeKickOff && playmode() != PM_TimeOver )
    {
        incMovableObjects();
        ++M_time;
        M_stoppage_time = 0;
        //for_each( M_referees.begin(), M_referees.end(), &Referee::doAnalyse );
        //for_each( M_referees.begin(), M_referees.end(), std::mem_fun( &Referee::analyse ) );
        for_each( M_referees.begin(), M_referees.end(), []( Referee * ref ) { ref->analyse(); } );
    }
    else if ( playmode() == PM_TimeOver )
    {
        ++M_stoppage_time;
    }

    //
    // update stamina etc
    //
    for ( PlayerCont::reference p : M_players )
    {
        if ( ! p->isEnabled() ) continue;

        p->updateStamina();
        p->updateCapacity();
    }

    if ( stoppageTime() == 0
         && time() > 0
         && ServerParam::instance().nrNormalHalfs() > 0
         && time() % ( ServerParam::instance().nrNormalHalfs()
                       * ServerParam::instance().halfTime() ) == 0 )
    {
        //every game time cycles, give the online coach more messages
        M_olcoaches[0]->awardFreeformMessageCount();
        M_olcoaches[1]->awardFreeformMessageCount();
    }

    //
    // send to monitors and write game log
    //
    sendDisp();

    //
    // reset player state
    //
    for ( PlayerCont::reference p : M_players )
    {
        p->resetState();
    }
}

void
Stadium::turnMovableObjects()
{
    std::shuffle( M_movable_objects.begin(), M_movable_objects.end(),
                  DefaultRNG::instance() );
    for ( MPObjectCont::reference o : M_movable_objects )
    {
        o->_turn();
    }
}

void
Stadium::incMovableObjects()
{
    std::shuffle( M_movable_objects.begin(), M_movable_objects.end(),
                  DefaultRNG::instance() );
    for ( MPObjectCont::reference o : M_movable_objects )
    {
        if ( o->isEnabled() )
        {
            o->_inc();
        }
    }

    collisions();

    // move_caughat_ball() [2000.07.21: I.Noda]
    if ( M_ball_catcher )
    {
        // keeps the caught ball infront of the player
        PVector rpos = PVector::fromPolar( M_ball_catcher->size()
                                           + ServerParam::instance().ballSize(),
                                           M_ball_catcher->angleBodyCommitted() );
        M_ball->moveTo( M_ball_catcher->pos() + rpos );
    }
}


void
Stadium::sendDisp()
{
    const std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();

    // send to displays
    for ( MonitorCont::reference m : M_monitors )
    {
        m->sendShow();
    }

    // record game log
    Logger::instance().writeGameLog( *this );
    Logger::instance().flush();

    const std::chrono::system_clock::time_point end_time = std::chrono::system_clock::now();
    Logger::instance().writeProfile( *this, start_time, end_time, "DISP" );
}


void
Stadium::recoveryPlayers()
{
    for ( PlayerCont::reference p : M_players )
    {
        if ( ! p->isEnabled() ) continue;

        p->recoverAll();
    }
}


BallPosInfo
Stadium::ballPosInfo()
{
    static RArea g_l( PVector( ( - ServerParam::PITCH_LENGTH
                                 - ServerParam::GOAL_DEPTH )*0.5
                               - ServerParam::instance().ballSize(),
                               0.0 ),
                      PVector( ServerParam::GOAL_DEPTH ,
                               ServerParam::instance().goalWidth()
                               + ServerParam::instance().goalPostRadius() ) );
    static RArea g_r( PVector( ( + ServerParam::PITCH_LENGTH
                                 + ServerParam::GOAL_DEPTH )*0.5
                               + ServerParam::instance().ballSize(),
                               0.0 ),
                      PVector( ServerParam::GOAL_DEPTH,
                               ServerParam::instance().goalWidth()
                               + ServerParam::instance().goalPostRadius() ) );

    static RArea pt( PVector( 0.0, 0.0 ),
                     PVector( ServerParam::PITCH_LENGTH
                              + ServerParam::instance().ballSize() * 2 ,
                              ServerParam::PITCH_WIDTH
                              + ServerParam::instance().ballSize() * 2) );

    if ( g_l.inArea( M_ball->pos() ) ) return BPI_GoalL;
    if ( g_r.inArea( M_ball->pos() ) ) return BPI_GoalR;
    if ( ! pt.inArea( M_ball->pos() ) ) return BPI_OutOfField;
    return BPI_InField;
}

void
Stadium::placePlayersInField()
{
    static const RArea fld( PVector( 0.0, 0.0 ),
                            PVector( ServerParam::PITCH_LENGTH
                                     + ServerParam::PITCH_MARGIN*2.0,
                                     ServerParam::PITCH_WIDTH
                                     + ServerParam::PITCH_MARGIN*2.0 ) );

    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        if ( ! M_players[i]->isEnabled() ) continue;

        if ( ! fld.inArea( M_players[i]->pos() ) )
        {
            M_players[i]->moveTo( fld.nearestEdge( M_players[i]->pos() ) );
        }
    }
}


Player *
Stadium::getPlayer( const Side side,
                    const int unum )
{
//     for ( int i = 0; i < MAX_PLAYER*2; ++i )
//     {
//         if ( M_players[i]->side() == side
//              && M_players[i]->unum() == unum )
//         {
//             return M_players[i];
//         }
//     }

    if ( side == LEFT )
    {
        return M_players[unum - 1];
    }
    else if ( side == RIGHT )
    {
        return M_players[MAX_PLAYER + unum - 1];
    }

    return static_cast< Player * >( 0 );
}


void
Stadium::kickOff()
{
    const ServerParam & SP = ServerParam::instance();

    if ( SP.keepAwayMode() )
    {
        changePlayMode( PM_PlayOn );
        return;
    }

    int normal_time = SP.halfTime() * SP.nrNormalHalfs();

    if ( SP.halfTime() <= 0
         || ( time() <= normal_time
              && time() % SP.halfTime() == 0 )
         || ( time() > normal_time
              && SP.extraHalfTime() > 0
              && ( time() - normal_time ) % SP.extraHalfTime() == 0 )
         )
    {
        int current_time = time();
        int half_time = SP.halfTime();
        if ( current_time > normal_time )
        {
            current_time -= normal_time;
            half_time = SP.extraHalfTime();
        }

        if ( half_time <= 0
             || ( current_time / half_time ) % 2 == 0 )
        {
            if ( SP.nrNormalHalfs() >= 0
                 && time() < SP.halfTime() * SP.nrNormalHalfs() )
            {
                recoveryPlayers();
            }

            placeBall( LEFT, PVector( 0.0, 0.0 ) );

            if ( time() == 0 )
            {
                assignPlayerTypes();
            }

            changePlayMode( PM_KickOff_Left );
            std::cout << "Kick_off_left" << std::endl;
        }
        else // if ( ( time / half_time ) % 2 == 1 )
        {
            if ( SP.nrNormalHalfs() >= 0
                 && time() < SP.halfTime() * SP.nrNormalHalfs() )
            {
                recoveryPlayers();
            }

            placeBall( RIGHT, PVector( 0.0, 0.0 ) );
            changePlayMode( PM_KickOff_Right );
            std::cout << "Kick_off_right" << std::endl;
        }
    }
}


void
Stadium::callHalfTime( const Side kick_off_side,
                       const int half_time_count )
{
    //std::cerr << time() << ": callHalfTime  count=" << half_time_count
    //          << std::endl;

    M_ball_catcher = static_cast< const Player * >( 0 );
    placeBall( kick_off_side, PVector( 0.0, 0.0 ) );
    changePlayMode( PM_BeforeKickOff );

    if ( half_time_count < ServerParam::instance().nrNormalHalfs() )
    {
        M_time = ServerParam::instance().halfTime() * half_time_count;
    }
    else
    {
        int extra_count = half_time_count - ServerParam::instance().nrNormalHalfs();
        M_time
            = ServerParam::instance().halfTime() * ServerParam::instance().nrNormalHalfs()
            + ServerParam::instance().extraHalfTime() * extra_count;
    }

    // recover only stamina capacity at the start of extra halves
    if ( half_time_count == ServerParam::instance().nrNormalHalfs() )
    {
        for ( PlayerCont::reference p : M_players )
        {
            if ( ! p->isEnabled() ) continue;

            p->recoverStaminaCapacity();
        }
    }

    M_weather.halfTime();
}


void
Stadium::callFoul( const Side side,
                   PVector pos )
{
    if ( Referee::isPenaltyShootOut( playmode() ) )
    {
        return;
    }

    char msg[8];

    snprintf( msg, 8, "foul_%s", SideStr( -side ) );
    sendRefereeAudio( msg );
    M_ball_catcher = static_cast< const Player * >( 0 );

    pos = Referee::truncateToPitch( pos );

    if( side == LEFT )
    {
        placeBall( PM_FreeKick_Left, LEFT, pos );
    }
    else
    {
        placeBall( PM_FreeKick_Right, RIGHT, pos );
    }

    placePlayersInField();
    M_ball_catcher = static_cast< const Player * >( 0 );
}


void
Stadium::dropBall( PVector pos )
{
    M_ball_catcher = static_cast< const Player * >( 0 );

    pos = Referee::truncateToPitch( pos );

    placeBall( PM_Drop_Ball, NEUTRAL, pos );
    placePlayersInField();

    if ( ! Referee::isPenaltyShootOut( playmode() ) )
    {
        changePlayMode( PM_PlayOn );
    }
}

void
Stadium::discardPlayer( const Side side,
                        const int unum )
{
    Player * p = getPlayer( side, unum );
    if ( ! p
         || ! p->isEnabled() )
    {
        return;
    }

    p->discard();
}


void
Stadium::punishFoulPlay( const Side side,
                         const int unum )
{
    Player * p = getPlayer( side, unum );
    if ( ! p
         || ! p->isEnabled() )
    {
        return;
    }

    p->incFoulCount();
}


void
Stadium::yellowCard( const Side side,
                     const int unum )
{
    Player * p = getPlayer( side, unum );;

    if ( ! p
         || ! p->isEnabled() )
    {
        return;
    }

    char msg[32];
    snprintf( msg, 32, "yellow_card_%s_%d",
              SideStr( p->side() ),
              p->unum() );
    sendRefereeAudio( msg );

    if ( p->hasYellowCard() )
    {
        snprintf( msg, 32, "red_card_%s_%d",
                  SideStr( p->side() ),
                  p->unum() );
        sendRefereeAudio( msg );
    }

    p->yellowCard();
}


void
Stadium::redCard( const Side side,
                  const int unum )
{
    Player * p = getPlayer( side, unum );;

    if ( ! p
         || ! p->isEnabled() )
    {
        return;
    }

    char msg[32];
    snprintf( msg, 32, "red_card_%s_%d",
              SideStr( p->side() ),
              p->unum() );
    sendRefereeAudio( msg );

    p->redCard();
}


void
Stadium::score( const Side side )
{
    if ( side == LEFT
         && M_team_l->enabled() )
    {
        M_team_l->incPoint();
    }

    if ( side == RIGHT
         && M_team_r->enabled() )
    {
        M_team_r->incPoint();
    }
}


void
Stadium::penaltyScore( const Side side,
                       const bool scored )
{
    if ( side == LEFT
         && M_team_l->enabled() )
    {
        if ( scored )
        {
            M_team_l->setPenaltyScore();
        }
        else
        {
            M_team_l->setPenaltyMiss();
        }
    }

    if ( side == RIGHT
         && M_team_r->enabled() )
    {
        if ( scored )
        {
            M_team_r->setPenaltyScore();
        }
        else
        {
            M_team_r->setPenaltyMiss();
        }
    }

}


void
Stadium::penaltyWinner( const Side side )
{
    if ( side == LEFT )
    {
        M_team_l->setPenaltyWinner();
    }

    if ( side == RIGHT )
    {
        M_team_r->setPenaltyWinner();
    }
}


void
Stadium::placeBall( const Side kick_off_side,
                    const PVector & pos )
{
    M_ball->moveTo( pos,
                    PVector( 0.0, 0.0 ),
                    PVector( 0.0, 0.0 ) );
    M_kick_off_side = kick_off_side;
}


void
Stadium::placeBall( const PlayMode pm,
                    const Side kick_off_side,
                    const PVector & pos )
{
    placeBall( kick_off_side, pos );

    if ( Referee::isPenaltyShootOut( playmode() )
         && ( pm == PM_PlayOn || pm ==  PM_Drop_Ball ) )
    {
        ; // never change pm to play_on in penalty mode
    }
    else
    {
        changePlayMode( pm );
    }
}


void
Stadium::moveBall( const PVector & pos,
                   const PVector & vel )
{
    M_ball->moveTo( pos,
                    vel,
                    PVector( 0.0, 0.0 ) );
}


bool
Stadium::movePlayer( const Side side,
                     const int unum,
                     const PVector & pos,
                     const double * ang,
                     const PVector * vel )
{
    if ( unum < 1 || MAX_PLAYER < unum )
    {
        if ( ServerParam::instance().verboseMode() )
            std::cerr << "No such player. " << SideStr( side )
                      << " " << unum << std::endl;
        return false;
    }

    Player * player = getPlayer( side, unum );
    if ( ! player )
    {
        return false;
    }

    double new_angle = player->angleBodyCommitted();
    PVector new_vel = player->vel();
    PVector new_accel = player->accel();

    if ( vel )
    {
        new_vel = *vel;
    }

    if ( ang )
    {
        new_angle = *ang;
    }

    player->place( pos,
                   new_angle,
                   new_vel,
                   new_accel );
    collisions();
    return true;
}


void
Stadium::changePlayMode( const PlayMode pm )
{
    static const char * playmode_strings[] = PLAYMODE_STRINGS;

    M_playmode = pm;

    for_each( M_referees.begin(), M_referees.end(),
              //Referee::doPlayModeChange( pm ) );
              [&]( Referee * ref )
              {
                  ref->playModeChange( pm );
              } );

    if ( pm == PM_KickOff_Left
         || pm == PM_KickIn_Left
         || pm == PM_FreeKick_Left
         || pm == PM_IndFreeKick_Left
         || pm == PM_CornerKick_Left
         || pm == PM_GoalKick_Left
         || pm == PM_Illegal_Defense_Right )
    {
        M_kick_off_side = LEFT;
    }
    else if ( pm == PM_KickOff_Right
              || pm == PM_KickIn_Right
              || pm == PM_FreeKick_Right
              || pm == PM_IndFreeKick_Right
              || pm == PM_CornerKick_Right
              || pm == PM_GoalKick_Right
              || pm == PM_Illegal_Defense_Left )
    {
        M_kick_off_side = RIGHT;
    }
    else if ( pm == PM_Drop_Ball )
    {
        M_kick_off_side = NEUTRAL;
    }

    if ( pm == PM_PlayOn )
    {
        M_last_playon_start = time();
    }

    if ( pm != PM_AfterGoal_Left
         && pm != PM_AfterGoal_Right )
    {
        sendRefereeAudio( playmode_strings[pm] );
    }
}

void
Stadium::assignPlayerTypes()
{
    if ( M_team_l->enabled() )
    {
        M_team_l->assignPlayerTypes();
    }

    if ( M_team_r->enabled() )
    {
        M_team_r->assignPlayerTypes();
    }
}

void
Stadium::substitute( const Player * player,
                     const int player_type_id )
{
    if ( player->side() == LEFT )
    {
        M_team_l->substitute( player, player_type_id );
        broadcastSubstitution( LEFT, player->unum(), player_type_id );
    }
    else if ( player->side() == RIGHT )
    {
        M_team_r->substitute( player, player_type_id );
        broadcastSubstitution( RIGHT, player->unum(), player_type_id );
    }
}

void
Stadium::broadcastSubstitution( const int side,
                                const int unum,
                                const int player_type )
{
    char allies[64];
    char enemies[64];
    snprintf( allies, 64, "(change_player_type %d %d)", unum, player_type );
    snprintf( enemies, 64, "(change_player_type %d)", unum );

    // tell players
    for ( int i = 0 ; i < MAX_PLAYER * 2; ++i )
    {
        if ( ! M_players[i]->isEnabled() ) continue;
        if ( M_players[i]->version() < 7.0 ) continue;

        if ( ( side == LEFT && M_players[i]->team() == M_team_l )
             || ( side == RIGHT && M_players[i]->team() == M_team_r ) )
        {
            M_players[i]->send( allies );
        }
        else
        {
            M_players[i]->send( enemies );
        }
    }

    // tell coaches
    if ( M_olcoaches[1]
         && M_olcoaches[1]->assigned()
         && M_olcoaches[1]->version() >= 7.0 )
    {
        if ( side == RIGHT )
        {
            //team_r->olcoach()->send( allies );
            M_olcoaches[1]->send( allies );
        }
        if ( side == LEFT )
        {
            //team_r->olcoach()->send( enemies );
            M_olcoaches[1]->send( enemies );
        }
    }

    if ( M_olcoaches[0]
         && M_olcoaches[0]->assigned()
         && M_olcoaches[0]->version() >= 7.0 )
    {
        if ( side == LEFT )
        {
            //team_l->olcoach()->send( allies );
            M_olcoaches[0]->send( allies );
        }
        if ( side == RIGHT )
        {
            //team_l->olcoach()->send( enemies );
            M_olcoaches[0]->send( enemies );
        }
    }

    // TODO: send to offline coach

    char buffer[64];
    snprintf( buffer, 64,
              "(change_player_type %s %d %d)",
              ( side == LEFT ? "l" : "r" ),
              unum, player_type );
    for ( MonitorCont::reference m : M_monitors )
    {
        m->sendMsg( MSG_BOARD, buffer );
    }

    Logger::instance().writeTextLog( *this, buffer, SUBS );
}


bool
Stadium::changePlayerToGoalie( const Player * player )
{
    if ( player->side() == LEFT )
    {
        if ( M_team_l->changePlayerToGoalie( player ) )
        {
            broadcastChangePlayerToGoalie( player );
            return true;
        }
    }
    else if ( player->side() == RIGHT )
    {
        if ( M_team_r->changePlayerToGoalie( player ) )
        {
            broadcastChangePlayerToGoalie( player );
            return true;
        }
    }

    return false;
}

void
Stadium::broadcastChangePlayerToGoalie( const Player * player )
{
    char msg[64];
    snprintf( msg, 64, "(change_player_type %c %d goalie)",
              player->side() == LEFT ? 'l' : player->side() == RIGHT ? 'r' : '?',
              player->unum() );

    // tell players
    for ( int i = 0 ; i < MAX_PLAYER * 2; ++i )
    {
        if ( M_players[i]->isEnabled()
             && M_players[i]->version() >= 16.0 )
        {
            M_players[i]->send( msg );
        }
    }

    // tell coaches
    for ( int i = 0; i < 2; ++i )
    {
        if ( M_olcoaches[i]
             && M_olcoaches[i]->assigned()
             && M_olcoaches[i]->version() >= 16.0 )
        {
            M_olcoaches[i]->send( msg );
        }
    }

    // TODO: send to offline coach

    // monitors
    for ( MonitorCont::reference m : M_monitors )
    {
        m->sendMsg( MSG_BOARD, msg );
    }

    Logger::instance().writeTextLog( *this, msg, SUBS );
}


void
Stadium::collisions()
{
    bool col = false;
    int max_loop = 10;

    const std::size_t SIZE = M_players.size();

    do
    {
        col = false;
        M_ball->clearCollision();
        for ( PlayerCont::reference p : M_players )
        {
            p->clearCollision();
        }

        // check ball to player
        for ( PlayerCont::reference p : M_players )
        {
            if ( p->isEnabled()
                 && p != M_ball_catcher
                 && M_ball->pos().distance2( p->pos() ) < std::pow( M_ball->size() + p->size(), 2 ) )
            {
                col = true;
                p->collidedWithBall();
                for_each( M_referees.begin(), M_referees.end(),
                          //Referee::doBallTouched( *p ) );
                          [&]( Referee * ref )
                          {
                              ref->ballTouched( *p );
                          } );
                calcBallCollisionPos( p );
            }
        }

        // check player to player
        for ( std::size_t i = 0; i < SIZE - 1; ++i )
        {
            for ( std::size_t j = i + 1; j < SIZE; ++j )
            {
                if ( M_players[i]->isEnabled()
                     && M_players[j]->isEnabled()
                     && M_players[i]->pos().distance2( M_players[j]->pos() ) < std::pow( M_players[i]->size() + M_players[j]->size(), 2 ) )
                {
                    col = true;
                    M_players[i]->collidedWithPlayer();
                    M_players[j]->collidedWithPlayer();
                    calcCollisionPos( M_players[i], M_players[j] );
                }
            }
        }

        M_ball->moveToCollisionPos();
        for ( PlayerCont::reference p : M_players )
        {
            p->moveToCollisionPos();
        }

        --max_loop;
    }
    while ( col && max_loop > 0 );

    //if ( max_loop < 8
    //    std::cerr << M_time << ": collision loop " << max_loop
    //              << std::endl;

    M_ball->updateCollisionVel();
    for ( PlayerCont::reference p : M_players )
    {
        p->updateCollisionVel();
    }

}

void
Stadium::calcBallCollisionPos( Player * p )
{
    if ( playmode() == PM_PlayOn )
    {
        calcCollisionPos( M_ball, p );
        return;
    }

    PVector b2p;
    if ( M_ball->pos() == p->pos() )
    {
        double p_ang = drand( -M_PI, M_PI );
        b2p = PVector::fromPolar( M_ball->size() + p->size() + EPS, p_ang );
    }
    else
    {
        b2p = p->pos() - M_ball->pos();
        b2p.normalize( M_ball->size() + p->size() + EPS );
    }

    M_ball->collide( M_ball->pos() );
    p->collide( M_ball->pos() + b2p );
}

void
Stadium::calcCollisionPos( MPObject * a,
                           MPObject * b )
{
    if ( ! a || ! b )
    {
        return;
    }

    PVector apos = a->pos();
    PVector bpos = b->pos();
    {
        PVector mid = a->pos() + b->pos();
        mid /= 2.0;

        PVector mid2a = a->pos() - mid;
        PVector mid2b = b->pos() - mid;

        /* pfr 10/25/01
           This was a really nasty bug. This used to be the condition
           if ( a->pos == b->pos )
           If a->pos and b->pos are approximately equal (but
           not ==, then a->pos - mid and b->pos - mid can both be 0.
           This means that in the else clause below, we call PVector::r(v)
           on two zero vectors, which makes them both (v,0).
           Then, the while statement below is an infinite loop */
        if ( a->pos() == b->pos()
             || ( ( a->pos() - mid ).r() < EPS
                  && ( b->pos() - mid ).r() < EPS )
             )
        {
            // if the two objects are directly on top on one and other
            // then they will be separated at a random angle
            double a_ang = drand ( -M_PI, M_PI );
            double b_ang = normalize_angle( a_ang + M_PI );
            mid2a = PVector::fromPolar( ( a->size() + b->size() ) / 2.0 + EPS, a_ang );
            mid2b = PVector::fromPolar( ( a->size() + b->size() ) / 2.0 + EPS, b_ang );
        }
        else
        {
            mid2a.normalize( ( a->size() + b->size() ) / 2.0 + EPS );
            mid2b.normalize( ( a->size() + b->size() ) / 2.0 + EPS );
        }

        apos = mid + mid2a;
        bpos = mid + mid2b;

        // 0.01% is added to the movement, as sometimes structural noise
        // means that even though mid2a and mid2b should be
        // a->size + b->size apart, they are ever so slightly
        // less.
        int count = 0;
        const double collision_dist2 = std::pow( a->size() + b->size(), 2 );
        while ( apos.distance2( bpos ) < collision_dist2
                && count < 10 )
        {
            mid2a.normalize( mid2a.r() * 1.0001 );
            mid2b.normalize( mid2b.r() * 1.0001 );
            apos = mid + mid2a;
            bpos = mid + mid2b;
            ++count;
        }
    }


    /*    cout << "apos = " << a->pos << std::endl; */
    /*    cout << "anewpos = " << (mid + mid2a ) << std::endl; */

    /*    cout << "bpos = " << b->pos << std::endl; */
    /*    cout << "bnewpos = " << (mid + mid2b ) << std::endl; */

    /*    cout << "old distance = " << a->pos.distance ( b->pos ) << std::endl; */
    /*    cout << "new distance = " << apos.distance ( bpos ) << std::endl; */

    a->collide( apos );
    b->collide( bpos );
}


void
Stadium::setPlayerState( const Side side,
                         const int unum,
                         const int state )
{
    if ( unum < 1 || MAX_PLAYER < unum )
    {
        if ( ServerParam::instance().verboseMode() )
        {
            std::cerr << __FILE__ << ':' << __LINE__
                      << " No such player. " << SideStr( side )
                      << " " << unum << std::endl;
        }
        return;
    }

    Player * player = getPlayer( side, unum );
    if ( ! player )
    {
        if ( ServerParam::instance().verboseMode() )
        {
            std::cerr << __FILE__ << ':' << __LINE__
                      << " No such player. " << SideStr( side )
                      << " " << unum << std::endl;
        }
        return;
    }

    if ( player->isEnabled() )
    {
        player->addState( state );
    }
}

void
Stadium::kickTaken( const Player & kicker,
                    const PVector & accel )
{
    M_ball_catcher = static_cast< const Player * >( 0 );

    M_ball->push( accel );

    const double accel_r = accel.r();
    for_each( M_referees.begin(), M_referees.end(),
              //Referee::doKickTaken( kicker, accel.r() ) );
              [&]( Referee * ref )
              {
                  ref->kickTaken( kicker, accel_r );
              } );
}

void
Stadium::failedKickTaken( const Player & kicker )
{
    for_each( M_referees.begin(), M_referees.end(),
              //Referee::doFailedKickTaken( kicker ) );
              [&]( Referee * ref )
              {
                  ref->failedKickTaken( kicker );
              } );
}

void
Stadium::tackleTaken( const Player & tackler,
                      const PVector & accel,
                      const bool foul )
{
    M_ball_catcher = static_cast< const Player * >( 0 );

    M_ball->push( accel );

    const double accel_r = accel.r();
    for_each( M_referees.begin(), M_referees.end(),
              //Referee::doTackleTaken( tackler, accel.r(), foul ) );
              [&]( Referee * ref )
              {
                  ref->tackleTaken( tackler, accel_r, foul );
              } );
}

void
Stadium::failedTackleTaken( const Player & tackler,
                            const bool foul )
{
    for_each( M_referees.begin(), M_referees.end(),
              //Referee::doFailedTackleTaken( tackler, foul ) );
              [&]( Referee * ref )
              {
                  ref->failedTackleTaken( tackler, foul );
              } );
}


void
Stadium::ballCaught( const Player & catcher )
{
    if ( ! Referee::isPenaltyShootOut( playmode() ) )
    {
        std::string msg = "goalie_catch_ball_";
        msg += SideStr( catcher.side() );
        sendRefereeAudio( msg.c_str() );
    }

    collisions();

    for_each( M_referees.begin(), M_referees.end(),
              //Referee::doBallTouched( catcher ) );
              [&]( Referee * ref )
              {
                  ref->ballTouched( catcher );
              } );

    for_each( M_referees.begin(), M_referees.end(),
              //Referee::doCaughtBall( catcher ) );
              [&]( Referee * ref )
              {
                  ref->ballCaught( catcher );
              } );

    if ( playmode() == PM_FreeKick_Left
         || playmode() == PM_FreeKick_Right )
    {
        M_ball_catcher = &catcher;
    }
}

void
Stadium::ballPunched( const Player & catcher,
                      const PVector & accel )
{
    M_ball->push( accel );

    for_each( M_referees.begin(), M_referees.end(),
              //Referee::doBallTouched( catcher ) );
              [&]( Referee * ref )
              {
                  ref->ballTouched( catcher );
              } );

    for_each( M_referees.begin(), M_referees.end(),
              //Referee::doPunchedBall( catcher ) );
              [&]( Referee * ref )
              {
                  ref->ballPunched( catcher );
              } );
}

void
Stadium::sendTeamGraphic( const Side side,
                          const unsigned int x,
                          const unsigned int y )
{
    Logger::instance().writeTeamGraphic( side, x, y );
}


void
Stadium::removeDisconnectedClients()
{
    for ( PlayerCont::iterator i = M_remote_players.begin();
          i != M_remote_players.end(); )
    {
        if ( ! (*i)->connected() )
        {
            (*i)->disable();
            removeListener( *i );
            i = M_remote_players.erase( i );
        }
        else
        {
            ++i;
        }
    }

    for ( OfflineCoachCont::iterator i = M_remote_offline_coaches.begin();
          i != M_remote_offline_coaches.end(); )
    {
        if ( ! (*i)->connected() )
        {
            (*i)->disable();
            removeListener( *i );
            i = M_remote_offline_coaches.erase( i );
        }
        else
        {
            ++i;
        }
    }

    for ( OnlineCoachCont::iterator i = M_remote_online_coaches.begin();
          i != M_remote_online_coaches.end(); )
    {
        if ( ! (*i)->connected() )
        {
            (*i)->disable();
            removeListener( *i );
            i = M_remote_online_coaches.erase( i );
        }
        else
        {
            ++i;
        }
    }

    for ( MonitorCont::iterator i = M_monitors.begin();
          i != M_monitors.end(); )
    {
        if ( ! (*i)->connected() )
        {
            delete *i;
            i = M_monitors.erase( i );
            std::cout << "A monitor disconnected\n";
        }
        else
        {
            ++i;
        }
    }
}


void
Stadium::sendRefereeAudio( const char * msg )
{
    std::shuffle( M_listeners.begin(), M_listeners.end(),
                  DefaultRNG::instance() );

    // the following should work, but I haven't tested it yet
    //      std::for_each( M_listeners.begin(), M_listeners.end(),
    //                     std::bind2nd( std::mem_fun( &rcss::Listener::sendRefereeAudio ),
    //                                   msg.c_str() ) );

    for ( ListenerCont::reference l : M_listeners )
    {
        l->sendRefereeAudio( msg );
    }

    Logger::instance().writeRefereeAudio( *this, msg );

    if ( ServerParam::instance().sendComms() )
    {
        char buf[max_message_length_for_display];
        snprintf( buf,
                  max_message_length_for_display,
                  "(%s %s)",
                  REFEREE_NAME, msg );

        for ( MonitorCont::reference m : M_monitors )
        {
            m->sendMsg( MSG_BOARD, buf );
        }
    }
}


void
Stadium::sendPlayerAudio( const Player & player,
                          const char * msg )
{
    std::shuffle( M_listeners.begin(), M_listeners.end(),
                  DefaultRNG::instance() );

    for ( ListenerCont::reference l : M_listeners )
    {
        l->sendPlayerAudio( player, msg );
    }

    Logger::instance().writePlayerAudio( *this, player, msg );

    if ( ServerParam::instance().sendComms() )
    {
        char buf[max_message_length_for_display];
        snprintf( buf,
                  max_message_length_for_display,
                  "(%s %s)",
                  player.name().c_str(), msg );

        // send to monitors
        for ( MonitorCont::reference m : monitors() )
        {
            m->sendMsg( MSG_BOARD, buf );
        }
    }
}


void
Stadium::sendCoachAudio( const Coach & coach,
                         const char * msg )
{
    std::shuffle( M_listeners.begin(), M_listeners.end(),
                  DefaultRNG::instance() );

    for ( ListenerCont::reference l : M_listeners )
    {
        l->sendCoachAudio( coach, msg );
    }

    Logger::instance().writeCoachAudio( *this, coach, msg );

    if ( ServerParam::instance().sendComms() )
    {
        char buf[max_message_length_for_display];
        char format[40];
        snprintf( format, 40, "(%%s %%.%ds)",
                  max_message_length_for_display - (MaxMesg - MaxCoachMesg) );
        snprintf( buf,
                  max_message_length_for_display,
                  format,
                  ( coach.side() == RIGHT
                    ? OLCOACH_NAME_R
                    : coach.side() == LEFT
                    ? OLCOACH_NAME_L: REFEREE_NAME ),
                  msg );

        for ( MonitorCont::reference m : monitors() )
        {
            m->sendMsg( MSG_BOARD, buf );
        }
    }
}

void
Stadium::sendCoachStdAudio( const OnlineCoach & coach,
                            const rcss::clang::Msg & msg )
{
    std::shuffle( M_listeners.begin(), M_listeners.end(),
                  DefaultRNG::instance() );

    for ( ListenerCont::reference l : M_listeners )
    {
        l->sendCoachStdAudio( msg );
    }

    Logger::instance().writeCoachStdAudio( *this, coach, msg );

    if ( ServerParam::instance().sendComms() )
    {
        std::ostringstream coach_mess;

        coach_mess << msg.getTimeRecv() << " ";
        msg.print( coach_mess );

        char buf[max_message_length_for_display];
        char format[40];
        snprintf( format, 40, "(%%s %%.%ds)",
                  max_message_length_for_display - (MaxMesg - MaxCoachMesg));
        snprintf( buf,
                  max_message_length_for_display,
                  format,
                  (coach.side() == RIGHT) ? OLCOACH_NAME_R : OLCOACH_NAME_L,
                  coach_mess.str().c_str() );

        for ( MonitorCont::reference m : monitors() )
        {
            m->sendMsg( MSG_BOARD, buf );
        }
    }

}


void
Stadium::doRecvFromClients()
{
    static int s_time = 0;
    static int s_stoppage_time = 0;

    const std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();

    //
    // delayed effects
    //
    if ( s_time != M_time
         && s_stoppage_time != M_stoppage_time )
    {
        s_time = M_time;
        s_stoppage_time = M_stoppage_time;

        std::shuffle( M_shuffle_players.begin(), M_shuffle_players.end(),
                      DefaultRNG::instance() );
        for ( PlayerCont::reference p : M_shuffle_players )
        {
            p->doLongKick();
        }
    }

    //
    // receive message and process commands
    //
    udp_recv_message();
    udp_recv_from_online_coach();
    udp_recv_from_coach();

    removeDisconnectedClients();

    const std::chrono::system_clock::time_point end_time = std::chrono::system_clock::now();
    Logger::instance().writeProfile( *this, start_time, end_time, "RECV" );
}

void
Stadium::doNewSimulatorStep()
{
    static std::chrono::system_clock::time_point prev_time = std::chrono::system_clock::now();
    const std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();

    // th 6.3.00
    //  tp_old = tp_new;
    //  write_times displays nonsense at first call, since tp_old is never
    //  initialized. Don't want to handle special exception for first call.
    Logger::instance().writeTimes( *this, prev_time, start_time );
    prev_time = start_time;

    //
    // step
    //
    step();
    startTeams();
    checkAutoMode();

    const std::chrono::system_clock::time_point end_time = std::chrono::system_clock::now();
    Logger::instance().writeProfile( *this, start_time, end_time, "SIM" );
}

void
Stadium::doSendSenseBody()
{
    const std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();

    std::shuffle( M_remote_players.begin(), M_remote_players.end(),
                  DefaultRNG::instance() );

    //
    // send sense_body & fullstate
    //
    for ( PlayerCont::reference p : M_remote_players )
    {
        if ( p->isEnabled()
             && p->connected() )
        {
            p->sendBody();

            if ( ( p->side() == LEFT
                   && ServerParam::instance().fullstateLeft() )
                 || ( p->side() == RIGHT
                      && ServerParam::instance().fullstateRight() ) )
            {
                p->sendFullstate();
            }
        }

        // reset collision flags
        p->resetCollisionFlags();
    }

    //
    // send audio message
    //
    std::for_each( M_listeners.begin(), M_listeners.end(),
                   []( rcss::Listener * l )
                   {
                       l->newCycle();
                   } );
                   //rcss::Listener::NewCycle() ); //std::mem_fun( &rcss::Listener::newCycle ) );

    //
    // write profile
    //
    const std::chrono::system_clock::time_point end_time = std::chrono::system_clock::now();
    Logger::instance().writeProfile( *this, start_time, end_time, "SB" );
}

void
Stadium::doSendVisuals()
{
    const std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();

    std::shuffle( M_remote_players.begin(), M_remote_players.end(),
                  DefaultRNG::instance() );

    for ( PlayerCont::reference p : M_remote_players )
    {
        if ( p->isEnabled()
             && p->connected() )
        {
            p->sendVisual();
        }
    }

    const std::chrono::system_clock::time_point end_time = std::chrono::system_clock::now();
    Logger::instance().writeProfile( *this, start_time, end_time, "VIS" );
}

void
Stadium::doSendSynchVisuals()
{
    const std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();

    std::shuffle( M_remote_players.begin(), M_remote_players.end(),
                  DefaultRNG::instance() );

    for ( PlayerCont::reference p : M_remote_players )
    {
        if ( p->isEnabled()
             && p->connected() )
        {
            p->sendSynchVisual();
        }
    }

    const std::chrono::system_clock::time_point end_time = std::chrono::system_clock::now();
    Logger::instance().writeProfile( *this, start_time, end_time, "VIS_S" );
}

void
Stadium::doSendCoachMessages()
{
    const std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();

    if ( M_coach->assigned()
         && M_coach->isEyeOn() )
    {
        M_coach->send_visual_info();
    }

    for ( int i = 0; i < 2; ++i )
    {
        if ( M_olcoaches[i]->assigned() && M_olcoaches[i]->isEyeOn() )
        {
            M_olcoaches[i]->send_visual_info();
        }
    }

    const std::chrono::system_clock::time_point end_time = std::chrono::system_clock::now();
    Logger::instance().writeProfile( *this, start_time, end_time, "COACH" );

#if 0
    // At each cycle we flush to logs otherwise the buffers
    // buildup and the server will pause while they are flushed
    // automatically.  It's better to flush them often at this point in
    // the code as they will do less damage than occuring randomly
    // within the code.
    //     "Why is this point good?" I hear you ask.  Well, at this point
    // in the code, the server has just done it's simulation step, sent
    // sense body messages and visuals to players and coaches.  The next
    // important action is in 35ms when the next visuals (possibly) need
    // to be sent.  Hopefully and since we flush the logs every 100ms,
    // the flushing will be completed before the 35ms is up.
    //     "Is there a better way to do it so we don't have to hope that
    // the flush is completed within the 35ms?" I hear you enquire
    // further.  Well, one way to make it better would be to create a
    // thread for logging (yes, I know their are problems with doing
    // that).  The main thread would pass data to the logging thread
    // (which would happen rather quickly) and then the logging thread
    // would write it to the log files tacking as much time as it likes.
    //    "Why don't we do that?" you persist.  Because other things,
    // need to be worked on first and I would like to have a better
    // reason for introducting threads to the code that to improve
    // logging.  Maybe later, when we already have a thread for each
    // client it will seem like less of a hurdle.
    Logger::instance().flush();
#endif
}

bool
Stadium::doSendThink()
{
    const char * think_command = "(think)";
    const double max_msec_waited = 25 * 50;
    const int max_cycles_missed = 20;

    static int cycles_missed = 0; //number of cycles where someone missed

    bool shutdown = false;

    if ( time() <= 0 )
    {
        //still waiting for players to connect, so let's run a little more slowly
        std::chrono::microseconds sleep_count( 50 * 1000 );
        std::this_thread::sleep_for( sleep_count );
        //usleep( 50 * 1000 );
    }
    else if ( ! monitors().empty() )
    {
        static int monitor_wait_count = 0;
        if ( ++monitor_wait_count >= 32 )
        {
            monitor_wait_count = 0;
            std::chrono::microseconds sleep_count( 20 * 1000 );
            std::this_thread::sleep_for( sleep_count );
            //usleep( 20 * 1000 );
        }
    }

    //figure out who we are going to wait for
    bool wait_players[MAX_PLAYER*2];
    for ( int i = 0; i < MAX_PLAYER*2; ++i )
    {
        wait_players[i] = M_players[i]->isEnabled();
    }

    bool wait_coach[2];
    for ( int i = 0; i < 2; ++i )
    {
        wait_coach[i] = M_olcoaches[i]->isEyeOn();
    }

    bool wait_trainer = M_coach->isEyeOn();

    //tell the clients they should start thinking
    for ( int i = 0; i < MAX_PLAYER*2; ++i )
    {
        if ( wait_players[i] && M_players[i]->connected() )
        {
            M_players[i]->send( think_command );
        }
    }

    for ( int i = 0; i < 2; ++i )
    {
        if ( wait_coach[i] && M_olcoaches[i]->connected() )
        {
            M_olcoaches[i]->send( think_command );
        }
    }

    if ( wait_trainer
         && M_coach->connected() )
    {
        M_coach->send( think_command );
    }

    //wait for confirmations from the clients

    enum {
        DS_FALSE = 0,
        DS_TRUE = 1,
        DS_TRUE_BUT_INCOMPLETE = 2
    } done;

    int num_sleeps = 0;

    const std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();

    do
    {
        done = DS_TRUE;
        ++num_sleeps;

        std::chrono::microseconds sleep_count( ServerParam::instance().synchMicroSleep() );
        std::this_thread::sleep_for( sleep_count );
        //usleep( ServerParam::instance().synchMicroSleep() );

        doRecvFromClients();

        for ( int i = 0; i < MAX_PLAYER*2; ++i )
        {
            if ( wait_players[i]
                 && M_players[i]->connected()
                 && ! M_players[i]->doneReceived()
                 && M_players[i]->isEnabled() )
            {
                done = DS_FALSE;
                break;
            }
        }

        for ( int i = 0; i < 2; ++i )
        {
            if ( wait_coach[i]
                 && M_olcoaches[i]->connected()
                 && ! M_olcoaches[i]->doneReceived()
                 && M_olcoaches[i]->assigned() )
            {
                done = DS_FALSE;
                break;
            }
        }

        if ( wait_trainer
             && M_coach->connected()
             && ! M_coach->doneReceived() )
        {
            done = DS_FALSE;
        }

        // get time differnce with start of loop, first get time difference in
        // seconds, then multiply with 1000 to get msec.
        const std::chrono::nanoseconds nano_diff = std::chrono::duration_cast< std::chrono::nanoseconds >( std::chrono::system_clock::now() - start_time );
        const double time_diff = nano_diff.count() * 0.001 * 0.001;

        if ( time_diff > max_msec_waited )
        {
            done = DS_TRUE_BUT_INCOMPLETE;
            if ( time() > 0 )
            {
                ++cycles_missed;
                std::cerr << "Someone missed a cycle at " << time() << std::endl;
            }
            if ( cycles_missed > max_cycles_missed )
            {
                std::cerr << "Waiting too long for clients! Exiting" << std::endl;
                shutdown = true;
            }
        }
    }
    while ( done == DS_FALSE );

    if ( done != DS_TRUE_BUT_INCOMPLETE )
    {
        cycles_missed = 0;
    }

    if ( ServerParam::instance().logTimes() )
    {
        char buf[128];
        snprintf( buf, 127, "Num sleeps called: %d", num_sleeps );
        Logger::instance().writeTextLog( *this, buf, LOG_TEXT );
    }

    if ( shutdown )
    {
        M_alive = false;
    }

    return shutdown;
}



namespace {
template < class T >
void
recv_from_clients( std::vector< T > & clients )
{
    std::shuffle( clients.begin(), clients.end(),
                  DefaultRNG::instance() );

    for ( typename std::vector< T >::iterator i = clients.begin();
          i != clients.end(); )
    {
        if ( (*i)->recv() == -1 )
        {
            ++i;
        }
    }
}
}


void
Stadium::udp_recv_message()
{
    recv_from_clients( M_remote_players );
    recv_from_clients( M_monitors );

    while ( 1 )
    {
        char message[MaxMesg];
        std::memset( &message, 0, sizeof( char ) * MaxMesg );

        rcss::net::Addr cli_addr;

        int len = M_player_socket.recv( message, MaxMesg, cli_addr );

        if ( len > 0 )
        {
            //              std::cerr << "Got: ";
            //              std::cerr.write( message, iMsgLength );
            //              std::cerr << std::endl;

            bool found = false;
            for ( PlayerCont::reference p : M_remote_players )
            {
                if ( p->getDest() == cli_addr )
                {
                    p->undedicatedRecv( message, len );
                    found = true;
                    break;
                }
            }

            if ( ! found )
            {
                for ( MonitorCont::reference m : M_monitors )
                {
                    if ( m->getDest() == cli_addr )
                    {
                        m->undedicatedRecv( message, len );
                        found = true;
                        break;
                    }
                }
            }

            if ( ! found )
            {
                // a new monitor or a new player

                /* chop newline */
                if ( message[len - 1] == '\n' )
                {
                    --len;
                }
                message[len] = '\0';

                if ( ! parseMonitorInit( message, cli_addr ) )
                {
                    parsePlayerInit( message, cli_addr );
                }
            }
        }
        else if ( errno != EWOULDBLOCK )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": Error recv'ing from socket: "
                      << std::strerror( errno ) << std::endl;
        }

        if ( len < 0 )
        {
            break;
        }
    }
}



void
Stadium::parsePlayerInit( const char * message,
                          const rcss::net::Addr & cli_addr )
{
    //
    // init : a new player connects to the server
    //
    if ( ! std::strncmp( message, "(init ", std::strlen( "(init " ) ) )
    {
        // (init <TeamName> [(version <Ver>)][ (goalie)])

        const char * msg = message;

        char teamname[16];
        double version = 3.0;
        bool goalie = false;

        int n_read = 0;
        if ( std::sscanf( msg, " ( init %15[+-_a-zA-Z0-9] %n ",
                          teamname, &n_read ) != 1
             || n_read == 0 )
        {
            sendToPlayer( "(error illegal_teamname)", cli_addr );
            return;
        }
        msg += n_read;

        if ( *msg != '(' && *msg != ')' && ! std::isspace( *msg ) )
        {
            sendToPlayer( "(error illegal_teamname_or_too_long_teamname)", cli_addr );
            return;
        }

        while ( *msg != '\0' && *msg != '(' ) ++msg;

        while ( *msg != '\0' && *msg != ')' )
        {
            if ( ! std::strncmp( msg, "(version ", std::strlen( "(version " ) ) )
            {
                n_read = 0;
                if ( std::sscanf( msg, " ( version %lf ) %n ",
                                  &version, &n_read ) != 1
                     || n_read == 0 )
                {
                    sendToPlayer( "(error illegal_command_form)", cli_addr );
                    return;
                }
                msg += n_read;
            }
            else if ( ! std::strncmp( msg, "(goalie)", std::strlen( "(goalie)" )  ) )
            {
                goalie = true;
                msg += std::strlen( "(goalie)" );
            }
            else
            {
                sendToPlayer( "(error illegal_command_form)", cli_addr );
                return;
            }

            if ( *msg != '(' && *msg != ')' && ! std::isspace( *msg ) )
            {
                sendToPlayer( "(error illegal_command_form)", cli_addr );
                return;
            }

            while ( *msg != '\0' && std::isspace( *msg ) ) ++msg;
        }

        Player * p = initPlayer( teamname, version, goalie, cli_addr );
        if ( p )
        {
            std::cout << "A new (v" << static_cast< int >( version ) << ") "
                      << "player (" << teamname << ' ' << p->unum() << ") connected."
                      << std::endl;

            Logger::instance().writePlayerLog( *this, *p, message, RECV );
        }

        return;
    }

    //
    // reconnect : a player reconnects to the server
    //
    if ( ! std::strncmp( message, "(reconnect ", std::strlen( "(reconnect " ) ) )
    {
        if ( playmode() == PM_PlayOn )
        {
            sendToPlayer( "(error cannot_reconnect_while_playon)", cli_addr );
            return;
        }

        char teamname[128];
        int unum;

        if ( std::sscanf( message,
                          " ( reconnect %127s %d ) ",
                          teamname, &unum ) < 2 )
        {
            sendToPlayer( "(error illegal_command_form)", cli_addr );
        }

        Player * p = reconnectPlayer( teamname, unum, cli_addr );
        if ( p )
        {
            std::cout << "A player (" << teamname << ' ' << p->unum() << ") reconnected."
                      << std::endl;
            Logger::instance().writePlayerLog( *this, *p, message, RECV );
        }

        return;
    }

    sendToPlayer( "(error unknown_command)", cli_addr );
}


bool
Stadium::parseMonitorInit( const char * message,
                           const rcss::net::Addr & addr )
{
    double ver = 1.0;
    if ( ! std::strncmp( message, "(dispinit)", 10 )
         || std::sscanf( message, " ( dispinit version %lf ) ", &ver ) == 1 )
    {
        if ( ServerParam::instance().maxMonitors() > 0
             && static_cast< int >( M_monitors.size() ) >= ServerParam::instance().maxMonitors() )
        {
            sendToPlayer( "(error no_more_monitor)", addr );
            return true;
        }

        // a new monitor connected
        Monitor * mon = new Monitor( *this, ver );

        if( ! mon->connect( addr ) )
        {
            sendToPlayer( "(error connection_failed)", addr );
            delete mon;
            return true;
        }

        if ( ! mon->setSenders() )
        {
            sendToPlayer( "(error illegal_client_version)", addr );
            delete mon;
            return true;
        }
        std::cout << "A new (v" << ver << ") monitor connected." << std::endl;

        mon->setEnforceDedicatedPort( ver >= 2.0 );
        M_monitors.push_back( mon );

        // send server parameter information to monitor
        mon->sendInit();
        return true;
    }

    return false;
}

void
Stadium::udp_recv_from_coach()
{
    const bool allow_coach = ( ServerParam::instance().coachMode()
                               || ServerParam::instance().coachWithRefereeMode() );

    if ( allow_coach )
    {
        recv_from_clients( M_remote_offline_coaches );
    }

    while ( 1 )
    {
        char message[MaxMesg];
        std::memset( &message, 0, sizeof( char ) * MaxMesg );

        rcss::net::Addr cli_addr;

        int len = M_offline_coach_socket.recv( message, MaxMesg,  cli_addr );

        if ( len > 0 )
        {
            if ( ! allow_coach )
            {
                sendToCoach( "(error connected_offline_coach_without_coach_mode)", cli_addr );
                continue;
            }

            bool found = false;
            for ( OfflineCoachCont::reference c : M_remote_offline_coaches )
            {
                if ( c->getDest() == cli_addr )
                {
                    c->undedicatedRecv( message, len );
                    found = true;
                    break;
                }
            }

            if ( ! found )
            {
                // a new offline coach

                // chop newline
                if ( message[len - 1] == '\n' )
                {
                    --len;
                }
                message[len] = '\0';

                parseCoachInit( message, cli_addr );
            }
        }
        else if ( errno != EWOULDBLOCK )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": Error recv'ing from socket: "
                      << strerror( errno ) << std::endl;
        }

        if ( len < 0 )
        {
            break;
        }
    }
}

bool
Stadium::parseCoachInit( const char * message,
                         const rcss::net::Addr & cli_addr )
{
    if ( ! M_remote_offline_coaches.empty() )
    {
        sendToCoach( "(error already_have_offline_coach)", cli_addr );
        return false;
    }

    char command[128];
    int n = std::sscanf( message, " ( %127[-0-9a-zA-Z.+*/?<>_] ", command );
    if( n < 1 )
    {
        sendToCoach( "(error illegal_command_form)", cli_addr );
        return false;
    }

    // (init[ (version <Ver>)])

    if ( ! std::strcmp( command, "init" ) )
    {
        double version = 3.0;
        int n = std::sscanf( message, " ( init ( version %lf ) ) ", &version );
        if ( ( n != 0 && n != 1 )
             || version < 1.0 )
        {
            sendToCoach( "(error illegal_command_form)", cli_addr );
            return false;
        }

        Coach * coach = initCoach( version, cli_addr );
        if ( coach )
        {
            std::cout << "A new (v" << coach->version() << ") "
                      << "offline coach connected" << std::endl;
            Logger::instance().writeCoachLog( *this, message, RECV );
        }
    }
    else
    {
        kickOff(); // need to remove this line if we
        // dont want the server to start when the coach connects
        M_coach->parse_command( message );
        Logger::instance().writeCoachLog( *this, message, RECV );
    }

    return true;
}

void
Stadium::udp_recv_from_online_coach()
{
    recv_from_clients( M_remote_online_coaches );

    while ( 1 )
    {
        char message[MaxMesg];
        std::memset( &message, 0, sizeof ( char ) * MaxMesg );

        rcss::net::Addr cli_addr;

        int len = M_online_coach_socket.recv( message, MaxMesg, cli_addr );

        if ( len > 0 )
        {
            bool found = false;
            for ( OnlineCoachCont::reference c : M_remote_online_coaches )
            {
                if ( c->getDest() == cli_addr )
                {
                    c->undedicatedRecv( message, len );
                    found = true;
                    break;
                }
            }

            if ( ! found )
            {
                // a new online coach

                /* chop newline */
                if ( message[len - 1] == '\n' )
                {
                    --len;
                }
                message[len] = '\0';

                parseOnlineCoachInit( message, cli_addr );
            }
        }
        else if ( errno != EWOULDBLOCK )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": Error recv'ing from socket: "
                      << strerror( errno ) << std::endl;
        }

        if ( len < 0 )
        {
            break;
        }
    }
}

void
Stadium::parseOnlineCoachInit( const char * message,
                               const rcss::net::Addr & addr )
{
    if ( std::strncmp( message, "(init", std::strlen( "(init" ) ) != 0 )
    {
        sendToOnlineCoach( "(error illegal_command_form)",
                           addr );
        return;
    }

    // (init <TeamName>[ <CoachName>][ (version <Ver>)])

    const char * msg = message;

    const double default_olcoach_version = 5.0;

    char teamname[16];
    char coachname[128];
    double version = default_olcoach_version;

    std::memset( teamname, 0, 16 );
    std::memset( coachname, 0, 128 );

    int n_read = 0;

    // read team name
    if ( std::sscanf( msg, " ( init %15[+-_a-zA-Z0-9] %n ",
                      teamname, &n_read ) != 1
         || n_read == 0 )
    {
        sendToOnlineCoach( "(error illegal_command_form)", addr );
        return;
    }
    msg += n_read;

    while ( *msg != '\0' && std::isspace( *msg ) ) ++msg;

    // read coach name
    if ( *msg != '(' )
    {
        if ( std::sscanf( msg, " %127[+-_a-zA-Z0-9] %n ",
                          coachname, &n_read ) != 1 )
        {
            sendToOnlineCoach( "(error illegal_command_form)", addr );
            return;
        }
        msg += n_read;

        if ( *msg != '(' && *msg != ')' && ! std::isspace( *msg ) )
        {
            sendToOnlineCoach( "(error illegal_coachname_or_too_long_coachname)", addr );
            return;
        }
    }

    while ( *msg != '\0' && *msg != '(' ) ++msg;

    // read protocol version
    if ( ! std::strncmp( "(version", msg, std::strlen( "(version" ) ) )
    {
        if ( std::sscanf( msg, " ( version %lf ) %n ",
                          &version, &n_read ) != 1
             || n_read == 0 )
        {
            sendToOnlineCoach( "(error illegal_command_form)", addr );
            return;
        }
        msg += n_read;

        if ( *msg != '(' && *msg != ')' && ! std::isspace( *msg ) )
        {
            sendToOnlineCoach( "(error illegal_client_version)", addr );
            return;
        }

        if ( version < default_olcoach_version )
        {
            sendToOnlineCoach( "(error illegal_client_version)", addr );
            return;
        }
    }

    //
    // assign new online coach
    //

    OnlineCoach * olc = initOnlineCoach( teamname, coachname, version, addr );
    if ( olc )
    {
        std::cout << "A new (v" << static_cast< int >( version ) << ") "
                  << "online coach (" << teamname << ") connected."
                  << std::endl;;

        Logger::instance().writeOnlineCoachLog( *this, *olc, message, RECV );
    }
}

void
Stadium::sendToPlayer( const char * msg,
                       const rcss::net::Addr & cli_addr )
{
    if ( M_player_socket.send( msg, std::strlen( msg ) + 1, cli_addr ) == -1 )
    {
        std::cerr << __FILE__ ": " << __LINE__
                  << ": Error sending to socket: "
                  << strerror( errno ) << std::endl;
    }
}

void
Stadium::sendToCoach( const char * msg,
                      const rcss::net::Addr & cli_addr )
{
    if ( M_offline_coach_socket.send( msg, std::strlen( msg ) + 1, cli_addr ) == -1 )
    {
        std::cerr << __FILE__ ": " << __LINE__
                  << ": Error sending to socket: "
                  << strerror( errno ) << std::endl;
    }
}

void
Stadium::sendToOnlineCoach( const char * msg,
                            const rcss::net::Addr & cli_addr )
{
    if ( M_online_coach_socket.send( msg, std::strlen( msg ) + 1, cli_addr ) == -1 )
    {
        std::cerr << __FILE__ ": " << __LINE__
                  << ": Error sending to socket: "
                  << strerror( errno ) << std::endl;
    }
}

void
Stadium::doQuit()
{
    finalize( "Quit Server. Exiting..." );
}

void
Stadium::finalize( const std::string & msg )
{
    static bool s_first = true;
    if ( s_first )
    {
        s_first = false;
        killTeams();
        std::cout << '\n' << msg << '\n';
        Logger::instance().close( *this );
        saveResults();
        disable();
    }
}

void
Stadium::disable()
{
    M_alive = false;
}

#include "resultsaver.hpp"

namespace rcss {
void
save_results( ResultSaver::team_id id,
              const Team & team,
              ResultSaver & saver )
{
    if ( ! team.name().empty() )
    {
        saver.saveTeamName( id, team.name() );
    }

    if ( team.olcoach()
         && ! team.olcoach()->name().empty() )
    {
        saver.saveCoachName( id, team.olcoach()->name() );
    }

    saver.saveScore( id, team.point() );

    if ( team.penaltyTaken() )
    {
        saver.savePenTaken( id, team.penaltyTaken() );
        saver.savePenScored( id, team.penaltyPoint() );
    }

    if ( team.penaltyWon() )
    {
        saver.saveCoinTossWinner( id );
    }
}
}

void
Stadium::saveResults()
{
    if ( M_savers.empty() )
    {
        return;
    }

    std::cout << "\nSaving Results:"  << std::endl;
    for ( std::list< ResultSaver::Ptr >::reference s : M_savers )
    {
        std::cout << "\t" << s->getName() << ": saving...\n";
        if ( s->enabled() )
        {
            s->saveStart();
            s->saveTime( getStartTime() );
            if ( M_team_l )
            {
                rcss::save_results( ResultSaver::TEAM_LEFT,
                                    *M_team_l,
                                    *s );
            }

            if ( M_team_r )
            {
                rcss::save_results( ResultSaver::TEAM_RIGHT,
                                    *M_team_r,
                                    *s );
            }

            if ( s->saveComplete() )
            {
                std::cout << "\t" << s->getName() << ": ...saved\n";
            }
            else
            {
                std::cout << "\t" << s->getName() << ": ...failed\n";
            }
        }
        else
        {
            std::cout << "\t" << s->getName() << ": ...disabled\n";
        }
    }
    std::cout << '\n';
    std::cout << "Saving Results Complete" << std::endl;
}

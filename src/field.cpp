/* -*- Mode: C++ -*-
 *Header:
 *File: field.C
 *Author: Noda Itsuki
 *Date: 1996/02/14
 *EndHeader:
 */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000-2007 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


 *EndCopyright:
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "audio.h"
#include "clangmsg.h"
#include "coach.h"
#include "field.h"
#include "landmarkreader.h"
#include "monitor.h"
#include "object.h"
#include "param.h"
#include "player.h"
#include "random.h"
#include "referee.h"
#include "remoteclient.h"
#include "serializer.h"
#include "serverparam.h"
#include "types.h"
#include "utility.h"
#include "xpmholder.h"

#include "initsender.h"
#include "serializercommonstdv8.h"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cctype>
#include <cerrno>

#include <unistd.h>
#include <strings.h>
#include <sys/time.h>
#include <netinet/in.h>

const std::string Stadium::DEF_TEXT_NAME = "incomplete";
const std::string Stadium::DEF_TEXT_SUFFIX = ".rcl";
const std::string Stadium::DEF_GAME_NAME = "incomplete";
const std::string Stadium::DEF_GAME_SUFFIX = ".rcg";
const std::string Stadium::DEF_KAWAY_NAME = "incomplete";
const std::string Stadium::DEF_KAWAY_SUFFIX = ".kwy";


/*
 *===================================================================
 *Part: Field
 *===================================================================
 */
Field::Field()
    : line_l( LINE_L_NAME, LINE_L_NAME_SHORT, "(Line)", "(L)",
              PVector( - ServerParam::PITCH_LENGTH / 2.0, 0.0 ) ),
      line_r( LINE_R_NAME, LINE_R_NAME_SHORT, "(Line)", "(L)",
              PVector( + ServerParam::PITCH_LENGTH / 2.0, 0.0 ) ),
      line_t( LINE_T_NAME, LINE_T_NAME_SHORT, "(Line)", "(L)",
              PVector( - ServerParam::PITCH_WIDTH / 2.0, 0.0 ) ),
      line_b( LINE_B_NAME, LINE_B_NAME_SHORT, "(Line)", "(L)",
              PVector( + ServerParam::PITCH_WIDTH / 2.0, 0.0 ) )

{

}

Field::~Field()
{
    M_goals.clear();

    for ( std::vector< PObject * >::iterator i = M_landmarks.begin();
          i != M_landmarks.end();
          ++i )
    {
        delete *i;
        *i = static_cast< PObject * >( 0 );
    }
    M_landmarks.clear();
}

void
Field::addLandmark( PObject * new_obj )
{
    if ( new_obj )
    {
        if ( new_obj->closeName() == O_TYPE_GOAL_NAME )
        {
            M_goals.push_back( new_obj );
        }
        M_landmarks.push_back( new_obj );
    }
}


Stadium::Stadium()
    : M_alive( true ),
      M_ball( NULL ),
      M_players( MAX_PLAYER*2, static_cast< Player * >( 0 ) ),
      M_coach( NULL ),
      M_olcoaches( 2, static_cast< OnlineCoach * >( 0 ) ),
      M_team_l( NULL ),
      M_team_r( NULL ),
      M_playmode( PM_BeforeKickOff ),
      M_time( 0 ),
      M_stoppage_time( 0 ),
      M_ball_catcher( NULL ),
      M_kick_off_side( LEFT ),
      M_nr_monitor_v1( 0 ),
      M_nr_monitor_v2( 0 ),
      M_last_playon_start( 0 ),
      M_game_over_wait( 0 ),
      M_left_child( 0 ),
      M_right_child( 0 )
{
    time_t tmp_time = std::time( NULL );
    tm* tmp_local_time = localtime( &tmp_time );
    if ( tmp_local_time == NULL )
    {
        std::cerr << __FILE__ << ":" << __LINE__
                  << ": Error getting time: "
                  << strerror( errno ) << std::endl;
        //this->exit( EXIT_FAILURE );
        disable();
        return;
    }
    m_real_time = *tmp_local_time;

    srand( tmp_time );
    srandom( tmp_time );
    rcss::random::DefaultRNG::instance( static_cast< rcss::random::DefaultRNG::result_type >( tmp_time ) );

    // !!! registration order is very important !!!
    // TODO: fix the dependencies between referees.
    M_referees.push_back( new TimeRef( *this ) );
    M_referees.push_back( new BallStuckRef( *this ) );
    M_referees.push_back( new OffsideRef( *this ) );
    M_referees.push_back( new FreeKickRef( *this ) );
    M_referees.push_back( new TouchRef( *this ) );
    M_referees.push_back( new CatchRef( *this ) );
    M_referees.push_back( new KeepawayRef( *this ) );
    M_referees.push_back( new PenaltyRef( *this ) );

    M_movable_objects.reserve( MAX_PLAYER*2 + 1 );
}

Stadium::~Stadium()
{
    for ( std::vector< HeteroPlayer * >::iterator it = M_player_types.begin();
          it != M_player_types.end();
          ++it )
    {
        if ( *it != NULL )
        {
            delete *it;
        }
    }
    M_player_types.clear();

    for ( std::list< Referee * >::iterator i = M_referees.begin();
          i != M_referees.end();
          ++i )
    {
        delete *i;
    }
    M_referees.clear();

    for ( MonitorCont::iterator i = M_monitors.begin();
          i != M_monitors.end();
          ++i )
    {
        if ( !(*i)->connected() )
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

    delete M_team_l; M_team_l = NULL;
    delete M_team_r; M_team_r = NULL;

    delete M_coach; M_coach = NULL;
    delete M_ball; M_ball = NULL;

    //std::cout << "\nDestructed Stadium" << std::endl;
}



/*
 *===================================================================
 *Part: Create Stadium Window
 *===================================================================
 */
bool
Stadium::init()
{
    M_game_over_wait = ServerParam::instance().gameOverWait();

    // we create the result savers now, so that if there are any
    // errors creating them, it will be reported before
    // the game starts, not after it has finished.
    std::list< const char* > savers = rcss::ResultSaver::factory().list();
    for ( std::list< const char* >::iterator i = savers.begin();
          i != savers.end();
          ++i )
    {
        rcss::ResultSaver::Creator creator;
        if ( rcss::ResultSaver::factory().getCreator( creator, *i ) )
        {
            rcss::ResultSaver::Ptr saver = creator();
            std::cout << saver->getName() << ": Ready\n";
            m_savers.push_back( saver );
        }
        else
        {
            std::cerr << *i << ": error loading" << std::endl;
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
        LandmarkReader * reader
            = new LandmarkReader( M_field, ServerParam::instance().landmarkFile() );
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

    if ( M_player_socket.bind( rcss::net::Addr( ServerParam::instance().playerPort() ) )
         && M_offline_coach_socket.bind( rcss::net::Addr( ServerParam::instance().offlineCoachPort() ) )
         && M_online_coach_socket.bind( rcss::net::Addr( ServerParam::instance().onlineCoachPort() ) ) )
    {
        if ( M_player_socket.setNonBlocking() == -1
             || M_offline_coach_socket.setNonBlocking() == -1
             || M_online_coach_socket.setNonBlocking() == -1 )
        {
            std::cerr << "Error setting sockets non-blocking: "
                      << strerror( errno ) << std::endl;
            disable();
            return false;
        }
    }
    else
    {
        std::cerr << "Error initializing sockets: "
                  << strerror( errno ) << std::endl;
        disable();
        return false;
    }

    if ( ServerParam::instance().textLogging() )
		{
        if ( ! openTextLog() )
        {
            disable();
            return false;
        }
		}

    if ( ServerParam::instance().gameLogging() )
    {
				if ( ! openGameLog() )
        {
            disable();
            return false;
        }
    }

    if ( ServerParam::instance().keepAwayMode()
         && ServerParam::instance().kawayLogging() )
    {
        if ( ! openKawayLog() )
        {
            disable();
            return false;
        }
    }

    M_weather.init();

    initObjects();

    change_play_mode( PM_BeforeKickOff );

    for ( int i = 0; i <= MAX_PLAYER * 2; ++i )
    {
        M_dinfo.body.show.pos[i].enable = htons( DISABLE );
    }
    M_dinfo.mode = htons( SHOW_MODE );
    std::memset( &M_dinfo.body.show.team[0].name, 0, sizeof(M_dinfo.body.show.team[0].name) );
    std::memset( &M_dinfo.body.show.team[1].name, 0, sizeof(M_dinfo.body.show.team[0].name) );
    M_dinfo.body.show.time = htons((unsigned short)-1);

    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        M_dinfo2.body.show.pos[i].mode = htons( DISABLE );
    }
    M_dinfo2.mode = htons( SHOW_MODE );
    std::memset( &M_dinfo2.body.show.team[0].name, 0, sizeof(M_dinfo.body.show.team[0].name) );
    std::memset( &M_dinfo2.body.show.team[1].name, 0, sizeof(M_dinfo.body.show.team[1].name) );
    M_dinfo2.body.show.time = htons((unsigned short)-1);


    M_kick_off_wait = std::max( 0, ServerParam::instance().kickOffWait() );
    M_connect_wait = std::max( 0, ServerParam::instance().connectWait() );

    return true;
}

void
Stadium::startTeams()
{
    if ( playmode() != PM_PlayOn )
    {
        if( M_left_child == 0
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
Stadium::startTeam( const std::string& start )
{
    if ( start.empty() )
        return 0;

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
        execlp( "/bin/sh", "sh", "-c", start.c_str(), (char *)NULL );
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
Stadium::teamConnected( Side side )
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
        return NULL;
    }
}

void
Stadium::initObjects()
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

    M_coach = new Coach( *this );

    M_olcoaches[0] = new OnlineCoach( *this );
    M_team_l->assignCoach( M_olcoaches[0] );

    M_olcoaches[1] = new OnlineCoach( *this );
    M_team_r->assignCoach( M_olcoaches[1] );
}

Player *
Stadium::initPlayer( const char * init_message,
                     const rcss::net::Addr & addr )

{
    // (init <TeamName> [(version <Ver>)][ (goalie)])
    const char * msg = init_message;

    char teamname[16];
    double version = 3.0;
    bool goalie = false;

    if ( std::strncmp( msg, "(init ", std::strlen( "(init " ) ) != 0 )
    {
        if ( ServerParam::instance().verboseMode() )
            std::cerr << "Warning:Illegal initialize message." << std::endl
                      << "   message = " << init_message << std::endl;
        sendToPlayer( "(error unknown_command)", addr );
        return NULL;
    }

    int n_read = 0;
    if ( std::sscanf( msg, " ( init %15[-_a-zA-Z0-9] %n ",
                      teamname, &n_read ) != 1
         || n_read == 0 )
    {
        if ( ServerParam::instance().verboseMode() )
            std::cerr << "Warning: Illegal team name string.\n"
                      << "   message = " << init_message << std::endl;
        sendToPlayer( "(error illegal_teamname)", addr );
        return NULL;
    }
    msg += n_read;

    if ( *msg != '(' && *msg != ')' && ! std::isspace( *msg ) )
    {
        if ( ServerParam::instance().verboseMode() )
            std::cerr << "Warning: Illegal team name string or too long team name.\n"
                      << "   message = " << init_message << std::endl;
        sendToPlayer( "(error illegal_teamname_or_too_long_teamname)", addr );
        return NULL;
    }

    while ( *msg != '\0' && *msg != '(' ) ++msg;

    while ( *msg != '\0' && *msg != ')' )
    {
        if ( ! std::strncmp( msg, "(version ", std::strlen( "(version " ) ) )
        {
            if ( std::sscanf( msg, " ( version %lf ) %n ",
                              &version, &n_read ) != 1
                 || n_read == 0 )
            {
                if ( ServerParam::instance().verboseMode() )
                    std::cerr << "Warning: Illegal initialize message.\n"
                              << "   message = " << init_message << std::endl;
                sendToPlayer( "(error illegal_command_form)", addr );
                return NULL;
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
            if ( ServerParam::instance().verboseMode() )
                std::cerr << "Warning: Illegal initialize message.\n"
                          << "   message = " << init_message << std::endl;
            sendToPlayer( "(error illegal_command_form)", addr );
            return NULL;
        }

        if ( *msg != '(' && *msg != ')' && ! std::isspace( *msg ) )
        {
            if ( ServerParam::instance().verboseMode() )
                std::cerr << "Warning: Illegal initialize message.\n"
                          << "   message = " << init_message << std::endl;
            sendToPlayer( "(error illegal_command_form)", addr );
            return NULL;
        }

        while ( *msg != '\0' && std::isspace( *msg ) ) ++msg;
    }

    return newPlayer( teamname, version, goalie, addr );
}


Player*
Stadium::newPlayer( const char * teamname,
                    const double & version,
                    const bool goalie_flag,
                    const rcss::net::Addr & addr )
{
    Team *tm;

    if ( M_team_l->name().empty() )
    {
        tm = M_team_l;
        tm->setName( teamname );
    }
    else if ( M_team_l->name() == teamname )
    {
        tm = M_team_l;
    }
    else if ( M_team_r->name().empty() )
    {
        tm = M_team_r;
        tm->setName( teamname );
    }
    else if ( M_team_r->name() == teamname )
    {
        tm = M_team_r;
    }
    else
    {
        if ( ServerParam::instance().verboseMode() )
            std::cerr << "Warning:Too many teams. [teamname = '"
                      << teamname << "']" << std::endl;
        sendToPlayer( "(error no_more_team)", addr );
        return NULL;
    }

    Player * p = tm->newPlayer( version, goalie_flag );

    if ( p == NULL )
    {
        sendToPlayer( "(error no_more_player_or_goalie_or_invalid_client_version)", addr );
        return NULL;
    }

    if ( ! p->connect( addr ) )
    {
        sendToPlayer( "(error connection_failed)", addr );
        p->disable();
        return NULL;
    }

    addListener( p );
    M_remote_players.push_back( p );
    M_movable_objects.push_back( p );

    p->setEnforceDedicatedPort( version >= 8.0 );
    p->sendInit();

    return p;
}

Player*
Stadium::reconnectPlayer( const char * reconnect_message,
                          const rcss::net::Addr & addr )

{
    char teamname[128];
    int rnum;

    if ( std::sscanf( reconnect_message,
                      "(reconnect %127s %d)",
                      teamname, &rnum ) < 2 )
    {
        if ( ServerParam::instance().verboseMode() )
            std::cerr << "Warning:Illegal reconnect message.\n"
                      << "   message = " << reconnect_message << std::endl;
        sendToPlayer( "(error illegal_command_form)", addr );
        return NULL;
    }

    int r = 0;
    for ( r = 0; r < MAX_PLAYER * 2; ++r )
    {
        if ( M_players[r]->team()->enabled() )
        {
            if ( M_players[r]->team()->name() == teamname
                 && M_players[r]->unum() == rnum )
            {
                break;
            }
        }
    }

    /* It's same. */
    if ( r < MAX_PLAYER * 2 )
    {
        if ( M_players[r]->open() != 0 )
        {
            sendToPlayer( "(error socket_open_failed)", addr );
            return NULL;
        }
        if ( ! M_players[r]->connect( addr ) )
        {
            sendToPlayer( "(error connection_failed)", addr );
            return NULL;
        }
        if ( ! M_players[r]->setSenders() )
        {
            sendToPlayer( "(error illegal_client_version)", addr );
            return NULL;
        }

        addListener( M_players[r] );
        M_remote_players.push_back( M_players[r] );

        M_players[r]->setEnforceDedicatedPort( M_players[r]->version() >= 8.0 );
        M_players[r]->setEnable();
        M_players[r]->sendReconnect();
        return M_players[r];
    }
    else
    {
        sendToPlayer( "(error no_such_team_or_player)", addr );
        return NULL;
    }
}

Coach *
Stadium::initCoach( const char * init_message,
                    const rcss::net::Addr & addr )
{
    if ( std::strncmp( init_message, "(init ", std::strlen( "(init " ) ) != 0 )
    {
        if ( ServerParam::instance().verboseMode() )
            std::cerr << "Warning:Illegal initialize message." << std::endl
                      << "   message = " << init_message << std::endl;
        sendToCoach( "(error unknown_command)", addr );
        return NULL;
    }

    double ver = 3.0;
    int n = std::sscanf( init_message, "(init (version %lf))", &ver );
    if ( ( n != 0 && n != 1 )
         || ver < 1.0 )
    {
        sendToCoach( "(error illegal_command_form)", addr );
        return NULL;
    }

    if ( M_coach->open() != 0 )
    {
        sendToCoach( "(error socket_open_failed)", addr );
        return NULL;
    }

    if ( ! M_coach->connect( addr ) )
    {
        sendToCoach( "(error connection_failed)", addr );
        return NULL;
    }

    if ( ! M_coach->setSenders( ver ) )
    {
        std::cerr << "Error: Could not find serializer or sender for version"
                  << ver << std::endl;
        sendToCoach( "(error illegal_client_version)", addr );
        return NULL;
    }

    addOfflineCoach( M_coach );
    addListener( M_coach );
    M_coach->setEnforceDedicatedPort( ver >= 8.0 );
    M_coach->sendInit();

    return M_coach;
}

OnlineCoach *
Stadium::initOnlineCoach( const char * init_message,
                          const rcss::net::Addr & addr )

{
    // (init <TeamName>[ <CoachName>][ (version <Ver>)])

    const char * msg = init_message;

		const double default_olcoach_version = 5.0;

		char teamname[16];
    char coachname[128];
		double version = default_olcoach_version;

    std::memset( teamname, 0, 16 );
    std::memset( coachname, 0, 128 );

    if ( std::strncmp( msg, "(init", std::strlen( "(init" ) ) != 0 )
    {
        if ( ServerParam::instance().verboseMode() )
            std::cerr << "Warning:Illegal initialize message." << std::endl
                      << "   message = " << init_message << std::endl;
        sendToOnlineCoach( "(error unknown_command)", addr );
        return NULL;
    }

    int n_read = 0;

    // read team name
    if ( std::sscanf( msg, " ( init %15[-_a-zA-Z0-9] %n ",
                      teamname, &n_read ) != 1
         || n_read == 0 )
    {
        if ( ServerParam::instance().verboseMode() )
            std::cerr << "Warning:Illegal team name.\n"
                      << "   message = " << init_message << std::endl;
        sendToOnlineCoach( "(error illegal_command_form)", addr );
        return NULL;
    }
    msg += n_read;

    // read coach name
    if ( *msg != '(' )
    {
        if ( std::sscanf( msg, " %127[-_a-zA-Z0-9] %n ",
                          coachname, &n_read ) != 1 )
        {
            if ( ServerParam::instance().verboseMode() )
                std::cerr << "Warning:Illegal coach name.\n"
                          << "   message = " << init_message << std::endl;
            sendToOnlineCoach( "(error illegal_command_form)", addr );
            return NULL;
        }
        msg += n_read;

        if ( *msg != '(' && *msg != ')' && ! std::isspace( *msg ) )
        {
            if ( ServerParam::instance().verboseMode() )
                std::cerr << "Warning: Illegal coach name string or Too long coach name.\n"
                          << "   message = " << init_message << std::endl;
            sendToOnlineCoach( "(error illegal_coachname_or_too_long_coachname)", addr );
            return NULL;
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
            if ( ServerParam::instance().verboseMode() )
                std::cerr << "Warning: Illegal coach version.\n"
                          << "   message = " << init_message << std::endl;
            sendToPlayer( "(error illegal_command_form)", addr );
            return NULL;
        }
        msg += n_read;

        if ( *msg != '(' && *msg != ')' && ! std::isspace( *msg ) )
        {
            if ( ServerParam::instance().verboseMode() )
                std::cerr << "Warning: Illegal client version.\n"
                          << "   message = " << init_message << std::endl;
            sendToOnlineCoach( "(error illegal_client_version)", addr );
            return NULL;
        }

        if ( version < default_olcoach_version )
        {
            if ( ServerParam::instance().verboseMode() )
                std::cerr << "Warning: No online coach for versions less than than 5.00" << std::endl;
            sendToOnlineCoach( "(error illegal_client_version)", addr );
            return NULL;
        }
    }

    OnlineCoach* olc = NULL;

    if ( ! M_team_l->name().empty()
         && M_team_l->name() == teamname )
    {
        olc = M_team_l->olcoach();
        olc->setSide( LEFT );
    }
    else if ( ! M_team_r->name().empty()
              && M_team_r->name() == teamname )
    {
        olc = M_team_r->olcoach();
        olc->setSide( RIGHT );
    }
    else
    {
        sendToOnlineCoach( "(error no_such_team)", addr );
        return NULL;
    }

    if ( olc->assigned() )
    {
        sendToOnlineCoach( "(error already_have_coach)", addr );
        return NULL;
    }

    if ( ! olc->connect( addr ) )
    {
        sendToOnlineCoach( "(error connection_failed)", addr );
        return NULL;
    }

    if ( ! olc->setSenders( version ) )
    {
        std::cerr << "Error: Could not find serializer or sender for version "
                  << (int)olc->version() << std::endl;
        sendToOnlineCoach( "(error illegal_client_version)", addr );
        return NULL;
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
Stadium::stepBegin()
{
    const PlayerCont::iterator end = M_players.end();
    for ( PlayerCont::iterator p = M_players.begin();
          p != end;
          ++p )
    {
        (*p)->resetCommandFlags();
        (*p)->incArmAge();
    }

    for ( int i = 0; i < 2; ++i )
    {
        M_olcoaches[i]->resetCommandFlags();
        M_olcoaches[i]->check_message_queue( time() );
        M_olcoaches[i]->update_messages_left( time() );
    }

    M_coach->resetCommandFlags();
}

void
Stadium::step()
{
    stepBegin();

    if ( playmode() == PM_BeforeKickOff )
    {
        turnMovableObjects();
        ++M_stoppage_time;
        for_each( M_referees.begin(), M_referees.end(), &Referee::doAnalyse );
    }
    else if ( playmode() == PM_AfterGoal_Right
              || playmode() == PM_AfterGoal_Left
              || playmode() == PM_OffSide_Right
              || playmode() == PM_OffSide_Left
              || playmode() == PM_Back_Pass_Right
              || playmode() == PM_Back_Pass_Left
              || playmode() == PM_Free_Kick_Fault_Right
              || playmode() == PM_Free_Kick_Fault_Left
              || playmode() == PM_CatchFault_Right
              || playmode() == PM_CatchFault_Left )
    {
        M_ball_catcher = NULL;
        incMovableObjects();
        ++M_stoppage_time;
        for_each( M_referees.begin(), M_referees.end(), &Referee::doAnalyse );
    }
    else if ( playmode() != PM_BeforeKickOff && playmode() != PM_TimeOver )
    {
        incMovableObjects();

        ++M_time;
        M_stoppage_time = 0;

        move_caught_ball();
        for_each( M_referees.begin(), M_referees.end(), &Referee::doAnalyse );
        //for_each( M_referees.begin(), M_referees.end(), std::mem_fun( &Referee::analyse ) );

        if ( time() > 0
             && ServerParam::instance().nrNormalHalfs() > 0
             && time() % ( ServerParam::instance().nrNormalHalfs()
                           * ServerParam::instance().halfTime() ) == 0 )
        {
            //every game time cycles, give the online coach more messages
            M_olcoaches[0]->awardFreeformMessageCount();
            M_olcoaches[1]->awardFreeformMessageCount();
        }

    }

    stepEnd();
}

void
Stadium::stepEnd()
{
    //
    // update stamina
    //
    const PlayerCont::iterator end = M_players.end();
    for ( PlayerCont::iterator p = M_players.begin();
          p != end;
          ++p )
    {
        if ( (*p)->state() == DISABLE ) continue;

        (*p)->updateStamina();
        (*p)->updateCapacity();
    }

    sendDisp();
}

void
Stadium::turnMovableObjects()
{
    std::random_shuffle( M_movable_objects.begin(),
                         M_movable_objects.end(),
                         irand ); //rcss::random::UniformRNG::instance() );
    const MPObjectCont::iterator end = M_movable_objects.end();
    for ( MPObjectCont::iterator it = M_movable_objects.begin();
          it != end;
          ++it )
    {
        (*it)->_turn();
    }
}

void
Stadium::incMovableObjects()
{
    std::random_shuffle( M_movable_objects.begin(),
                         M_movable_objects.end(),
                         irand ); //rcss::random::UniformRNG::instance() );
    const MPObjectCont::iterator end = M_movable_objects.end();
    for ( MPObjectCont::iterator it = M_movable_objects.begin();
          it != end;
          ++it )
    {
        if ( (*it)->isEnable() )
        {
            (*it)->_inc();
        }
    }

    collisions();
}

void
Stadium::sendDisp()
{
    makeMonitorMessage();
    sendToMonitors();
    writeCurrentGameLog();

    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        M_players[i]->resetState();
    }
}

void
Stadium::makeMonitorMessage()
{
    if ( M_nr_monitor_v1 > 0
         || ( game_log_open()
              && ( ServerParam::instance().gameLogVersion() == REC_OLD_VERSION
                   || ServerParam::instance().gameLogVersion() == REC_VERSION_2 )
              )
         )
    {
        M_dinfo.body.show.time = htons( time() );

        M_dinfo.body.show.pos[0].x = htons( (Int16)rint( ( M_ball->pos().x * SHOWINFO_SCALE ) ) );
        M_dinfo.body.show.pos[0].y = htons( (Int16)rint( ( M_ball->pos().y * SHOWINFO_SCALE ) ) );

        for ( int i = 0; i < MAX_PLAYER * 2; ++i )
        {
            M_dinfo.body.show.pos[i+1].enable = htons( M_players[i]->state() );
            //M_players[i]->alive &= (STAND | GOALIE | DISCARD);
            M_dinfo.body.show.pos[i+1].x
                = htons( (Int16)rint( M_players[i]->pos().x * SHOWINFO_SCALE ) );
            M_dinfo.body.show.pos[i+1].y
                = htons( (Int16)rint( M_players[i]->pos().y * SHOWINFO_SCALE ) );
            M_dinfo.body.show.pos[i+1].angle = htons( (Int16)Rad2IDegRound( M_players[i]->angleBodyCommitted() ) );
            M_dinfo.body.show.pos[i+1].side = htons( M_players[i]->team()->side() );
            M_dinfo.body.show.pos[i+1].unum = htons( M_players[i]->unum() );
        }

        if ( ! M_team_l->name().empty() )
        {
            std::strncpy( M_dinfo.body.show.team[0].name, M_team_l->name().c_str(), 16 );
            M_dinfo.body.show.team[0].score = htons( M_team_l->point() );
        }

        if ( ! M_team_r->name().empty() )
        {
            std::strncpy( M_dinfo.body.show.team[1].name, M_team_r->name().c_str(), 16 );
            M_dinfo.body.show.team[1].score = htons( M_team_r->point() );
        }
    }

    if ( M_nr_monitor_v2 > 0
         || ( game_log_open()
              && ServerParam::instance().gameLogVersion() == REC_VERSION_3 )
         )
    {
        M_dinfo2.body.show.time = htons( time() );

        M_dinfo2.body.show.ball.x = htonl( (Int32)rint( ( M_ball->pos().x * SHOWINFO_SCALE2 ) ) );
        M_dinfo2.body.show.ball.y = htonl( (Int32)rint( ( M_ball->pos().y * SHOWINFO_SCALE2 ) ) );
        M_dinfo2.body.show.ball.deltax = htonl( (Int32)rint( ( M_ball->vel().x * SHOWINFO_SCALE2 ) ) );
        M_dinfo2.body.show.ball.deltay = htonl( (Int32)rint( ( M_ball->vel().y * SHOWINFO_SCALE2) ) );

        for ( int i = 0; i < MAX_PLAYER * 2; ++i )
        {
            M_dinfo2.body.show.pos[i].mode = htons( M_players[i]->state() );
            M_dinfo2.body.show.pos[i].type = htons( M_players[i]->playerTypeId() );

            //M_players[i]->alive &= (STAND | GOALIE | DISCARD);
            M_dinfo2.body.show.pos[i].x = htonl( (Int32)rint( M_players[i]->pos().x * SHOWINFO_SCALE2 ) );
            M_dinfo2.body.show.pos[i].y = htonl( (Int32)rint( M_players[i]->pos().y * SHOWINFO_SCALE2 ) );
            M_dinfo2.body.show.pos[i].deltax = htonl( (Int32)rint( M_players[i]->vel().x * SHOWINFO_SCALE2 ) );
            M_dinfo2.body.show.pos[i].deltay = htonl( (Int32)rint( M_players[i]->vel().y * SHOWINFO_SCALE2 ) );
            M_dinfo2.body.show.pos[i].body_angle = htonl( (Int32)rint( M_players[i]->angleBodyCommitted() * SHOWINFO_SCALE2 ) );
            M_dinfo2.body.show.pos[i].head_angle = htonl( (Int32)rint( M_players[i]->angleNeckCommitted() * SHOWINFO_SCALE2 ) );
            M_dinfo2.body.show.pos[i].view_width = htonl( (Int32)rint( M_players[i]->visibleAngle() * SHOWINFO_SCALE2 ) );
            M_dinfo2.body.show.pos[i].view_quality = htons( (Int16)rint( M_players[i]->highquality() ) );

            M_dinfo2.body.show.pos[i].stamina = htonl( (Int32)rint( M_players[i]->stamina() * SHOWINFO_SCALE2 ) );
            M_dinfo2.body.show.pos[i].effort = htonl( (Int32)rint( M_players[i]->effort() * SHOWINFO_SCALE2 ) );
            M_dinfo2.body.show.pos[i].recovery = htonl( (Int32)rint( M_players[i]->recovery() * SHOWINFO_SCALE2 ) );

            M_dinfo2.body.show.pos[i].kick_count = htons( M_players[i]->kickCount() );
            M_dinfo2.body.show.pos[i].dash_count = htons( M_players[i]->dashCount() );
            M_dinfo2.body.show.pos[i].turn_count = htons( M_players[i]->turnCount() );
            M_dinfo2.body.show.pos[i].say_count = htons( M_players[i]->sayCount() );
            M_dinfo2.body.show.pos[i].tneck_count = htons( M_players[i]->turnNeckCount() );
            M_dinfo2.body.show.pos[i].catch_count = htons( M_players[i]->catchCount() );
            M_dinfo2.body.show.pos[i].move_count = htons( M_players[i]->moveCount() );
            M_dinfo2.body.show.pos[i].chg_view_count = htons( M_players[i]->changeViewCount() );
        }

        if ( ! M_team_l->name().empty() )
        {
            std::strncpy( M_dinfo2.body.show.team[0].name, M_team_l->name().c_str(), 16 );
            M_dinfo2.body.show.team[0].score = htons( M_team_l->point() );
        }
        else
        {
            std::strncpy( M_dinfo2.body.show.team[0].name, "", 16 );
            M_dinfo2.body.show.team[0].score = htons( 0 );
        }

        if ( ! M_team_r->name().empty() )
        {
            std::strncpy( M_dinfo2.body.show.team[1].name, M_team_r->name().c_str(), 16 );
            M_dinfo2.body.show.team[1].score = htons( M_team_r->point() );
        }
        else
        {
            std::strncpy( M_dinfo2.body.show.team[1].name, "", 16 );
            M_dinfo2.body.show.team[1].score = htons( 0 );
        }
    }

}

void
Stadium::sendToMonitors()
{
    // send to displays
    for ( MonitorCont::iterator i = M_monitors.begin();
          i != M_monitors.end();
          ++i )
    {
        if ( (*i)->version() >= 3.0 )
        {
            (*i)->sendShow();
        }
        else if ( (*i)->version() >= 2.0 )
        {
            (*i)->RemoteClient::send( reinterpret_cast< const char * >( &M_dinfo2 ),
                                      sizeof( dispinfo_t2 ) );
        }
        else if ( (*i)->version() >= 1.0 )
        {
            (*i)->RemoteClient::send( reinterpret_cast< const char * >( &M_dinfo ),
                                      sizeof( dispinfo_t ) );
        }
    }
}

void
Stadium::writeCurrentGameLog()
{
    /* TH - 2-NOV-2000 */
    static bool wrote_final_cycle = false;

    if ( game_log_open() )
	  {
        if ( playmode() != PM_BeforeKickOff && playmode() != PM_TimeOver )
        {
            if ( ServerParam::instance().gameLogVersion() == REC_VERSION_4 )
            {
                writeCurrentGameLogV4();
            }
            else if ( ServerParam::instance().gameLogVersion() == REC_VERSION_3 )
            {
                writeGameLog( &M_dinfo2 );
            }
            else if ( ServerParam::instance().gameLogVersion() == REC_VERSION_2
                      || ServerParam::instance().gameLogVersion() == REC_OLD_VERSION )
            {
                writeGameLog( &M_dinfo );
            }
        }
        else if ( playmode() == PM_TimeOver && ! wrote_final_cycle )
        {
            if ( ServerParam::instance().gameLogVersion() == REC_VERSION_4 )
            {
                writeCurrentGameLogV4();
            }
            else if ( ServerParam::instance().gameLogVersion() == REC_VERSION_3 )
            {
                writeGameLog( &M_dinfo2 );
            }
            else if ( ServerParam::instance().gameLogVersion() == REC_VERSION_2
                      || ServerParam::instance().gameLogVersion() == REC_OLD_VERSION )
            {
                writeGameLog( &M_dinfo );
            }
            wrote_final_cycle = true;
        }
	  }
}

void
Stadium::writeCurrentGameLogV4()
{
    static char * playmode_strings[] = PLAYMODE_STRINGS;

    const double prec = 0.0001;
    const double dprec = 0.001;

    std::ostream & os = gameLogStream();

    static PlayMode pm = PM_Null;
    static std::string team_l_name, team_r_name;
    static int team_l_score = 0, team_r_score = 0;
    static int team_l_pen_score = 0, team_r_pen_score = 0;

    // if playmode has changed wirte playmode
    if ( pm != playmode() )
    {
        pm = playmode();

        os << "(playmode " << playmode_strings[pm] << ")\n";
    }

    // if teams or score has changed, write teams and score
    if ( team_l_score != M_team_l->point()
         || team_l_pen_score != M_team_l->penaltyPoint()
         || team_r_score != M_team_r->point()
         || team_r_pen_score != M_team_r->penaltyPoint()
         || M_team_l->name() != team_l_name
         || M_team_r->name() != team_r_name
         )
    {
        team_l_name = M_team_l->name();
        team_r_name = M_team_r->name();
        team_l_score = M_team_l->point();
        team_r_score = M_team_r->point();
        team_l_pen_score = M_team_l->penaltyPoint();
        team_r_pen_score = M_team_r->penaltyPoint();

        os << "(team " << ( team_l_name.empty() ? "null" : team_l_name.c_str() )
           << ' ' << ( team_r_name.empty() ? "null" : team_r_name.c_str() )
           << ' ' << team_l_score
           << ' ' << team_r_score;
        if ( M_team_l->penaltyTaken() > 0 )
        {
            os << ' ' << team_l_pen_score
                << ' ' << team_r_pen_score;
        }
        os << ")\n";
    }

    os << "(show " << time();
    os << " (" << BALL_NAME_SHORT
       << ' ' << Quantize( ball().pos().x, prec )
       << ' ' << Quantize( ball().pos().y, prec )
       << ' ' << Quantize( ball().vel().x, prec )
       << ' ' << Quantize( ball().vel().y, prec )
       << ')';

    const Stadium::PlayerCont::const_iterator end = players().end();
    for ( Stadium::PlayerCont::const_iterator p = players().begin();
          p != end;
          ++p )
    {
        os << " (";
        os << "(" << SideStr( (*p)->team()->side() )
           << ' ' << (*p)->unum()
           << ')';
        os << ' ' << (*p)->playerTypeId()
           << ' ' << (*p)->state(); // include goalie flag
        os << ' ' << Quantize( (*p)->pos().x, prec )
           << ' ' << Quantize( (*p)->pos().y, prec )
           << ' ' << Quantize( (*p)->vel().x, prec )
           << ' ' << Quantize( (*p)->vel().y, prec )
           << ' ' << Quantize( Rad2Deg( (*p)->angleBodyCommitted() ), dprec )
           << ' ' << Quantize( Rad2Deg( (*p)->angleNeckCommitted() ), dprec );
        if ( (*p)->arm().isPointing() )
        {
            rcss::geom::Vector2D arm_dest;
            if ( (*p)->arm().getRelDest( rcss::geom::Vector2D( (*p)->pos().x,
                                                               (*p)->pos().y ),
                                         (*p)->angleBodyCommitted()
                                         + (*p)->angleNeckCommitted(),
                                         arm_dest ) )
            {
                os << ' ' << Quantize( arm_dest.getMag(), prec )
                   << ' ' << Quantize( Rad2Deg( arm_dest.getHead() ), dprec );
            }
        }

        os << " (v "
           << ( (*p)->highquality() ? "h " : "l " )
           << Quantize( Rad2Deg( (*p)->visibleAngle() ), dprec )
           << ')';
        os << " (s "
           << (*p)->stamina() << ' '
           << (*p)->effort() << ' '
           << (*p)->recovery() << ')';
        if ( (*p)->state() != DISABLE
             && (*p)->getFocusTarget() != NULL )
        {
            os << " (f " << SideStr( (*p)->getFocusTarget()->team()->side() )
               << ' ' << (*p)->getFocusTarget()->unum()
               << ')';
        }
        os << " (c "
           << (*p)->kickCount()
           << ' ' << (*p)->dashCount()
           << ' ' << (*p)->turnCount()
           << ' ' << (*p)->catchCount()
           << ' ' << (*p)->moveCount()
           << ' ' << (*p)->turnNeckCount()
           << ' ' << (*p)->changeViewCount()
           << ' ' << (*p)->sayCount()
           << ' ' << (*p)->tackleCount()
           << ' ' << (*p)->arm().getCounter()
           << ' ' << (*p)->attentiontoCount()
           << ')';
        os << ')'; // end of player
    }

    os << ')' << std::endl; // new line & flush
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
Stadium::set_ball( const Side kick_off_side,
                   const PVector & pos,
                   const PVector & vel )
{
    M_ball->moveTo( pos,
                    vel,
                    PVector( 0.0, 0.0 ) );
    M_kick_off_side = kick_off_side;
}

void
Stadium::setHalfTime( const Side kick_off_side,
                      const int half_time_count )
{
    M_ball_catcher = NULL;
    set_ball( kick_off_side, PVector( 0.0, 0.0 ) );
    change_play_mode( PM_BeforeKickOff );
    M_time = ServerParam::instance().halfTime() * half_time_count;
    M_weather.wind_vector.x *= -1;
    M_weather.wind_vector.y *= -1;
}

void
Stadium::recoveryPlayers()
{
    const PlayerCont::iterator end = M_players.end();
    for ( PlayerCont::iterator p = M_players.begin();
          p != end;
          ++p )
    {
        if ( (*p)->state() == DISABLE )
        {
            continue;
        }

        (*p)->recoverAll();
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
        if ( M_players[i]->state() == DISABLE ) continue;

        if ( ! fld.inArea( M_players[i]->pos() ) )
        {
            M_players[i]->moveTo( fld.nearestEdge( M_players[i]->pos() ) );
        }
    }
}

void
Stadium::change_play_mode( const PlayMode pm )
{
    static char *PlayModeString[] = PLAYMODE_STRINGS;

    M_playmode = pm;
    M_dinfo.body.show.pmode = (char)pm;
    M_dinfo2.body.show.pmode = (char)pm;

    for_each( M_referees.begin(), M_referees.end(),
              Referee::doPlayModeChange( pm ) );

    if ( pm == PM_KickOff_Left
         || pm == PM_KickIn_Left
         || pm == PM_FreeKick_Left
         || pm == PM_IndFreeKick_Left
         || pm == PM_CornerKick_Left
         || pm == PM_GoalKick_Left )
    {
        M_kick_off_side = LEFT;
    }
    else if ( pm == PM_KickOff_Right
              || pm == PM_KickIn_Right
              || pm == PM_FreeKick_Right
              || pm == PM_IndFreeKick_Right
              || pm == PM_CornerKick_Right
              || pm == PM_GoalKick_Right )
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
        say( PlayModeString[pm] );
    }
}

void
Stadium::referee_get_foul( const double & x, const double & y,
                           const Side side )
{
    if ( Referee::isPenaltyShootOut( playmode() ) )
    {
        return;
    }

    char msg[8];

    std::snprintf( msg, 8, "foul_%s", SideStr( -side ) );
    say( msg );
    M_ball_catcher = NULL;

    PVector pos = Referee::truncateToPitch( PVector( x, y ) );

    if( side == LEFT )
    {
        placeBall( PM_FreeKick_Left, LEFT, pos );
    }
    else
    {
        placeBall( PM_FreeKick_Right, RIGHT, pos );
    }
    placePlayersInField();
    M_ball_catcher = NULL;
}

void
Stadium::referee_drop_ball( const double & x, const double & y,
                            const Side side )
{
    M_ball_catcher = NULL;

    PVector pos = Referee::truncateToPitch( PVector( x, y ) );

    placeBall( PM_Drop_Ball, side, pos );
    placePlayersInField();

    if ( ! Referee::isPenaltyShootOut( playmode() ) )
    {
        change_play_mode( PM_PlayOn );
    }
}

void
Stadium::discard_player( const Side side,
                         const int unum )
{
    int i = 0;
    for ( i = 0; i < MAX_PLAYER * 2; ++i )
    {
        if ( M_players[i]->team()->side() == side
             && M_players[i]->unum() == unum )
        {
            break;
        }
    }

    if ( i < MAX_PLAYER * 2 )
    {
        M_players[i]->discard();
    }
}

/*
 *===================================================================
 *Part: Weather
 *===================================================================
 */
void Weather::init()
{
    if ( ServerParam::instance().windNone() )
    {
        wind_vector = PVector( 0.0, 0.0 );
        wind_rand = 0.0;
    }
    else if ( ServerParam::instance().windRandom() )
    {
        wind_vector = PVector::fromPolar( drand( 0, 100 ), drand( -M_PI, +M_PI ) );
        wind_rand = drand( 0.0, 0.5 );
    }
    else
    {
        wind_vector = PVector::fromPolar( ServerParam::instance().windForce(),
                                          Deg2Rad( ServerParam::instance().windDir() ) );
        wind_rand = ServerParam::instance().windRand();
    }

#if 1
    std::printf( "wind factor: rand: %f, vector: (%f, %f)\n",
                 wind_rand, wind_vector.x, wind_vector.y );
#endif

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

    Player * player = NULL;
    if ( side == LEFT )
    {
        player = M_players[unum - 1];
    }
    else if ( side == RIGHT )
    {
        player = M_players[MAX_PLAYER + unum - 1];
    }
    else
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
Stadium::_Start( Stadium& stad )
{
    if ( stad.time() % ServerParam::instance().halfTime() == 0 )
    {
        if ( ( stad.time() / ServerParam::instance().halfTime() ) % 2 == 0 )
        {
            if ( ServerParam::instance().nrNormalHalfs() >= 0
                 && stad.time() < ( ServerParam::instance().halfTime()
                                    * ServerParam::instance().nrNormalHalfs() )
                 )
            {
                stad.recoveryPlayers();
            }
            stad.set_ball( LEFT, PVector( 0.0, 0.0 ) );
            if ( stad.time() == 0
                 && ! PlayerParam::instance().allowMultDefaultType() )
            {
                stad.assignPlayerTypes();
            }
            stad.change_play_mode( PM_KickOff_Left );
            std::cout << "Kick_off_left" << std::endl;
        }
        else if ( ( stad.time() / ServerParam::instance().halfTime() ) % 2 == 1 )
        {
            if ( ServerParam::instance().nrNormalHalfs() >= 0
                 && stad.time() < ( ServerParam::instance().halfTime()
                                    * ServerParam::instance().nrNormalHalfs() )
                 )
            {
                stad.recoveryPlayers();
            }
            stad.set_ball( RIGHT, PVector( 0.0, 0.0 ) );
            stad.change_play_mode( PM_KickOff_Right );
            std::cout << "Kick_off_right" << std::endl;
        }
    }
}

bool
Stadium::openTextLog()
{
    boost::filesystem::path dir( ServerParam::instance().textLogDir(),
                                 boost::filesystem::portable_posix_name );
    if ( ! boost::filesystem::exists( dir )
         && ! boost::filesystem::create_directory( dir ) )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't create text log dir. text_log_dir = ["
                  << ServerParam::instance().textLogDir() << ']'
                  << std::endl;
        return false;
    }

    M_text_log_name = ServerParam::instance().textLogDir();
#if defined(_WIN32) || defined(__WIN32__) || defined (WIN32) || defined (__CYGWIN__)
    if ( *M_text_log_name.rbegin() != '\\' )
    {
        M_text_log_name += '\\';
    }
#else
    if ( *M_text_log_name.rbegin() != '/' )
    {
        M_text_log_name += '/';
    }
#endif

    if ( ServerParam::instance().textLogFixed() )
    {
        M_text_log_name += ServerParam::instance().textLogFixedName();
    }
    else
    {
        M_text_log_name += Stadium::DEF_TEXT_NAME;
    }
    M_text_log_name += Stadium::DEF_TEXT_SUFFIX;

    if ( ServerParam::instance().textLogCompression() > 0 )
    {
        M_text_log_name += std::string ( ".gz" );
        M_gz_text_log.open( M_text_log_name.c_str(),
                            ServerParam::instance().textLogCompression() );
    }
    else
    {
        M_text_log.open( M_text_log_name.c_str() );
    }

    if ( ! text_log_open() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't open log_file " << M_text_log_name << std::endl;
        return false;
    }

    return true;
}

bool
Stadium::openGameLog()
{
    boost::filesystem::path dir( ServerParam::instance().gameLogDir(),
                                 boost::filesystem::portable_posix_name );
    if ( ! boost::filesystem::exists( dir )
         && ! boost::filesystem::create_directory( dir ) )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't create game log dir. game_log_dir = ["
                  << ServerParam::instance().gameLogDir() << ']'
                  << std::endl;
        return false;
    }

    M_game_log_name = ServerParam::instance().gameLogDir();
#if defined(_WIN32) || defined(__WIN32__) || defined (WIN32) || defined (__CYGWIN__)
    if ( *M_game_log_name.rbegin() != '\\' )
    {
        M_game_log_name += '\\';
    }
#else
    if ( *M_game_log_name.rbegin() != '/' )
    {
        M_game_log_name += '/';
    }
#endif

    if ( ServerParam::instance().gameLogFixed() )
    {
        M_game_log_name += ServerParam::instance().gameLogFixedName();
    }
    else
    {
        M_game_log_name += Stadium::DEF_GAME_NAME;
    }
    M_game_log_name += Stadium::DEF_GAME_SUFFIX;

    if ( ServerParam::instance().gameLogCompression() > 0 )
    {
        M_game_log_name += ".gz";
        M_gz_game_log.open( M_game_log_name.c_str(),
                            ServerParam::instance().gameLogCompression() );
    }
    else
    {
        M_game_log.open( M_game_log_name.c_str(), ( std::ofstream::binary
                                                    | std::ofstream::out
                                                    | std::ofstream::trunc ) );
    }

    if ( ! game_log_open() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't open record_file " << M_game_log_name << std::endl;
        return false;
    }

    if ( ServerParam::instance().gameLogVersion() != REC_OLD_VERSION )
    {
        // write version information
        char buf[6];
        buf[0] = 'U';
        buf[1] = 'L';
        buf[2] = 'G';
        if ( ServerParam::instance().gameLogVersion() == REC_VERSION_4 )
        {
            buf[3] = '4';
            buf[4] = '\n';
            writeGameLog( buf, 5 );
        }
        else
        {
            buf[3] = ServerParam::instance().gameLogVersion();
            writeGameLog( buf, 4 );
        }

        if ( ServerParam::instance().gameLogVersion() == REC_VERSION_4 )
        {
            rcss::InitSenderCommonV8 init_sender( gameLogStream(),
                                                  rcss::SerializerCommonStdv8::instance(),
                                                  *this,
                                                  999, // version number
                                                  true ); // new line
            init_sender.sendServerParams();
            init_sender.sendPlayerParams();
            init_sender.sendPlayerTypes();
        }
        else if ( ServerParam::instance().gameLogVersion() == REC_VERSION_3 )
        {
            Int16 mode = htons( PARAM_MODE );
            server_params_t stmp = ServerParam::instance().convertToStruct();
            writeGameLog( reinterpret_cast< const char* >( &mode ), sizeof( mode ) );
            writeGameLog( reinterpret_cast< const char* >( &stmp ), sizeof( stmp ) );

            mode = htons( PPARAM_MODE );
            player_params_t ptmp = PlayerParam::instance().convertToStruct();
            writeGameLog( reinterpret_cast< const char* >( &mode ), sizeof( mode ) );
            writeGameLog( reinterpret_cast< const char* >( &ptmp ), sizeof( ptmp ) );

            mode = htons( PT_MODE );
            for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
            {
                try
                {
                    player_type_t pt_tmp = M_player_types.at( i )->convertToStruct( i );
                    writeGameLog( reinterpret_cast< const char* >( &mode ), sizeof( mode ) );
                    writeGameLog( reinterpret_cast< const char* >( &pt_tmp ), sizeof( pt_tmp ) );
                }
                catch ( std::exception & e )
                {
                    std::cerr << __FILE__ << ':' << __LINE__
                              << " Exception caught! " << e.what() << std::endl;
                    return false;
                }
            }
        }

        flushLogs();
    }

    return true;
}


bool
Stadium::openKawayLog()
{
    boost::filesystem::path dir( ServerParam::instance().kawayLogDir(),
                                 boost::filesystem::portable_posix_name );
    if ( ! boost::filesystem::exists( dir )
         && ! boost::filesystem::create_directory( dir ) )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't create keepaway log dir."
                  << std::endl;
        return false;
    }

    M_kaway_log_name = ServerParam::instance().kawayLogDir();
#if defined(_WIN32) || defined(__WIN32__) || defined (WIN32) || defined (__CYGWIN__)
    if ( *M_kaway_log_name.rbegin() != '\\' )
    {
        M_kaway_log_name += '\\';
    }
#else
    if ( *M_kaway_log_name.rbegin() != '/' )
    {
        M_kaway_log_name += '/';
    }
#endif

    if ( ServerParam::instance().kawayLogFixed() )
    {
        M_kaway_log_name += ServerParam::instance().kawayLogFixedName();
    }
    else
    {
        M_kaway_log_name += Stadium::DEF_KAWAY_NAME;
    }
    M_kaway_log_name += Stadium::DEF_KAWAY_SUFFIX;

    M_kaway_log.open( M_kaway_log_name.c_str() );

    if ( ! M_kaway_log.is_open() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't open keepaway_log_file " << M_kaway_log_name
                  << std::endl;
        return false;
    }

    return true;
}


std::ostream &
Stadium::writeGameLog( const char* str, std::streamsize n )
{
#ifdef HAVE_LIBZ
    if ( ServerParam::instance().gameLogCompression() > 0 )
        return M_gz_game_log.write( str, n );
    else
#endif
        return M_game_log.write( str, n );
}

void
Stadium::writeGameLog( const dispinfo_t * msg )
{
    if ( ServerParam::instance().gameLogVersion() >= REC_VERSION_4 )
    {
        std::cerr << __FILE__ << " " << __LINE__
                  << ": dispinfo_t cannot be recorded for rcg version "
                  << ServerParam::instance().gameLogVersion()
                  << std::endl;
    }
    else if ( ServerParam::instance().gameLogVersion() != REC_OLD_VERSION )
    {
        // write the type of next msg
        if ( ntohs( msg->mode ) != MSG_MODE
             || ServerParam::instance().recordMessages() )
        {
            switch ( ntohs( msg->mode ) ) {
            case SHOW_MODE:
                // write a show struct
                writeGameLog( reinterpret_cast< const char* >( &msg->mode ),
                              sizeof( msg->mode ) );
                writeGameLog( reinterpret_cast< const char* >( &msg->body.show ),
                              sizeof( msg->body.show ) );
                break;
            case MSG_MODE:
                {
                    // it is a message write the board info
                    writeGameLog( reinterpret_cast< const char* >( &msg->mode ),
                                  sizeof( msg->mode ) );
                    writeGameLog( reinterpret_cast< const char* >( &msg->body.msg.board ),
                                  sizeof( msg->body.msg.board ) );

                    // calculate the string length and write it
                    Int16 len = 1;
                    while ( (msg->body.msg.message[len-1] != '\0') && (len < 2048) )
                    {
                        ++len;
                    }
                    /* pfr 1/7/00 Need to put length in network byte order */
                    Int16 nlen = htons( len );
                    writeGameLog( reinterpret_cast< const char* >( &nlen ),
                                  sizeof( nlen ) );

                    // write the message
                    writeGameLog( msg->body.msg.message, len );
                }
                break;
            default:
                std::cerr << "writeGameLog : unknown mode dispinfo_t " << ntohs( msg->mode )
                          << std::endl;
                break;
            }
        }
    }
    else
    {
        writeGameLog( reinterpret_cast< const char * >( msg ),
                      sizeof( dispinfo_t ) );
    }
}

void
Stadium::writeGameLog( const dispinfo_t2 * msg )
{
    Int16 mode;
    static char pm = (char)0;
    static team_t teams[2] = { { "", 0 },
                               { "", 0 } };


    // if playmode has changed wirte playmode
    if ( pm != msg->body.show.pmode )
    {
        pm = msg->body.show.pmode;

        mode = htons( PM_MODE );
        writeGameLog( reinterpret_cast< const char* >( &mode ),
                      sizeof( mode ) );
        writeGameLog( reinterpret_cast< const char* >( &pm ),
                      sizeof( pm ) );
    }

    // if teams or score has changed, write teams and score
    if ( std::strlen( teams[ 0 ].name ) != std::strlen( msg->body.show.team[ 0 ].name )
         || std::strcmp( teams[ 0 ].name, msg->body.show.team[ 0 ].name )
         || teams[ 0 ].score != msg->body.show.team[ 0 ].score
         || std::strlen( teams[ 1 ].name ) != std::strlen( msg->body.show.team[ 1 ].name )
         || std::strcmp( teams[ 1 ].name, msg->body.show.team[ 1 ].name )
         || teams[ 1 ].score != msg->body.show.team[ 1 ].score
         )
    {
        std::strncpy( teams[ 0 ].name, msg->body.show.team[ 0 ].name, 16 );
        teams[ 0 ].score = msg->body.show.team[ 0 ].score;
        std::strncpy( teams[ 1 ].name, msg->body.show.team[ 1 ].name, 16 );
        teams[ 1 ].score = msg->body.show.team[ 1 ].score;

        mode = htons( TEAM_MODE );
        writeGameLog( reinterpret_cast< const char* >( &mode ),
                      sizeof( mode ) );
        writeGameLog( reinterpret_cast< const char* >( teams ),
                      sizeof( team_t ) * 2 );

    }

    // write positional data
    short_showinfo_t2 pos_data;
    pos_data.ball = msg->body.show.ball;
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        pos_data.pos[ i ] = msg->body.show.pos[ i ];
    }
    pos_data.time = msg->body.show.time;

    mode = htons( SHOW_MODE );
    writeGameLog( reinterpret_cast< const char* >( &mode ),
                  sizeof( mode ) );
    writeGameLog( reinterpret_cast< const char* >( &pos_data ),
                  sizeof( pos_data ) );
}


void
Stadium::writeMsgToGameLog( const BoardType board_type,
                            const char * msg )
{
    if ( ServerParam::instance().gameLogVersion() == REC_VERSION_4 )
    {
        if ( ServerParam::instance().recordMessages() )
        {
            gameLogStream() << "(msg " << board_type << ' ' << msg << ")\n";
        }
    }
    else if ( ServerParam::instance().gameLogVersion() != REC_OLD_VERSION )
    {
        if ( ServerParam::instance().recordMessages() )
        {
            Int16 mode = htons( MSG_MODE );
            writeGameLog( reinterpret_cast< const char* >( &mode ),
                          sizeof( mode ) );
            Int16 board = htons( board_type );
            writeGameLog( reinterpret_cast< const char* >( &board ),
                          sizeof( board ) );
            // calculate the string length and write it
            Int16 len = 1;
            while ( (msg[len-1] != '\0') && (len < 2048) )
            {
                ++len;
            }
            /* pfr 1/7/00 Need to put length in network byte order */
            Int16 nlen = htons( len );
            writeGameLog( reinterpret_cast< const char* >( &nlen ),
                          sizeof( nlen ) );
            // write the message
            writeGameLog( msg, len );
        }
    }
    else
    {
        dispinfo_t disp;
        disp.mode = htons( MSG_MODE );
        disp.body.msg.board = htons( board_type );
        std::strncpy( disp.body.msg.message,
                      msg,
                      std::min( sizeof( disp.body.msg.message ),
                                std::strlen( msg ) ) );
        writeGameLog( reinterpret_cast< const char * >( &disp ),
                      sizeof( dispinfo_t ) );
    }

}

void
Stadium::writeTextLog( const char * message, int flag )
{
    if ( flag == RECV )
    {
        if ( text_log_open() )
        {
            text_log_stream() << time() << "\t" << message;
        }
    }

    if ( ( flag == RECV || flag == SUBS )
         && ( ( game_log_open()
                && ServerParam::instance().recordMessages() )
              || ServerParam::instance().sendComms() )
         )
    {
        char buf[max_message_length_for_display];
        std::strncpy( buf, message, std::min( max_message_length_for_display,
                                              (int)std::strlen( message ) ) );

        if ( ServerParam::instance().sendComms() )
        {
            for ( Stadium::MonitorCont::iterator i = monitors().begin();
                  i != monitors().end();
                  ++i )
            {
                (*i)->sendMsg( LOG_BOARD, buf );
            }
        }

        if ( game_log_open()
             && ServerParam::instance().recordMessages()
             && playmode() != PM_TimeOver )
        {
            writeMsgToGameLog( LOG_BOARD, buf );
        }
    }
}



void
Stadium::writeTextLog( const Player & p,
                       const char * message,
                       const int flag )
{
    if ( text_log_open()
         || ( game_log_open() && flag == RECV )
         || ( ServerParam::instance().sendComms() && flag == RECV ) )
    {
        char tmp[MaxMesg];
        std::snprintf( tmp, MaxMesg,
                       "%s %s_%d: %s\n",
                       (flag == SEND) ? "Send" : "Recv",
                       p.team()->name().c_str(), p.unum(), message );
        writeTextLog( tmp, flag );
    }
}

void
Stadium::writeTextLog( const Coach &,
                       const char * message,
                       const int flag )
{
    if ( text_log_open()
         || ( game_log_open() && flag == RECV )
         || ( ServerParam::instance().sendComms() && flag == RECV ) )
    {
        char tmp[MaxMesg];
        std::snprintf( tmp, MaxMesg,
                       "%s Coach: %s\n",
                       (flag == SEND) ? "Send" : "Recv",
                       message ) ;
        writeTextLog( tmp, flag );
    }
}

void
Stadium::writeTextLog( const OnlineCoach & p,
                       const char * message,
                       const int flag )
{
    if ( text_log_open()
         || ( game_log_open() && flag == RECV )
         || ( ServerParam::instance().sendComms() && flag == RECV ) )
    {
        char tmp[MaxMesg];
        std::snprintf( tmp, MaxMesg,
                       "%s %s_Coach: %s\n",
                       (flag == SEND) ? "Send" : "Recv",
                       ( p.side() == LEFT ) ? M_team_l->name().c_str() : M_team_r->name().c_str(),
                       message );
        writeTextLog( tmp, flag );
    }
}

// th 6.3.00
void
Stadium::write_times( timeval tp_new, timeval tp_old )
{
    if ( text_log_open()
         && ServerParam::instance().logTimes() )
    {
        double diff = (tp_new.tv_sec - tp_old.tv_sec) * 1000
            + (double)(tp_new.tv_usec - tp_old.tv_usec) / 1000;

        text_log_stream() << time() << "\tCYCLE_TIMES: " << diff << std::endl;
    }
}
//

void
Stadium::write_profile( timeval tv_start, timeval tv_end, char* str )
{
    if ( text_log_open()
         && ServerParam::instance().profile() )
    {
        double diff = (tv_end.tv_sec - tv_start.tv_sec) * 1000
            + (double)(tv_end.tv_usec - tv_start.tv_usec) / 1000;

        text_log_stream() << time() << "\t" << str << ": " << diff << std::endl;
    }
}


void
Stadium::renameLogs()
{
    if ( ! M_team_l
         || ! M_team_r )
    {
        return;
    }

    // add penalty to logfile when penalties are score or was draw and one team won
    bool bAddPenaltyScore = ( M_team_r->point() == M_team_l->point()
                              && M_team_l->penaltyTaken() > 0
                              && M_team_r->penaltyTaken() > 0 );

    char time_str[32];
    std::strftime( time_str, 32,
                   ServerParam::instance().logDateFormat().c_str(),
                   &( realTime() ) );

    std::string team_name_score( "" );

    if ( M_team_l->enabled() )
    {
        team_name_score += M_team_l->name();
        team_name_score += "_";
        if ( ! M_team_l->olcoach()->getName().empty() )
        {
            team_name_score += M_team_l->olcoach()->getName();
            team_name_score += "_";
        }
        team_name_score += boost::lexical_cast< std::string >( M_team_l->point() );
        if ( bAddPenaltyScore )
        {
            team_name_score += "_";
            team_name_score += boost::lexical_cast< std::string >( M_team_l->penaltyPoint() );
            team_name_score += ( M_team_l->penaltyWon() ? "w" : "" );
        }
    }
    else
    {
        team_name_score += "null";
    }

    team_name_score += "-vs-";
    if ( M_team_r->enabled() )
    {
        team_name_score += M_team_r->name();
        team_name_score += "_";
        if ( ! M_team_r->olcoach()->getName().empty() )
        {
            team_name_score += M_team_r->olcoach()->getName();
            team_name_score += "_";
        }
        team_name_score += boost::lexical_cast< std::string >( M_team_r->point() );
        if ( bAddPenaltyScore )
        {
            team_name_score += "_";
            team_name_score += boost::lexical_cast< std::string >( M_team_r->penaltyPoint() );
            team_name_score += ( M_team_r->penaltyWon() ? "w" : "" );
        }
    }
    else
    {
        team_name_score += "null";
    }

    if ( text_log_open()
         && ! ServerParam::instance().textLogFixed() )
    {
        std::string newname = ServerParam::instance().textLogDir();
        if ( *newname.rbegin() != '/' )
        {
            newname += '/';
        }
        if ( ServerParam::instance().textLogDated() )
        {
            newname += time_str;
        }
        newname += team_name_score;
        newname += Stadium::DEF_TEXT_SUFFIX;
        if ( ServerParam::instance().textLogCompression() > 0 )
        {
            newname += ".gz";
        }

        M_text_log.close();
        M_gz_text_log.close();
        if ( std::rename( M_text_log_name.c_str(),
                          newname.c_str() ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": error renaming " << M_text_log_name << std::endl;
        }
        M_text_log_name = newname;
    }

    if ( game_log_open()
         && ! ServerParam::instance().gameLogFixed() )
    {
        std::string newname = ServerParam::instance().gameLogDir();
        if ( *newname.rbegin() != '/' )
        {
            newname += '/';
        }
        if ( ServerParam::instance().gameLogDated() )
        {
            newname += time_str;
        }
        newname += team_name_score;

        newname += Stadium::DEF_GAME_SUFFIX;
        if ( ServerParam::instance().gameLogCompression() > 0 )
        {
            newname += ".gz";
        }

        M_game_log.close();
        M_gz_game_log.close();
        if( std::rename( M_game_log_name.c_str(),
                         newname.c_str() ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": error renaming " << M_game_log_name << std::endl;
        }
        M_game_log_name = newname;
    }

    if ( M_kaway_log.is_open()
         && ! ServerParam::instance().kawayLogFixed() )
    {
        std::string newname = ServerParam::instance().kawayLogDir();
        if ( *newname.rbegin() != '/' )
        {
            newname += '/';
        }
        if ( ServerParam::instance().kawayLogDated() )
        {
            newname += time_str;
        }
        newname += team_name_score;
        newname += Stadium::DEF_KAWAY_SUFFIX;

        M_kaway_log.close();
        if( std::rename( M_kaway_log_name.c_str(),
                         newname.c_str() ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": error renaming " << M_kaway_log_name << std::endl;
        }
    }
}

void
Stadium::closeLogs()
{
    renameLogs();
    M_text_log.close();
    M_game_log.close();
    M_kaway_log.close();
    M_gz_text_log.close();
    M_gz_game_log.close();
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
    if ( player->team()->side() == LEFT )
    {
        M_team_l->substitute( player, player_type_id );
        broadcastSubstitution( LEFT, player->unum(), player_type_id );
    }
    else if ( player->team()->side() == RIGHT )
    {
        M_team_r->substitute( player, player_type_id );
        broadcastSubstitution( RIGHT, player->unum(), player_type_id );
    }
}

void
Stadium::collisions()
{
    bool col = false;
    int max_loop = 10;

    const std::size_t SIZE = M_players.size();
    const PlayerCont::iterator end = M_players.end();

    do
    {
        col = false;
        M_ball->clearCollide();
        for ( PlayerCont::iterator it = M_players.begin();
              it != end;
              ++it )
        {
            (*it)->clearCollide();
        }

        // check ball to player
        for ( PlayerCont::iterator it = M_players.begin();
              it != end;
              ++it )
        {
            if ( (*it)->isEnable()
                 && (*it) != M_ball_catcher
                 && M_ball->pos().distance( (*it)->pos() )
                 < M_ball->size() + (*it)->size() )
            {
                col = true;
                (*it)->collidedWithBall();
                for_each( M_referees.begin(), M_referees.end(),
                          Referee::doBallTouched( **it ) );
                calcBallCollPos( *it );
            }
        }

        // check player to player
        for ( std::size_t i = 0; i < SIZE - 1; ++i )
        {
            for ( std::size_t j = i + 1; j < SIZE; ++j )
            {
                if ( M_players[i]->isEnable()
                     && M_players[j]->isEnable()
                     && M_players[i]->pos().distance( M_players[j]->pos() )
                     < M_players[i]->size() + M_players[j]->size() )
                {
                    col = true;
                    M_players[i]->collidedWithPlayer();
                    M_players[j]->collidedWithPlayer();
                    calcCollPos( M_players[i], M_players[j] );
                }
            }
        }

        M_ball->moveToCollPos();
        for ( PlayerCont::iterator it = M_players.begin();
              it != end;
              ++it )
        {
            (*it)->moveToCollPos();
        }

        --max_loop;
    }
    while( col && max_loop > 0 );

    //if ( max_loop < 8
    //    std::cerr << M_time << ": collision loop " << max_loop
    //              << std::endl;

    M_ball->updateCollVel();
    for ( PlayerCont::iterator it = M_players.begin();
          it != end;
          ++it )
    {
        (*it)->updateCollVel();
    }

}

void
Stadium::calcBallCollPos( Player * p )
{
    if ( playmode() == PM_PlayOn )
    {
        calcCollPos( M_ball, p );
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
Stadium::calcCollPos( MPObject * a,
                      MPObject * b )
{
    if ( a == NULL || b == NULL )
        return;

    PVector apos;
    PVector bpos;
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
        while ( apos.distance( bpos ) < a->size() + b->size()
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
Stadium::placeBall( const PlayMode pm,
                    const Side side,
                    PVector location )
{
    set_ball( side, location );

    if ( Referee::isPenaltyShootOut( playmode() )
         && ( pm == PM_PlayOn || pm ==  PM_Drop_Ball ) )
    {
       ; // never change pm to play_on in penalty mode
    }
    else
    {
        change_play_mode( pm );
    }
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

    Player * player = NULL;
    if ( side == LEFT )
    {
        player = M_players[unum - 1];
    }
    else if ( side == RIGHT )
    {
        player = M_players[MAX_PLAYER + unum - 1];
    }

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

    if ( player->state() != DISABLE )
    {
        player->addState( state );
    }
}

void
Stadium::kickTaken( const Player & kicker,
                    const PVector & accel )
{
    M_ball_catcher = NULL;

    M_ball->push( accel );

    for_each( M_referees.begin(), M_referees.end(),
              Referee::doKickTaken( kicker ) );
}

void
Stadium::ballCaught( const Player & catcher )
{
    if( ! Referee::isPenaltyShootOut( playmode() ) )
    {
        std::string msg = "goalie_catch_ball_";
        msg += SideStr( catcher.team()->side() );
        say( msg.c_str() );
    }

    //M_motable.collisions( playmode() );
    collisions();

    for_each( M_referees.begin(), M_referees.end(),
              Referee::doBallTouched( catcher ) );

    for_each( M_referees.begin(), M_referees.end(),
              Referee::doCaughtBall( catcher ) );

    if ( playmode() == PM_FreeKick_Left
         || playmode() == PM_FreeKick_Right )
    {
        M_ball_catcher = &catcher;
    }
}

void
Stadium::sendGraphic( Side side,
                      unsigned int x,
                      unsigned int y,
                      std::auto_ptr< XPMHolder > holder )
{
#ifdef HAVE_SSTREAM
    std::ostringstream data;
#else
    std::ostrstream data;
#endif
    data << "(team_graphic_"
         << ( side == LEFT ? "l" : "r" )
         << " (" << x << " " << y << " "
         << *holder << ")" << std::endl;

#ifndef HAVE_SSTREAM
    data << std::ends;
#endif

    for ( MonitorCont::iterator i = M_monitors.begin();
          i != M_monitors.end(); ++i )
    {
#ifdef HAVE_SSTREAM
        (*i)->sendMsg( MSG_BOARD, data.str().c_str() );
#else
        (*i)->sendMsg( MSG_BOARD, data.str() );
#endif
    }
#ifndef HAVE_SSTREAM
    data.freeze( false );
#endif

    if ( side == LEFT )
    {
        M_team_l->addGraphic( x, y, holder );
    }

    if ( side == RIGHT )
    {
        M_team_r->addGraphic( x, y, holder );
    }
}


void
Stadium::move_caught_ball()
{
    if ( M_ball_catcher )
    {
        // keeps the caught ball infront of the player
        PVector temp = PVector::fromPolar( M_ball_catcher->size()
                                           + ServerParam::instance().ballSize(),
                                           M_ball_catcher->angleBodyCommitted() );
        M_ball->moveTo( M_ball_catcher->pos() + temp );
    }
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
            ++i;
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
            ++i;
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
            ++i;
    }

    for ( MonitorCont::iterator i = M_monitors.begin();
          i != M_monitors.end(); )
    {
        if ( ! (*i)->connected() )
        {
            if ( (*i)->version() >= 3.0 )
            {

            }
            else if ( (*i)->version() >= 2.0 )
            {
                --M_nr_monitor_v2;
            }
            else if ( (*i)->version() >= 1.0 )
            {
                --M_nr_monitor_v1;
            }

            delete *i;
            i = M_monitors.erase( i );
            std::cout << "A monitor disconnected\n";
        }
        else
            ++i;
    }
}


void
Stadium::sendRefAudio( const std::string& msg )
{
    std::random_shuffle( M_listeners.begin(), M_listeners.end(),
                         irand ); //rcss::random::UniformRNG::instance() );

    // the following should work, but I haven't tested it yet
    //      std::for_each( M_listeners.begin(), M_listeners.end(),
    //                     std::bind2nd( std::mem_fun( &rcss::Listener::sendRefAudio ),
    //                                   msg.c_str() ) );

    const ListenerCont::iterator end = M_listeners.end();
    for ( ListenerCont::iterator it = M_listeners.begin();
          it != end;
          ++it )
    {
        (*it)->sendRefAudio( msg.c_str() );
    }
}

void
Stadium::sendCoachAudio( const Coach & coach,
                         const std::string & msg )
{
    std::random_shuffle( M_listeners.begin(), M_listeners.end(),
                         irand ); //rcss::random::UniformRNG::instance() );

    const ListenerCont::iterator end = M_listeners.end();
    for ( ListenerCont::iterator it = M_listeners.begin();
          it != end;
          ++it )
    {
        (*it)->sendCoachAudio( coach, msg.c_str() );
    }


    if ( ( game_log_open()
           && ServerParam::instance().recordMessages() )
         || ServerParam::instance().sendComms() )
	  {
        char buf[max_message_length_for_display];
        char format[40];
        std::snprintf( format, 40, "(%%s %%.%ds)\n",
                       max_message_length_for_display - (MaxMesg - MaxCoachMesg) );
        std::snprintf( buf,
                       max_message_length_for_display,
                       format,
                       (coach.side() == RIGHT) ? OLCOACH_NAME_R : OLCOACH_NAME_L,
                       msg.c_str() );

        if ( ServerParam::instance().sendComms() )
        {
            for ( MonitorCont::iterator i = monitors().begin();
                  i != monitors().end();
                  ++i )
            {
                (*i)->sendMsg( MSG_BOARD, buf );
            }
        }

        if ( game_log_open()
             && ServerParam::instance().recordMessages() )
        {
            if ( playmode() != PM_BeforeKickOff && playmode() != PM_TimeOver )
            {
                writeMsgToGameLog( MSG_BOARD, buf );
            }
        }
    }
}

void
Stadium::sendCoachStdAudio( const OnlineCoach & coach,
                            const rcss::clang::Msg & msg )
{
    std::random_shuffle( M_listeners.begin(), M_listeners.end(),
                         irand ); //rcss::random::UniformRNG::instance() );

    const ListenerCont::iterator end = M_listeners.end();
    for ( ListenerCont::iterator it = M_listeners.begin();
          it != end;
          ++it )
    {
        (*it)->sendCoachStdAudio( msg );
    }


    if ( ( game_log_open()
           && ServerParam::instance().recordMessages() )
         || ServerParam::instance().sendComms() )
    {
#ifdef HAVE_SSTREAM
        std::ostringstream coach_mess;
#else
        std::ostrstream coach_mess;
#endif

        coach_mess << msg.getTimeRecv() << " ";
        msg.print( coach_mess );
#ifndef HAVE_SSTREAM
        coach_mess << std::ends; //null-terminate
#endif

        char buf[max_message_length_for_display];
        char format[40];
        std::snprintf( format, 40, "(%%s %%.%ds)\n",
                       max_message_length_for_display - (MaxMesg - MaxCoachMesg));
        std::snprintf( buf,
                       max_message_length_for_display,
                       format,
                       (coach.side() == RIGHT) ? OLCOACH_NAME_R : OLCOACH_NAME_L,
#ifdef HAVE_SSTREAM
                       coach_mess.str().c_str()
#else
                       coach_mess.str()
#endif
                ) ;
#ifndef HAVE_SSTREAM
        coach_mess.freeze( false );
#endif

        if ( ServerParam::instance().sendComms() )
        {
            for ( MonitorCont::iterator i = monitors().begin();
                  i != monitors().end();
                  ++i )
            {
                (*i)->sendMsg( MSG_BOARD, buf );
            }
        }

        if ( game_log_open()
             && ServerParam::instance().recordMessages() )
        {
            if ( playmode() != PM_BeforeKickOff
                 && playmode() != PM_TimeOver )
            {
                writeMsgToGameLog( MSG_BOARD, buf );
            }
        }
    }

}

void
Stadium::sendPlayerAudio( const Player & player,
                          const std::string & msg )
{
    std::random_shuffle( M_listeners.begin(), M_listeners.end(),
                         irand ); //rcss::random::UniformRNG::instance() );

    const ListenerCont::iterator end = M_listeners.end();
    for ( ListenerCont::iterator it = M_listeners.begin();
          it != end;
          ++it )
    {
        (*it)->sendPlayerAudio( player, msg.c_str() );
    }


    if ( ( game_log_open()
           && ServerParam::instance().recordMessages() )
         || ServerParam::instance().sendComms() )
    {
        char buf[max_message_length_for_display];
        std::snprintf( buf,
                       max_message_length_for_display,
                       "(%s %s)\n",
                       player.name().c_str(), msg.c_str() );

        // send to monitors
        if ( ServerParam::instance().sendComms() )
        {
            for ( Stadium::MonitorCont::iterator i = monitors().begin();
                  i != monitors().end();
                  ++i )
            {
                (*i)->sendMsg( MSG_BOARD, buf );
            }
        }

        if ( game_log_open()
             && ServerParam::instance().recordMessages() )
        {
            if ( playmode() != PM_BeforeKickOff && playmode() != PM_TimeOver )
            {
                writeMsgToGameLog( MSG_BOARD, buf );
            }
        }
    }
}


void
Stadium::doRecvFromClients()
{
    timeval tv_start, tv_end;

    if ( text_log_open()
         && ServerParam::instance().profile() )
    {
        gettimeofday( &tv_start, NULL );
    }

    udp_recv_message();
    udp_recv_from_online_coach();
    if ( ServerParam::instance().coachMode()
         || ServerParam::instance().coachWithRefereeMode() )
    {
        udp_recv_from_coach();
    }

    removeDisconnectedClients();

    if ( ServerParam::instance().autoMode() )
    {
        if ( playmode() == PM_BeforeKickOff
             && time() < ( ServerParam::instance().halfTime()
                           * ( ServerParam::instance().nrNormalHalfs()
                               + ServerParam::instance().nrExtraHalfs() ) )
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
                    if ( ServerParam::instance().synchMode() )
                    {
                        M_kick_off_wait -= ( ( ServerParam::instance().simStep()
                                               / ServerParam::instance().slowDownFactor() )
                                             / ServerParam::instance().recvStep() );
                    }
                    else
                    {
                        --M_kick_off_wait;
                    }
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
                _Start( *this );
            }
        }
        else
        {
            M_kick_off_wait = ServerParam::instance().kickOffWait();
            M_connect_wait = ServerParam::instance().connectWait();
        }
    }

    if ( text_log_open() && ServerParam::instance().profile() )
    {
        gettimeofday ( &tv_end, NULL );
        write_profile ( tv_start, tv_end, "RECV" );
    }
}

void
Stadium::doNewSimulatorStep()
{
    static timeval tp_old;
    timeval tp_new, tv_start, tv_end;

    // th 6.3.00
    if ( text_log_open() )
    {
        if ( ServerParam::instance().logTimes() )
        {
            //  tp_old = tp_new;
            //  write_times displays nonsense at first call, since tp_old is never
            //  initialized. Don't want to handle special exception for first call.
            gettimeofday( &tp_new, NULL );
            write_times( tp_new, tp_old );
            gettimeofday( &tp_old, NULL );
        }

        if ( ServerParam::instance().profile() )
        {
            gettimeofday( &tv_start, NULL );
        }
    }

    //
    // step
    //
    step();
    startTeams();

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

            if ( M_kick_off_wait > 0 )
            {
                M_game_over_wait--;
            }

            if ( M_game_over_wait == 0 )
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

    if ( text_log_open()
         && ServerParam::instance().profile() )
    {
        gettimeofday( &tv_end, NULL );
        write_profile( tv_start, tv_end, "SIM" );
    }
}

void
Stadium::doSendSenseBody()
{
    struct timeval tv_start, tv_end;
    if ( text_log_open()
         && ServerParam::instance().profile() )
    {
        gettimeofday( &tv_start, NULL );
    }

    std::random_shuffle( M_remote_players.begin(), M_remote_players.end(),
                         irand ); //rcss::random::UniformRNG::instance() );

    //
    // send sense_body & fullstate
    //
    const PlayerCont::iterator end = M_remote_players.end();
    for ( PlayerCont::iterator it = M_remote_players.begin();
          it != end;
          ++it )
    {
        if ( (*it)->state() != DISABLE
             && (*it)->connected() )
        {
            (*it)->sense_body();

            if ( ( (*it)->team()->side() == LEFT
                   && ServerParam::instance().fullstateLeft() )
                 || ( (*it)->team()->side() == RIGHT
                      && ServerParam::instance().fullstateRight() ) )
            {
                (*it)->send_fullstate_information();
            }
        }

        // reset collision flags
        (*it)->resetCollisionFlags();
    }


    //
    // send audio message
    //
    std::for_each( M_listeners.begin(), M_listeners.end(),
                   rcss::Listener::NewCycle() );

    //
    // send visual
    //
    for ( PlayerCont::iterator it = M_remote_players.begin();
          it != end;
          ++it )
    {
        if ( (*it)->state() != DISABLE
             && (*it)->connected() )
        {
            (*it)->sendSynchVisual();
        }
    }

    //
    // write profile
    //
    if ( text_log_open()
         && ServerParam::instance().profile() )
    {
        gettimeofday( &tv_end, NULL );
        write_profile( tv_start, tv_end, "SB" );
    }
}

void
Stadium::doSendVisuals()
{
    struct timeval tv_start, tv_end;

    if ( text_log_open()
         && ServerParam::instance().profile() )
    {
        gettimeofday( &tv_start, NULL );
    }

    std::random_shuffle( M_remote_players.begin(), M_remote_players.end(),
                         irand ); //rcss::random::UniformRNG::instance() );

    const PlayerCont::iterator end = M_remote_players.end();
    for ( PlayerCont::iterator it = M_remote_players.begin();
          it != end;
          ++it )
    {
        if ( (*it)->state() != DISABLE
             && (*it)->connected() )
        {
            (*it)->sendVisual();
        }
    }

    if ( text_log_open()
         && ServerParam::instance().profile() )
    {
        gettimeofday( &tv_end, NULL );
        write_profile( tv_start, tv_end, "VIS" );
    }
}

void
Stadium::doSendCoachMessages()
{
    struct timeval tv_start, tv_end;
    if ( text_log_open()
         && ServerParam::instance().profile() )
    {
        gettimeofday ( &tv_start, NULL );
    }

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

    if ( text_log_open()
         && ServerParam::instance().profile() )
    {
        gettimeofday( &tv_end, NULL );
        write_profile( tv_start, tv_end, "COACH" );
    }

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
    flushLogs();
}

bool
Stadium::doSendThink()
{
    bool             shutdown = false;
    timeval          tv_start, tv_now;
    //  const int        max_alrms_wait = 25;
    const int        max_msec_waited = 25 * 50;
    char*            think_command = "(think)";
    static int       cycles_missed = 0; //number of cycles where someone missed
    const int        max_cycles_missed = 20;

    if ( time() <= 0 )
    {
        //still waiting for players to connect, so let's run a little more slowly
        usleep( 50 * 1000 );
    }
    else if ( ! monitors().empty() )
    {
        static int monitor_wait_count = 0;
        if ( ++monitor_wait_count >= 32 )
        {
            monitor_wait_count = 0;
            usleep( 20 * 1000 );
        }
    }

    //figure out who we are going to wait for
    bool wait_players[MAX_PLAYER*2];
    bool waitCoach[2];
    for ( int i = 0; i < MAX_PLAYER*2; ++i )
    {
        wait_players[i] = ! ( M_players[i]->state() == DISABLE );
    }
    for ( int i = 0; i < 2; ++i )
    {
        waitCoach[i] = M_olcoaches[i]->isEyeOn();
    }
    bool waitTrainer = M_coach->isEyeOn();

    //tell the clients they should start thinking
    for ( int i = 0; i < MAX_PLAYER*2; ++i )
    {
        if ( wait_players[i] && M_players[i]->connected() )
            M_players[i]->send( think_command );
    }

    for ( int i = 0; i < 2; ++i )
    {
        if ( waitCoach[i] && M_olcoaches[i]->connected() )
        {
            M_olcoaches[i]->send( think_command );
        }
    }

    if ( waitTrainer
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

    gettimeofday( &tv_start, NULL );

    do
    {
        done = DS_TRUE;
        ++num_sleeps;

        usleep( ServerParam::instance().synchMicroSleep() );

        doRecvFromClients();

        for ( int i = 0; i < MAX_PLAYER*2; ++i )
        {
            if ( wait_players[i]
                 && M_players[i]->connected()
                 && ! M_players[i]->doneReceived()
                 && M_players[i]->state() != DISABLE )
            {
                done = DS_FALSE;
                break;
            }
        }

        for ( int i = 0; i < 2; ++i )
        {
            if ( waitCoach[i]
                 && M_olcoaches[i]->connected()
                 && ! M_olcoaches[i]->doneReceived()
                 && M_olcoaches[i]->assigned() )
            {
                done = DS_FALSE;
                break;
            }
        }

        if ( waitTrainer
             && M_coach->connected()
             && ! M_coach->doneReceived() )
        {
            done = DS_FALSE;
        }

        // get time differnce with start of loop, first get time difference in
        // seconds, then multiply with 1000 to get msec.
        gettimeofday( &tv_now, NULL );
        double time_diff=((double)tv_now.tv_sec + (double)tv_now.tv_usec/1000000)-
            ((double)tv_start.tv_sec + (double)tv_start.tv_usec/1000000);
        time_diff *= 1000;

        if ( (int)time_diff > max_msec_waited )
        {
            done = DS_TRUE_BUT_INCOMPLETE;
            if ( time() > 0 )
            {
                cycles_missed++;
                std::cerr << "Someone missed a cycle at " << time() << std::endl;
            }
            if ( cycles_missed > max_cycles_missed )
            {
                std::cerr << "Waiting too long for clients! Exiting" << std::endl;
                shutdown = true;
            }
        }
    } while ( done == DS_FALSE );

    if ( done != DS_TRUE_BUT_INCOMPLETE )
    {
        cycles_missed = 0;
    }

    if ( text_log_open()
         && ServerParam::instance().logTimes() )
    {
        text_log_stream() << time()
                          << "  Num sleeps called: "
                          << num_sleeps << std::endl;
    }

    if ( shutdown )
    {
        M_alive = false;
    }
    return shutdown;
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
        std::cout << msg << '\n';
        closeLogs();
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

namespace rcss
{
void
save_results( rcss::ResultSaver::team_id id,
              const Team & team,
              rcss::ResultSaver::Ptr & saver )
{
    if ( ! team.name().empty() )
    {
        saver->saveTeamName( id, team.name() );
    }

    if ( team.olcoach() && ! team.olcoach()->getName().empty() )
    {
        saver->saveCoachName( id, team.name() );
    }

    saver->saveScore( id, team.point() );

    if ( team.penaltyTaken() )
    {
        saver->savePenTaken( id, team.penaltyTaken() );
        saver->savePenScored( id, team.penaltyPoint() );
    }

    if ( team.penaltyWon() )
    {
        saver->saveCoinTossWinner( id );
    }
}
}

void
Stadium::saveResults()
{
    if ( m_savers.empty() )
    {
        return;
    }

    std::cout << "\nSaving Results:"  << std::endl;
    for ( std::list< rcss::ResultSaver::Ptr >::iterator i = m_savers.begin();
          i != m_savers.end();
          ++i )
    {
        std::cout << "\t" << (*i)->getName() << ": saving...\n";
        if ( (*i)->enabled() )
        {
            (*i)->saveStart();
            (*i)->saveTime( realTime() );
            if ( M_team_l )
                rcss::save_results( rcss::ResultSaver::TEAM_LEFT,
                                    *M_team_l,
                                    *i );
            if ( M_team_r )
                rcss::save_results( rcss::ResultSaver::TEAM_RIGHT,
                                    *M_team_r,
                                    *i );
            if ( (*i)->saveComplete() )
            {
                std::cout << "\t" << (*i)->getName() << ": ...saved\n";
            }
            else
            {
                std::cout << "\t" << (*i)->getName() << ": ...failed\n";
            }
        }
        else
        {
            std::cout << "\t" << (*i)->getName() << ": ...disabled\n";
        }
    }
    std::cout << std::endl;
    m_savers.clear();
    std::cout << "Saving Results Complete" << std::endl;
}

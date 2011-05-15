/* -*- Mode: C++ -*-
 *Header:
 *File: netif.C (for C++ & cc)
 *Author: Noda Itsuki
 *Date: 1996/02/14
 *EndHeader:
 */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
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

// [2000/01/14:I.Noda]
//  escape prototype of recvfrom
//  for escape conversion of signed-unsigned ints
//#define recvfrom _escape_recvfrom
//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stadium.h"

#include "param.h"
#include "types.h"
#include "utility.h"
#include "object.h"
#include "coach.h"
#include "player.h"

#include "random.h"
#include "serializer.h"
#include "clangparser.h"
#include "clangmsg.h"
//#include "coach_lang_comp.h"
#include "clangmsgbuilder.h"
#include "xpmholder.h"
#include "remoteclient.h"
#include "monitor.h"

#include <rcssbase/net/udpsocket.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

// #ifdef __APPLE__
// extern "C" {
//     int isnan (double);
// }
// #endif


/*
 *===================================================================
 *Part: Recv
 *===================================================================
 */

namespace {
template < class T >
void
recv_from_clients( std::vector< T > & clients )
{
    std::random_shuffle( clients.begin(), clients.end(),
                         irand ); //rcss::random::UniformRNG::instance() );

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
            for ( PlayerCont::iterator i = M_remote_players.begin();
                  i != M_remote_players.end();
                  ++i )
            {
                if ( (*i)->getDest() == cli_addr )
                {
                    (*i)->undedicatedRecv( message, len );
                    found = true;
                    break;
                }
            }

            if ( ! found )
            {
                for ( MonitorCont::iterator i = M_monitors.begin();
                      i != M_monitors.end();
                      ++i )
                {
                    if ( (*i)->getDest() == cli_addr )
                    {
                        (*i)->undedicatedRecv( message, len );
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

            M_logger.writePlayerLog( *p, message, RECV );
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
            M_logger.writePlayerLog( *p, message, RECV );
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
            for ( OfflineCoachCont::iterator i = M_remote_offline_coaches.begin();
                  i != M_remote_offline_coaches.end();
                  ++i )
            {
                if ( (*i)->getDest() == cli_addr )
                {
                    (*i)->undedicatedRecv( message, len );
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
            M_logger.writeCoachLog( message, RECV );
        }
    }
    else
    {
        kickOff(); // need to remove this line if we
        // dont want the server to start when the coach connects
        M_coach->parse_command( message );
        M_logger.writeCoachLog( message, RECV );
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
            for ( OnlineCoachCont::iterator i = M_remote_online_coaches.begin();
                  i != M_remote_online_coaches.end();
                  ++i )
            {
                if ( (*i)->getDest() == cli_addr )
                {
                    (*i)->undedicatedRecv( message, len );
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

        M_logger.writeOnlineCoachLog( *olc, message, RECV );
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
    for ( MonitorCont::iterator i = M_monitors.begin();
          i != M_monitors.end(); ++i )
    {
        (*i)->sendMsg( MSG_BOARD, buffer );
    }

    M_logger.writeTextLog( buffer, SUBS );
}

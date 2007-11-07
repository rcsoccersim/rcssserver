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


// [2000/01/14:I.Noda]
//  escape prototype of recvfrom
//  for escape conversion of signed-unsigned ints
//#define recvfrom _escape_recvfrom
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rcssbase/net/udpsocket.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

//#if defined(HAVE_SYS_FILIO_H)
//#include <sys/filio.h>
//#elif defined(HAVE_SYS_IOCTL_H)
//#include <sys/ioctl.h>
//#endif

//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <netdb.h>
#include <cerrno>
#include <cmath>
//#include <fcntl.h>

#ifdef __APPLE__
extern "C" {
    int isnan (double);
}
#endif

#include <iostream>
#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include "param.h"
#include "types.h"
#include "utility.h"
#include "object.h"
#include "coach.h"
#include "player.h"
#include "field.h"

#include "random.h"
#include "serializer.h"
#include "clangparser.h"
#include "clangmsg.h"
//#include "coach_lang_comp.h"
#include "clangmsgbuilder.h"
#include "xpmholder.h"
#include "remoteclient.h"
#include "monitor.h"


/*
 *===================================================================
 *Part: Recv
 *===================================================================
 */

template < class T >
void
Stadium::recv( std::vector< T >& clients )
{
    std::random_shuffle( clients.begin(), clients.end(),
                         irand ); //rcss::random::UniformRNG::instance() );

    for( typename std::vector< T >::iterator i = clients.begin();
         i != clients.end(); )
    {
        if ( (*i)->recv() == -1 )
            ++i;
    }
}


void
Stadium::udp_recv_message()
{
    recv( M_remote_players );
    recv( M_monitors );

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
                message[len] = NULLCHAR;

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
		if ( ! std::strncmp( message, "(reconnect ", std::strlen( "(reconnect " ) ) )
		{
        //              std::cerr << "Got reconnect" << std::endl;
				// a player reconnects to the server
				if ( playmode() == PM_PlayOn )
        {
            sendToPlayer( "(error cannot_reconnect_while_playon)", cli_addr );
        }
				else
				{
            Player * p = reconnectPlayer( message, cli_addr );
            if ( p )
            {
                p->sendReconnect();
                writeTextLog( *p, message, RECV );
            }
				}
		}
		else if ( ! std::strncmp( message, "(init ", std::strlen( "(init " ) ) )
    {
        // a new player connects to the server
        Player * p = newPlayer( message, cli_addr );
        if ( p )
        {
            p->sendInit();
            writeTextLog( *p, message, RECV );
        }
    }
    else
    {
        sendToPlayer( "(error unknown_command)", cli_addr );
    }
}


bool
Stadium::parseMonitorInit( const char * message,
                           const rcss::net::Addr & addr )
{
    double ver = 1.0;
    if ( ! std::strcmp( message, "(dispinit)" ) )
    {
        if ( ServerParam::instance().maxMonitors() > 0
             && ( static_cast< int >( M_monitors.size() )
                  >= ServerParam::instance().maxMonitors()  ) )
        {
            sendToPlayer( "(error no_more_monitor)", addr );
            return true;
        }

        Monitor * mon = new Monitor( *this, 1 );
        if( ! mon->connect( addr ) )
        {
            delete mon;
            return true;
        }
        std::cout << "a new (v1) monitor connected\n";
        mon->setEnforceDedicatedPort( false );
        M_monitors.push_back( mon );
        ++M_nr_monitor_v1;
        return true;
    }
    else if ( std::sscanf( message, "(dispinit version %lf)", &ver ) == 1 )
    {
        if ( ServerParam::instance().maxMonitors() > 0
             && ( static_cast< int >( M_monitors.size() )
                  >= ServerParam::instance().maxMonitors()  ) )
        {
            sendToPlayer( "(error no_more_monitor)", addr );
            return true;
        }

        if ( ver < 1.0 || 3.0 <= ver )
        {
            std::cout << "Unsupported monitor protocol version. " << ver
                      << std::endl;
            return true;
        }

        // a new monitor connected
        Monitor * mon = new Monitor( *this, ver );

        if( ! mon->connect( addr ) )
        {
            delete mon;
            return true;
        }
        std::cout << "a new (v" << ver << ") monitor connected\n";

        mon->setEnforceDedicatedPort( ver >= 2.0 );
        M_monitors.push_back( mon );

        // send server parameter information to monitor
        if ( ver >= 2.0 )
        {
            ++M_nr_monitor_v2;

            dispinfo_t2 di;

            di.mode = htons( PARAM_MODE );
            di.body.sparams = ServerParam::instance().convertToStruct();
            mon->RemoteClient::send( reinterpret_cast< const char* >( &di ),
                                     sizeof( dispinfo_t2 ) );

            di.mode = htons( PPARAM_MODE );
            di.body.pparams = PlayerParam::instance().convertToStruct();
            mon->RemoteClient::send( reinterpret_cast< const  char* >( &di ),
                                     sizeof( dispinfo_t2 ) );

            di.mode = htons ( PT_MODE );
            for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
            {
                try
                {
                    di.body.ptinfo = M_player_types.at( i )->convertToStruct( i );
                    mon->RemoteClient::send( reinterpret_cast< const char* >( &di ),
                                             sizeof( dispinfo_t2 ) );
                }
                catch ( std::exception & e )
                {
                    std::cerr << __FILE__ << ':' << __LINE__
                              << " Exception caught! " << e.what() << std::endl;
                }
            }
        }
        else if ( ver >= 1.0 )
        {
            ++M_nr_monitor_v1;
        }
        return true;
    }

    return false;
}

void
Stadium::udp_recv_from_coach()
{
    recv( M_remote_offline_coaches );

    while ( 1 )
    {
        char message[MaxMesg];
        std::memset( &message, 0, sizeof( char ) * MaxMesg );

        rcss::net::Addr cli_addr;

        int len = M_offline_coach_socket.recv( message, MaxMesg,  cli_addr );

        if ( len > 0 )
        {
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
                message[len] = NULLCHAR;

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
    int n = std::sscanf( message, "(%127[-0-9a-zA-Z.+*/?<>_]", command );
    if( n < 1 )
    {
        sendToCoach( "(error illegal_command_form)", cli_addr );
        return false;
    }

    // (init[ (version <Ver>)])

    if ( ! std::strcmp ( command, "init" ) )
    {
        double ver = 3.0;
        n = std::sscanf( message, "(init (version %lf))", &ver );
        if ( ( n != 0 && n != 1 )
             || ver < 1.0 )
        {
            sendToCoach( "(error illegal_command_form)", cli_addr );
            return false;
        }

        if ( M_coach->open() != 0 )
        {
            sendToCoach( "(error socket_open_failed)", cli_addr );
            return false;
        }

        if ( ! M_coach->connect( cli_addr ) )
        {
            sendToCoach( "(error connection_failed)", cli_addr );
            return false;
        }

        if ( ! M_coach->setSenders( ver ) )
        {
            std::cerr << "Error: Could not find serializer or sender for version"
                      << ver << std::endl;
            sendToCoach( "(error illegal_client_version)", cli_addr );
            return false;
        }

        M_coach->setEnforceDedicatedPort( ver >= 8.0 );
        addOfflineCoach( M_coach );
        addListener( M_coach );
        M_coach->sendInit();

        std::cout << "a new offline coach connected" << std::endl;

        writeTextLog( *M_coach, message, RECV );
    }
    else
    {
        _Start( *this ); // need to remove this line if we
        // dont want the server to start when the coach connects
        M_coach->parse_command( message );
        writeTextLog( *M_coach, message, RECV );
    }

    return true;
}

void
Stadium::udp_recv_from_online_coach()
{
    recv( M_remote_online_coaches );

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
                message[len] = NULLCHAR ;

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
    if ( ! std::strncmp( message, "(init ", std::strlen( "(init " ) ) )
    {
        OnlineCoach * olc = newOnlineCoach( message, addr );
        if ( olc )
        {
            writeTextLog( *olc, message, RECV );
        }
    }
    else
    {
        sendToOnlineCoach( "(error illegal_command_form)",
                           addr );
    }
}

void
Stadium::say( const char *message, bool ref )
{
    if ( ref )
    {
        sendRefAudio( message );
    }
    else
    {
        sendCoachAudio( *M_coach, message );
    }

    //    send_audio_info(message, NULL, FALSE, standard) ;

    if ( text_log_open()
         || game_log_open()
         || ServerParam::instance().sendComms() )
	  {
        M_minfo.body.msg.board = htons( MSG_BOARD );
        std::snprintf( M_minfo.body.msg.message,
                       sizeof( M_minfo.body.msg.message ),
                       "(%s %s)",
                       REFEREE_NAME, message );
        // write to text log
        if ( text_log_open() )
	      {
            text_log_stream() << time() << "\t" << M_minfo.body.msg.message << '\n';
	      }

        // send to monitors
        if ( ServerParam::instance().sendComms() )
        {
            for ( MonitorCont::iterator i = M_monitors.begin();
                  i != M_monitors.end();
                  ++i )
            {
                if ( (*i)->version() >= 2.0 )
                {
                    dispinfo_t2 minfo2;
                    minfo2.mode = M_minfo.mode;
                    minfo2.body.msg = M_minfo.body.msg;
                    (*i)->RemoteClient::send( reinterpret_cast< const char * >( &minfo2 ),
                                              sizeof( dispinfo_t2 ) );
                }
                else if ( (*i)->version() >= 1.0 )
                {
                    (*i)->RemoteClient::send( reinterpret_cast< const char * >( &M_minfo ),
                                              sizeof( dispinfo_t ) );
                }
            }
        }

        // write to game log
        if ( game_log_open() )
        {
            if ( playmode() != PM_BeforeKickOff
                 && playmode() != PM_TimeOver )
            {
                writeGameLog( &M_minfo );
            }
        }
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
    std::snprintf( allies, 64, "(change_player_type %d %d)", unum, player_type );
    std::snprintf( enemies, 64, "(change_player_type %d)", unum );

    // tell players
    for ( int i = 0 ; i < MAX_PLAYER * 2; ++i )
    {
        if ( M_players[i]->alive() == DISABLE
             || M_players[i]->version() < 7.0 )
        {
            continue;
        }

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
    std::snprintf( buffer, 64,
                   "(change_player_type %s %d %d)",
                   ( side == LEFT ? "l" : "r" ),
                   unum, player_type );
    for ( MonitorCont::iterator i = M_monitors.begin();
          i != M_monitors.end(); ++i )
    {
        (*i)->send( buffer );
    }

    std::strcat( buffer, "\n" );
    writeTextLog( buffer, SUBS );
}

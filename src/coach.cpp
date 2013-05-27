// -*-c++-*-

/***************************************************************************
                              coach.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "coach.h"

#include "audio.h"
#include "clangparser.h"
#include "clangmsg.h"
#include "clangmsgbuilder.h"
#include "stadium.h"
#include "object.h"
#include "player.h"
#include "serializer.h"
#include "team.h"
#include "xpmholder.h"
#include "initsendercoach.h"
#include "initsenderonlinecoach.h"
#include "visualsendercoach.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cmath>

namespace {

PlayMode
play_mode_id( const char * mode )
{
    static const char * playmode_strings[] = PLAYMODE_STRINGS;

    for ( int n = 0; n < PM_MAX; ++n )
    {
        if ( ! std::strcmp( playmode_strings[n], mode ) )
        {
            return static_cast< PlayMode >( n );
        }
    }
    return PM_Null;
}

void
chop_last_parenthesis( char * str,
                       int max_size )
{
    int l = std::strlen( str );

    if ( l > max_size )
    {
        str[max_size] = '\0';
    }
    else
    {
        --l;
        if ( str[l] == ')' ) str[l] = '\0';
    }
}

}

Coach::Coach( Stadium & stadium )
    : M_init_observer_coach( static_cast< rcss::InitObserverOfflineCoach * >( 0 ) ),
      M_observer( new rcss::ObserverCoach ),
      M_stadium( stadium ),
      M_assigned( false ),
      M_eye( false ),
      M_hear( false ),
      M_version( 0.0 ),
      M_done_received( false )
{

}

Coach::~Coach()
{
    if ( M_init_observer_coach )
    {
        delete M_init_observer_coach;
        M_init_observer_coach = static_cast< rcss::InitObserverOfflineCoach * >( 0 );
    }

    if ( M_observer )
    {
        delete M_observer;
        M_observer = static_cast< rcss::ObserverCoach * >( 0 );
    }
}

void
Coach::disable()
{
    M_assigned = false;
    RemoteClient::close();
    std::cout << "An offline coach disconnected\n";
}

bool
Coach::setSenders( const double & client_version )
{
    M_version = client_version;

    rcss::SerializerCoach::Creator ser_cre;
    if ( ! rcss::SerializerCoach::factory().getCreator( ser_cre,
                                                        (int)client_version ) )
    {
        std::cerr << "No SerializerCoach::Creator v" << client_version << std::endl;
        return false;
    }

    const rcss::SerializerCoach::Ptr ser = ser_cre();
    if( ! ser )
    {
        std::cerr << "No SerializerCoach v" << client_version << std::endl;
        return false;
    }

    // visual
    rcss::VisualSenderCoach::Params visual_params( getTransport(),
                                                   *this,
                                                   ser,
                                                   M_stadium );
    rcss::VisualSenderCoach::Creator visual_cre;
    if ( ! rcss::VisualSenderCoach::factory().getCreator( visual_cre,
                                                          (int)client_version ) )
    {
        std::cerr << "No VisualSenderCoach::Creator v" << client_version << std::endl;
        return false;
    }
    M_observer->setVisualSender( visual_cre( visual_params ) );

    // audio
    rcss::AudioSenderCoach::Params audio_params( getTransport(),
                                                 *this,
                                                 ser,
                                                 M_stadium );

    rcss::AudioSenderCoach::Creator audio_cre;
    if( ! rcss::AudioSenderCoach::factory().getCreator( audio_cre,
                                                        (int)client_version ) )
    {
        std::cerr << "No AudioSenderCoach::Creator v" << client_version << std::endl;
        return false;
    }
    setAudioSender( audio_cre( audio_params ) );

    // init
    rcss::InitSenderOfflineCoach::Params init_params( getTransport(),
                                                      *this,
                                                      ser,
                                                      M_stadium );
    rcss::InitSenderOfflineCoach::Creator init_cre;
    if ( ! rcss::InitSenderOfflineCoach::factory().getCreator( init_cre,
                                                               (int)client_version ) )
    {
        std::cerr << "No InitSenderCoach::Creator v" << client_version << std::endl;
        return false;
    }

    M_init_observer_coach = new rcss::InitObserverOfflineCoach;

    //rcss::InitSenderOfflineCoach::Ptr isoc = init_cre( init_params );
    //rcss::InitObserver<rcss::InitSenderOfflineCoach>::setInitSender( isoc );
    M_init_observer_coach->setInitSender( init_cre( init_params ) );

    M_assigned = true;
    return true;
}

void
Coach::sendInit()
{
    M_init_observer_coach->sendInit();

    M_init_observer_coach->sendServerParams();
    M_init_observer_coach->sendPlayerParams();
    M_init_observer_coach->sendPlayerTypes();
    M_init_observer_coach->sendChangedPlayers();
}

void
Coach::sendOKEye()
{
    if ( M_assigned )
    {
        M_observer->sendOKEye();
    }
}


void
Coach::send( const char * msg )
{
    if ( RemoteClient::send( msg, std::strlen( msg ) + 1 ) != -1 )
    {
        M_stadium.logger().writeCoachLog( msg, SEND );
    }
}

void
Coach::parseMsg( char * msg,
                 const size_t & len )
{
    char * str = msg;
    if ( str[ len - 1 ] != 0 )
    {
        if ( version() >= 8.0 )
        {
            send( "(warning message_not_null_terminated)" );
        }
        str[ len ] = 0;
    }
    M_stadium.logger().writeCoachLog( str, RECV );
    parse_command( str );
}


void
Coach::parse_command( const char * command )
{
    char com[128];

    if ( std::sscanf( command, " ( %127[-0-9a-zA-Z.+*/?<>_] ", com ) != 1 )
    {
        send( "(error illegal_command_form)" );
        return;
    }

    if ( ! std::strcmp( com, "start" ) )
    {
        M_stadium.kickOff();
        send( "(ok start)" );
    }
    else if ( ! std::strcmp( com, "change_mode" ) )
    {
        parse_change_mode( command );
    }
    else if ( ! std::strcmp( com, "move" ) )
    {
        parse_move( command );
    }
    else if ( ! std::strcmp( com, "look" ) )
    {
        look();
    }
    else if ( ! std::strcmp( com,"team_names" ) )
    {
        team_names();
    }
    else if ( ! std::strcmp( com, "recover" ) )
    {
        recover();
    }
    else if ( ! std::strcmp( com, "check_ball" ) )
    {
        check_ball();
    }
    else if( ! std::strcmp( com, "say" ) )
    {
        char msg[MaxMesg] ;
        if ( std::sscanf( command, SAY_MESSAGE_SCAN_FORMAT, com, msg ) != 2 )
        {
            send( "(error illegal_command_form)" );
            return;
        }
        chop_last_parenthesis( msg, ServerParam::instance().freeformMsgSize() );
        M_stadium.sendCoachAudio( *this, msg );
        send( "(ok say)" );
    }
    else if ( ! std::strcmp( com, "ear" ) )
    {
        char mode[16] ;
        if ( std::sscanf( command," ( ear %15[^ ()] ) ", mode ) != 1 )
        {
            send( "(error illegal_command_form)" );
            return;
        }

        ear( mode );
        return;
    }
    else if ( ! std::strcmp( com, "eye" ) )
    {
        char mode[16] ;
        if ( std::sscanf( command, " ( eye %15[^ ()] ) ", mode ) != 1 )
        {
            send( "(error illegal_command_form)" );
            return;
        }

        eye( mode );
        return;
    }
    else if ( ! std::strcmp( com, "change_player_type" ) )
    {
        char name[128];
        int unum, player_type;
        // char goalie[16];

        if ( std::sscanf( command, " ( change_player_type %127s %d %d ) ",
                          name, &unum, &player_type ) == 3 )
        {
            change_player_type( name, unum, player_type );
        }
#if 0
        else if ( std::sscanf( command, " ( change_player_type %127s %d %15[^ ( )] ) ",
                               name, &unum, goalie ) == 3
                  && ! std::strncmp( goalie, "goalie", 6 ) )
        {
            change_player_type_goalie( name, unum );
        }
#endif
        else
        {
            send( "(error illegal_command_form)" );
        }

        return;
    }
    //pfr:SYNCH
    else if ( ! std::strcmp( com, "done" ) )
    {
        //std::cerr << "Recv trainer done" << std::endl;
        M_done_received = true;
        return;
    }
    else if ( ! std::strcmp( com, "compression" ) )
    {
        int level;
        if ( std::sscanf( command, " ( compression %d ) ", &level ) != 1 )
        {
            send( "(error illegal_command_form)" );
            return;
        }

        compression( level );
        return;
    }
    else
    {
        send( "(error unknown_command)" );
        return;
    }
}


void
Coach::sendExternalMsg()
{
    //std::cerr << "sendExternalMsg" << std::endl;
    if ( ServerParam::instance().coachMsgFile().empty() )
    {
        //std::cerr << "sendExternalMsg no input file" << std::endl;
        return;
    }

    std::ifstream fin( ServerParam::instance().coachMsgFile().c_str() );
    if ( ! fin.is_open() )
    {
        std::cerr << "Error: Could not open the file ["
                  << ServerParam::instance().coachMsgFile()
                  << "]" << std::endl;
        return;
    }

    std::vector< char > buf;
    std::copy( std::istreambuf_iterator< char >( fin ),
               std::istreambuf_iterator< char >(),
               std::back_inserter( buf ) );

    if ( buf.empty() )
    {
        std::cerr << "Error: coach_msg_file is empty."
                  << std::endl;
        return;
    }

    if ( buf.size() >= MaxMesg - std::strlen( "(include 8192 )" ) )
    {
        std::cerr << "Too large messages in ["
                  << ServerParam::instance().coachMsgFile()
                  << "]"
                  << std::endl;
        return;
    }

    std::string msg = "(include ";
    msg += boost::lexical_cast< std::string >( buf.size() );
    msg += ' ';
    msg.append( buf.begin(), buf.end() );
    msg += ')';

    std::cout << "send external message in ["
              << ServerParam::instance().coachMsgFile()
              << "]"
              << std::endl;

    send( msg.c_str() );
}

int
Coach::parse_change_mode( const char * command )
{
    int n_read = 0;
    char new_mode[128];
    if ( std::sscanf( command,
                      " ( change_mode %127[-0-9a-zA-Z.+*/?<>_] ) %n ",
                      new_mode, &n_read ) != 1 )
    {
        send( "(error illegal_command_form)" );
        return 0;
    }

    change_mode( new_mode );
    return n_read;
}

void
Coach::change_mode( std::string mode )
{
    PlayMode mode_id = play_mode_id( mode.c_str() );

    if ( mode_id == PM_Null )
    {
        send( "(error illegal_mode)" );
        return;
    }

    M_stadium.changePlayMode( mode_id );
    send( "(ok change_mode)" );
}

int
Coach::parse_move( const char * command )
{
    int n_read = 0;
    char obj[128];
    double x = 0.0, y = 0.0, ang = 0.0, velx = 0.0, vely = 0.0;

    int n = std::sscanf( command,
                         " ( move ( %127[^)] ) %lf %lf %lf %lf %lf ) %n ",
                         obj, &x, &y, &ang, &velx, &vely,
                         &n_read );

    if ( n < 3
         || std::isnan( x ) != 0
         || std::isnan( y ) != 0
         || std::isnan( ang ) != 0
         || std::isnan( velx ) != 0
         || std::isnan( vely ) != 0 )
    {
        send( "(error illegal_object_form)" );
        return 0;
    }

    std::string obj_name = "(";
    obj_name += obj;
    obj_name += ')';

    if ( obj_name == BALL_NAME )
    {
        M_stadium.clearBallCatcher();

        if ( n == 3 || n == 4 )
        {
            M_stadium.moveBall( PVector( x, y ), PVector( 0.0, 0.0 ) );
        }
        else if ( n == 6 )
        {
            M_stadium.moveBall( PVector( x, y ), PVector( velx, vely ) );
        }
        else
        {
            send( "(error illegal_command_form)" );
            return 0;
        }
    }
    else
    {
        char teamname[128];
        int unum = 0;

        if ( std::sscanf( obj_name.c_str(),
                          PLAYER_NAME_FORMAT,
                          teamname, &unum ) != 2
             || unum < 1
             || MAX_PLAYER < unum )
        {
            send( "(error illegal_object_form)" );
            return 0;
        }

        Side side = ( M_stadium.teamLeft().name() == teamname
                      ? LEFT
                      : M_stadium.teamRight().name() == teamname
                      ? RIGHT
                      : NEUTRAL );

        PVector pos( x, y );
        PVector vel( velx, vely );
        ang = Deg2Rad( rcss::bound( ServerParam::instance().minMoment(),
                                    ang,
                                    ServerParam::instance().maxMoment() ) );
        if ( n == 3 )
        {
            M_stadium.movePlayer( side, unum, pos, NULL, NULL );
        }
        else if ( n == 4 )
        {
            M_stadium.movePlayer( side, unum, pos, &ang, NULL );
        }
        else if ( n == 6 )
        {
            M_stadium.movePlayer( side, unum, pos, &ang, &vel );
        }
        else
        {
            send( "(error illegal_command_form)" );
            return 0;
        }
    }

    send( "(ok move)" );

    return n_read;
}

void
Coach::ear( std::string mode )
{
    if ( mode == "on" )
    {
        M_hear = true;
        send( "(ok ear on)" );
    }
    else if ( mode == "off" )
    {
        M_hear = false;
        send( "(ok ear off)" );
    }
    else
    {
        send( "(error illegal_command_form)" );
    }
}

void
Coach::eye( std::string mode )
{
    if ( mode == "on" )
    {
        M_eye = true;
        //send( "(ok eye on)" );
    }
    else if ( mode == "off" )
    {
        M_eye = false;
        //send( "(ok eye off)" );
    }
    else
    {
        send( "(error illegal_command_form)" );
        return;
    }

    sendOKEye();
}

void
Coach::compression( int level )
{
#ifdef HAVE_LIBZ
    if( level > 9 )
    {
        send( "(error illegal_command_form)" );
        return;
    }


    std::ostringstream reply;
    reply << "(ok compression " << level << ")";
    send ( reply.str().c_str() );

    setCompressionLevel ( level );
#else
    send ( "(warning compression_unsupported)" );
#endif
}

void
Coach::look()
{
    if ( M_assigned )
    {
        M_observer->sendLook();
    }
//     std::ostringstream ost;

//     ost << "(ok look " << stad.time();
//     if ( version() >= 7.0 )
//         ost << " (" << GOAL_L_NAME_SHORT;
//     else
//         ost << " (" << GOAL_L_NAME;

//     ost << " " << -(PITCH_LENGTH/2.0)
//         << " " << 0.0
//         << ")" ;

//     if ( version() >= 7.0 )
//         ost << " (" << GOAL_R_NAME_SHORT;
//     else
//         ost << " (" << GOAL_R_NAME;

//     ost << " " << (PITCH_LENGTH/2.0)
//         << " " << 0.0
//         << ")" ;
//     if ( version() >= 7.0 )
//         ost << " (" << BALL_NAME_SHORT;
//     else
//         ost << " (" << BALL_NAME;

//     ost << " " << stad.ball().pos().x
//         << " " << stad.ball().pos().y
//         << " " << stad.ball().vel().x
//         << " " << stad.ball().vel().y
//         << ")" ;

//     const Stadium::PlayerCont::const_iterator end = stad.players().end();
//     for ( Stadium::PlayerCont::const_iterator p = stad.players().begin();
//           p != end;
//           ++p )
//     {
//         if ( (*p)->alive == DISABLE )
//             continue ;
//         if ( version() >= 7.0 )
//             ost << " (" << (*p)->shortName()
//                 << " " << (*p)->pos().x
//                 << " " << (*p)->pos().y
//                 << " " << (*p)->vel().x
//                 << " " << (*p)->vel().y
//                 << " " << Rad2IDegRound( (*p)->angleBodyCommitted() )
//                 << " " << Rad2IDegRound( (*p)->angleNeckCommitted() )
//                 << ")" ;
//         else
//             ost << " (" << (*p)->name()
//                 << " " << (*p)->pos().x
//                 << " " << (*p)->pos().y
//                 << " " << Rad2IDeg( (*p)->angleBodyCommitted() )
//                 << " " << Rad2IDeg( (*p)->angleNeckCommitted() )
//                 << " " << (*p)->vel().x
//                 << " " << (*p)->vel().y
//                 << ")" ;
//     }
//     ost << ")" << std::endl;
//     send(ost.str().c_str());
}

void
Coach::team_names()
{
    std::ostringstream ost;

    ost << "(ok team_names";

    if ( ! M_stadium.teamLeft().name().empty() )
    {
        ost << " (team l " << M_stadium.teamLeft().name() << ")";
    }

    if ( ! M_stadium.teamRight().name().empty() )
    {
        ost << " (team r " << M_stadium.teamRight().name() << ")";
    }

    ost << ")" << std::endl;
    send( ost.str().c_str() );
}

void
Coach::recover()
{
    M_stadium.recoveryPlayers();

    std::ostringstream ost;
    ost << "(ok recover)" << std::endl;
    send( ost.str().c_str() );
}


void
Coach::change_player_type( const std::string & team_name,
                           int unum,
                           int player_type )
{
    const Team * team = NULL;
    if ( M_stadium.teamLeft().name() == team_name )
    {
        team = &( M_stadium.teamLeft() );
    }

    if ( M_stadium.teamRight().name() == team_name )
    {
        team = &( M_stadium.teamRight() );
    }

    if ( team == NULL )
    {
        send( "(warning no_team_found)" );
        return;
    }

    if ( player_type < 0
         || player_type >= PlayerParam::instance().playerTypes() )
    {
        send( "(error out_of_range_player_type)" );
        return;
    }

    const Player * player = NULL;
    for ( int i = 0; i < team->size(); ++i )
    {
        const Player * p = team->player( i );
        if ( p && p->unum() == unum )
        {
            player = p;
            break;
        }
    }

    if ( player == NULL )
    {
        send( "(warning no_such_player)" );
        return;
    }

    M_stadium.substitute( player, player_type );

    char buf[64];
    snprintf ( buf, 64, "(ok change_player_type %s %d %d)",
               team_name.c_str(), unum, player_type );

    send( buf );
}

void
Coach::change_player_type_goalie( const std::string & team_name,
                                  int unum )
{
    const Team * team = NULL;
    if ( M_stadium.teamLeft().name() == team_name )
    {
        team = &( M_stadium.teamLeft() );
    }

    if ( M_stadium.teamRight().name() == team_name )
    {
        team = &( M_stadium.teamRight() );
    }

    if ( ! team )
    {
        send( "(warning no_team_found)" );
        return;
    }

    if ( change_player_type_goalie_impl( team, unum ) )
    {
        char buf[64];
        snprintf ( buf, 64, "(ok change_player_type %s %d goalie)",
                   team_name.c_str(), unum );
        send( buf );
    }
}

bool
Coach::change_player_type_goalie_impl( const Team * team,
                                       int unum )
{
    if ( ! team )
    {
        send( "(warning no_team_found)" );
        return false;
    }

    const Player * player = static_cast< const Player * >( 0 );

    for ( int i = 0; i < team->size(); ++i )
    {
        const Player * p = team->player( i );

        if ( ! p ) continue;
        if ( ! p->isEnabled() ) continue;

        if ( p->isGoalie() )
        {
            send( "(error goalie_already_exists)" );
            return false;
        }

        if ( p->unum() == unum )
        {
            player = p;
        }
    }

    if ( ! player )
    {
        send( "(warning no_such_player)" );
        return false;
    }

    M_stadium.changePlayerToGoalie( player );

    return true;
}


void
Coach::send_visual_info()
{
    M_observer->sendVisual();
//     std::ostringstream ost;

//     if ( version() >= 7.0 )
//         ost << "(see_global " << M_stadium.time();
//     else
//         ost << "(see " << M_stadium.time();

//     if ( version() >= 7.0 )
//         ost << " (" << GOAL_L_NAME_SHORT;
//     else
//         ost << " (" << GOAL_L_NAME;
//     ost << " " << -(PITCH_LENGTH/2.0)
//         << " " << 0.0
//         << ")" ;

//     if ( version() >= 7.0 )
//         ost << " (" << GOAL_R_NAME_SHORT;
//     else
//         ost << " (" << GOAL_R_NAME;
//     ost << " " << (PITCH_LENGTH/2.0)
//         << " " << 0.0
//         << ")" ;

//     if ( version() >= 7.0 )
//         ost << " (" << BALL_NAME_SHORT;
//     else
//         ost << " (" << BALL_NAME;
//     ost << " " << M_stadium.ball().pos().x
//         << " " << M_stadium.ball().pos().y
//         << " " << M_stadium.ball().vel().x
//         << " " << M_stadium.ball().vel().y
//         << ")" ;

//     const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
//     for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
//           p != end;
//           ++p )
//     {
//         if ( (*p)->alive == DISABLE )
//             continue ;

//         if ( version() >= 7.0 )
//         {
//             ost << " (" << (*p)->shortName()
//                 << " " << (*p)->pos().x
//                 << " " << (*p)->pos().y
//                 << " " << (*p)->vel().x
//                 << " " << (*p)->vel().y
//                 << " " << Rad2IDegRound( (*p)->angleBodyCommitted() )
//                 << " " << Rad2IDegRound( (*p)->angleNeckCommitted() );
//             if ( version() >= 8.0 )
//             {
//                 double arm_dir;
//                 if ( (*p)->getArm().getRelDir ( rcss::geom::Vector2D( (*p)->pos().x,
//                                                                       (*p)->pos().y ),
//                                                 (*p)->angleBodyCommitted()
//                                                 + (*p)->angleNeckCommitted(),
//                                                 arm_dir ) )
//                     ost << " " << Rad2IDegRound( normalize_angle( arm_dir
//                                                                   + (*p)->angleNeckCommitted()
//                                                                   + (*p)->angleBodyCommitted() ) );
//                 if ( (*p)->isTackling() )
//                     ost << " t";
//             }
//             ost << ")" ;
//         }
//         else
//         {
//             ost << " (" << (*p)->name()
//                 << " " << (*p)->pos().x
//                 << " " << (*p)->pos().y
//                 << " " << Rad2IDeg( (*p)->angleBodyCommitted() )
//                 << " " << Rad2IDeg( (*p)->angleNeckCommitted() )
//                 << " " << (*p)->vel().x
//                 << " " << (*p)->vel().y
//                 << ")" ;
//         }
//     }
//     ost << ")" << std::endl;
//     send( ost.str().c_str() );
}

void
Coach::check_ball()
{
    static const char * BallPosInfoStr[] = BALL_POS_INFO_STRINGS;
    BallPosInfo info = M_stadium.ballPosInfo();

    std::ostringstream ost;
    ost << "(ok check_ball " << M_stadium.time() << " " ;
    ost << BallPosInfoStr[info] << ")";
    ost << std::ends;

    send( ost.str().c_str() );
}



OnlineCoach::OnlineCoach( Stadium & stadium,
                          Team & team )
    : Coach( stadium ),
      M_init_observer_olcoach( static_cast< rcss::InitObserverOnlineCoach * >( 0 ) ),
      M_team( team ),
      M_side( team.side() )
{
    M_freeform_messages_said = 0;
    M_freeform_messages_allowed = ServerParam::instance().freeformCountMax();
    M_define_messages_left = -1;
    M_advice_messages_left = -1;
    M_info_messages_left = -1;
    M_meta_messages_left = -1;
    M_del_messages_left = -1;
    M_rule_messages_left = -1;
    M_msg_left_update_time = -1;
}

OnlineCoach::~OnlineCoach()
{
    while ( ! M_message_queue.empty() )
    {
        rcss::clang::Msg * msg = *M_message_queue.begin();
        M_message_queue.pop_front();
        delete msg;
    }

    if ( M_init_observer_olcoach )
    {
        delete M_init_observer_olcoach;
        M_init_observer_olcoach = static_cast< rcss::InitObserverOnlineCoach * >( 0 );
    }
}

void
OnlineCoach::disable()
{
    if ( M_assigned )
    {
        std::cout << "An online coach disconnected : ("
                  << M_team.name();
        if ( ! name().empty() ) std::cout <<  " " << name();
        std::cout << ")\n";
    }

    M_assigned = false;

    if ( connected() )
    {
        RemoteClient::close();
    }
}


bool
OnlineCoach::setSenders( const double & client_version )
{
    M_version = client_version;

    rcss::SerializerOnlineCoach::Creator creator;

    if ( ! rcss::SerializerOnlineCoach::factory().getCreator( creator,
                                                              (int)client_version ) )
    {
        std::cerr << "No SerializerOnlineCoach::Creator v" << client_version << std::endl;
        return false;
    }

    const rcss::SerializerOnlineCoach::Ptr ser = creator();
    if ( ! ser )
    {
        std::cerr << "No SerializerOnlineCoach v" << client_version << std::endl;
        return false;
    }
    //this->setSerializer( *ser );

    // visual
    rcss::VisualSenderCoach::Params visual_params( getTransport(),
                                                   *this,
                                                   ser->coachSerializerPtr(),
                                                   M_stadium );
    rcss::VisualSenderCoach::Creator visual_cre;
    if ( ! rcss::VisualSenderCoach::factory().getCreator( visual_cre,
                                                          (int)client_version ) )
    {
        std::cerr << "No VisualSenderCoach::Creator v" << client_version << std::endl;
        return false;
    }
    M_observer->setVisualSender( visual_cre( visual_params ) );

    // audio
    rcss::AudioSenderOnlineCoach::Params audio_params( getTransport(),
                                                       *this,
                                                       ser,
                                                       M_stadium );
    rcss::AudioSenderOnlineCoach::Creator audio_cre;
    if ( ! rcss::AudioSenderOnlineCoach::factory().getCreator( audio_cre,
                                                               (int)client_version ) )
    {
        std::cerr << "No AudioSenderCoach::Creator v" << client_version << std::endl;
        return false;
    }
    this->setAudioSender( audio_cre( audio_params ) );

    // init
    rcss::InitSenderOnlineCoach::Params init_params( getTransport(),
                                                     *this,
                                                     ser,
                                                     M_stadium );
    rcss::InitSenderOnlineCoach::Creator init_cre;
    if ( ! rcss::InitSenderOnlineCoach::factory().getCreator( init_cre,
                                                              (int)client_version ) )
    {
        std::cerr << "No InitSenderCoach::Creator v" << client_version << std::endl;
        return false;
    }

    M_init_observer_olcoach = new rcss::InitObserverOnlineCoach;
    M_init_observer_olcoach->setInitSender( init_cre( init_params ) );

    M_assigned = true;
    return true;
}


void
OnlineCoach::sendInit()
{
    M_init_observer_olcoach->sendInit();

    M_init_observer_olcoach->sendServerParams();
    M_init_observer_olcoach->sendPlayerParams();
    M_init_observer_olcoach->sendPlayerTypes();
    M_init_observer_olcoach->sendChangedPlayers();
}

void
OnlineCoach::send( const char * msg )
{
    if ( RemoteClient::send( msg, std::strlen( msg ) + 1 ) != -1 )
    {
        M_stadium.logger().writeOnlineCoachLog( *this, msg, SEND );
    }
    else
    {
        std::cerr << __FILE__ << ": " << __LINE__ << ": ";
        perror( "Error sending to online coach" );
    }
}


void
OnlineCoach::parseMsg( char * msg,
                       const size_t & len )
{
    char * str = msg;
    if ( str[ len - 1 ] != 0 )
    {
        if ( version() >= 8.0 )
        {
            send( "(warning message_not_null_terminated)" );
        }
        str[ len ] = 0;
    }

    M_stadium.logger().writeOnlineCoachLog( *this, str, RECV );
    parse_command( str );
}


void
OnlineCoach::parse_command( const char * command )
{
    char com[128];
    int n;

    n = std::sscanf( command, " ( %127[-0-9a-zA-Z.+*/?<>_] ", com );
    if ( n < 1 )
    {
        send( "(error illegal_command_form)" );
        return;
    }

    if ( ! std::strcmp( com, "check_ball" ) )
    {
        check_ball();
    }
    else if ( ! std::strcmp( com, "look" ) )
    {
        look();
    }
    else if ( ! std::strcmp( com, "team_names" ) )
    {
        team_names();
    }
    else if ( ! std::strcmp( com, "say" ) )
    {
        if ( version() >= 7.0 )
        {
            /* parsing for new coach language */

            rcss::clang::MsgBuilder builder;
            builder.setFreeformMsgSize( ServerParam::instance().freeformMsgSize() );

            rcss::clang::Parser parser( builder );

            int ret = 1;
            try
            {
                ret = parser.parse( command );
            }
            catch ( std::exception & e )
            {
                std::cerr << e.what() << std::endl;
                if ( builder.getMsg() != NULL )
                {
                    rcss::clang::Msg * msg = builder.detatchMsg().release();
                    delete msg;
                }
                send( "(error could_not_parse_say)" );
                return;
            }

            if ( ret == 0 && builder.getMsg() != NULL )
            {
                //succeful parse
                bool should_queue = false;

                rcss::clang::Msg * msg = builder.detatchMsg().release();

                msg->setTimeRecv( M_stadium.time() );

                switch ( msg->getType() ) {
                case rcss::clang::Msg::META:
                    if ( M_stadium.playmode() != PM_PlayOn )
                    {
                        should_queue = true;
                    }
                    else if ( M_meta_messages_left > 0 )
                    {
                        //remember, negative implies we can send as many as we want
                        --M_meta_messages_left;
                        should_queue = true;
                    }
                    else
                    {
                        send( "(error said_too_many_meta_messages)" );
                    }
                    break;
                case rcss::clang::Msg::FREEFORM:
                    //if ( M_stadium.playmode() != PM_PlayOn
                    //     || M_stadium.canSendFreeform() )
                    if ( canSendFreeform() )
                    {
                        if ( M_freeform_messages_said < M_freeform_messages_allowed
                             || M_freeform_messages_allowed < 0 )
                        {
                            should_queue = true;
                            ++M_freeform_messages_said;
                        }
                        else
                        {
                            send( "(error said_too_many_freeform_messages)" );
                        }
                    }
                    else
                    {
                        send( "(error cannot_say_freeform_while_playon)" );
                    }
                    break;
                case rcss::clang::Msg::INFO:
                    if ( M_stadium.playmode() != PM_PlayOn )
                    {
                        should_queue = true;
                    }
                    else if ( M_info_messages_left > 0 )
                    {
                        //remember, negative implies we can send as many as we want
                        --M_info_messages_left;
                        should_queue = true;
                    }
                    else
                    {
                        send( "(error said_too_many_info_messages)" );
                    }
                    break;
                case rcss::clang::Msg::ADVICE:
                    if( M_stadium.playmode() != PM_PlayOn )
                    {
                        should_queue = true;
                    }
                    else if ( M_advice_messages_left > 0 )
                    {
                        //remember, negative implies we can send as many as we want
                        --M_advice_messages_left;
                        should_queue = true;
                    }
                    else
                    {
                        send( "(error said_too_many_advice_messages)" );
                    }
                    break;
                case rcss::clang::Msg::DEFINE:
                    if ( M_stadium.playmode() != PM_PlayOn )
                    {
                        should_queue = true;
                    }
                    else if ( M_define_messages_left > 0 )
                    {
                        //remember, negative implies we can send as many as we want
                        --M_define_messages_left;
                        should_queue = true;
                    }
                    else
                    {
                        send( "(error said_too_many_define_messages)" );
                    }
                    break;
                case rcss::clang::Msg::DEL:
                    if( M_stadium.playmode() != PM_PlayOn )
                    {
                        should_queue = true;
                    }
                    else if ( M_del_messages_left > 0 )
                    {
                        //remember, negative implies we can send as many as we want
                        --M_del_messages_left;
                        should_queue = true;
                    }
                    else
                    {
                        send( "(error said_too_many_del_messages)" );
                    }
                    break;
                case rcss::clang::Msg::RULE:
                    if( M_stadium.playmode() != PM_PlayOn )
                    {
                        should_queue = true;
                    }
                    else if ( M_rule_messages_left > 0 )
                    {
                        //remember, negative implies we can send as many as we want
                        --M_rule_messages_left;
                        should_queue = true;
                    }
                    else
                    {
                        send( "(error said_too_many_rule_messages)" );
                    }
                    break;
                default:
                    std::cerr << __FILE__ << ": " << __LINE__
                              << ": Coach msg has an unknown type: "
                              << msg << std::endl;
                    break;
                }

                if ( should_queue )
                {
                    M_message_queue.push_back( msg );
                    send( "(ok say)" );
                }
                else
                {
                    delete msg;
                }
            }
            else
            {
                if ( builder.getMsg() != NULL )
                {
                    rcss::clang::Msg * msg = builder.detatchMsg().release();
                    delete msg;
                }
                send( "(error could_not_parse_say)" );
            }
        }
        else
        {
            //pre v7.0
            if ( M_stadium.playmode() != PM_PlayOn )
            {
                if ( M_freeform_messages_said < M_freeform_messages_allowed
                     || M_freeform_messages_allowed < 0 )
                {
                    static char msg[MaxMesg];
                    n = std::sscanf( command, SAY_MESSAGE_SCAN_FORMAT, com, msg );
                    if( n < 2 )
                    {
                        send( "(error illegal_command_form)" );
                        return;
                    }
                    chop_last_parenthesis( msg, ServerParam::instance().freeformMsgSize() );
                    say( msg );
                    send( "(ok say)" );
                }
                else
                {
                    send( "(error said_too_many_messages)" );
                    return;
                }
            }
            else
            {
                send( "(warning cannot_say_while_playon)" );
                return;
            }
        }
    }
    else if ( ! std::strcmp( com, "bye" ) )
    {
        disable();
        return;
    }
    else if ( ! std::strcmp( com, "eye" ) )
    {
        char mode[16] ;
        if ( std::sscanf( command, " ( eye %15[^ ()] ) ", mode ) != 1 )
        {
            send( "(error illegal_command_form)" );
            return;
        }

        eye( mode );
        return;
    }
    else if ( ! std::strcmp ( com, "change_player_type" ) )
    {
        int unum, player_type;
        char goalie[16];
        if ( std::sscanf( command, " ( change_player_type %d %d )",
                          &unum, &player_type ) == 2 )
        {
            change_player_type( unum, player_type );
        }
        else if ( std::sscanf( command, " ( change_player_type %d %15[^ ( )] ) ",
                               &unum, goalie ) == 2
                  && ! std::strncmp( goalie, "goalie", 6 ) )
        {
            change_player_type_goalie( unum );
        }
        else
        {
            send( "(error illegal_command_form)" );
        }

        return;
    }
    //pfr:SYNCH
    else if ( ! std::strcmp( com, "done" ) )
    {
        //std::cerr << "Recv olc done" << std::endl;
        M_done_received = true;
        return;
    }
    else if ( ! std::strcmp( com, "compression" ) )
    {
        int level ;
        if ( std::sscanf( command, " ( compression %d ) ", &level ) != 1 )
        {
            send( "(error illegal_command_form)" );
            return;
        }

        compression( level );
        return;
    }
    else if ( ! std::strcmp( com, "team_graphic" ) )
    {
        team_graphic( command );
    }
    else
    {
        send( "(error unknown_command)" );
        return;
    }
}


void
OnlineCoach::awardFreeformMessageCount()
{
    if ( M_freeform_messages_allowed > 0 )
    {
        M_freeform_messages_allowed += ServerParam::instance().freeformCountMax();
    }
}

bool
OnlineCoach::canSendFreeform() const
{
    if ( M_stadium.playmode() != PM_PlayOn )
    {
        return true;
    }

    int playon_period = M_stadium.time() - M_stadium.lastPlayOnStart();
    if ( playon_period > (int)ServerParam::instance().freeformWaitPeriod() )
    {
        playon_period %= ServerParam::instance().freeformWaitPeriod();
        return playon_period < (int)ServerParam::instance().freeformSendPeriod();
    }
    return false;
}


void
OnlineCoach::sendPlayerClangVer()
{
    M_init_observer_olcoach->sendPlayerClangVer();
}

void
OnlineCoach::sendPlayerClangVer( const Player & player )
{
    M_init_observer_olcoach->sendPlayerClangVer( player );
}


int
OnlineCoach::check_message_queue( int time )
{
    if ( M_message_queue.size() <= 0 )
    {
        return 0;
    }

    int messages_sent = 0;
    for ( int i = ServerParam::instance().clangMessPerCycle();
          i > 0 && M_message_queue.size() > 0;
          ++i )
    {
        rcss::clang::Msg* msg = *( M_message_queue.begin() );

        if ( M_stadium.playmode() != PM_PlayOn
             || time - msg->getTimeRecv() >= ServerParam::instance().clangMessDelay() )
        {
            msg->setTimeSend( M_stadium.time() );
            msg->setSide( side() );
            say( *msg );

            M_message_queue.pop_front();
            delete msg;
            ++messages_sent;
        }
        else
        {
            break;
        }
    }
    return messages_sent;
}


void
OnlineCoach::say( const rcss::clang::Msg & message )
{
    M_stadium.sendCoachStdAudio( *this, message );
}


void
OnlineCoach::say( const char * message )
{
    M_stadium.sendCoachAudio( *this, message );
}

void
OnlineCoach::change_player_type( int unum,
                                 int player_type )
{
    if ( M_stadium.playmode() == PM_PlayOn )
    {
        send( "(warning cannot_sub_while_playon)" );
        return;
    }
    else if ( ServerParam::instance().halfTime() < 0 )
    {
        // OK
    }
    else if ( M_stadium.time()
              >= ( ( ServerParam::instance().halfTime()
                     * ServerParam::instance().nrNormalHalfs() )
                   + ( ServerParam::instance().extraHalfTime()
                       * ServerParam::instance().nrExtraHalfs() ) )
              )
    {
        // when time elapsed, do not allow substitutions anymore (for penalties)
        //send( "(warning no_subs_left)" );
        send( "(warning cannot_sub_while_penalty_shootouts)" );
        return;
    }

    if ( M_team.subsCount() >= PlayerParam::instance().subsMax() )
    {
        send( "(warning no_subs_left)" );
        return;
    }

    if ( player_type < 0
         || PlayerParam::instance().playerTypes() <= player_type )
    {
        send( "(error out_of_range_player_type)" );
        return;
    }

    // check the substitution count for each player type
    const int used_count = M_team.ptypeUsedCount( player_type );
    if ( used_count > 0 )
    {
        if ( player_type == 0
             && PlayerParam::instance().allowMultDefaultType() )
        {
            // no restriction
        }
        else if ( used_count >= PlayerParam::instance().ptMax() )
        {
            send( "(warning no_left_of_that_type)" );
            std::cerr << "(warning no_left_of_that_type) unum=" << unum
                      << " type=" << player_type
                      << std::endl;
            return;
        }
    }

    const Player * player = static_cast< const Player * >( 0 );
    for ( int i = 0; i < M_team.size(); ++i )
    {
        const Player * p = M_team.player( i );
        if ( p && p->unum() == unum )
        {
            player = p;
            break;
        }
    }

    if ( ! player )
    {
        send( "(warning no_such_player)" );
        return;
    }

    // 2009-10-29 akiyama
    // allow heterogeneous goalie
//     if ( player->isGoalie() && player_type != 0 )
//     {
//         send( "(warning cannot_change_goalie)" );
//         return;
//     }

    if ( player_type == 0
         && PlayerParam::instance().allowMultDefaultType() )
    {
        // no restriction
    }
    else if ( M_team.ptypeCount( player_type ) >= PlayerParam::instance().ptMax() )
    {
        if ( player->substituted()
             && player_type == player->playerTypeId() )
        {
            // ok
        }
        else
        {
            send( "(warning max_of_that_type_on_field)" );
            return;
        }
    }

    M_stadium.substitute( player, player_type );

    char buf[64];
    snprintf( buf, 64, "(ok change_player_type %d %d)",
              unum, player_type );
    send( buf );
}

void
OnlineCoach::change_player_types( const char * command )
{
    if ( M_stadium.time() > 0 )
    {
        send( "(warning only_before_kick_off)" );
        return;
    }
    else if ( ServerParam::instance().halfTime() < 0 )
    {
        // OK
    }
    else if ( M_stadium.time()
              >= ( ( ServerParam::instance().halfTime()
                     * ServerParam::instance().nrNormalHalfs() )
                   + ( ServerParam::instance().extraHalfTime()
                       * ServerParam::instance().nrExtraHalfs() ) )
              )
    {
        send( "(warning no_subs_left)" );
        return;
    }

    if ( M_team.subsCount() >= PlayerParam::instance().subsMax() )
    {
        send( "(warning no_subs_left)" );
        return;
    }

    int n_read = 0;
    std::sscanf( command, " ( change_player_types %n ", &n_read );
    if ( n_read == 0 )
    {
        send( "(error illegal_command_form)" );
        return;
    }

    command += n_read;

    //
    // parse the player type assignment
    //

    // key: unum, value: type id
    std::map< const Player *, int > new_assign_map;

    while ( *command == '(' )
    {
        int unum = 0, type = -1;
        int n_read = 0;
        if ( std::sscanf( command, " ( %d %d ) %n ",
                          &unum, &type, &n_read ) != 2
             || n_read == 0 )
        {
            send( "(error illegal_command_form)" );
            return;
        }

        command += n_read;

        if ( type < 0
             || PlayerParam::instance().playerTypes() <= type )
        {
            send( "(error out_of_range_player_type)" );
            return;
        }

        const Player * player = NULL;
        for ( int i = 0; i < M_team.size(); ++i )
        {
            const Player * p = M_team.player( i );
            if ( p
                 && p->isEnabled()
                 && p->unum() == unum )
            {
                player = p;
                break;
            }
        }

        if ( ! player )
        {
            send( "(error no_such_player)" );
            return;
        }

        if ( player->isGoalie() && type != 0 )
        {
            send( "(warning cannot_change_goalie)" );
            return;
        }

        new_assign_map.insert( std::make_pair( player, type ) );
    }

    if ( new_assign_map.empty() )
    {
        send( "(error illegal_command_form)" );
        return;
    }

    // copy the assigment map contained in the command
    const std::map< const Player *, int > command_assign_map = new_assign_map;

    // add the prevous assignment
    for ( int i = 0; i < M_team.size(); ++i )
    {
        const Player * p = M_team.player( i );
        if ( p )
        {
            std::map< const Player *, int >::iterator it = new_assign_map.find( p );
            if ( it == new_assign_map.end() )
            {
                new_assign_map.insert( std::make_pair( p, p->playerTypeId() ) );
            }
        }
    }


    // check the total number of each player type
    {
        // key: type id, value: count
        std::map< int, int > type_count;

        for ( int id = 0; id < PlayerParam::instance().playerTypes(); ++id )
        {
            type_count[ id ] = 0;
        }

        for ( std::map< const Player *, int >::iterator it = new_assign_map.begin();
              it != new_assign_map.end();
              ++it )
        {
            type_count[ it->second ] += 1;
        }

        for ( std::map< int, int >::const_iterator it = type_count.begin();
              it != type_count.end();
              ++it )
        {
            if ( ServerParam::instance().verboseMode() )
            {
                std::cout << "change_player_types: the number of type "
                          << it->first << " = " << it->second
                          << std::endl;
            }

            if ( it->first == 0
                 && PlayerParam::instance().allowMultDefaultType() )
            {
                // no error
            }
            else if ( it->second > PlayerParam::instance().ptMax() )
            {
                char err_msg[128];
                snprintf( err_msg, 128, "(warning max_of_type_%d_on_field)",
                          it->first );
                send( err_msg );
                return;
            }
        }
    }

    // substitute & send ok message
    for ( std::map< const Player *, int >::const_iterator it = command_assign_map.begin();
          it != command_assign_map.end();
          ++it )
    {
        if ( M_team.subsCount() >= PlayerParam::instance().subsMax() )
        {
            send( "(warning no_subs_left)" );
            return;
        }

        if ( ServerParam::instance().verboseMode() )
        {
            std::cerr << "change_player_types: substitute (player "
                      << M_team.name() << ' '
                      << it->first->unum()
                      << ") to type " << it->second
                      << std::endl;
        }

        M_stadium.substitute( it->first, it->second );

        char buf[64];
        snprintf( buf, 64, "(ok change_player_type %d %d)",
                  it->first->unum(), it->second );
        send( buf );
    }

}

void
OnlineCoach::team_graphic( const char * command )
{
    if ( M_stadium.playmode() != PM_BeforeKickOff )
    {
        send( "(warning only_before_kick_off)" );
        return;
    }

    unsigned int x, y;
    if ( std::sscanf( command, " ( team_graphic ( %u %u ", &x, &y ) != 2 )
    {
        send( "(error illegal_command_form)" );
        return;
    }

    if ( x >= 32 || y >= 8 )
        // needs to be parameterised
    {
        send( "(warning invalid_tile_location)" );
    }

    std::auto_ptr< XPMHolder > holder( new XPMHolder( command ) );
    if ( ! holder->valid() )
    {
        send( "(error invalid_xpm_data)" );
        return;
    }

    if ( holder->width() != 8 || holder->height() != 8 )
        // needs to be parameterised
    {
        send( "(warning invalid_tile_size)" );
        return;
    }

    M_team.addTeamGraphic( x, y, holder );
    M_stadium.sendTeamGraphic( side(), x, y );

    std::ostringstream msg;
    msg << "(ok team_graphic " << x << " " << y << ")";
    send( msg.str().c_str() );
}


void
OnlineCoach::change_player_type_goalie( int unum )
{
    const PlayMode pm = M_stadium.playmode();
    if ( pm == PM_PlayOn )
    {
        send( "(warning cannot_change_goalie_while_playon)" );
        return;
    }

    if ( pm == PM_PenaltyReady_Left
         || pm == PM_PenaltyReady_Right
         || pm == PM_PenaltyTaken_Left
         || pm == PM_PenaltyTaken_Right )
    {
        send( "(warning cannot_change_goalie_while_penalty_taken)" );
        return;
    }

    if ( change_player_type_goalie_impl( &M_team, unum ) )
    {
        char buf[64];
        snprintf( buf, 64, "(ok change_player_type %d goalie)",
                  unum );
        send( buf );
    }
}


bool
OnlineCoach::update_messages_left( int time )
{
    if ( M_msg_left_update_time >= 0 &&
         time - M_msg_left_update_time < ServerParam::instance().clangWinSize() )
    {
        return false;
    }

    M_define_messages_left = ServerParam::instance().clangDefineWin();
    M_meta_messages_left = ServerParam::instance().clangMetaWin();
    M_advice_messages_left = ServerParam::instance().clangAdviceWin();
    M_info_messages_left = ServerParam::instance().clangInfoWin();
    M_del_messages_left = ServerParam::instance().clangDelWin();
    M_rule_messages_left = ServerParam::instance().clangRuleWin();

    M_msg_left_update_time = time;

    return true;
}

// -*-c++-*-

/***************************************************************************
                                 monitor.cc
    A class providing the communication interface for remote monitors that
    connect to the server
                             -------------------
    begin                : 27-DEC-2001
    copyright            : (C) 2001 by The RoboCup Soccer Server
                           Maintainance Group.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "monitor.h"

#include "field.h"
#include "player.h"
#include "types.h"

namespace {

PlayMode
play_mode_id( const char *mode )
{
    static char * s_playmode_strings[] = PLAYMODE_STRINGS;

    for ( int n = 0; n < PM_MAX; ++n )
    {
        if ( ! std::strcmp( s_playmode_strings[n], mode ) )
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
        str[max_size] = NULLCHAR;
    }
    else
    {
        --l;
        if( str[l] == ')' ) str[l] = NULLCHAR;
    }
}

}


Monitor::Monitor( Stadium & stadium,
                  const double & version )
    : M_stadium( stadium ),
      M_version ( version )
{

}

Monitor::~Monitor()
{

}


int
Monitor::send( const char * msg )
{
    if ( version() >= 3.0 )
    {
        return RemoteClient::send( msg, std::strlen( msg ) + 1 );
    }
    else if ( version() >= 2.0 )
    {
        dispinfo_t2 minfo;
        minfo.mode = htons( MSG_MODE );
        minfo.body.msg.board = htons( MSG_BOARD );
        std::strncpy( minfo.body.msg.message, msg, max_message_length_for_display );
        return RemoteClient::send( reinterpret_cast< char * >( &minfo ),
                                   sizeof( dispinfo_t2 ) );
    }
    else if ( version() >= 1.0 )
    {
        dispinfo_t minfo;
        minfo.mode = htons( MSG_MODE );
        minfo.body.msg.board = htons(MSG_BOARD);
        std::strncpy( minfo.body.msg.message, msg, max_message_length_for_display );
        return RemoteClient::send( reinterpret_cast< const char * >( &minfo ),
                                   sizeof( dispinfo_t ) );
    }

    return 0;
}


void
Monitor::sendInit()
{
    if ( version() >= 3.0 )
    {


    }
    else if ( version() >= 2.0 )
    {
        dispinfo_t2 di;

        di.mode = htons( PARAM_MODE );
        di.body.sparams = ServerParam::instance().convertToStruct();
        RemoteClient::send( reinterpret_cast< const char* >( &di ),
                            sizeof( dispinfo_t2 ) );

        di.mode = htons( PPARAM_MODE );
        di.body.pparams = PlayerParam::instance().convertToStruct();
        RemoteClient::send( reinterpret_cast< const  char* >( &di ),
                            sizeof( dispinfo_t2 ) );

        di.mode = htons ( PT_MODE );
        for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
        {
            const HeteroPlayer * p = M_stadium.playerType( i );
            if ( p )
            {
                di.body.ptinfo = p->convertToStruct( i );
                RemoteClient::send( reinterpret_cast< const char* >( &di ),
                                    sizeof( dispinfo_t2 ) );
            }
        }
    }
    else
    {
        // no message
    }
}

void
Monitor::sendShow()
{
    if ( version() < 3.0 )
    {
        return;
    }

#ifdef HAVE_SSTREAM
    std::ostringstream os;
#else
    std::ostrstream os;
#endif

    os << "(show " << M_stadium.time();
    os << " (" << BALL_NAME_SHORT << ' '
       << M_stadium.ball().pos().x << ' '
       << M_stadium.ball().pos().y << ' '
       << M_stadium.ball().vel().x << ' '
       << M_stadium.ball().vel().y << ')';

    const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        os << " (";
        os << "(p " << SideStr( (*p)->team()->side() )
           << ' ' << (*p)->unum()
           << ' ' << (*p)->playerTypeId()
           << ')';
        os << " (state " << (*p)->state() << ')'; // include goalie flag
        os << " (pos "
           << (*p)->pos().x << ' '
           << (*p)->pos().y << ' '
           << (*p)->vel().x << ' '
           << (*p)->vel().y << ' '
           << (*p)->angleBodyCommitted() << ' '
           << (*p)->angleNeckCommitted();
        rcss::geom::Vector2D arm_dest;
        if ( (*p)->arm().getRelDest( rcss::geom::Vector2D( (*p)->pos().x,
                                                           (*p)->pos().y ),
                                     (*p)->angleBodyCommitted()
                                     + (*p)->angleNeckCommitted(),
                                     arm_dest ) )
        {
            os << ' ' << arm_dest.getMag()
               << ' ' << arm_dest.getHead();
        }
        os << ')';
        os << " (view "
           << (*p)->visibleAngle()
           << ( (*p)->highquality() ? " high" : " low" )
           << ')';
        os << " (stamina "
           << (*p)->stamina() << ' '
           << (*p)->effort() << ' '
           << (*p)->recovery() << ')';
#if 0
        os << " (count (k "
           << (*p)->kickCount()
           << ") (d " << (*p)->dashCount()
           << ") (t " << (*p)->turnCount()
           << ") (s " << (*p)->sayCount()
           << ") (tn " << (*p)->turnNeckCount()
           << ") (c " << (*p)->catchCount()
           << ") (m " << (*p)->moveCount()
           << ") (cv " << (*p)->changeViewCount()
           << ") (ta " << (*p)->tackleCount()
           << ") (p " << (*p)->arm().getCounter()
           << ") (a " << (*p)->attentiontoCount()
           << "))";
#endif
        os << ')'; // end of player
    }

    os << ')';
#ifdef HAVE_SSTREAM
    send( os.str().c_str() );
#else
    os << std::ends;
    send( os.str() );
    os.freeze( false );
#endif
}

bool
Monitor::parseCommand( const char * message )
{
    if ( ! std::strcmp( message, "(dispbye)" ) )
	  {
        disable();
        return true;
    }
    else if ( ! std::strcmp( message, "(dispstart)" ) )
    {
        // kick off
        Stadium::_Start( M_stadium );
        return true;
    }
    else if ( ! std::strncmp( message, "(dispplayer", 11 ) )
    {
        return dispplayer( message );
    }
    else if ( ! std::strncmp( message, "(dispdiscard", 12 ) )
    {
        return dispdiscard( message );
    }
    else if ( ! std::strncmp( message, "(compression", 12 ) )
    {
        return compression( message );
    }
    else if ( ! std::strncmp( message, "(dispfoul", 9 ) )
    {
        return dispfoul( message );
    }
    else if ( ServerParam::instance().coachMode() )
    {
        if ( ! std::strncmp( message, "(start)", 7 ) )
        {
            Stadium::_Start( M_stadium );
            send( "(ok start)" );
            return true;
        }
        else if ( ! std::strncmp( message, "(change_mode", 12 ) )
        {
            return coach_change_mode( message );
        }
        else if ( ! std::strncmp( message, "(move", 5 ) )
        {
            return coach_move( message );
        }
        else if ( ! std::strncmp( message, "(recover", 8 ) )
        {
            return coach_recover();
        }
        else if ( ! std::strcmp( message, "change_player_type" ) )
        {
            return coach_change_player_type( message );
        }
        else if ( ! std::strncmp( message, "(check_ball", 11 ) )
        {
            return coach_check_ball();
        }
        else
        {
            send( "(error illegal_command_form)" );
            return false;
        }
    }
    else
    {
        send( "(error illegal_command_form)" );
        return false;
    }

    return true;
}

bool
Monitor::dispfoul( const char * command )
{
    // foul or drop_ball
    int x, y, side;
    if ( std::sscanf( command,
                      "(dispfoul %d %d %d)",
                      &x, &y, &side ) != 3 )
    {
        send( "(error illegal_command_form)" );
        return false;
    }

    double real_x = x / SHOWINFO_SCALE;
    double real_y = y / SHOWINFO_SCALE;
    if ( static_cast< Side >( side ) == NEUTRAL )
    {
        M_stadium.referee_drop_ball( real_x, real_y,
                                     static_cast< Side >( side ) );
    }
    else
    {
        M_stadium.referee_get_foul( real_x, real_y,
                                    static_cast< Side >( side ) );
    }

    return true;
}

bool
Monitor::dispplayer( const char * command )
{
    // a player is given new position by the monitor
    int side, unum;
    int x, y, a;
    if ( std::sscanf( command,
                      "(dispplayer %d %d %d %d %d)",
                      &side, &unum, &x, &y, &a ) != 5 )
    {
        send( "(error illegal_command_form)" );
        return false;
    }

    double real_x = x / SHOWINFO_SCALE;
    double real_y = y / SHOWINFO_SCALE;
    double angle = Deg2Rad( a );
    PVector vel( 0.0, 0.0 );

    return M_stadium.movePlayer( static_cast< Side >( side ),
                                 unum,
                                 PVector( real_x, real_y ),
                                 &angle,
                                 &vel );
}

bool
Monitor::dispdiscard( const char * command )
{
    // a player is discarded by the monitor
    int side = 0, unum = 0;
    if ( std::sscanf( command,
                      "(dispdiscard %d %d)",
                      &side, &unum ) != 2 )
    {
        send( "(error illegal_command_form)" );
        return false;
    }

    M_stadium.discard_player( static_cast< Side >( side ), unum );
    return true;
}

bool
Monitor::compression( const char * command )
{
    int level = 0;
    if ( std::sscanf( command, "(compression %d)",
                      &level ) != 1 )
    {
        send( "(error illegal_command_form)" );
        return false;
    }
#ifdef HAVE_LIBZ
    if ( level > 9 )
    {
        send( "(error illegal_command_form)" );
        return false;
    }

#ifdef HAVE_SSTREAM
    std::ostringstream reply;
    reply << "(ok compression " << level << ")";
    send( reply.str().c_str() );
#else
    std::ostrstream reply;
    reply << "(ok compression " << level << ")" << std::ends;
    send( reply.str() );
    reply.freeze( false );
#endif
    setCompressionLevel( level );
    return true;
#else
    send( "(warning compression_unsupported)" );
    return false;
#endif
}

bool
Monitor::coach_change_mode( const char * command )
{
    char new_mode[128];
    if ( std::sscanf( command,
                      "(change_mode %127[-0-9a-zA-Z.+*/?<>_])",
                      new_mode ) != 1 )
    {
        send( "(error illegal_command_form)" );
        return false;
    }

    PlayMode mode_id = play_mode_id( new_mode );

    if ( mode_id == PM_Null )
    {
        send( "(error illegal_mode)" );
        return false;
    }

    M_stadium.change_play_mode( mode_id );
    send( "(ok change_mode)" );
    return true;
}

bool
Monitor::coach_move( const char * command )
{
    char obj[128];
    double x = 0.0, y = 0.0, ang = 0.0, velx = 0.0, vely = 0.0;

    int n = std::sscanf( command,
                         " (move (%127[^)]) %lf %lf %lf %lf %lf ) ",
                         obj, &x, &y, &ang, &velx, &vely );

    if ( n < 3
         || std::isnan( x ) != 0
         || std::isnan( y ) != 0
         || std::isnan( ang ) != 0
         || std::isnan( velx ) != 0
         || std::isnan( vely ) != 0 )
    {
        send( "(error illegal_object_form)" );
        return false;
    }

    std::string obj_name = "(";
    obj_name += obj;
    obj_name += ')';

    if ( obj_name == BALL_NAME )
    {
        M_stadium.clearBallCatcher();

        if ( n == 3 || n == 4 )
        {
            M_stadium.set_ball( LEFT, PVector( x, y ) );
        }
        else if ( n == 6 )
        {
            M_stadium.set_ball( NEUTRAL, PVector( x, y ), PVector( velx, vely ) );
        }
        else
        {
            send( "(error illegal_command_form)" );
            return false;
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
            return false;
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
            return false;
        }
    }

    send( "(ok move)" );
    return true;
}

bool
Monitor::coach_recover()
{
    M_stadium.recoveryPlayers();

    send( "(ok recover)" );
    return true;
}

bool
Monitor::coach_check_ball()
{
#ifdef HAVE_SSTREAM
    std::ostringstream ost;
#else
    std::ostrstream ost;
#endif

    static char* s_ball_pos_info_str[] = BALL_POS_INFO_STRINGS;
    BallPosInfo info = M_stadium.ballPosInfo();

    ost << "(ok check_ball " << M_stadium.time() << " " ;

    ost << s_ball_pos_info_str[info] << ")";

    ost << std::ends;

#ifdef HAVE_SSTREAM
    send( ost.str().c_str() );
#else
    ost << std::ends;

    send( ost.str() );
    ost.freeze( false );
#endif

    return true;
}

bool
Monitor::coach_change_player_type( const char * command )
{
    char teamname[128];
    int unum, player_type;
    if ( std::sscanf( command,
                      "(change_player_type %127s %d %d)",
                      teamname, &unum, &player_type ) != 3 )
    {
        send( "(error illegal_command_form)" );
        return false;
    }

    const Team * team = NULL;
    if ( M_stadium.teamLeft().name() == teamname )
    {
        team = &( M_stadium.teamLeft() );
    }
    else if ( M_stadium.teamRight().name() == teamname )
    {
        team = &( M_stadium.teamRight() );
    }

    if ( team == NULL )
    {
        send( "(warning no_team_found)" );
        return false;
    }

    if ( player_type < 0
         || player_type >= PlayerParam::instance().playerTypes() )
    {
        send( "(error out_of_range_player_type)" );
        return false;
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
        return false;
    }

    M_stadium.substitute( player, player_type );

    char buf[64];
    std::snprintf( buf, 64,
                   "(ok change_player_type %s %d %d)",
                   teamname, unum, player_type );

    send( buf );

    return true;
}

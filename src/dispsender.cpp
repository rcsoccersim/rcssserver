// -*-c++-*-

/***************************************************************************
                                dispsender.cpp
                  Classes for sending display messages
                             -------------------
    begin                : 2008-02-14
    copyright            : (C) 2008 by The RoboCup Soccer Simulator
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

#include "dispsender.h"

#include "monitor.h"
#include "serializermonitor.h"
#include "team.h"
#include "player.h"
#include "stadium.h"
#include "types.h"
#include "xpmholder.h"

#include <sstream>
#include <cmath>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif

namespace rcss {

/*!
//===================================================================
//
//  CLASS: DispSender
//
//  DESC: Base class for the display protocol.
//
//===================================================================
*/

DispSender::DispSender( std::ostream & transport )
    : Sender( transport )
{

}

DispSender::~DispSender()
{

}

/*!
//===================================================================
//
//  CLASS: DispSenderMonitor
//
//  DESC: Base class for the display protocol for monitors.
//
//===================================================================
*/

DispSenderMonitor::FactoryHolder &
DispSenderMonitor::factory()
{
    static FactoryHolder rval;
    return rval;
}

DispSenderMonitor::DispSenderMonitor( const Params & params )
    : DispSender( params.M_transport ),
      M_serializer( params.M_serializer ),
      M_self( params.M_self ),
      M_stadium( params.M_stadium )
{

}

DispSenderMonitor::~DispSenderMonitor()
{
    //std::cerr << "delete DispSenderMonitor" << std::endl;
}


/*!
//===================================================================
//
//  CLASS: DispSenderMonitorV1
//
//  DESC: version 1 of the display protocol.
//
//===================================================================
*/

DispSenderMonitorV1::DispSenderMonitorV1( const Params & params )
    : DispSenderMonitor( params )
{

}

DispSenderMonitorV1::~DispSenderMonitorV1()
{

}

void
DispSenderMonitorV1::sendShow()
{
    static dispinfo_t dinfo;
    static int last_sent_time = -1;
    static int last_sent_stoppage_time = -1;

    //
    // send cached data
    //

    if ( stadium().time() == last_sent_time
         && stadium().stoppageTime() == last_sent_stoppage_time )
    {
        transport().write( reinterpret_cast< char * >( &dinfo ),
                           sizeof( dispinfo_t ) );
        transport().flush();
        return;
    }

    last_sent_time = stadium().time();
    last_sent_stoppage_time = stadium().stoppageTime();

    //
    // show mode
    //

    dinfo.mode = htons( SHOW_MODE );

    //
    // playmode
    //

    dinfo.body.show.pmode = static_cast< char >( stadium().playmode() );

    //
    // teams
    //

    if ( ! stadium().teamLeft().name().empty() )
    {
        std::strncpy( dinfo.body.show.team[0].name,
                      stadium().teamLeft().name().c_str(),
                      std::min( stadium().teamLeft().name().length(), size_t( 16 ) ) );
        dinfo.body.show.team[0].score = htons( stadium().teamLeft().point() );
    }
    else
    {
        std::strcpy( dinfo.body.show.team[0].name, "" );
        dinfo.body.show.team[0].score = htons( 0 );
    }

    if ( ! stadium().teamRight().name().empty() )
    {
        std::strncpy( dinfo.body.show.team[1].name,
                      stadium().teamRight().name().c_str(),
                      std::min( stadium().teamRight().name().length(), size_t( 16 ) ) );
        dinfo.body.show.team[1].score = htons( stadium().teamRight().point() );
    }
    else
    {
        std::strcpy( dinfo.body.show.team[1].name, "" );
        dinfo.body.show.team[1].score = htons( 0 );
    }

    //
    // ball
    //

    dinfo.body.show.pos[0].x = htons( (Int16)rint( ( stadium().ball().pos().x * SHOWINFO_SCALE ) ) );
    dinfo.body.show.pos[0].y = htons( (Int16)rint( ( stadium().ball().pos().y * SHOWINFO_SCALE ) ) );

    //
    // players
    //

    const Stadium::PlayerCont & players = stadium().players();
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        dinfo.body.show.pos[i+1].enable = htons( players[i]->state() );
        dinfo.body.show.pos[i+1].x = htons( (Int16)rint( players[i]->pos().x * SHOWINFO_SCALE ) );
        dinfo.body.show.pos[i+1].y = htons( (Int16)rint( players[i]->pos().y * SHOWINFO_SCALE ) );
        dinfo.body.show.pos[i+1].angle = htons( (Int16)Rad2IDegRound( players[i]->angleBodyCommitted() ) );
        dinfo.body.show.pos[i+1].side = htons( players[i]->side() );
        dinfo.body.show.pos[i+1].unum = htons( players[i]->unum() );
    }

    //
    // time
    //

    dinfo.body.show.time = htons( stadium().time() );


    //
    // write to stream
    //

    transport().write( reinterpret_cast< char * >( &dinfo ),
                       sizeof( dispinfo_t ) );
    transport().flush();
}

void
DispSenderMonitorV1::sendMsg( const BoardType board,
                              const char * msg )
{
    dispinfo_t minfo;
    minfo.mode = htons( MSG_MODE );
    minfo.body.msg.board = htons( board );
    std::strncpy( minfo.body.msg.message, msg, max_message_length_for_display );

    transport().write( reinterpret_cast< const char * >( &minfo ),
                       sizeof( dispinfo_t ) );
    transport().flush();
}

void
DispSenderMonitorV1::sendTeamGraphic( const Side side,
                                      const unsigned int x,
                                      const unsigned int y )
{
    const XPMHolder * xpm = ( side == LEFT
                              ? stadium().teamLeft().teamGraphic( x, y )
                              : side == RIGHT
                              ? stadium().teamRight().teamGraphic( x, y )
                              : static_cast< const XPMHolder * >( 0 ) );
    if ( ! xpm
         || ! xpm->valid() )
    {
        return;
    }

    std::ostringstream data;

    serializer().serializeTeamGraphic( data, side, x, y, xpm );

    sendMsg( MSG_BOARD, data.str().c_str() );
}

/*!
//===================================================================
//
//  CLASS: DispSenderMonitorV2
//
//  DESC: version 2 of display protocol.
//
//===================================================================
*/

DispSenderMonitorV2::DispSenderMonitorV2( const Params & params )
    : DispSenderMonitorV1( params )
{

}

DispSenderMonitorV2::~DispSenderMonitorV2()
{

}

void
DispSenderMonitorV2::sendShow()
{
    static dispinfo_t2 dinfo;
    static int last_sent_time = -1;
    static int last_sent_stoppage_time = -1;

    //
    // send cached data
    //

    if ( stadium().time() == last_sent_time
         && stadium().stoppageTime() == last_sent_stoppage_time )
    {
        transport().write( reinterpret_cast< char * >( &dinfo ),
                           sizeof( dispinfo_t2 ) );
        transport().flush();
        return;
    }

    last_sent_time = stadium().time();
    last_sent_stoppage_time = stadium().stoppageTime();

    //
    // show mode
    //

    dinfo.mode = htons( SHOW_MODE );

    //
    // playmode
    //

    dinfo.body.show.pmode = static_cast< char >( stadium().playmode() );

    //
    // teams
    //

    if ( ! stadium().teamLeft().name().empty() )
    {
        std::strncpy( dinfo.body.show.team[0].name,
                      stadium().teamLeft().name().c_str(),
                      std::min( stadium().teamLeft().name().length(), size_t( 16 ) ) );
        dinfo.body.show.team[0].score = htons( stadium().teamLeft().point() );
    }
    else
    {
        std::strcpy( dinfo.body.show.team[0].name, "" );
        dinfo.body.show.team[0].score = htons( 0 );
    }

    if ( ! stadium().teamRight().name().empty() )
    {
        std::strncpy( dinfo.body.show.team[1].name,
                      stadium().teamRight().name().c_str(),
                      std::min( stadium().teamRight().name().length(), size_t( 16 ) ) );
        dinfo.body.show.team[1].score = htons( stadium().teamRight().point() );
    }
    else
    {
        std::strcpy( dinfo.body.show.team[1].name, "" );
        dinfo.body.show.team[1].score = htons( 0 );
    }

    //
    // ball
    //

    dinfo.body.show.ball.x = htonl( (Int32)rint( ( stadium().ball().pos().x * SHOWINFO_SCALE2 ) ) );
    dinfo.body.show.ball.y = htonl( (Int32)rint( ( stadium().ball().pos().y * SHOWINFO_SCALE2 ) ) );
    dinfo.body.show.ball.deltax = htonl( (Int32)rint( ( stadium().ball().vel().x * SHOWINFO_SCALE2 ) ) );
    dinfo.body.show.ball.deltay = htonl( (Int32)rint( ( stadium().ball().vel().y * SHOWINFO_SCALE2) ) );

    //
    // players
    //

    const Stadium::PlayerCont & players = stadium().players();
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        dinfo.body.show.pos[i].mode = htons( players[i]->state() );
        dinfo.body.show.pos[i].type = htons( players[i]->playerTypeId() );

        //M_players[i]->alive &= (STAND | GOALIE | DISCARD);
        dinfo.body.show.pos[i].x = htonl( (Int32)rint( players[i]->pos().x * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].y = htonl( (Int32)rint( players[i]->pos().y * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].deltax = htonl( (Int32)rint( players[i]->vel().x * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].deltay = htonl( (Int32)rint( players[i]->vel().y * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].body_angle = htonl( (Int32)rint( players[i]->angleBodyCommitted() * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].head_angle = htonl( (Int32)rint( players[i]->angleNeckCommitted() * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].view_width = htonl( (Int32)rint( players[i]->visibleAngle() * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].view_quality = htons( (Int16)rint( players[i]->highQuality() ) );

        dinfo.body.show.pos[i].stamina = htonl( (Int32)rint( players[i]->stamina() * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].effort = htonl( (Int32)rint( players[i]->effort() * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].recovery = htonl( (Int32)rint( players[i]->recovery() * SHOWINFO_SCALE2 ) );

        dinfo.body.show.pos[i].kick_count = htons( players[i]->kickCount() );
        dinfo.body.show.pos[i].dash_count = htons( players[i]->dashCount() );
        dinfo.body.show.pos[i].turn_count = htons( players[i]->turnCount() );
        dinfo.body.show.pos[i].say_count = htons( players[i]->sayCount() );
        dinfo.body.show.pos[i].tneck_count = htons( players[i]->turnNeckCount() );
        dinfo.body.show.pos[i].catch_count = htons( players[i]->catchCount() );
        dinfo.body.show.pos[i].move_count = htons( players[i]->moveCount() );
        dinfo.body.show.pos[i].chg_view_count = htons( players[i]->changeViewCount() );
    }

    //
    // time
    //

    dinfo.body.show.time = htons( stadium().time() );


    //
    // write to stream
    //

    transport().write( reinterpret_cast< char * >( &dinfo ),
                       sizeof( dispinfo_t2 ) );
    transport().flush();
}

void
DispSenderMonitorV2::sendMsg( const BoardType board,
                              const char * msg )
{
    dispinfo_t2 minfo;
    minfo.mode = htons( MSG_MODE );
    minfo.body.msg.board = htons( board );
    std::strncpy( minfo.body.msg.message, msg, max_message_length_for_display );

    transport().write( reinterpret_cast< char * >( &minfo ),
                       sizeof( dispinfo_t2 ) );
    transport().flush();
}

/*!
//===================================================================
//
//  CLASS: DispSenderMonitorV3
//
//  DESC: version 3 of display protocol.
//
//===================================================================
*/

DispSenderMonitorV3::DispSenderMonitorV3( const Params & params )
    : DispSenderMonitorV2( params )
{

}

DispSenderMonitorV3::~DispSenderMonitorV3()
{

}

void
DispSenderMonitorV3::sendShow()
{
    static std::string message;
    static int last_sent_time = -1;
    static int last_sent_stoppage_time = -1;

    //
    // send cached data
    //

    if ( stadium().time() == last_sent_time
         && stadium().stoppageTime() == last_sent_stoppage_time )
    {
        transport() << message << std::ends << std::flush;
        return;
    }

    last_sent_time = stadium().time();
    last_sent_stoppage_time = stadium().stoppageTime();

    message.erase();

    //
    // create new data
    //

    std::ostringstream ostr;

    serializer().serializeShowBegin( ostr,
                                     stadium().time() );
    serializer().serializePlayModeId( ostr,
                                      stadium().playmode() );
    serializer().serializeScore( ostr,
                                 stadium().teamLeft(),
                                 stadium().teamRight() );

    serializer().serializeBall( ostr,
                                stadium().ball() );

    const Stadium::PlayerCont::const_iterator end = stadium().players().end();
    for ( Stadium::PlayerCont::const_iterator p = stadium().players().begin();
          p != end;
          ++p )
    {
        serializer().serializePlayerBegin( ostr, **p );
        serializer().serializePlayerPos( ostr, **p );
        serializer().serializePlayerArm( ostr, **p );
        serializer().serializePlayerViewMode( ostr, **p );
        serializer().serializePlayerStamina( ostr, **p );
        serializer().serializePlayerFocus( ostr, **p );
        serializer().serializePlayerCounts( ostr, **p );
        serializer().serializePlayerEnd( ostr );
    }

    serializer().serializeShowEnd( ostr );

    message = ostr.str();
    transport() << message << std::ends << std::flush;
}

void
DispSenderMonitorV3::sendMsg( const BoardType board,
                              const char * msg )
{
    char buf[MaxMesg];
    snprintf( buf, MaxMesg,
              "(msg %d %d \"%s\")",
              stadium().time(), board, msg );

    transport() << buf << std::ends << std::flush;
}


// /*!
// //===================================================================
// //
// //  CLASS: DispSenderMonitorV4
// //
// //  DESC: version 4 of display protocol.
// //
// //===================================================================
// */

// DispSenderMonitorV4::DispSenderMonitorV4( const Params & params )
//     : DispSenderMonitorV3( params )
// {

// }

// DispSenderMonitorV4::~DispSenderMonitorV4()
// {

// }

// void
// DispSenderMonitorV4::sendShow()
// {

// }

// void
// DispSenderMonitorV4::sendMsg( const BoardType board,
//                               const char * msg )
// {

// }


/*!
//===================================================================
//
//  CLASS: DispSenderLogger
//
//  DESC: Base class for the display protocol for logger.
//
//===================================================================
*/

DispSenderLogger::FactoryHolder &
DispSenderLogger::factory()
{
    static FactoryHolder rval;
    return rval;
}

DispSenderLogger::DispSenderLogger( const Params & params )
    : DispSender( params.M_transport )
    , M_serializer( params.M_serializer )
    , M_self( params.M_self )
    , M_stadium( params.M_stadium )
{

}

DispSenderLogger::~DispSenderLogger()
{
    //std::cerr << "delete DispSenderLogger" << std::endl;
}


/*!
//===================================================================
//
//  CLASS: DispSenderLoggerV1
//
//  DESC: version 1 log format
//
//===================================================================
*/


DispSenderLoggerV1::DispSenderLoggerV1( const Params & params )
    : DispSenderLogger( params )
{

}

DispSenderLoggerV1::~DispSenderLoggerV1()
{

}

void
DispSenderLoggerV1::sendShow()
{
    dispinfo_t disp;

    disp.mode = htons( SHOW_MODE );

    disp.body.show.time = htons( stadium().time() );

    disp.body.show.pmode = static_cast< char >( stadium().playmode() );

    disp.body.show.pos[0].x = htons( (Int16)rint( ( stadium().ball().pos().x * SHOWINFO_SCALE ) ) );
    disp.body.show.pos[0].y = htons( (Int16)rint( ( stadium().ball().pos().y * SHOWINFO_SCALE ) ) );

    const Stadium::PlayerCont & players = stadium().players();
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        disp.body.show.pos[i+1].enable = htons( players[i]->state() );
        disp.body.show.pos[i+1].x = htons( (Int16)rint( players[i]->pos().x * SHOWINFO_SCALE ) );
        disp.body.show.pos[i+1].y = htons( (Int16)rint( players[i]->pos().y * SHOWINFO_SCALE ) );
        disp.body.show.pos[i+1].angle = htons( (Int16)Rad2IDegRound( players[i]->angleBodyCommitted() ) );
        disp.body.show.pos[i+1].side = htons( players[i]->side() );
        disp.body.show.pos[i+1].unum = htons( players[i]->unum() );
    }

    if ( ! stadium().teamLeft().name().empty() )
    {
        std::strncpy( disp.body.show.team[0].name,
                      stadium().teamLeft().name().c_str(),
                      std::min( stadium().teamLeft().name().length() + 1, size_t( 16 ) ) );
        disp.body.show.team[0].score = htons( stadium().teamLeft().point() );
    }
    else
    {
        std::memset( &disp.body.show.team[0].name,
                     0,
                     sizeof( disp.body.show.team[0].name ) );
    }

    if ( ! stadium().teamRight().name().empty() )
    {
        std::strncpy( disp.body.show.team[1].name,
                      stadium().teamRight().name().c_str(),
                      std::min( stadium().teamRight().name().length() + 1, size_t( 16 ) ) );
        disp.body.show.team[1].score = htons( stadium().teamRight().point() );
    }
    else
    {
        std::memset( &disp.body.show.team[1].name,
                     0,
                     sizeof( disp.body.show.team[1].name ) );
    }

    transport().write( reinterpret_cast< const char * >( &disp ),
                       sizeof( dispinfo_t ) );
    //transport().flush();
}

void
DispSenderLoggerV1::sendMsg( const BoardType board,
                             const char * msg )
{
    dispinfo_t disp;
    disp.mode = htons( MSG_MODE );
    disp.body.msg.board = htons( board );
    std::strncpy( disp.body.msg.message,
                  msg,
                  std::min( sizeof( disp.body.msg.message ),
                            std::strlen( msg ) ) );

    transport().write( reinterpret_cast< const char * >( &disp ),
                       sizeof( dispinfo_t ) );
}

void
DispSenderLoggerV1::sendTeamGraphic( const Side side,
                                     const unsigned int x,
                                     const unsigned int y )
{
    const XPMHolder * xpm = ( side == LEFT
                              ? stadium().teamLeft().teamGraphic( x, y )
                              : side == RIGHT
                              ? stadium().teamRight().teamGraphic( x, y )
                              : static_cast< const XPMHolder * >( 0 ) );
    if ( ! xpm
         || ! xpm->valid() )
    {
        return;
    }

    std::ostringstream data;

    serializer().serializeTeamGraphic( data, side, x, y, xpm );

    sendMsg( MSG_BOARD, data.str().c_str() );
}

/*!
//===================================================================
//
//  CLASS: DispSenderLoggerV2
//
//  DESC: version 2 log format
//
//===================================================================
*/


DispSenderLoggerV2::DispSenderLoggerV2( const Params & params )
    : DispSenderLoggerV1( params )
{

}

DispSenderLoggerV2::~DispSenderLoggerV2()
{

}

void
DispSenderLoggerV2::sendShow()
{
    Int16 mode = htons( SHOW_MODE );

    showinfo_t show;

    show.time = htons( stadium().time() );

    show.pmode = static_cast< char >( stadium().playmode() );

    show.pos[0].x = htons( (Int16)rint( ( stadium().ball().pos().x * SHOWINFO_SCALE ) ) );
    show.pos[0].y = htons( (Int16)rint( ( stadium().ball().pos().y * SHOWINFO_SCALE ) ) );

    const Stadium::PlayerCont & players = stadium().players();
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        show.pos[i+1].enable = htons( players[i]->state() );
        show.pos[i+1].x = htons( (Int16)rint( players[i]->pos().x * SHOWINFO_SCALE ) );
        show.pos[i+1].y = htons( (Int16)rint( players[i]->pos().y * SHOWINFO_SCALE ) );
        show.pos[i+1].angle = htons( (Int16)Rad2IDegRound( players[i]->angleBodyCommitted() ) );
        show.pos[i+1].side = htons( players[i]->side() );
        show.pos[i+1].unum = htons( players[i]->unum() );
    }

    if ( ! stadium().teamLeft().name().empty() )
    {
        std::strncpy( show.team[0].name,
                      stadium().teamLeft().name().c_str(),
                      std::min( stadium().teamLeft().name().length() + 1, size_t( 16 ) ) );
        show.team[0].score = htons( stadium().teamLeft().point() );
    }
    else
    {
        std::memset( &show.team[0].name,
                     0,
                     sizeof( show.team[0].name ) );
        show.team[0].score = htons( 0 );
    }

    if ( ! stadium().teamRight().name().empty() )
    {
        std::strncpy( show.team[1].name,
                      stadium().teamRight().name().c_str(),
                      std::min( stadium().teamRight().name().length() + 1, size_t( 16 ) ) );
        show.team[1].score = htons( stadium().teamRight().point() );
    }
    else
    {
        std::memset( &show.team[1].name,
                     0,
                     sizeof( show.team[1].name ) );
        show.team[1].score = htons( 0 );
    }

    // write a show struct
    transport().write( reinterpret_cast< const char * >( &mode ),
                       sizeof( Int16 ) );
    transport().write( reinterpret_cast< const char * >( &show ),
                       sizeof( showinfo_t ) );
    //transport().flush();
}

void
DispSenderLoggerV2::sendMsg( const BoardType board,
                             const char * msg )
{
    Int16 mode = htons( MSG_MODE );
    transport().write( reinterpret_cast< const char * >( &mode ),
                       sizeof( mode ) );

    Int16 board_value = htons( board );
    transport().write( reinterpret_cast< const char * >( &board_value ),
                       sizeof( board_value ) );
    // calculate the string length and write it

    const Int16 max_len = std::min( Int16( std::strlen( msg ) ), Int16( 2048 ) );
    Int16 len = 1;
    while ( (msg[len-1] != '\0') && (len < max_len) )
    {
        ++len;
    }

    /* pfr 1/7/00 Need to put length in network byte order */
    Int16 nlen = htons( len );
    transport().write( reinterpret_cast< const char* >( &nlen ),
                       sizeof( nlen ) );

    // write the message
    transport().write( msg, len );
}


/*!
//===================================================================
//
//  CLASS: DispSenderLoggerV3
//
//  DESC: version 3 log format
//
//===================================================================
*/


DispSenderLoggerV3::DispSenderLoggerV3( const Params & params )
    : DispSenderLoggerV2( params )
{

}

DispSenderLoggerV3::~DispSenderLoggerV3()
{

}

void
DispSenderLoggerV3::sendShow()
{
    Int16 mode = htons( SHOW_MODE );

    short_showinfo_t2 show;

    show.time = htons( stadium().time() );

    show.ball.x = htonl( (Int32)rint( ( stadium().ball().pos().x * SHOWINFO_SCALE2 ) ) );
    show.ball.y = htonl( (Int32)rint( ( stadium().ball().pos().y * SHOWINFO_SCALE2 ) ) );
    show.ball.deltax = htonl( (Int32)rint( ( stadium().ball().vel().x * SHOWINFO_SCALE2 ) ) );
    show.ball.deltay = htonl( (Int32)rint( ( stadium().ball().vel().y * SHOWINFO_SCALE2) ) );

    const Stadium::PlayerCont & players = stadium().players();
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        show.pos[i].mode = htons( players[i]->state() );
        show.pos[i].type = htons( players[i]->playerTypeId() );

        show.pos[i].x = htonl( (Int32)rint( players[i]->pos().x * SHOWINFO_SCALE2 ) );
        show.pos[i].y = htonl( (Int32)rint( players[i]->pos().y * SHOWINFO_SCALE2 ) );
        show.pos[i].deltax = htonl( (Int32)rint( players[i]->vel().x * SHOWINFO_SCALE2 ) );
        show.pos[i].deltay = htonl( (Int32)rint( players[i]->vel().y * SHOWINFO_SCALE2 ) );
        show.pos[i].body_angle = htonl( (Int32)rint( players[i]->angleBodyCommitted() * SHOWINFO_SCALE2 ) );
        show.pos[i].head_angle = htonl( (Int32)rint( players[i]->angleNeckCommitted() * SHOWINFO_SCALE2 ) );
        show.pos[i].view_width = htonl( (Int32)rint( players[i]->visibleAngle() * SHOWINFO_SCALE2 ) );
        show.pos[i].view_quality = htons( (Int16)rint( players[i]->highQuality() ) );

        show.pos[i].stamina = htonl( (Int32)rint( players[i]->stamina() * SHOWINFO_SCALE2 ) );
        show.pos[i].effort = htonl( (Int32)rint( players[i]->effort() * SHOWINFO_SCALE2 ) );
        show.pos[i].recovery = htonl( (Int32)rint( players[i]->recovery() * SHOWINFO_SCALE2 ) );

        show.pos[i].kick_count = htons( players[i]->kickCount() );
        show.pos[i].dash_count = htons( players[i]->dashCount() );
        show.pos[i].turn_count = htons( players[i]->turnCount() );
        show.pos[i].say_count = htons( players[i]->sayCount() );
        show.pos[i].tneck_count = htons( players[i]->turnNeckCount() );
        show.pos[i].catch_count = htons( players[i]->catchCount() );
        show.pos[i].move_count = htons( players[i]->moveCount() );
        show.pos[i].chg_view_count = htons( players[i]->changeViewCount() );
    }

    transport().write( reinterpret_cast< const char * >( &mode ),
                       sizeof( mode ) );
    transport().write( reinterpret_cast< const char * >( &show ),
                       sizeof( short_showinfo_t2 ) );
    //transport().flush();
}

// void
// DispSenderLoggerV3::sendMsg( const BoardType board,
//                              const char * msg )
// {
// }

/*!
//===================================================================
//
//  CLASS: DispSenderLoggerV4
//
//  DESC: version 4 log format
//
//===================================================================
*/


DispSenderLoggerV4::DispSenderLoggerV4( const Params & params )
    : DispSenderLoggerV3( params )
{

}

DispSenderLoggerV4::~DispSenderLoggerV4()
{

}

void
DispSenderLoggerV4::sendShow()
{
    serializer().serializeShowBegin( transport(),
                                     stadium().time() );

    serializer().serializeBall( transport(),
                                stadium().ball() );

    const Stadium::PlayerCont::const_iterator end = stadium().players().end();
    for ( Stadium::PlayerCont::const_iterator p = stadium().players().begin();
          p != end;
          ++p )
    {
        serializer().serializePlayerBegin( transport(), **p );
        serializer().serializePlayerPos( transport(), **p );
        serializer().serializePlayerArm( transport(), **p );
        serializer().serializePlayerViewMode( transport(), **p );
        serializer().serializePlayerStamina( transport(), **p );
        serializer().serializePlayerFocus( transport(), **p );
        serializer().serializePlayerCounts( transport(), **p );
        serializer().serializePlayerEnd( transport() );
    }

    serializer().serializeShowEnd( transport() );

    transport() << '\n';
}

void
DispSenderLoggerV4::sendMsg( const BoardType board,
                             const char * msg )
{
    std::string str( msg );
    std::replace( str.begin(), str.end(), '\n', ' ' );

    transport() << "(msg " << stadium().time()
                << ' ' << board << " \"" << str << "\")\n";
}


// /*!
// //===================================================================
// //
// //  CLASS: DispSenderLoggerV5
// //
// //  DESC: version 5 log format
// //
// //===================================================================
// */

// DispSenderLoggerV5::DispSenderLoggerV5( const Params & params )
//     : DispSenderLoggerV4( params )
// {

// }

// DispSenderLoggerV5::~DispSenderLoggerV5()
// {

// }

// void
// DispSenderLoggerV5::sendShow()
// {
//     DispSenderLoggerV4::sendShow();
// }

// void
// DispSenderLoggerV4::sendMsg( const BoardType board,
//                              const char * msg )
// {

// }


namespace dispsender {

template< typename Sender >
DispSenderMonitor::Ptr
create( const DispSenderMonitor::Params & params )
{
    return DispSenderMonitor::Ptr( new Sender( params ) );
}

RegHolder vm1 = DispSenderMonitor::factory().autoReg( &create< DispSenderMonitorV1 >, 1 );
RegHolder vm2 = DispSenderMonitor::factory().autoReg( &create< DispSenderMonitorV2 >, 2 );
RegHolder vm3 = DispSenderMonitor::factory().autoReg( &create< DispSenderMonitorV3 >, 3 );
RegHolder vm4 = DispSenderMonitor::factory().autoReg( &create< DispSenderMonitorV3 >, 4 );


template< typename Sender >
DispSenderLogger::Ptr
create( const DispSenderLogger::Params & params )
{
    return DispSenderLogger::Ptr( new Sender( params ) );
}

RegHolder vl1 = DispSenderLogger::factory().autoReg( &create< DispSenderLoggerV1 >, 1 );
RegHolder vl2 = DispSenderLogger::factory().autoReg( &create< DispSenderLoggerV2 >, 2 );
RegHolder vl3 = DispSenderLogger::factory().autoReg( &create< DispSenderLoggerV3 >, 3 );
RegHolder vl4 = DispSenderLogger::factory().autoReg( &create< DispSenderLoggerV4 >, 4 );
RegHolder vl5 = DispSenderLogger::factory().autoReg( &create< DispSenderLoggerV4 >, 5 );

}
}

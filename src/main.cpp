// -*-c++-*-

/***************************************************************************
                                  main.cpp
                           Main for rcssserver
                             -------------------
    begin                : 1996
    copyright            : (C) 1996-2000 Electrotechnical Laboratory.
                           Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
                           (C) 2001- by The RoboCup Soccer Server
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

#include "stadium.h"
#include "serverparam.h"
#include "version.h"

#include "stdtimer.h"
#include "synctimer.h"

#include <boost/asio/signal_set.hpp>

#include <memory>
#include <iostream>
#include <locale>
#include <cmath>
#include <csignal>

namespace {

Stadium Std;

}

int
main( int argc, char *argv[] )
{
    std::locale::global( std::locale::classic() );

    std::cout << PACKAGE << "-" << VERSION << "\n\n"
              << Copyright << std::endl;

    if ( ! ServerParam::init( argc, argv ) )
    {
        return 1;
    }

    if ( ! Std.init() )
    {
        ServerParam::instance().clear();
        return 1;
    }

    // The signals are handled in the event loop of the server, because
    // Stadium::finalize() cannot be called in a signal handler safely.
    // The event loop exists after Std.init().
    boost::asio::signal_set signals( Std.ioContext() );
    boost::system::error_code error;
    signals.add( SIGINT, error );
    if ( ! error )
    {
        signals.add( SIGTERM, error );
    }
    if ( ! error )
    {
        signals.add( SIGHUP, error );
    }
    if ( error )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": could not set signal handler: "
                  << error.message() << std::endl;
        ServerParam::instance().clear();
        return 1;
    }
    signals.async_wait( []( const boost::system::error_code & error,
                            int )
                        {
                            if ( ! error )
                            {
                                Std.finalize( "Server Killed. Exiting..." );
                            }
                        } );

    std::shared_ptr< Timer > timer;
    if ( ServerParam::instance().synchMode() )
    {
        timer = std::shared_ptr< Timer >( new SyncTimer( Std ) );
    }
    else
    {
        timer = std::shared_ptr< Timer >( new StandardTimer( Std ) );
    }

    std::cout << "\nHit CTRL-C to exit\n";

    timer->run();

    ServerParam::instance().clear();

    return 0;
}

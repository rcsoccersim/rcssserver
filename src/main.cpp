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

#include <boost/shared_ptr.hpp>

#include <iostream>
#include <locale>
#include <cmath>
#include <cstring>
#include <csignal>
#include <cerrno>

namespace {

Stadium Std;

void
sigHandle( int )
{
    Std.finalize( "Server Killed. Exiting..." );
}

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

    struct sigaction sig_action;
    sig_action.sa_handler = &sigHandle;
    sig_action.sa_flags = 0;
    if ( sigaction( SIGINT, &sig_action , NULL ) != 0
         || sigaction( SIGTERM, &sig_action , NULL ) != 0
         || sigaction( SIGHUP, &sig_action , NULL ) != 0 )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": could not set signal handler: "
                  << strerror( errno ) << std::endl;
        ServerParam::instance().clear();
        return 1;
    }

    if ( ! Std.init() )
    {
        ServerParam::instance().clear();
        return 1;
    }

    boost::shared_ptr< Timer > timer;
    if ( ServerParam::instance().synchMode() )
    {
        timer = boost::shared_ptr< Timer >( new SyncTimer( Std ) );
    }
    else
    {
        timer = boost::shared_ptr< Timer >( new StandardTimer( Std ) );
    }

    std::cout << "\nHit CTRL-C to exit\n";

    timer->run();

    ServerParam::instance().clear();

    return 0;
}

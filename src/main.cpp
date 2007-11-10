// -*-c++-*-

/***************************************************************************
                                  main.cc
                           Main for rcssserver
                             -------------------
    begin                : 1996
    copyright            : (C) 1996-2000 Electrotechnical Laboratory.
                           Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
                           (C) 2001-2002 by The RoboCup Soccer Server
                           Maintenance Group.
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

#include <iostream>
#include <cmath>

#include "param.h"
#include "types.h"
#include "utility.h"
#include "object.h"
#include "player.h"
#include "field.h"
#include "version.h"

#include "timer.h"
#include "stdtimer.h"
#include "synctimer.h"
//#include <rcssbase/lib/loader.hpp>
#include <rcssbase/version.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
//#include <dirent.h>

#include <csignal>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace {

Stadium Std;

void
sigHandle ( int )
{
    Std.finalize( "Server Killed. Exiting..." );
}

}

int
main( int argc, char *argv[] )
{
    std::cout << PACKAGE << "-" << VERSION << std::endl << std::endl
              << Copyright << std::endl;
    std::cout << "Using rcssbase-" << rcss::base::version() << std::endl << std::endl;

    {
#if defined(_WIN32) || defined(__WIN32__) || defined (WIN32) || defined (__CYGWIN__)
        std::string config_dir_name = tildeExpand( "~\\.rcssserver" );
#else
        std::string config_dir_name = tildeExpand( "~/.rcssserver" );
#endif
        boost::filesystem::path config_dir( config_dir_name,
                                            boost::filesystem::portable_posix_name );
        if ( ! boost::filesystem::exists( config_dir )
             && ! boost::filesystem::create_directory( config_dir ) )
        {
            std::cerr << "Could not read or create config directory '" << config_dir_name
                      << std::endl;
        }
//         DIR* config_dir = opendir( config_dir_name.c_str() );
//         if ( config_dir == NULL )
//         {
//             int err = mkdir( config_dir_name.c_str(), 0777 );
//             if ( err != 0 )
//             {
//                 std::cerr << "Could not read or create config directory '" << config_dir_name << "': "
//                           << strerror( errno ) << std::endl;
//             }
//         }
    }

    ServerParam::init( argc, argv );

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

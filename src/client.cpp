// -*-c++-*-

/***************************************************************************
                          client.cc  -  A basic client that connects to
                          the server
                             -------------------
    begin                : 27-DEC-2001
    copyright            : (C) 2001 by The RoboCup Soccer Server
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

#include "client.h"
#include <cstdlib>

int
main ( int argc, char **argv )
{
    const char *server = "localhost";
    int port = 6000;
    bool use_ncurses = true;

    for( int i = 0; i < argc; ++i )
    {
        if( std::strcmp( argv[ i ], "-server" ) == 0 )
        {
            if( i + 1 < argc )
            {
                server = argv[ i + 1 ];
                ++i;
            }
        }
        else if( std::strcmp( argv[ i ], "-port" ) == 0 )
        {
            if( i + 1 < argc )
            {
                port = atoi( argv[ i + 1 ] );
                ++i;
            }
        }
        else if( std::strcmp( argv[ i ], "-nogui" ) == 0 )
            use_ncurses = false;
    }


    rcss::net::Addr dest( port );
    if( dest.setHost( server ) )
    {
        Client* client;
#ifdef HAVE_LIBNCURSES
        if( use_ncurses )
            client = new NCursesClient( dest );
        else
#endif
            client = new NoGuiClient( dest );


        client->start();
        delete client;

        return EXIT_SUCCESS;
    }
    else
    {
        std::cerr << "Could not resolve host '" << server << "'\n";
        return EXIT_FAILURE;
    }
}

// -*-c++-*-

/***************************************************************************
                          client.cpp  -  A basic client that connects to
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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "compress.h"

#include <rcssbase/net/socketstreambuf.hpp>
#include <rcssbase/net/udpsocket.hpp>
#include <rcssbase/gzip/gzstream.hpp>

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif
#include <iostream>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef __CYGWIN__
// cygwin is not win32
#elif defined(_WIN32) || defined(__WIN32__) || defined (WIN32)
#  define RCSS_WIN
#  include <winsock2.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h> // select()
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h> // select()
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h> // select()
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h> // select()
#endif


class Client {
private:
    rcss::net::Addr M_dest;
    rcss::net::UDPSocket M_socket;
    rcss::net::SocketStreamBuf * M_socket_buf;
    rcss::gz::gzstreambuf * M_gz_buf;
    std::ostream * M_transport;
    int M_comp_level;
    bool M_clean_cycle;

#ifdef HAVE_LIBZ
    Decompressor M_decomp;
#endif

    Client();
    Client( const Client & );
    Client & operator=( const Client & );
public:
    Client( const std::string & server,
            const int port )
        : M_dest( port ),
          M_socket(),
          M_socket_buf( NULL ),
          M_gz_buf( NULL ),
          M_transport( NULL ),
          M_comp_level( -1 ),
          M_clean_cycle( true )
      {
          M_dest.setHost( server );
          open();
          bind();

          M_socket_buf->setEndPoint( M_dest );
      }

    virtual
    ~Client()
      {
          close();
      }

    void run()
      {
          messageLoop();
      }

private:

    int open()
      {
          if ( M_socket.open() )
          {
              if ( M_socket.setNonBlocking() < 0 )
              {
                  std::cerr << __FILE__ << ": " << __LINE__
                      << ": Error setting socket non-blocking: "
                            << strerror( errno ) << std::endl;
                  M_socket.close();
                  return -1;
              }
          }
          else
          {
              std::cerr << __FILE__ << ": " << __LINE__
                        << ": Error opening socket: "
                        << strerror( errno ) << std::endl;
              M_socket.close();
              return -1;
          }

          M_socket_buf = new rcss::net::SocketStreamBuf( M_socket );
          M_transport = new std::ostream( M_socket_buf );
          return 0;
      }

    bool bind()
      {
          if ( ! M_socket.bind( rcss::net::Addr() ) )
          {
              std::cerr << __FILE__ << ": " << __LINE__
                        << ": Error connecting socket" << std::endl;
              M_socket.close();
              return false;
          }
          return true;
      }

    void close()
      {
          M_socket.close();

          if ( M_transport )
          {
              delete M_transport;
              M_transport = NULL;
          }

          if ( M_gz_buf )
          {
              delete M_gz_buf;
              M_gz_buf = NULL;
          }

          if ( M_socket_buf )
          {
              delete M_socket_buf;
              M_socket_buf = NULL;
          }
      }

    int setCompression( int level )
      {
#ifdef HAVE_LIBZ
          if ( level >= 0 )
          {
              if ( ! M_gz_buf )
              {
                  M_gz_buf = new rcss::gz::gzstreambuf( *M_socket_buf );
              }
              M_gz_buf->setLevel( level );
              M_transport->rdbuf( M_gz_buf );
          }
          else
          {
              M_transport->rdbuf( M_socket_buf );
          }
          return M_comp_level = level;
#endif
          return M_comp_level = -1;
      }

    void processMsg( const char * msg,
                     const size_t len )
      {
#ifdef HAVE_LIBZ
          if ( M_comp_level >= 0 )
          {
              M_decomp.decompress( msg, len, Z_SYNC_FLUSH );
              char * out;
              int size;
              M_decomp.getOutput( out, size );
              if ( size > 0 )
              {
                  parseMsg( out, size );
              }
          }
          else
#endif
          {
              parseMsg( msg, len );
          }
      }

    void parseMsg( const char * msg,
                   const size_t len )
      {
          if ( ! std::strncmp( msg, "(ok compression", 15 ) )
          {
              int level;
              if ( std::sscanf( msg, " ( ok compression %d )", &level ) == 1 )
              {
                  setCompression( level );
              }
          }
          else if ( ! std::strncmp( msg, "(sense_body", 11 )
                    || ! std::strncmp( msg, "(see_global", 11 )
                    || ! std::strncmp( msg, "(init", 5 ) )
          {
              M_clean_cycle = true;
          }

          std::cout << std::string( msg, len - 1 ) << std::endl;
      }

    void messageLoop()
      {
          fd_set read_fds;
          fd_set read_fds_back;
          char buf[8192];
          std::memset( &buf, 0, sizeof( char ) * 8192 );

          int in = fileno( stdin );
          FD_ZERO( &read_fds );
          FD_SET( in, &read_fds );
          FD_SET( M_socket.getFD(), &read_fds );
          read_fds_back = read_fds;

#ifdef RCSS_WIN
          int max_fd = 0;
#else
          int max_fd = M_socket.getFD() + 1;
#endif
          while ( 1 )
          {

              read_fds = read_fds_back;
              int ret = ::select( max_fd, &read_fds, NULL, NULL, NULL );
              if ( ret < 0 )
              {
                  perror( "Error selecting input" );
                  break;
              }
              else if ( ret != 0 )
              {
                  // read from stdin
                  if ( FD_ISSET( in, &read_fds ) )
                  {
                      if ( std::fgets( buf, sizeof( buf ), stdin ) != NULL )
                      {
                          size_t len = std::strlen( buf );
                          if ( buf[len-1] == '\n' )
                          {
                              buf[len-1] = '\0';
                              --len;
                          }

                          M_transport->write( buf, len + 1 );
                          M_transport->flush();
                          if ( ! M_transport->good() )
                          {
                              if ( errno != ECONNREFUSED )
                              {
                                  std::cerr << __FILE__ << ": " << __LINE__
                                            << ": Error sending to socket: "
                                            << strerror( errno ) << std::endl
                                            << "msg = [" << buf << "]\n";
                              }
                              M_socket.close();
                          }
                          std::cout << buf << std::endl;
                      }
                  }

                  // read from socket
                  if ( FD_ISSET( M_socket.getFD(), &read_fds ) )
                  {
                      rcss::net::Addr from;
                      int len = M_socket.recv( buf, sizeof( buf ) - 1, from );
                      if ( len == -1
                           && errno != EWOULDBLOCK )
                      {
                          if ( errno != ECONNREFUSED )
                          {
                              std::cerr << __FILE__ << ": " << __LINE__
                                        << ": Error receiving from socket: "
                                        << strerror( errno ) << std::endl;
                          }
                          M_socket.close();
                      }
                      else if ( len > 0 )
                      {
                          M_dest.setPort( from.getPort() );
                          M_socket_buf->setEndPoint( M_dest );
                          processMsg( buf, len );
                      }
                  }
              }
          }
      }
};


namespace {
Client * client = static_cast< Client * >( 0 );

void
sig_exit_handle( int )
{
    std::cerr << "\nKilled. Exiting..." << std::endl;
    if ( client )
    {
        delete client;
        client =  static_cast< Client * >( 0 );
    }
    std::exit( EXIT_FAILURE );
}
}


int
main ( int argc, char **argv )
{
    if ( std::signal( SIGINT, &sig_exit_handle) == SIG_ERR
         || std::signal( SIGTERM, &sig_exit_handle ) == SIG_ERR
         || std::signal( SIGHUP, &sig_exit_handle ) == SIG_ERR )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": could not set signal handler: "
                  << std::strerror( errno ) << std::endl;
        std::exit( EXIT_FAILURE );
    }

    std::cerr << "Hit Ctrl-C to exit." << std::endl;

    std::string server = "localhost";
    int port = 6000;

    for ( int i = 0; i < argc; ++i )
    {
        if ( std::strcmp( argv[ i ], "-server" ) == 0 )
        {
            if ( i + 1 < argc )
            {
                server = argv[ i + 1 ];
                ++i;
            }
        }
        else if ( std::strcmp( argv[ i ], "-port" ) == 0 )
        {
            if ( i + 1 < argc )
            {
                port = std::atoi( argv[ i + 1 ] );
                ++i;
            }
        }
    }

    client = new Client( server, port );
    client->run();

    return EXIT_SUCCESS;
}

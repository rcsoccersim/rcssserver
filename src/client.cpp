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
#include "transport/datagrambuf.h"

#include <rcss/gzip/gzstream.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <sstream>
#include <iostream>
#include <string>
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
    boost::asio::io_context M_io_context;
    boost::asio::ip::udp::endpoint M_dest;
    boost::asio::ip::udp::socket M_socket;
    rcss::DatagramBuf * M_socket_buf;
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
        : M_io_context(),
          M_dest(),
          M_socket( M_io_context ),
          M_socket_buf( nullptr ),
          M_gz_buf( nullptr ),
          M_transport( nullptr ),
          M_comp_level( -1 ),
          M_clean_cycle( true )
      {
          if ( resolve( server, port )
               && open() == 0 )
          {
              bind();
          }
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

    bool resolve( const std::string & server,
                  const int port )
      {
          // IPv4 only, like the UDP ports of the server
          boost::system::error_code error;
          boost::asio::ip::udp::resolver resolver( M_io_context );
          const boost::asio::ip::udp::resolver::results_type results
              = resolver.resolve( boost::asio::ip::udp::v4(),
                                  server,
                                  std::to_string( port ),
                                  boost::asio::ip::udp::resolver::numeric_service,
                                  error );
          if ( error
               || results.empty() )
          {
              std::cerr << __FILE__ << ": " << __LINE__
                        << ": Error resolving " << server << ": "
                        << error.message() << std::endl;
              return false;
          }

          M_dest = results.begin()->endpoint();
          return true;
      }

    int open()
      {
          boost::system::error_code error;
          M_socket.open( boost::asio::ip::udp::v4(), error );
          if ( ! error )
          {
              M_socket.non_blocking( true, error );
              if ( error )
              {
                  std::cerr << __FILE__ << ": " << __LINE__
                      << ": Error setting socket non-blocking: "
                            << error.message() << std::endl;
                  M_socket.close( error );
                  return -1;
              }
          }
          else
          {
              std::cerr << __FILE__ << ": " << __LINE__
                        << ": Error opening socket: "
                        << error.message() << std::endl;
              return -1;
          }

          // The socket is not connected, because the server replies from
          // another port (the dedicated port) than the one the client
          // sends its first message to.
          M_socket_buf = new rcss::DatagramBuf( M_socket, M_dest );
          M_transport = new std::ostream( M_socket_buf );
          return 0;
      }

    bool bind()
      {
          boost::system::error_code error;
          M_socket.bind( boost::asio::ip::udp::endpoint( boost::asio::ip::udp::v4(), 0 ), error );
          if ( error )
          {
              std::cerr << __FILE__ << ": " << __LINE__
                        << ": Error binding socket: "
                        << error.message() << std::endl;
              M_socket.close( error );
              return false;
          }
          return true;
      }

    void close()
      {
          boost::system::error_code ignored;
          M_socket.close( ignored );

          if ( M_transport )
          {
              delete M_transport;
              M_transport = nullptr;
          }

          if ( M_gz_buf )
          {
              delete M_gz_buf;
              M_gz_buf = nullptr;
          }

          if ( M_socket_buf )
          {
              delete M_socket_buf;
              M_socket_buf = nullptr;
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
          if ( ! M_socket.is_open() )
          {
              return;
          }

          fd_set read_fds;
          fd_set read_fds_back;
          char buf[8192];
          std::memset( &buf, 0, sizeof( char ) * 8192 );

          int in = fileno( stdin );
          const int socket_fd = M_socket.native_handle();
          FD_ZERO( &read_fds );
          FD_SET( in, &read_fds );
          FD_SET( socket_fd, &read_fds );
          read_fds_back = read_fds;

#ifdef RCSS_WIN
          int max_fd = 0;
#else
          int max_fd = socket_fd + 1;
#endif
          // The loop ends when the socket is closed because of an error.
          while ( M_socket.is_open() )
          {

              read_fds = read_fds_back;
              int ret = ::select( max_fd, &read_fds, nullptr, nullptr, nullptr );
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
                      if ( std::fgets( buf, sizeof( buf ), stdin ) )
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
                              boost::system::error_code ignored;
                              M_socket.close( ignored );
                          }
                          std::cout << buf << std::endl;
                      }
                  }

                  // read from socket
                  if ( M_socket.is_open()
                       && FD_ISSET( socket_fd, &read_fds ) )
                  {
                      boost::asio::ip::udp::endpoint from;
                      std::size_t len = 0;
                      boost::system::error_code error;
                      do
                      {
                          len = M_socket.receive_from( boost::asio::buffer( buf, sizeof( buf ) - 1 ), from, 0, error );
                      }
                      while ( error == boost::asio::error::interrupted );

                      if ( error )
                      {
                          if ( error != boost::asio::error::would_block
                               && error != boost::asio::error::try_again )
                          {
                              if ( error != boost::asio::error::connection_refused )
                              {
                                  std::cerr << __FILE__ << ": " << __LINE__
                                            << ": Error receiving from socket: "
                                            << error.message() << std::endl;
                              }
                              M_socket.close( error );
                          }
                      }
                      else if ( len > 0 )
                      {
                          // the server replies from the dedicated port
                          M_dest.port( from.port() );
                          M_socket_buf->setDest( M_dest );
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

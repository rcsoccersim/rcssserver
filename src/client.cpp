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
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/streambuf.hpp>

#include <array>
#include <iostream>
#include <istream>
#include <memory>
#include <string>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>

namespace {

/*!
  \brief open the standard input to read it with asio.

  asio sets O_NONBLOCK on the descriptor that it reads. The flag belongs to
  the open file, which a terminal usually shares with the standard output
  and with the shell, so a terminal is opened again. The other kinds of
  input are duplicated.
  \return the new descriptor, or -1 on failure
 */
int
open_input()
{
    if ( ::isatty( STDIN_FILENO ) )
    {
        const char * name = ::ttyname( STDIN_FILENO );
        if ( name )
        {
            // never become the controlling terminal
            const int fd = ::open( name, O_RDONLY | O_NOCTTY | O_CLOEXEC );
            if ( fd != -1 )
            {
                return fd;
            }
        }
    }

    return ::dup( STDIN_FILENO );
}

}


/*!
  \class Client
  \brief sends the lines of the standard input to the server, and prints
  the messages from the server.

  The client sends its first message to the port of the server. The server
  replies from the dedicated port of the client, to which the client sends
  its following messages.
 */
class Client {
private:
    //! the longest datagram that the server sends
    static constexpr std::size_t MAX_DATAGRAM = 8192;

    boost::asio::io_context & M_io_context;

    // the transport over UDP
    boost::asio::ip::udp::socket M_socket;
    boost::asio::ip::udp::endpoint M_dest; //!< the server. The port changes to the dedicated port.
    boost::asio::ip::udp::endpoint M_sender; //!< the sender of the received datagram
    std::array< char, MAX_DATAGRAM > M_recv_buf;
    rcss::DatagramBuf M_socket_buf; //!< must be declared after M_socket and M_dest

    // the messages to the server, which may be compressed
    std::unique_ptr< rcss::gz::gzstreambuf > M_gz_buf; //!< must be declared after M_socket_buf
    std::ostream M_transport;
    int M_comp_level;
#ifdef HAVE_LIBZ
    Decompressor M_decomp;
#endif

    // the standard input
    boost::asio::posix::stream_descriptor M_input;
    boost::asio::streambuf M_input_buf;

    bool M_failed;

public:
    Client( boost::asio::io_context & io_context,
            const boost::asio::ip::udp::endpoint & dest )
        : M_io_context( io_context ),
          M_socket( io_context ),
          M_dest( dest ),
          M_sender(),
          M_recv_buf(),
          // The socket is not connected, because the server replies from
          // another port than the one the client sends its first message to.
          M_socket_buf( M_socket, dest ),
          M_gz_buf(),
          M_transport( &M_socket_buf ),
          M_comp_level( -1 ),
          M_input( io_context ),
          M_input_buf(),
          M_failed( false )
      { }

    Client( const Client & ) = delete;
    Client & operator=( const Client & ) = delete;

    /*!
      \brief open the socket and the standard input, and start reading them.
      \return true if successful
     */
    bool start()
      {
          boost::system::error_code error;
          M_socket.open( boost::asio::ip::udp::v4(), error );
          if ( ! error )
          {
              M_socket.bind( boost::asio::ip::udp::endpoint( boost::asio::ip::udp::v4(), 0 ), error );
          }
          if ( error )
          {
              std::cerr << __FILE__ << ": " << __LINE__
                        << ": Error opening socket: "
                        << error.message() << std::endl;
              return false;
          }

          const int fd = open_input();
          if ( fd == -1 )
          {
              std::cerr << __FILE__ << ": " << __LINE__
                        << ": Error opening the standard input: "
                        << std::strerror( errno ) << std::endl;
              return false;
          }
          M_input.assign( fd, error );
          if ( error )
          {
              ::close( fd );
              std::cerr << __FILE__ << ": " << __LINE__
                        << ": Error opening the standard input: "
                        << error.message() << std::endl;
              return false;
          }

          receive();
          readInput();
          return true;
      }

    //! true if the client has stopped because of an error
    bool failed() const
      {
          return M_failed;
      }

private:

    //! stop the client because of an error
    void fail()
      {
          M_failed = true;
          M_io_context.stop();
      }

    void readInput()
      {
          boost::asio::async_read_until( M_input, M_input_buf, '\n',
                                         [this]( const boost::system::error_code & error,
                                                 std::size_t )
                                         {
                                             handleInput( error );
                                         } );
      }

    void handleInput( const boost::system::error_code & error )
      {
          if ( error
               && error != boost::asio::error::eof )
          {
              if ( error != boost::asio::error::operation_aborted )
              {
                  std::cerr << __FILE__ << ": " << __LINE__
                            << ": Error reading the standard input: "
                            << error.message() << std::endl;
              }
              return;
          }

          // At the end of the input, the last line may have no newline.
          std::istream input( &M_input_buf );
          std::string line;
          if ( std::getline( input, line ) )
          {
              send( line );
              std::cout << line << std::endl;
          }

          if ( error == boost::asio::error::eof )
          {
              // Keep printing the messages from the server until Ctrl-C.
              return;
          }

          readInput();
      }

    void send( const std::string & msg )
      {
          // a message ends with a null character
          M_transport.write( msg.c_str(), msg.size() + 1 );
          M_transport.flush();
          if ( ! M_transport.good() )
          {
              if ( errno != ECONNREFUSED )
              {
                  std::cerr << __FILE__ << ": " << __LINE__
                            << ": Error sending to socket: "
                            << std::strerror( errno ) << std::endl
                            << "msg = [" << msg << "]\n";
              }
              fail();
          }
      }

    void receive()
      {
          M_socket.async_receive_from( boost::asio::buffer( M_recv_buf ),
                                       M_sender,
                                       [this]( const boost::system::error_code & error,
                                               std::size_t bytes )
                                       {
                                           handleReceive( error, bytes );
                                       } );
      }

    void handleReceive( const boost::system::error_code & error,
                        const std::size_t bytes )
      {
          if ( error == boost::asio::error::operation_aborted )
          {
              return;
          }

          if ( error )
          {
              // ECONNREFUSED means the server has gone away,
              // which is not worth reporting.
              if ( error != boost::asio::error::connection_refused )
              {
                  std::cerr << __FILE__ << ": " << __LINE__
                            << ": Error receiving from socket: "
                            << error.message() << std::endl;
              }
              fail();
              return;
          }

          if ( bytes > 0 )
          {
              // the server replies from the dedicated port
              M_dest.port( M_sender.port() );
              M_socket_buf.setDest( M_dest );
              processMsg( M_recv_buf.data(), bytes );
          }

          receive();
      }

    void setCompression( const int level )
      {
#ifdef HAVE_LIBZ
          if ( level >= 0 )
          {
              if ( ! M_gz_buf )
              {
                  M_gz_buf.reset( new rcss::gz::gzstreambuf( M_socket_buf ) );
              }
              M_gz_buf->setLevel( level );
              M_transport.rdbuf( M_gz_buf.get() );
          }
          else
          {
              M_transport.rdbuf( &M_socket_buf );
          }
          M_comp_level = level;
#else
          (void)level;
          M_comp_level = -1;
#endif
      }

    void processMsg( const char * msg,
                     const std::size_t len )
      {
#ifdef HAVE_LIBZ
          if ( M_comp_level >= 0 )
          {
              M_decomp.decompress( msg, static_cast< int >( len ), Z_SYNC_FLUSH );
              char * out;
              int size;
              M_decomp.getOutput( out, size );
              if ( size > 0 )
              {
                  parseMsg( out, static_cast< std::size_t >( size ) );
              }
          }
          else
#endif
          {
              parseMsg( msg, len );
          }
      }

    void parseMsg( const char * msg,
                   const std::size_t len )
      {
          // a message ends with a null character
          std::string message( msg, len );
          if ( ! message.empty()
               && message.back() == '\0' )
          {
              message.pop_back();
          }

          if ( ! message.compare( 0, 15, "(ok compression" ) )
          {
              int level;
              if ( std::sscanf( message.c_str(), " ( ok compression %d )", &level ) == 1 )
              {
                  setCompression( level );
              }
          }

          std::cout << message << std::endl;
      }
};


int
main( int argc, char **argv )
{
    std::string server = "localhost";
    int port = 6000;

    for ( int i = 1; i < argc; ++i )
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

    boost::asio::io_context io_context;

    boost::system::error_code error;
    boost::asio::signal_set signals( io_context );
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
        return EXIT_FAILURE;
    }

    bool killed = false;
    signals.async_wait( [&killed, &io_context]( const boost::system::error_code & error,
                                                int )
                        {
                            if ( ! error )
                            {
                                std::cerr << "\nKilled. Exiting..." << std::endl;
                                killed = true;
                                io_context.stop();
                            }
                        } );

    std::cerr << "Hit Ctrl-C to exit." << std::endl;

    // IPv4 only, like the UDP ports of the server
    boost::asio::ip::udp::resolver resolver( io_context );
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
        return EXIT_FAILURE;
    }

    Client client( io_context, results.begin()->endpoint() );
    if ( ! client.start() )
    {
        return EXIT_FAILURE;
    }

    io_context.run();

    return ( killed || client.failed()
             ? EXIT_FAILURE
             : EXIT_SUCCESS );
}

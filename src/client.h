// -*-c++-*-

/***************************************************************************
                          client.h  -  A basic client that connects to
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

#ifndef TESTCLIENT_H
#define TESTCLIENT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rcssbase/net/udpsocket.hpp>

#include <cstdio>
#include <netdb.h>

#include <rcssbase/net/socketstreambuf.hpp>
#include <rcssbase/thread.h>
#include "param.h"
#include "compress.h"
#include <cerrno>
#include <fstream>
#include <iostream>
#include "utility.h"
//  #include "test_8x8.xpm"
#include <rcssbase/messagequeue.h>
#include <rcssbase/sharedvar.h>

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif


#include <rcssbase/gzip/gzstream.hpp>

#ifdef HAVE_LIBNCURSES
#include <ncurses.h>
#endif

class Client
{
private:
    //friend
    class ServerReader
    {
    private:
        Client& M_client;
        std::istream& M_src;
    public:
        ServerReader( Client& client,
                      std::istream& src
                      )
            : M_client( client ),
              M_src( src )
          {}

        ~ServerReader()
          {}

        void
        operator()()
          {
              while( M_src.good() )
              {
                  char buffer[ 8192 ];
                  M_src.getline( buffer, 8192, 0 );
                  M_client.processInput( buffer );
              }
              M_client.displayError( "Error receiving from server" );
          }
    };

    //friend
    class ServerWriter
    {
    private:
        Client& M_client;
        std::ostream& M_dest;
    public:
        ServerWriter( Client& client,
                      std::ostream& dest )
            : M_client( client ),
              M_dest( dest )
          {}

        ~ServerWriter()
          {}

        void
        operator()()
          {
              while( M_dest.good() )
              {
                  char buffer[ 8192 ];
                  M_client.getData( buffer, 8192 );
                  M_dest << buffer << std::ends << std::flush;
              }
              M_client.displayError( "Error sending to server" );
          }
    };

    rcss::net::UDPSocket M_socket;
    rcss::net::SocketStreamBuf M_write_strmbuf;
    rcss::net::SocketStreamBuf M_read_strmbuf;
    rcss::gz::gzostream M_write_strm;
    rcss::gz::gzistream M_read_strm;
    rcss::thread::SharedVar< bool > M_cycle_is_clean;
    rcss::thread::MessageQueue< int > M_level;

    ServerWriter M_server_writer;
    rcss::thread::Thread M_server_writer_thread;
    ServerReader M_server_reader;
    rcss::thread::Thread M_server_reader_thread;

    void
    setCompression( int level )
      {
          M_write_strm.setLevel( level );
          M_read_strm.setLevel( level );
          M_level.queueMessage( level );
      }

    void
    waitForCompressionChange( long seconds, long nanoseconds = 0 )
      {
          try { M_level.getNext( seconds, nanoseconds ); }
          catch(...) {}
      }

    void
    processInput( const char* buffer )
      {
          doProcessInput( buffer );
          { // limit the scope of the lock
              rcss::thread::SharedVar< bool >::Lock lock( M_cycle_is_clean );
              if( !M_cycle_is_clean.get( lock )
                  && ( ! std::strncmp( buffer, "(sense_body", 11 )
                       || ! std::strncmp( buffer, "(see_global", 11 )
                       || ! std::strncmp( buffer, "(init", 5 ) ) )
              {
                  M_cycle_is_clean = true;
                  M_cycle_is_clean.signal();
              }
          }
          if ( ! std::strncmp( buffer, "(ok compression", 15 ) )
          {
              int level;
              if ( std::sscanf( buffer, "(ok compression %d)", &level ) == 1 )
                  setCompression( level );
          }
          else if ( ! std::strncmp( buffer, "(warning compression_unsupported)", 34 ) )
              setCompression( -1 );
      }

    virtual
    void
    doProcessInput( const char* buffer ) = 0;

    void
    getData( char* buffer, int size )
      {
          static int compression_wait = false;

          doGetData( buffer, size );
          if ( compression_wait )
          {
              waitForCompressionChange( 5 );
              compression_wait = false;
          }
          {
              rcss::thread::SharedVar< bool >::Lock lock( M_cycle_is_clean );
              displayError( "Got lock\n" );
              while( !M_cycle_is_clean.get( lock ) )
                  M_cycle_is_clean.wait( lock );
              M_cycle_is_clean = false;
          }
          if ( strncmp( "(compression", buffer, 12 ) == 0 )
              compression_wait = true;
      }

    virtual
    void
    doGetData( char* buffer, int size ) = 0;

    void
    displayError( const char* msg )
      { doDisplayError( msg ); }

    virtual
    void
    doDisplayError( const char* msg ) = 0;

public:
    Client( const rcss::net::Addr dest )
        : M_socket(),
          M_write_strmbuf( M_socket, dest ),
          M_read_strmbuf( M_socket, dest ),
          M_write_strm( M_write_strmbuf ),
          M_read_strm( M_read_strmbuf ),
          M_cycle_is_clean( true ),
          M_server_writer( *this, M_write_strm ),
          M_server_writer_thread( M_server_writer ),
          M_server_reader( *this, M_read_strm ),
          M_server_reader_thread( M_server_reader )
      {
          M_write_strm.setLevel( -1 );
          M_read_strm.setLevel( -1 );
      }

    virtual ~Client()
      {}

    void
    start()
      {
          M_server_writer_thread.start();
          M_server_reader_thread.start();
          M_server_writer_thread.join();
          M_server_reader_thread.cancel();
          M_server_reader_thread.join();
      }
};


class NoGuiClient
    : public Client
{
private:
    virtual
    void
    doProcessInput( const char* buffer )
      { std::cout << buffer << std::endl; }

    virtual
    void
    doGetData( char* buffer, int size )
      { std::cin.getline( buffer, size ); }

    virtual
    void
    doDisplayError( const char* msg )
      { std::cerr << msg << std::endl; }

public:
    NoGuiClient( const rcss::net::Addr& dest )
        : Client( dest )
      {
          std::cout << "Sending to " << dest << std::endl;
      }

    virtual
    ~NoGuiClient()
      {}
};



#ifdef HAVE_LIBNCURSES
class NCursesClient
    : public Client
{
private:
    int M_height;
    int M_width;
    WINDOW* M_output_win_border;
    WINDOW* M_output_win;
    WINDOW* M_visual_win;
    WINDOW* M_sb_win;
    WINDOW* M_other_win;
    WINDOW* M_input_win;
    WINDOW* M_input_header;

    virtual
    void
    doProcessInput( const char* buffer )
      {
          int size = std::strlen( buffer );
          if ( size >= 4 && strncmp( buffer, "(see", 4 ) == 0 )
          {
              wclear( M_visual_win );
              mvwaddstr( M_visual_win, 0, 0, buffer );
              wrefresh( M_visual_win );
          }
          else if ( size >= 6 && strncmp( buffer, "(sense", 6 ) == 0 )
          {
              wclear( M_sb_win );
              mvwaddstr( M_sb_win, 0, 0, buffer );
              wrefresh( M_sb_win );
          }
          else
          {
              wclear( M_other_win );
              mvwaddstr( M_other_win, 0, 0, buffer );
              wrefresh( M_other_win );
          }
      }

    virtual
    void
    doGetData( char* buffer, int size )
      {
          int idx = 0;
          char curr;
          do
          {
              curr = getch();
              if( curr == 74 || curr == 7 )
              {
                  if( idx > 0 )
                  {
                      --idx;
                      mvwaddch( M_input_win, 1, idx + 3, ' ' );
                      mvwaddch( M_input_win, 1, idx + 2, '_' | A_BLINK | A_BOLD );
                      wrefresh( M_input_win );
                  }
              }
              else if( curr == 0 || curr == '\n' || curr == EOF )
              {
                  curr = 0;
                  buffer[ idx ] = curr;
                  ++idx;
                  wclear( M_input_win );
                  mvwaddstr( M_input_win, 0, 0, "> " );
                  waddstr( M_input_win, buffer );
                  mvwaddstr( M_input_win, 1, 0, "> " );
                  waddch( M_input_win, '_' | A_BLINK | A_BOLD );
                  wrefresh( M_input_win );
              }
              else
              {
                  mvwaddch( M_input_win, 1, idx + 2, curr | A_BOLD );
                  mvwaddch( M_input_win, 1, idx + 3, '_' | A_BLINK | A_BOLD );
                  wrefresh( M_input_win );
                  buffer[ idx ] = curr;
                  ++idx;
              }
          }
          while( idx < size - 1 && curr != 0 );
          buffer[ idx ] = 0;
      }

    virtual
    void
    doDisplayError( const char* msg )
      {
          wclear( M_other_win );
          mvwaddstr( M_other_win, 0, 0, msg );
          wrefresh( M_other_win );
      }

public:
    NCursesClient( const rcss::net::Addr& dest )
        : Client( dest )
      {
          initscr();
          cbreak();
          noecho();
          curs_set( 0 );
          refresh();
          int height, width;
          getmaxyx( stdscr, height, width );
          keypad( stdscr, true );
          WINDOW* header = newwin( 1, width, 0, 0 );
          wattron( header, A_REVERSE );
          wbkgdset( header, A_REVERSE );
          wclear( header );
          mvwprintw( header, 0, 2, "RCSSClient-%s",
                     VERSION );

#ifdef HAVE_SSTREAM
          std::ostringstream tmp;
#else
          std::ostrstream tmp;
#endif

          tmp << dest;
#ifndef HAVE_SSTREAM
          tmp << std::ends;
#endif

#ifdef HAVE_SSTREAM
          mvwaddstr( header, 0, width - tmp.str().length() - 2, tmp.str().c_str() );
#else
          mvwaddstr( header, 0, width - std::strlen( tmp.str() ) - 2, tmp.str() );
          tmp.freeze( false );
#endif
          wrefresh( header );

          getmaxyx( stdscr, M_height, M_width );
          int outheight = M_height - 4;
          M_output_win = newwin( outheight, M_width, 1, 0 );
          wbkgdset( M_output_win, ACS_HLINE | A_REVERSE );
          wclear( M_output_win );
          wbkgdset( M_output_win, A_REVERSE );
          int visheight = ( outheight + 1 ) / 2 - 1;
          M_visual_win = subwin( M_output_win, visheight, M_width, 2, 0 );
          wbkgdset( M_visual_win, A_NORMAL );
          wclear( M_visual_win );
          int sbheight = visheight / 2;
          M_sb_win = subwin( M_output_win, sbheight, M_width, 3 + visheight, 0 );
          wbkgdset( M_sb_win, A_NORMAL );
          wclear( M_sb_win );
          int otherheight = outheight - visheight - sbheight - 3;
          M_other_win = subwin( M_output_win, otherheight, M_width,
                                outheight-otherheight+1, 0 );
          wbkgdset( M_other_win, A_NORMAL );
          wclear( M_other_win );
          wattron( M_output_win, A_REVERSE );
          mvwaddstr( M_output_win, 0, 2, " Visuals " );
          mvwaddstr( M_output_win, 1 + visheight, 2, " Sense Body " );
          mvwaddstr( M_output_win, outheight-otherheight-1, 2, " Other " );
          wattroff( M_output_win, A_REVERSE );
          wrefresh( M_output_win );

          M_input_header = newwin( 1, M_width, M_height-3, 0 );
          M_input_win = newwin( 2, M_width, M_height-2, 0 );
          wbkgdset( M_input_header, ACS_HLINE | A_STANDOUT );
          wclear( M_input_header );
          wbkgdset( M_input_header, A_STANDOUT );
          mvwaddstr( M_input_header, 0, 2, " Input " );
          wrefresh( M_input_header );

          mvwaddstr( M_input_win, 1, 0, "> " );
          mvwaddch( M_input_win, 1, 2, '_' | A_BLINK | A_BOLD );
          wrefresh( M_input_win );
      }

    ~NCursesClient()
      {
          delwin( M_visual_win );
          delwin( M_sb_win );
          delwin( M_other_win );
          delwin( M_output_win );
          delwin( M_output_win_border );

          delwin( M_input_win );
          delwin( M_input_header );
          endwin();
      }
};
#endif // HAVE_LIBNCURSES

#endif

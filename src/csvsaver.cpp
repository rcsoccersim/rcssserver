// -*-c++-*-

/***************************************************************************
                                csvsaver.cpp
         Class for saving results to a file as comma seperated values
                             -------------------
    begin                : 23-JUN-2003
    copyright            : (C) 2003 by The RoboCup Soccer Simulator
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


#include "resultsaver.hpp"
#include <fstream>
#include <rcssbase/lib/loader.hpp>
#include <rcssbase/conf/builder.hpp>
#include <rcssbase/conf/parser.hpp>
#include <errno.h>
#include "utility.h"
#include <time.h>

#if defined(_WIN32) || defined(__WIN32__) || defined (WIN32) || defined (__CYGWIN__)
#  ifndef WIN32
#    define WIN32
#  endif
#endif

class CSVSaverParams
    : public rcss::conf::Builder
{
public:
    CSVSaverParams( rcss::conf::Builder * parent,
                    const std::string & module_name )
        : rcss::conf::Builder( parent, module_name ),
          m_save( false ),
          m_filename( "rcssserver.csv" )
      {
          addParams();

#ifdef WIN32
          std::string user_dir = tildeExpand( "~\\.rcssserver\\" );
#else
          std::string user_dir = tildeExpand( "~/.rcssserver/" );
#endif
          std::string user_conf_name = user_dir + getModuleName() + ".conf";

          boost::filesystem::path conf_path( tildeExpand( user_conf_name ),
                                             boost::filesystem::portable_posix_name );
          parser()->parseCreateConf( conf_path, getModuleName() );
      }

    virtual
    ~CSVSaverParams()
      {
      }

    bool
    save() const
      { return m_save; }

    const std::string&
    filename() const
      { return m_filename; }
private:
    void
    addParams()
      {
          addParam( "save",
                    m_save,
                    "If save is on/true, then the saver will attempt to save the results to the database.  Otherwise it will do nothing." );


          addParam( "filename", m_filename,
                    "The file to save the results to.  If this file does not exist it will be created.  If the file does exist, the results will be appended to the end." );
      }

    bool m_save;
    std::string m_filename;

    static CSVSaverParams* s_instance;

public:
    static
    void
    initInstance()
      {
          s_instance = NULL;
      }

    static
    CSVSaverParams*
    instance()
      {
          return s_instance;
      }

    static
    rcss::lib::shared_ptr< rcss::conf::Builder >
    createInstance( rcss::conf::Builder* parent )
      {
          std::cerr << "CSVSaverParams::createInstance" << std::endl;
          if( s_instance == NULL )
          {
              s_instance = new CSVSaverParams( parent, "CSVSaver" );
              //std::cout << "instance = " << s_instance << std::endl;
          }
          return rcss::lib::shared_ptr< CSVSaverParams >( s_instance,
                                                          &destroyInstance,
                                                          rcss::lib::Loader::loadFromCache( "libcsvsaver" ) );
      }

    static
    void
    destroyInstance( CSVSaverParams* )
      {
          delete s_instance;
          s_instance = NULL;
      }
};

CSVSaverParams* CSVSaverParams::s_instance = NULL;


class CSVSaver
    : public rcss::ResultSaver
{
public:
    CSVSaver()
        : rcss::ResultSaver(),
          m_left_coin( false ),
          m_right_coin( false )
      {
          for ( int i = 0; i < TEAM_RIGHT + 1; ++i )
          {
              m_score[ i ] = 0;
              m_pen_taken[ i ] = 0;
              m_pen_scored[ i ] = 0;
          }

          if ( CSVSaverParams::instance()->save() )
          {
              m_file.open( CSVSaverParams::instance()->filename().c_str(),
                           std::ofstream::out
                           | std::ostream::app );
              if ( ! m_file.is_open() )
              {
                  std::cerr << "Error: could not open results file:"
                            << strerror( errno ) << std::endl;
              }
          }
      }

    virtual
    ~CSVSaver()
      {
          if ( CSVSaverParams::instance()->save() )
              m_file.close();
      }

private:
    virtual
    bool
    doEnabled() const
      {
          return CSVSaverParams::instance()->save();
      }

    virtual
    void
    doSaveStart()
      {}

    virtual
    void
    doSaveTime( const tm& time )
      {
          m_time = time;
      }

    virtual
    void
    doSaveTeamName( team_id id, const std::string& name )
      {
          m_team_name[ id ] = name;
      }

    virtual
    void
    doSaveCoachName( team_id id, const std::string& name )
      {
          m_coach_name[ id ] = name;
      }

    virtual
    void
    doSaveScore( team_id id, unsigned int score )
      {
          m_score[ id ] = score;
      }

    virtual
    void
    doSavePenTaken( team_id id, unsigned int taken )
      {
          m_pen_taken[ id ] = taken;
      }

    virtual
    void
    doSavePenScored( team_id id, unsigned int scored )
      {
          m_pen_scored[ id ] = scored;
      }

    virtual
    void
    doSaveCoinTossWinner( team_id id )
      {
          switch( id )
          {
          case TEAM_LEFT:
              m_left_coin = true;
              m_right_coin = false;
              break;
          case TEAM_RIGHT:
              m_left_coin = false;
              m_right_coin = true;
              break;
          default:
              m_left_coin = false;
              m_right_coin = false;
              break;
          }
      }

    virtual
    bool
    doSaveComplete()
      {
          if ( m_file.is_open() )
          {
              m_file.seekp( std::ofstream::end );
              char time_str[256];
              strftime( time_str, 256, "%Y-%m-%d %H:%M:%S", &m_time );
              m_file << time_str << ", ";

              if( m_team_name[ TEAM_LEFT ].empty() )
                  m_file << "NULL, ";
              else
                  m_file << "\"" << m_team_name[ TEAM_LEFT ] << "\", ";

              if( m_team_name[ TEAM_RIGHT ].empty() )
                  m_file << "NULL, ";
              else
                  m_file << "\"" << m_team_name[ TEAM_RIGHT ] << "\", ";


              if( m_coach_name[ TEAM_LEFT ].empty() )
                  m_file << "NULL, ";
              else
                  m_file << "\"" << m_coach_name[ TEAM_LEFT ] << "\", ";

              if( m_coach_name[ TEAM_RIGHT ].empty() )
                  m_file << "NULL, ";
              else
                  m_file << "\"" << m_coach_name[ TEAM_RIGHT ] << "\", ";

              m_file << m_score[ TEAM_LEFT ] << ", " << m_score[ TEAM_RIGHT ] << ", ";
              if( m_pen_taken[ TEAM_LEFT ] || m_pen_taken[ TEAM_RIGHT ] )
              {
                  m_file << m_pen_taken[ TEAM_LEFT ] << ", " << m_pen_taken[ TEAM_RIGHT ];
                  m_file << m_pen_scored[ TEAM_LEFT ] << ", " << m_pen_scored[ TEAM_RIGHT ];
              }
              else
              {
                  m_file << "NULL, NULL, ";
                  m_file << "NULL, NULL, ";
              }

              if( m_left_coin )
              {
                  m_file << "LEFT";
              }
              else if( m_right_coin )
              {
                  m_file << "RIGHT";
              }
              else
              {
                  m_file << "NULL";
              }
              m_file << std::endl;

              if( !m_file.good() )
              {
                  std::cerr << CSVSaverParams::instance()->filename()
                            << ": Error writing results: "
                            << strerror( errno ) << std::endl;
              }
              else
              {
                  return true;
              }
          }
          return false;
      }

    virtual
    const char*
    doGetName() const
      {
          return "CSVSaver";
      }


    tm m_time;
    std::string m_team_name[ 2 ];
    std::string m_coach_name[ 2 ];
    unsigned int m_score[ 2 ];
    unsigned int m_pen_taken[ 2 ];
    unsigned int m_pen_scored[ 2 ];
    bool m_left_coin;
    bool m_right_coin;

    bool m_save;
    std::string m_filename;
    std::ofstream m_file;

public:

    static
    void
    destroy( CSVSaver* c )
      { delete c; }

    static
    Ptr
    create()
      {
          return Ptr( new CSVSaver(),
                      &destroy,
                      rcss::lib::Loader::loadFromCache( "libcsvsaver" ) );
      }

};





RCSSLIB_INIT( libcsvsaver )
{
    // I'm not sure if this is needed, but I vaguly remember that on
    // solaris there was a problem with this.  Either way, it doesn't hurt.
    CSVSaverParams::initInstance();
    CSVSaverParams::factory().reg( &CSVSaverParams::createInstance,
                                   RCSS_MODULE_NAME );
    CSVSaver::factory().reg( &CSVSaver::create, RCSS_MODULE_NAME );
    return true;
}


RCSSLIB_FIN( libcsvsaver )
{
    CSVSaver::factory().dereg( RCSS_MODULE_NAME );
    CSVSaverParams::factory().dereg( RCSS_MODULE_NAME );
}

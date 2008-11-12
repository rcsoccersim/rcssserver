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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "csvsaver.h"

#include "utility.h"

#include <rcssbase/conf/builder.hpp>
#include <rcssbase/conf/parser.hpp>

#include <boost/filesystem/path.hpp>

#include <fstream>
#include <cerrno>
#include <ctime>

#ifdef __CYGWIN__
// cygwin is not win32
#elif defined(_WIN32) || defined(__WIN32__) || defined (WIN32)
#define RCSS_WIN
#endif

#ifdef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FS_FILE_STRING file_string
#define BOOST_FS_DIRECTORY_STRING directory_string
#else
#define BOOST_FS_FILE_STRING native_file_string
#define BOOST_FS_DIRECTORY_STRING native_directory_string
#endif

const std::string CSVSaver::CSVSAVER = "CSVSaver";

CSVSaverParam &
CSVSaverParam::instance()
{
    return CSVSaverParam::instance( NULL );
}


CSVSaverParam &
CSVSaverParam::instance( rcss::conf::Builder * parent )
{
    static bool parent_set = false;
    if ( parent != NULL || parent_set )
    {
        static CSVSaverParam rval( parent );
        parent_set = true;
        return rval;
    }
    // hack to allow link testing to call instance without crashing
    // do not used the return value in these situations
    CSVSaverParam * rval = NULL;
    return *rval;
}

bool
CSVSaverParam::init( rcss::conf::Builder * parent )
{
    assert( parent );
    instance( parent );

    if ( ! instance().M_builder->parser() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": internal error: CSVSaverParam could not find configuration parser\n";
        std::cerr << "Please submit a full bug report to sserver-bugs@lists.sf.net\n";
        return false;
    }

#if defined(RCSS_WIN) || defined(__CYGWIN__)
    //std::string conf_dir = "~\\.rcssserver\\";
    std::string conf_dir = ".";
#else
    std::string conf_dir = "~/.rcssserver";
#endif
    const char * env_conf_dir = std::getenv( "RCSS_CONF_DIR" );
    if ( env_conf_dir )
    {
        conf_dir = env_conf_dir;
    }

    boost::filesystem::path conf_path;
    try
    {
        conf_path = boost::filesystem::path( tildeExpand( conf_dir )
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
                                             , &boost::filesystem::native
#endif
                                             );

        conf_path /= "CSVSaver.conf";
    }
    catch ( std::exception & e )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " Exception caught! " << e.what()
                  << "\nCould not read config directory '"
                  << tildeExpand( conf_dir ) << "'"
                  << std::endl;
    }

    if ( ! instance().M_builder->parser()->parseCreateConf( conf_path,
                                                            "CSVSaver" ) )
    {
        std::cerr << "could not create or parse configuration file '"
                  << conf_path.BOOST_FS_FILE_STRING()
                  << "'" << std::endl;
        return false;
    }

    if ( instance().M_builder->version() != instance().M_builder->parsedVersion() )
    {
        std::cerr << "Version mismatched in the configuration file. "
                  << "Need to regenerate '" << conf_path.BOOST_FS_FILE_STRING() << "'"
                  << " or set '" << instance().M_builder->version() << "' to the 'version' option."
                  << std::endl;
//         std::cerr << "registered version = ["
//                   << instance().M_builder->version() << "]\n"
//                   << "parsed version = ["
//                   << instance().M_builder->parsedVersion() << "]\n"
//                   << std::flush;
        return false;
    }

    return true;
}



CSVSaverParam::CSVSaverParam( rcss::conf::Builder * parent )
    : M_builder( new rcss::conf::Builder( parent, VERSION, "CSVSaver" ) )
{
    setDefaults();
    addParams();
}


CSVSaverParam::~CSVSaverParam()
{
    //std::cerr << "delete CSVSaverParam" << std::endl;
}

void
CSVSaverParam::setDefaults()
{
    M_save = false;
    M_filename = "rcssserver.csv";
}

void
CSVSaverParam::addParams()
{
    M_builder->addParam( "save",
                         M_save,
                         "If save is on/true, then the saver will attempt to save"
                         " the results to the database.  Otherwise it will do nothing." );
    M_builder->addParam( "filename",
                         M_filename,
                         "The file to save the results to.  If this file does not"
                         " exist it will be created.  If the file does exist, the"
                         " results will be appended to the end." );
}




CSVSaver::CSVSaver()
    : ResultSaver(),
      m_left_coin( false ),
      m_right_coin( false )
{
    for ( int i = 0; i < TEAM_RIGHT + 1; ++i )
    {
        m_score[ i ] = 0;
        m_pen_taken[ i ] = 0;
        m_pen_scored[ i ] = 0;
    }

    if ( CSVSaverParam::instance().save() )
    {
        m_file.open( CSVSaverParam::instance().filename().c_str(),
                     std::ofstream::out | std::ostream::app );
        if ( ! m_file.is_open() )
        {
            std::cerr << "Error: could not open results file:"
                      << strerror( errno ) << std::endl;
        }
    }
}

CSVSaver::~CSVSaver()
{
    if ( CSVSaverParam::instance().save() )
    {
        m_file.close();
    }
}

bool
CSVSaver::doEnabled() const
{
    return CSVSaverParam::instance().save();
}

void
CSVSaver::doSaveStart()
{

}

void
CSVSaver::doSaveTime( const tm & time )
{
    m_time = time;
}

void
CSVSaver::doSaveTeamName( team_id id,
                          const std::string & name )
{
    m_team_name[ id ] = name;
}

void
CSVSaver::doSaveCoachName( team_id id,
                           const std::string & name )
{
    m_coach_name[ id ] = name;
}

void
CSVSaver::doSaveScore( team_id id,
                       unsigned int score )
{
    m_score[ id ] = score;
}

void
CSVSaver::doSavePenTaken( team_id id,
                          unsigned int taken )
{
    m_pen_taken[ id ] = taken;
}

void
CSVSaver::doSavePenScored( team_id id,
                           unsigned int scored )
{
    m_pen_scored[ id ] = scored;
}

void
CSVSaver::doSaveCoinTossWinner( team_id id )
{
    switch ( id ) {
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

bool
CSVSaver::doSaveComplete()
{
    if ( m_file.is_open() )
    {
        m_file.seekp( std::ofstream::end );
        char time_str[256];
        strftime( time_str, 256, "%Y-%m-%d %H:%M:%S", &m_time );
        m_file << time_str << ", ";

        if ( m_team_name[ TEAM_LEFT ].empty() )
            m_file << "NULL, ";
        else
            m_file << "\"" << m_team_name[ TEAM_LEFT ] << "\", ";

        if ( m_team_name[ TEAM_RIGHT ].empty() )
            m_file << "NULL, ";
        else
            m_file << "\"" << m_team_name[ TEAM_RIGHT ] << "\", ";


        if ( m_coach_name[ TEAM_LEFT ].empty() )
            m_file << "NULL, ";
        else
            m_file << "\"" << m_coach_name[ TEAM_LEFT ] << "\", ";

        if ( m_coach_name[ TEAM_RIGHT ].empty() )
            m_file << "NULL, ";
        else
            m_file << "\"" << m_coach_name[ TEAM_RIGHT ] << "\", ";

        m_file << m_score[ TEAM_LEFT ] << ", " << m_score[ TEAM_RIGHT ] << ", ";
        if ( m_pen_taken[ TEAM_LEFT ] || m_pen_taken[ TEAM_RIGHT ] )
        {
            m_file << m_pen_taken[ TEAM_LEFT ] << ", " << m_pen_taken[ TEAM_RIGHT ];
            m_file << m_pen_scored[ TEAM_LEFT ] << ", " << m_pen_scored[ TEAM_RIGHT ];
        }
        else
        {
            m_file << "NULL, NULL, ";
            m_file << "NULL, NULL, ";
        }

        if ( m_left_coin )
        {
            m_file << "LEFT";
        }
        else if ( m_right_coin )
        {
            m_file << "RIGHT";
        }
        else
        {
            m_file << "NULL";
        }
        m_file << std::endl;

        if ( ! m_file.good() )
        {
            std::cerr << CSVSaverParam::instance().filename()
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

const char *
CSVSaver::doGetName() const
{
    return "CSVSaver";
}


namespace {

template< typename Saver >
ResultSaver::Ptr
create()
{
    return ResultSaver::Ptr( new Saver() );
}

//rcss::RegHolder csv = ResultSaver::factory().autoReg( &create< CSVSaver >, "CSVSaver" );
rcss::RegHolder csv = ResultSaver::factory().autoReg( &create< CSVSaver >, CSVSaver::CSVSAVER );
}

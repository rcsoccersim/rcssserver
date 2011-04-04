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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "csvsaver.h"

#include "utility.h"

#include <rcssbase/conf/builder.hpp>
#include <rcssbase/conf/parser.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>
#include <cerrno>
#include <ctime>

#ifdef __CYGWIN__
// cygwin is not win32
#elif defined(_WIN32) || defined(__WIN32__) || defined (WIN32)
#define RCSS_WIN
#endif

const std::string CSVSaver::NAME = "CSVSaver";

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
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION == 2
#  ifndef BOOST_FILESYSTEM_NO_DEPRECATED
                                             , &boost::filesystem::native
#  endif
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
      M_left_coin( false ),
      M_right_coin( false )
{
    for ( int i = 0; i < TEAM_RIGHT + 1; ++i )
    {
        M_score[ i ] = 0;
        M_pen_taken[ i ] = 0;
        M_pen_scored[ i ] = 0;
    }
}

CSVSaver::~CSVSaver()
{
    if ( M_file.is_open() )
    {
        M_file.close();
    }
}

void
CSVSaver::openResultsFile()
{
    bool new_file = false;
    boost::filesystem::path file_path;
    try
    {
        file_path = boost::filesystem::path( tildeExpand( CSVSaverParam::instance().filename() )
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION == 2
#  ifndef BOOST_FILESYSTEM_NO_DEPRECATED
                                             , &boost::filesystem::native
#  endif
#endif
                                             );
        new_file = ! boost::filesystem::exists( file_path );
    }
    catch ( std::exception & e )
    {
        std::cerr << __FILE__ << ':' << __LINE__
                  << " Exception caught! " << e.what()
                  << "\nCould not open result file: "
                  << tildeExpand( CSVSaverParam::instance().filename() )
                  << std::endl;
        return;
    }

    M_file.open( file_path.BOOST_FS_FILE_STRING().c_str(),
                 std::ofstream::out | std::ostream::app );
    if ( ! M_file.is_open() )
    {
        std::cerr << "Error: could not open results file:"
                  << strerror( errno ) << std::endl;
        return;
    }

    if ( new_file )
    {
        M_file << "time, left team, right team, left coach, right coach,"
               << " left score, right score, left pen taken, right pen taken,"
               << " left pen score, right pen score, cointoss"
               << std::endl;
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
    if ( CSVSaverParam::instance().save() )
    {
        openResultsFile();
    }
}

void
CSVSaver::doSaveTime( const tm & time )
{
    M_time = time;
}

void
CSVSaver::doSaveTeamName( team_id id,
                          const std::string & name )
{
    M_team_name[ id ] = name;
}

void
CSVSaver::doSaveCoachName( team_id id,
                           const std::string & name )
{
    M_coach_name[ id ] = name;
}

void
CSVSaver::doSaveScore( team_id id,
                       unsigned int score )
{
    M_score[ id ] = score;
}

void
CSVSaver::doSavePenTaken( team_id id,
                          unsigned int taken )
{
    M_pen_taken[ id ] = taken;
}

void
CSVSaver::doSavePenScored( team_id id,
                           unsigned int scored )
{
    M_pen_scored[ id ] = scored;
}

void
CSVSaver::doSaveCoinTossWinner( team_id id )
{
    switch ( id ) {
    case TEAM_LEFT:
        M_left_coin = true;
        M_right_coin = false;
        break;
    case TEAM_RIGHT:
        M_left_coin = false;
        M_right_coin = true;
        break;
    default:
        M_left_coin = false;
        M_right_coin = false;
        break;
    }
}

bool
CSVSaver::doSaveComplete()
{
    if ( M_file.is_open() )
    {
        M_file.seekp( std::ofstream::end );
        char time_str[256];
        std::strftime( time_str, 256, "%Y-%m-%d %H:%M:%S", &M_time );
        M_file << time_str << ", ";

        if ( M_team_name[ TEAM_LEFT ].empty() )
        {
            M_file << "NULL, ";
        }
        else
        {
            M_file << "\"" << M_team_name[ TEAM_LEFT ] << "\", ";
        }

        if ( M_team_name[ TEAM_RIGHT ].empty() )
        {
            M_file << "NULL, ";
        }
        else
        {
            M_file << "\"" << M_team_name[ TEAM_RIGHT ] << "\", ";
        }


        if ( M_coach_name[ TEAM_LEFT ].empty() )
        {
            M_file << "NULL, ";
        }
        else
        {
            M_file << "\"" << M_coach_name[ TEAM_LEFT ] << "\", ";
        }

        if ( M_coach_name[ TEAM_RIGHT ].empty() )
        {
            M_file << "NULL, ";
        }
        else
        {
            M_file << "\"" << M_coach_name[ TEAM_RIGHT ] << "\", ";
        }

        M_file << M_score[ TEAM_LEFT ] << ", " << M_score[ TEAM_RIGHT ] << ", ";
        if ( M_pen_taken[ TEAM_LEFT ] || M_pen_taken[ TEAM_RIGHT ] )
        {
            M_file << M_pen_taken[ TEAM_LEFT ] << ", " << M_pen_taken[ TEAM_RIGHT ] << ", "
                   << M_pen_scored[ TEAM_LEFT ] << ", " << M_pen_scored[ TEAM_RIGHT ] << ", ";
        }
        else
        {
            M_file << "NULL, NULL, ";
            M_file << "NULL, NULL, ";
        }

        if ( M_left_coin )
        {
            M_file << "LEFT";
        }
        else if ( M_right_coin )
        {
            M_file << "RIGHT";
        }
        else
        {
            M_file << "NULL";
        }
        M_file << std::endl;

        if ( ! M_file.good() )
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
    //return "CSVSaver";
    return CSVSaver::NAME.c_str();
}

ResultSaver::Ptr
CSVSaver::create()
{
    return ResultSaver::Ptr( new CSVSaver() );
}

namespace {
// template< typename Saver >
// ResultSaver::Ptr
// create()
// {
//     return ResultSaver::Ptr( new Saver() );
// }
//rcss::RegHolder s = ResultSaver::factory().autoReg( &create< CSVSaver >, "CSVSaver" );

rcss::RegHolder s = ResultSaver::factory().autoReg( &CSVSaver::create, CSVSaver::NAME );
}

// -*-c++-*-

/***************************************************************************
                                mysqlsaver.cpp
                    Class for saving results to a mysql database
                             -------------------
    begin                : 09-MAY-2003
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


#include "resultsaver.hpp"
#include <time.h>
#include <iostream>
#include <memory>
#include <rcssbase/lib/loader.hpp>
#include <mysql/mysql.h>
#include <fstream>
#include <errno.h>
#include "utility.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif


#if defined(_WIN32) || defined(__WIN32__) || defined (WIN32) || defined (__CYGWIN__)
#  ifndef WIN32
#    define WIN32
#  endif
#endif

class MySQLSaver
    : public rcss::ResultSaver
{
public:
    MySQLSaver( int argc, const char** argv, const std::string& module_name )
        : rcss::ResultSaver( argc, argv, module_name ),
          m_mysql( NULL ),
          m_left_coin( false ),
          m_right_coin( false ),
          m_save( false ),
          m_port( 0 ),
          m_compress( false ),
          m_ssl( false ),
          m_round_id( -1 ),
          m_event_id( -1 )
    {
        for( int i = 0; i < TEAM_RIGHT + 1; ++i )
        {
            m_score[ i ] = 0;
            m_pen_taken[ i ] = 0;
            m_pen_scored[ i ] = 0;
        }
         
        addParams();
        
#ifdef WIN32
        std::string user_dir = tildeExpand( "~\\.rcssserver\\" );
#else
        std::string user_dir = tildeExpand( "~/.rcssserver/" );
#endif        
        std::string user_conf_name = user_dir + getModuleName() + ".conf";
        m_conf_parser.parseCreateConf( user_conf_name );
        m_conf_parser.parse( argc, argv );
        
        if( m_save )
        {
            m_mysql = mysql_init( NULL );
            if( m_mysql == 0 )
            {
                std::cerr << "Error: could not initialise MySQL object\n";
            }
            else
            {
                if( mysql_real_connect( m_mysql,
                                        ( m_host.empty() 
                                          ? NULL 
                                          : m_host.c_str() ),
                                        ( m_user.empty() 
                                          ? NULL 
                                          : m_user.c_str() ),
                                        m_pass.c_str(), 
                                        m_db.c_str(),
                                        m_port, 
                                        ( m_unix_socket.empty() 
                                          ? NULL 
                                          : m_host.c_str() ),
                                        ( ( m_compress 
                                            ? CLIENT_COMPRESS : 0 ) 
                                          | ( m_ssl 
                                              ? CLIENT_SSL : 0 ) ) )
                    == NULL )
                {
                    std::cerr << "Error: could not connect to database: "
                              << mysql_error(  m_mysql ) << std::endl;
                    mysql_close( m_mysql );
                    m_mysql = 0;
                }
                else
                {
                    std::cout << "Connected to database\n";
                    createTables();
                    determineRoundID();    
                }
            }
        }
    }
        
    virtual
    ~MySQLSaver()
    {
        if( m_save )
            mysql_close( m_mysql );
    }

private:
    void
    addParams()
    {
        addParam( "save", m_save, "If save is on/true, then the saver will attempt to save the results to the database.  Otherwise it will do nothing." );
        addParam( "host", m_host, "The value of host may be either a hostname or an IP address. If host is NULL(defualt) or the string \"localhost\", a connection to the local host is assumed. If the OS supports sockets (Unix) or named pipes (Windows), they are used instead of TCP/IP to connect to the server. " );
        addParam( "user", m_user, "The user parameter contains the user's MySQL login ID. If user is NULL(default), the current user is assumed. Under Unix, this is the current login name. Under Windows ODBC, the current user name must be specified explicitly." );
        addParam( "password", m_pass, "The passwd parameter contains the password for user. If passwd is NULL, only entries in the user table for the user that have a blank (empty) password field will be checked for a match. This allows the database administrator to set up the MySQL privilege system in such a way that users get different privileges depending on whether or not they have specified a password. Note: Do not attempt to encrypt the password; password encryption is handled automatically." );
        addParam( "db", m_db, "db is the database name. If db is not NULL, the connection will set the default database to this value." );
        addParam( "port", m_port, "If port is not 0(default), the value will be used as the port number for the TCP/IP connection. Note that the host parameter determines the type of the connection." );
        addParam( "unix_socket", m_unix_socket, "If unix_socket is not NULL(default), the string specifies the socket or named pipe that should be used. Note that the host parameter determines the type of the connection." );
        addParam( "compress", m_compress, "Use compression protocol." );
        addParam( "ssl", m_ssl, "Use SSL (encrypted protocol)." );


        addParam( "round_id", m_round_id, "The ID of the round this game is part of.  If less than zero(default: -1) then round_name and event_name/event_id are used to try and determine the round_id.  Failing that, the round_id is left at the default of zero." );
        addParam( "round_name", m_round_name, "The name of the round this game is part of.  This is only used if round_id is less than zero, there are no other rounds with this name in the specified event (see event_name/event_id) or no other round, if no event is specified" );
        addParam( "event_id", m_event_id, "The ID of the event this game is part of.  This vaule is used to try to distinguish between two rounds of the same names in different events if the round_id is less than  zero (default: -1) and the round_name is specfied" );
        addParam( "event_name", m_event_name, "The name of the event this game is part of.  This value is used to try to distinguish between two rounds of the same name in different events is the round_id is less than zero, the round_name is specified and the event_id is zero(default)" );
    }
    
    void
    createTables()
    {
        if( !doCreateTables() )
        {
            mysql_close( m_mysql );
        }
    }

    bool
    createEvents()
    { return doCreateEvents(); }

    bool
    createRounds()
    { return doCreateRounds(); }

    bool
    createGames()
    { return doCreateGames(); }

    void
    determineRoundID()
    { return doDetermineRoundID(); }

    void
    determineEventID()
    { return doDetermineEventID(); }

protected:
    virtual
    bool
    doCreateEvents()
    {
        std::string query = "CREATE TABLE IF NOT EXISTS `EVENTS` (\n";
        query += "`ID` int(10) unsigned NOT NULL default '0' auto_increment,\n";
        query += "`FROM` date default NULL,\n";
        query += "`TILL` date default NULL,\n";
        query += "`NAME` varchar(255) NOT NULL,\n";
        query += "`URL` varchar(255) default NULL,\n";
        query += "`TYPE` enum('World Cup','Regional','Internet','Other') NOT NULL default 'Other',\n";
        query += "`Comment` text NULL default NULL,\n";
        query += "PRIMARY KEY  (`ID`),\n";
        query += "UNIQUE KEY `NAME` (`NAME`)\n";
        query += ")\n";
        if( mysql_query( m_mysql, query.c_str() ) != 0 )
        {
            std::cerr << "Error: Could not create ROUNDS table in database: "
                      << mysql_error( m_mysql ) << std::endl;
            std::cerr << "Query was:\n" << query << std::endl;
            return false;
        }
        return true;
    }

    virtual
    bool
    doCreateRounds()
    {
        std::string query = "CREATE TABLE IF NOT EXISTS `ROUNDS` (\n";
        query += "`ID` int unsigned NOT NULL default '0' auto_increment,\n";
        query += "`EVENT_ID` int unsigned NULL default NULL,\n";
        query += "`NAME` varchar(255) NOT NULL,\n";
        query += "PRIMARY KEY  (`ID`),\n";
        query += "UNIQUE KEY `EVENT_ID` (`EVENT_ID`,`NAME`)\n";
        query += ")\n";
        if( mysql_query( m_mysql, query.c_str() ) != 0 )
        {
            std::cerr << "Error: Could not create ROUNDS table in database: "
                      << mysql_error( m_mysql ) << std::endl;
            std::cerr << "Query was:\n" << query << std::endl;
            return false;
        }
        return true;
    }

    virtual
    bool
    doCreateGames()
    {
        std::string query = "CREATE TABLE IF NOT EXISTS `GAMES` (";
        query += "`ID` INT UNSIGNED DEFAULT '0' NOT NULL AUTO_INCREMENT,";
        query += "`ROUND_ID` INT UNSIGNED NULL DEFAULT NULL ,";
        query += "`DATETIME` DATETIME DEFAULT 'now()' NOT NULL ,";
        query += "`LEFT` VARCHAR( 32 ) NOT NULL ,";
        query += "`RIGHT` VARCHAR( 32 ) NOT NULL ,";
        query += "`COACH_L` VARCHAR( 32 ) DEFAULT NULL ,";
        query += "`COACH_R` VARCHAR( 32 ) DEFAULT NULL ,";
        query += "`SCORE_L` INT UNSIGNED DEFAULT '0' NOT NULL ,";
        query += "`SCORE_R` INT UNSIGNED DEFAULT '0' NOT NULL ,";
        query += "`PEN_TAKEN_L` INT UNSIGNED DEFAULT NULL ,";
        query += "`PEN_TAKEN_R` INT UNSIGNED DEFAULT NULL ,";
        query += "`PEN_SCORED_L` INT DEFAULT NULL ,";
        query += "`PEN_SCORED_R` INT DEFAULT NULL ,";
        query += "`COIN_TOSS_WINNER` ENUM( 'LEFT', 'RIGHT' ) DEFAULT NULL ,";
        query += "`LEFT_FROM` INT UNSIGNED DEFAULT NULL ,";
        query += "`RIGHT_FROM` INT UNSIGNED DEFAULT NULL ,";
        query += "`LEFT_FROM_TYPE` ENUM( 'WINNER', 'LOOSER' ) DEFAULT NULL ,";
        query += "`RIGHT_FROM_TYPE` ENUM( 'WINNER', 'LOOSER' ) DEFAULT NULL ,";
        query += "PRIMARY KEY ( `ID` ) ,";
        query += "INDEX ( `ROUND_ID` , `DATETIME` , `LEFT` , `RIGHT` )"; 
        query += ")";
        if( mysql_query( m_mysql, query.c_str() ) != 0 )
        {
            std::cerr << "Error: Could not create GAMES table in database: "
                      << mysql_error( m_mysql ) << std::endl;
            return false;
        }
        return true;
    }


    virtual
    bool
    doCreateTables()
    {
        return createEvents()
            && createRounds() 
            && createGames();
    }

    void
    addRound()
    {
        std::cout << "Adding round: " << m_round_name << std::endl;
#ifdef HAVE_SSTREAM
        std::ostringstream add_round;
#else
        std::ostrstream add_round;
#endif
        add_round << "INSERT INTO `ROUNDS` (`NAME`, `EVENT_ID`) VALUES ('" << m_event_name << "', ";
        if( m_event_id < 0 )
            add_round << "null";
        else
            add_round << m_event_id;
        add_round << " )";
#ifndef HAVE_SSTREAM
        add_round << std::ends;
#endif
        if( mysql_query( m_mysql,
#ifdef HAVE_SSTREAM
                         add_round.str().c_str()
#else
                         add_round.str()
#endif
                ) != 0 )
        {
            std::cerr << "Error: Could not add round to database: "
                      << mysql_error( m_mysql ) << std::endl;
#ifndef HAVE_SSTREAM
            add_round.freeze( false );
#endif
        }
        else
        {
#ifndef HAVE_SSTREAM
            add_round.freeze( false );
#endif
            std::cout << "Round '" << m_round_name << "' added\n";
        }
    }
    
    void
    addEvent()
    {
        std::cout << "Adding event: " << m_event_name << std::endl;
#ifdef HAVE_SSTREAM
        std::ostringstream add_event;
#else
        std::ostrstream add_event;
#endif
        add_event << "INSERT INTO `EVENTS` (`NAME`) VALUES ('" << m_event_name << "')";
#ifndef HAVE_SSTREAM
        add_event << std::ends;
#endif
        if( mysql_query( m_mysql,
#ifdef HAVE_SSTREAM
                         add_event.str().c_str()
#else
                         add_event.str()
#endif
                ) != 0 )
        {
            std::cerr << "Error: Could not add event to database: "
                      << mysql_error( m_mysql ) << std::endl;
#ifndef HAVE_SSTREAM
            add_event.freeze( false );
#endif
        }
        else
        {
#ifndef HAVE_SSTREAM
            add_event.freeze( false );
#endif
            std::cout << "Event '" << m_event_name << "' added\n";
        }
    }
    
    void
    getRoundByName()
    {
#ifdef HAVE_SSTREAM
        std::ostringstream query;
#else
        std::ostrstream query;
#endif
        query << "SELECT `ID` FROM `ROUNDS` WHERE `NAME` = '" << m_round_name << "'";

        if( m_event_id >= 0 )
            query << " AND EVENT_ID = " << m_event_id;

#ifndef HAVE_SSTREAM
        query << std::ends;
#endif
        if( mysql_query( m_mysql,
#ifdef HAVE_SSTREAM
                         query.str().c_str()
#else
                         query.str()
#endif
                ) != 0 )
        {
            std::cerr << "Error: Could not select round from database: "
                      << mysql_error( m_mysql ) << std::endl;
            std::cerr << "Query was:\n" << query.str() << std::endl;
#ifndef HAVE_SSTREAM
            query.freeze( false );
#endif
        }
        else
        {
#ifndef HAVE_SSTREAM
            query.freeze( false );
#endif
            MYSQL_RES* results = mysql_store_result( m_mysql );
            if( results == NULL )
            {
                std::cerr << "Error: Could not get select round results from database: "
                          << mysql_error( m_mysql ) << std::endl;
            }
            else
            {
                my_ulonglong rows = mysql_num_rows( results );
                if( rows > 1 )
                {
                    std::cerr << "Error: Mutiple rounds returned for the name '" << m_round_name << "'\n";
                }
                else if( rows == 0 )
                {
                    std::cout << "No round with the name '" << m_round_name << "' found.\n";
                    addRound();
                    getRoundByName();
                }
                else
                {
                    MYSQL_ROW row = mysql_fetch_row( results );
                    if( row == NULL )
                    {
                        std::cerr << "Error: could not get row from round query:"
                                  << mysql_error( m_mysql ) << std::endl;
                    }
                    else
                    {
#ifdef HAVE_SSTREAM
                        std::istringstream id( row[ 0 ] );
#else
                        std::istrstream id( row[ 0 ] );
#endif

                        id >> m_round_id;
                    }
                }
            }
        }
    }

    void
    getEventByName()
    {
#ifdef HAVE_SSTREAM
        std::ostringstream query;
#else
        std::ostrstream query;
#endif
        query << "SELECT `ID` FROM `EVENTS` WHERE `NAME` = '" << m_event_name << "'";
#ifndef HAVE_SSTREAM
        query << std::ends;
#endif
        if( mysql_query( m_mysql,
#ifdef HAVE_SSTREAM
                         query.str().c_str()
#else
                         query.str()
#endif
                ) != 0 )
        {
            std::cerr << "Error: Could not select event from database: "
                      << mysql_error( m_mysql ) << std::endl;
            std::cerr << "Query was:\n" << query.str() << std::endl;
#ifndef HAVE_SSTREAM
            query.freeze( false );
#endif
        }
        else
        {
#ifndef HAVE_SSTREAM
            query.freeze( false );
#endif
            MYSQL_RES* results = mysql_store_result( m_mysql );
            if( results == NULL )
            {
                std::cerr << "Error: Could not get select event results from database: "
                          << mysql_error( m_mysql ) << std::endl;
            }
            else
            {
                my_ulonglong rows = mysql_num_rows( results );
                if( rows > 1 )
                {
                    std::cerr << "Error: Mutiple events returned for the name '" << m_event_name << "'\n";
                }
                else if( rows == 0 )
                {
                    std::cout << "No event with the name '" << m_event_name << "' found.\n";
                    addEvent();
                    getEventByName();
                }
                else
                {
                    MYSQL_ROW row = mysql_fetch_row( results );
                    if( row == NULL )
                    {
                        std::cerr << "Error: could not get row from event query:"
                                  << mysql_error( m_mysql ) << std::endl;
                    }
                    else
                    {
#ifdef HAVE_SSTREAM
                        std::istringstream id( row[ 0 ] );
#else
                        std::istrstream id( row[ 0 ] );
#endif
                        id >> m_event_id;
                    }
                }
            }
        }
    }

    void
    getRoundByID()
    {
#ifdef HAVE_SSTREAM
        std::ostringstream query;
#else
        std::ostrstream query;
#endif
        query << "SELECT `NAME` FROM `ROUNDS` WHERE `ID` = " << m_round_id;

#ifndef HAVE_SSTREAM
        query << std::ends;
#endif
        if( mysql_query( m_mysql,
#ifdef HAVE_SSTREAM
                         query.str().c_str()
#else
                         query.str()
#endif
                ) != 0 )
        {
            std::cerr << "Error: Could not select round from database: "
                      << mysql_error( m_mysql ) << std::endl;
            std::cerr << "Query was:\n" << query.str() << std::endl;
#ifndef HAVE_SSTREAM
            query.freeze( false );
#endif
        }
        else
        {
#ifndef HAVE_SSTREAM
            query.freeze( false );
#endif
            MYSQL_RES* results = mysql_store_result( m_mysql );
            if( results == NULL )
            {
                std::cerr << "Error: Could not get select round results from database: "
                          << mysql_error( m_mysql ) << std::endl;
            }
            else
            {
                my_ulonglong rows = mysql_num_rows( results );
                if( rows > 1 )
                {
                    std::cerr << "Error: Mutiple rounds returned for the ID '" << m_round_id << "'\n";
                    std::cerr << "Setting round_id to -1\n";
                }
                else if( rows == 0 )
                {
                    std::cout << "No round with the id '" << m_round_id << "' found.\n";
                    std::cout << "Setting round id to -1\n";
                    m_round_id = -1;
                    m_round_name.clear();
                }
                else
                {
                    MYSQL_ROW row = mysql_fetch_row( results );
                    if( row == NULL )
                    {
                        std::cerr << "Error: could not get row from round query:"
                                  << mysql_error( m_mysql ) << std::endl;
                    }
                    else
                    {
                        m_round_name = row[ 0 ];
                    }
                }
            }
        }
    }

    void
    getEventByID()
    {
#ifdef HAVE_SSTREAM
        std::ostringstream query;
#else
        std::ostrstream query;
#endif
        query << "SELECT `NAME` FROM `EVENTS` WHERE `ID` = " << m_event_id;
#ifndef HAVE_SSTREAM
        query << std::ends;
#endif
        if( mysql_query( m_mysql,
#ifdef HAVE_SSTREAM
                         query.str().c_str()
#else
                         query.str()
#endif
                ) != 0 )
        {
            std::cerr << "Error: Could not select event from database: "
                      << mysql_error( m_mysql ) << std::endl;
            std::cerr << "Query was:\n" << query.str() << std::endl;
#ifndef HAVE_SSTREAM
            query.freeze( false );
#endif
        }
        else
        {
#ifndef HAVE_SSTREAM
            query.freeze( false );
#endif
            MYSQL_RES* results = mysql_store_result( m_mysql );
            if( results == NULL )
            {
                std::cerr << "Error: Could not get select event results from database: "
                          << mysql_error( m_mysql ) << std::endl;
            }
            else
            {
                my_ulonglong rows = mysql_num_rows( results );
                if( rows > 1 )
                {
                    std::cerr << "Error: Mutiple events returned for the ID '" << m_event_id << "'\n";
                }
                else if( rows == 0 )
                {
                    std::cout << "No event with the id '" << m_event_id << "' found.\n";
                    std::cout << "Setting event id to -1\n";
                    m_event_id = -1;
                    m_event_name.clear();
                }
                else
                {
                    MYSQL_ROW row = mysql_fetch_row( results );
                    if( row == NULL )
                    {
                        std::cerr << "Error: could not get row from event query:"
                                  << mysql_error( m_mysql ) << std::endl;
                    }
                    else
                    {
                        m_event_name = row[ 0 ];
                    }
                }
            }
        }
    }
    
    virtual
    void
    doDetermineEventID()
    {
        if( m_event_id < 0 )
        {
            if( !m_event_name.empty() )
            {
                getEventByName();
            }
        }
        else
        {
            std::cout << "Using event id specifed\n";
            getEventByID();
        }
        if( m_event_id >= 0 )
            std::cout << "Event: " << m_event_id << ", '" << m_event_name << "'" << std::endl;
    }
    
    virtual
    void
    doDetermineRoundID()
    {
        determineEventID();
      
        if( m_round_id < 0 )
        {
            if( !m_round_name.empty() )
            {
                getRoundByName();
            }
        }
        else
        {
            std::cout << "Using round id specifed\n";
            getRoundByID();
        }
        
        if( m_round_id > -1 )
            std::cout << "Round: " << m_round_id << ", '" << m_round_name << "'" << std::endl;
    }

    
private:   
    
    virtual
    bool
    doEnabled() const
    { return m_save; }

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
        if( m_mysql )
        {
            if( m_team_name[ TEAM_LEFT ].empty()
                || m_team_name[ TEAM_RIGHT ].empty() )
            {
                std::cout << "Cannot save games unless two teams connect\n";
                return false;
            }

            char time_str[256];
            strftime( time_str, 256, "%Y-%m-%d %H:%M:%S", &m_time );
#ifdef HAVE_SSTREAM
            std::ostringstream query;
#else
            std::ostrstream query;
#endif
            query << "INSERT INTO `GAMES` ( `DATETIME` , `ROUND_ID` ,\n"
                  << "`LEFT` , `RIGHT` , `COACH_L` , `COACH_R` , `SCORE_L` ,\n"
                  << "`SCORE_R` , `PEN_TAKEN_L` , `PEN_TAKEN_R` , `PEN_SCORED_L` ,\n"
                  << "`PEN_SCORED_R` , `COIN_TOSS_WINNER` ) \n"
                  << "VALUES ( '" << time_str << "', '" << m_round_id << "', \n";

            query << "'" << m_team_name[ TEAM_LEFT ] << "', ";
            query << "'" << m_team_name[ TEAM_RIGHT ] << "', \n";


            if( m_coach_name[ TEAM_LEFT ].empty() )
                query << "NULL, ";
            else
                query << "'" << m_coach_name[ TEAM_LEFT ] << "', ";

            if( m_coach_name[ TEAM_RIGHT ].empty() )
                query << "NULL, \n";
            else
                query << "'" << m_coach_name[ TEAM_RIGHT ] << "', \n";

            

            query << "'" << m_score[ TEAM_LEFT ] << "', '" 
                  << m_score[ TEAM_RIGHT ] << "', \n";

            if( m_pen_taken[ TEAM_LEFT ] || m_pen_taken[ TEAM_RIGHT ] )
            {
                query << "'" << m_pen_taken[ TEAM_LEFT ] 
                      << "', '" << m_pen_taken[ TEAM_RIGHT ] << "', \n";
                query << "'" << m_pen_scored[ TEAM_LEFT ] << "', '" 
                      << m_pen_scored[ TEAM_RIGHT ] << "', \n";
            }
            else
            {
                query << "NULL, NULL, \n";
                query << "NULL, NULL, \n";
            }
            
            if( m_left_coin )
            {
                query << "'LEFT'";
            }
            else if( m_right_coin )
            {
                query << "'RIGHT'";
            }
            else
            {
                query << "NULL";
            }
            query << ")\n";

#ifndef HAVE_SSREAM
            query << std::ends;
#endif

            if( mysql_query( m_mysql,
#ifdef HAVE_SSTREAM
                             query.str().c_str()
#else
                             query.str()
#endif
                    ) != 0 )
            {
                std::cerr << "Error: Could not save results to database: "
                          << mysql_error( m_mysql ) << std::endl;
                std::cerr << "Query was:\n" << query.str() << std::endl;
#ifndef HAVE_SSTREAM
                query.freeze( false );
#endif
                return false;
            }
#ifndef HAVE_SSTREAM
            query.freeze( false );
#endif
            getGameID();
        }
        else
        {
            return false;
        }
        return true;
    }

    void
    getGameID()
    {
        char time_str[256];
        strftime( time_str, 256, "%Y-%m-%d %H:%M:%S", &m_time );
#ifdef HAVE_SSTREAM
        std::ostringstream query;
#else
        std::ostrstream query;
#endif
        query << "SELECT `ID` FROM `GAMES` WHERE\n"
              << "`DATETIME` = '" << time_str << "'\n";
        if( m_round_id > 0 )
            query << "AND `ROUND_ID` = " << m_round_id << std::endl;
        query << "AND `LEFT` = '" << m_team_name[ TEAM_LEFT ] << "'\n";
        query << "AND `RIGHT` = '" << m_team_name[ TEAM_RIGHT ] << "'\n";
        query << "AND `SCORE_L` = " << m_score[ TEAM_LEFT ] << "\n";
        query << "AND `SCORE_R` = " << m_score[ TEAM_RIGHT ] << "\n";

#ifndef HAVE_SSREAM
        query << std::ends;
#endif
        
        if( mysql_query( m_mysql,
#ifdef HAVE_SSTREAM
                         query.str().c_str()
#else
                         query.str()
#endif
                ) != 0 )
        {
            std::cerr << "Error: Could not get game id from database: "
                      << mysql_error( m_mysql ) << std::endl;
            std::cerr << "Query was:\n" << query.str() << std::endl;
#ifndef HAVE_SSTREAM
            query.freeze( false );
#endif
            return;
        }
#ifndef HAVE_SSTREAM
        query.freeze( false );
#endif

        MYSQL_RES* results = mysql_store_result( m_mysql );
        if( results == NULL )
        {
            std::cerr << "Error: Could not get select game results from database: "
                      << mysql_error( m_mysql ) << std::endl;
        }
        else
        {
            my_ulonglong rows = mysql_num_rows( results );
            if( rows > 1 )
            {
                std::cerr << "Error: Mutiple rounds returned for the game\n";
            }
            else if( rows == 0 )
            {
                std::cerr << "Could not find game that was just inserted\n";                }
            else
            {
                MYSQL_ROW row = mysql_fetch_row( results );
                if( row == NULL )
                {
                    std::cerr << "Error: could not get row from game query:"
                              << mysql_error( m_mysql ) << std::endl;
                }
                else
                {
                    std::cout << "Game ID = " << row[ 0 ] << std::endl;
                }
            }
        }
    }

    void
    setHost( const std::string& host )
    { m_host = host; }

    const std::string&
    getHost()
    { return m_host; }


    MYSQL* m_mysql;
    tm m_time;
    std::string m_team_name[ 2 ];
    std::string m_coach_name[ 2 ];
    unsigned int m_score[ 2 ];
    unsigned int m_pen_taken[ 2 ];
    unsigned int m_pen_scored[ 2 ];
    bool m_left_coin;
    bool m_right_coin;

    
    bool m_save;
    std::string m_host;
    std::string m_user;
    std::string m_pass;
    std::string m_db;
    int m_port;
    std::string m_unix_socket;
    bool m_compress;
    bool m_ssl;

    int m_round_id;
    std::string m_round_name;
    int m_event_id;
    std::string m_event_name;

public:
    static
    void
    destroy( MySQLSaver* c )
    { delete c; }

    static
    Ptr
    create( int argc, const char** argv, const std::string& module_name )
    {
        return Ptr( new MySQLSaver( argc,
                                    argv,
                                    module_name ),
                    &destroy,
                    rcss::lib::Loader::loadFromCache( "libmysqlsaver" ) ); 
    }
};


RCSSLIB_INIT( libmysqlsaver )
{ 
    MySQLSaver::factory().reg( &MySQLSaver::create, "MySQLSaver" );
    return true;
}

RCSSLIB_FIN( libmysqlsaver )
{
    MySQLSaver::factory().dereg( "MySQLSaver" );
}  



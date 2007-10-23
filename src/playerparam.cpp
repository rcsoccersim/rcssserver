/* -*- Mode: C++ -*- */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "playerparam.h"

#include <rcssbase/conf/parser.hpp>
#include <rcssbase/conf/builder.hpp>

#include <boost/filesystem/path.hpp>
#include <string>
#include <iostream>
#include <cerrno>


#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h> /* needed for htonl, htons, ... */
#endif
#ifdef HAVE_WINSOCK2_H
#include <Winsock2.h> /* needed for htonl, htons, ... */
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

inline
Int32
roundint( const double & value )
{
    return static_cast< Int32 >( value + 0.5 );
}

#if defined(_WIN32) || defined(__WIN32__) || defined (WIN32) || defined (__CYGWIN__)
#  ifndef WIN32
#    define WIN32
#  endif
#endif

#ifdef WIN32
const char PlayerParam::OLD_PLAYER_CONF[] = "~\\.rcssserver-player.conf";
const char PlayerParam::PLAYER_CONF[] = "~\\.rcssserver\\player.conf";
#else
const char PlayerParam::OLD_PLAYER_CONF[] = "~/.rcssserver-player.conf";
const char PlayerParam::PLAYER_CONF[] = "~/.rcssserver/player12.conf";
#endif

const int PlayerParam::DEFAULT_PLAYER_TYPES = 14; // [12.0.0] 7 -> 14
const int PlayerParam::DEFAULT_SUBS_MAX = 3;
const int PlayerParam::DEFAULT_PT_MAX = 1; // [12.0.0] 1 -> 3

const double PlayerParam::DEFAULT_PLAYER_SPEED_MAX_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_PLAYER_SPEED_MAX_DELTA_MAX = 0.0;
const double PlayerParam::DEFAULT_STAMINA_INC_MAX_DELTA_FACTOR = 0.0;

const double PlayerParam::DEFAULT_PLAYER_DECAY_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_PLAYER_DECAY_DELTA_MAX = 0.2;
const double PlayerParam::DEFAULT_INERTIA_MOMENT_DELTA_FACTOR = 25.0;

const double PlayerParam::DEFAULT_DASH_POWER_RATE_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_DASH_POWER_RATE_DELTA_MAX = 0.0;
const double PlayerParam::DEFAULT_PLAYER_SIZE_DELTA_FACTOR = -100.0;

const double PlayerParam::DEFAULT_KICKABLE_MARGIN_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_KICKABLE_MARGIN_DELTA_MAX = 0.2;
const double PlayerParam::DEFAULT_KICK_RAND_DELTA_FACTOR = 0.5;

const double PlayerParam::DEFAULT_EXTRA_STAMINA_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_EXTRA_STAMINA_DELTA_MAX = 100.0;
const double PlayerParam::DEFAULT_EFFORT_MAX_DELTA_FACTOR = -0.002;
const double PlayerParam::DEFAULT_EFFORT_MIN_DELTA_FACTOR = -0.002;

const int    PlayerParam::DEFAULT_RANDOM_SEED = -1; //negative means generate a new seed

const double PlayerParam::DEFAULT_NEW_DASH_POWER_RATE_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_NEW_DASH_POWER_RATE_DELTA_MAX = 0.002;
const double PlayerParam::DEFAULT_NEW_STAMINA_INC_MAX_DELTA_FACTOR = -10000.0;




PlayerParam &
PlayerParam::instance( rcss::conf::Builder* parent )
{
    static bool parent_set = false;
    if( parent != NULL || parent_set )
    {
        static PlayerParam rval( parent );
        parent_set = true;
        return rval;
    }
    // hack to allow link testing to call instance without crashing
    // do not used the return value in these situations
    PlayerParam* rval = NULL;
    return *rval;
}

PlayerParam &
PlayerParam::instance()
{
    return PlayerParam::instance( NULL );
}

bool
PlayerParam::init( rcss::conf::Builder* parent )
{
    instance( parent );
#ifndef WIN32
    if( system( ( "ls " + tildeExpand( PlayerParam::OLD_PLAYER_CONF ) + " > /dev/null 2>&1" ).c_str() ) == 0 )
    {
        if( system( ( "ls " + tildeExpand( PlayerParam::PLAYER_CONF ) + " > /dev/null 2>&1" ).c_str() ) != 0 )
        {
            if( system( "which awk > /dev/null 2>&1" ) == 0 )
            {
                std::cout << "Trying to convert old configuration file '"
                          << PlayerParam::OLD_PLAYER_CONF
                          << "'\n";

                char filename[] = "/tmp/rcssplayer-oldconf-XXXXXX";
                int fd = mkstemp( filename );
                if( fd != -1 )
                {
                    close( fd );
                    std::string command = "awk '/^[ \\t]*$/ {} ";
                    command += "/^[^#]+[:]/ { gsub(/:/, \"=\" ); $1 = \"player::\" $1; } ";
                    command += "/^[ \\t]*[^#:=]+$/ { $1 = \"player::\" $1 \" = true\"; }";
                    command += "{ print; }' ";
                    command +=  tildeExpand( PlayerParam::OLD_PLAYER_CONF );
                    command += " > ";
                    command += filename;
                    if( system( command.c_str() ) == 0 )
                    {
                        std::cout << "Conversion successful\n";
                        instance().m_builder->parser()->parse( filename );
                    }
                    else
                    {
                        std::cout << "Conversion failed\n";
                    }
                }
                else
                {
                    std::cout << "Conversion failed\n";
                }
            }
        }
    }
#endif // not win32
    if( ! instance().m_builder->parser() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": internal error: player param could not find configuration parser\n";
        std::cerr << "Please submit a full bug report to sserver-bugs@lists.sf.net\n";
        return false;
    }

    boost::filesystem::path conf_path( tildeExpand( PlayerParam::PLAYER_CONF ),
                                       boost::filesystem::portable_posix_name );
    if( !instance().m_builder->parser()->parseCreateConf( conf_path,
                                                          "player" ) )
    {
        std::cerr << "could not parse configuration file '"
                  << PlayerParam::PLAYER_CONF
                  << "'\n";
        return false;
    }

    return true;
}

PlayerParam::PlayerParam( rcss::conf::Builder* parent )
    : m_builder( new rcss::conf::Builder( parent, "player" ) )
{
    setDefaults();
    addParams();
}

PlayerParam::~PlayerParam()
{
}

void
PlayerParam::addParams()
{
    addParam( "player_types", player_types, "", 7 );
    addParam( "subs_max", subs_max, "", 7 );
    addParam( "pt_max", pt_max, "", 7 );
    addParam( "player_speed_max_delta_min", player_speed_max_delta_min, "", 7 );
    addParam( "player_speed_max_delta_max", player_speed_max_delta_max, "", 7 );
    addParam( "stamina_inc_max_delta_factor", stamina_inc_max_delta_factor, "", 7 );
    addParam( "player_decay_delta_min", player_decay_delta_min, "", 7 );
    addParam( "player_decay_delta_max", player_decay_delta_max, "", 7 );
    addParam( "inertia_moment_delta_factor", inertia_moment_delta_factor, "", 7 );
    addParam( "dash_power_rate_delta_min", dash_power_rate_delta_min, "", 7 );
    addParam( "dash_power_rate_delta_max", dash_power_rate_delta_max, "", 7 );
    addParam( "player_size_delta_factor", player_size_delta_factor, "", 7 );
    addParam( "kickable_margin_delta_min", kickable_margin_delta_min, "", 7 );
    addParam( "kickable_margin_delta_max", kickable_margin_delta_max, "", 7 );
    addParam( "kick_rand_delta_factor", kick_rand_delta_factor, "", 7 );
    addParam( "extra_stamina_delta_min", extra_stamina_delta_min, "", 7 );
    addParam( "extra_stamina_delta_max", extra_stamina_delta_max, "", 7 );
    addParam( "effort_max_delta_factor", effort_max_delta_factor, "", 7 );
    addParam( "effort_min_delta_factor", effort_min_delta_factor, "", 7 );
    addParam( "random_seed", random_seed, "", 8 );
    addParam( "new_dash_power_rate_delta_min", new_dash_power_rate_delta_min, "", 8 );
    addParam( "new_dash_power_rate_delta_max", new_dash_power_rate_delta_max, "", 8 );
    addParam( "new_stamina_inc_max_delta_factor", new_stamina_inc_max_delta_factor, "", 8 );
}

template< typename P >
void
PlayerParam::addParam(  const std::string& name,
                        P& param,
                        const std::string& desc,
                        int version )
{
    m_builder->addParam( name, param, desc );
    m_ver_map[ name ] = version;
}

template< typename S, typename G >
void
PlayerParam::addParam(  const std::string& name,
                        const S& setter,
                        const G& getter,
                        const std::string& desc,
                        int version )
{
    m_builder->addParam( name, setter, getter, desc );
    m_ver_map[ name ] = version;
}


void
PlayerParam::setDefaults()
{
    player_types = PlayerParam::DEFAULT_PLAYER_TYPES;
    subs_max = PlayerParam::DEFAULT_SUBS_MAX;
    pt_max = PlayerParam::DEFAULT_PT_MAX;

    player_speed_max_delta_min = PlayerParam::DEFAULT_PLAYER_SPEED_MAX_DELTA_MIN;
    player_speed_max_delta_max = PlayerParam::DEFAULT_PLAYER_SPEED_MAX_DELTA_MAX;
    stamina_inc_max_delta_factor = PlayerParam::DEFAULT_STAMINA_INC_MAX_DELTA_FACTOR;

    player_decay_delta_min = PlayerParam::DEFAULT_PLAYER_DECAY_DELTA_MIN;
    player_decay_delta_max = PlayerParam::DEFAULT_PLAYER_DECAY_DELTA_MAX;
    inertia_moment_delta_factor = PlayerParam::DEFAULT_INERTIA_MOMENT_DELTA_FACTOR;

    dash_power_rate_delta_min = PlayerParam::DEFAULT_DASH_POWER_RATE_DELTA_MIN;
    dash_power_rate_delta_max = PlayerParam::DEFAULT_DASH_POWER_RATE_DELTA_MAX;
    player_size_delta_factor = PlayerParam::DEFAULT_PLAYER_SIZE_DELTA_FACTOR;

    kickable_margin_delta_min = PlayerParam::DEFAULT_KICKABLE_MARGIN_DELTA_MIN;
    kickable_margin_delta_max = PlayerParam::DEFAULT_KICKABLE_MARGIN_DELTA_MAX;
    kick_rand_delta_factor = PlayerParam::DEFAULT_KICK_RAND_DELTA_FACTOR;

    extra_stamina_delta_min = PlayerParam::DEFAULT_EXTRA_STAMINA_DELTA_MIN;
    extra_stamina_delta_max = PlayerParam::DEFAULT_EXTRA_STAMINA_DELTA_MAX;
    effort_max_delta_factor = PlayerParam::DEFAULT_EFFORT_MAX_DELTA_FACTOR;
    effort_min_delta_factor = PlayerParam::DEFAULT_EFFORT_MIN_DELTA_FACTOR;

    random_seed = PlayerParam::DEFAULT_RANDOM_SEED;

    new_dash_power_rate_delta_min = PlayerParam::DEFAULT_NEW_DASH_POWER_RATE_DELTA_MIN;
    new_dash_power_rate_delta_max = PlayerParam::DEFAULT_NEW_DASH_POWER_RATE_DELTA_MAX;
    new_stamina_inc_max_delta_factor = PlayerParam::DEFAULT_NEW_STAMINA_INC_MAX_DELTA_FACTOR;
}

player_params_t PlayerParam::convertToStruct()
{
    player_params_t tmp;

    tmp.player_types = htons( static_cast< Int16 >( player_types ) );
    tmp.subs_max = htons( static_cast< Int16 >( subs_max ) );
    tmp.pt_max = htons( static_cast< Int16 >( pt_max ) );

    tmp.player_speed_max_delta_min = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * player_speed_max_delta_min ) ) ) );
    tmp.player_speed_max_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * player_speed_max_delta_max ) ) ) );
    tmp.stamina_inc_max_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * stamina_inc_max_delta_factor ) ) ) );

    tmp.player_decay_delta_min = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * player_decay_delta_min) ) ) );
    tmp.player_decay_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * player_decay_delta_max) ) ) );
    tmp.inertia_moment_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * inertia_moment_delta_factor ) ) ) );

    tmp.dash_power_rate_delta_min = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * dash_power_rate_delta_min ) ) ) );
    tmp.dash_power_rate_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * dash_power_rate_delta_max ) ) ) );
    tmp.player_size_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * player_size_delta_factor ) ) ) );

    tmp.kickable_margin_delta_min = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * kickable_margin_delta_min ) ) ) );
    tmp.kickable_margin_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * kickable_margin_delta_max ) ) ) );
    tmp.kick_rand_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * kick_rand_delta_factor ) ) ) );

    tmp.extra_stamina_delta_min = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * extra_stamina_delta_min ) ) ) );
    tmp.extra_stamina_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * extra_stamina_delta_max ) ) ) );
    tmp.effort_max_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * effort_max_delta_factor ) ) ) );
    tmp.effort_min_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * effort_min_delta_factor ) ) ) );
    tmp.random_seed = htonl( static_cast< Int32 >( random_seed ) );

    tmp.new_dash_power_rate_delta_min = htonl( static_cast< Int32 >( roundint(( SHOWINFO_SCALE2 * new_dash_power_rate_delta_min ) ) ) );
    tmp.new_dash_power_rate_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * new_dash_power_rate_delta_max ) ) ) );
    tmp.new_stamina_inc_max_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * new_stamina_inc_max_delta_factor ) ) ) );

    return tmp;
}

bool
PlayerParam::getInt( const std::string & param,
                     int & value ) const
{
    return m_builder->get( param, value );
}

bool
PlayerParam::getBool( const std::string & param,
                      bool & value ) const
{
    return m_builder->get( param, value );
}

bool
PlayerParam::getDoub( const std::string & param,
                      double & value ) const
{
    return m_builder->get( param, value );
}

bool
PlayerParam::getStr( const std::string & param,
                     std::string & value ) const
{
    return m_builder->get( param, value );
}

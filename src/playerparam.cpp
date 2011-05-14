/* -*- Mode: C++ -*- */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler
 Copyright (C) 2002- RoboCup Soccer Simulator Maintainance Group.

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

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
#include <config.h>
#endif

#include "playerparam.h"

#include "utility.h"

#include <rcssbase/conf/parser.hpp>
#include <rcssbase/conf/builder.hpp>

#include <boost/filesystem/path.hpp>

#include <string>
#include <iostream>
#include <cerrno>
#include <cassert>


#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h> /* needed for htonl, htons, ... */
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef __CYGWIN__
// cygwin is not win32
#elif defined(_WIN32) || defined(__WIN32__) || defined (WIN32)
#define RCSS_WIN
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h> /* needed for htonl, htons, ... */
#endif
#endif

namespace {

inline
Int32
roundint( const double & value )
{
    return static_cast< Int32 >( value + 0.5 );
}

}

#if defined(RCSS_WIN) || defined(__CYGWIN__)
const std::string PlayerParam::CONF_DIR = "."; //"~\\.rcssserver\\";
const std::string PlayerParam::PLAYER_CONF = "player.conf";
const std::string PlayerParam::OLD_PLAYER_CONF = "rcssserver-player.conf";
#else
const std::string PlayerParam::CONF_DIR = "~/.rcssserver/";
const std::string PlayerParam::PLAYER_CONF = "player.conf";
const std::string PlayerParam::OLD_PLAYER_CONF = "~/.rcssserver-player.conf";
#endif

const int PlayerParam::DEFAULT_PLAYER_TYPES = 18; // [12.0.0] 7 -> 18
const int PlayerParam::DEFAULT_SUBS_MAX = 3;
const int PlayerParam::DEFAULT_PT_MAX = 1; // [12.0.0] 3 -> 1

const double PlayerParam::DEFAULT_PLAYER_SPEED_MAX_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_PLAYER_SPEED_MAX_DELTA_MAX = 0.0;
const double PlayerParam::DEFAULT_STAMINA_INC_MAX_DELTA_FACTOR = 0.0;

// [13.0.0] -0.05 -> -0.1
// [12.0.0]  0.0  -> -0.05
const double PlayerParam::DEFAULT_PLAYER_DECAY_DELTA_MIN = -0.1;
// [12.0.0] 0.2 -> 0.1
const double PlayerParam::DEFAULT_PLAYER_DECAY_DELTA_MAX = 0.1;
const double PlayerParam::DEFAULT_INERTIA_MOMENT_DELTA_FACTOR = 25.0;

const double PlayerParam::DEFAULT_DASH_POWER_RATE_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_DASH_POWER_RATE_DELTA_MAX = 0.0;
const double PlayerParam::DEFAULT_PLAYER_SIZE_DELTA_FACTOR = -100.0;

const double PlayerParam::DEFAULT_KICKABLE_MARGIN_DELTA_MIN = -0.1; // [12.0.0] 0.0 -> -0.1
const double PlayerParam::DEFAULT_KICKABLE_MARGIN_DELTA_MAX = 0.1; // [12.0.0] 0.2 -> 0.1
const double PlayerParam::DEFAULT_KICK_RAND_DELTA_FACTOR = 1.0; // [12.0.0] 0.5 -> 1.0

const double PlayerParam::DEFAULT_EXTRA_STAMINA_DELTA_MIN = 0.0;
// [13.0.0] 100.0 -> 50.0
const double PlayerParam::DEFAULT_EXTRA_STAMINA_DELTA_MAX = 50.0;
// [13.0.0] -0.002 -> -0.004
const double PlayerParam::DEFAULT_EFFORT_MAX_DELTA_FACTOR = -0.004;
// [13.0.0] -0.002 -> -0.004
const double PlayerParam::DEFAULT_EFFORT_MIN_DELTA_FACTOR = -0.004;

const int    PlayerParam::DEFAULT_RANDOM_SEED = -1; //negative means generate a new seed

// [13.0.0] -0.0005 -> -0.0012
// [12.0.0]  0      -> -0.0005
const double PlayerParam::DEFAULT_NEW_DASH_POWER_RATE_DELTA_MIN = -0.0012;
// [13.0.0] 0.0015 -> 0.0008
// [12.0.0] 0.002  -> 0.0015
const double PlayerParam::DEFAULT_NEW_DASH_POWER_RATE_DELTA_MAX = 0.0008;
// [12.0.0] -10000.0 -> -6000.0
const double PlayerParam::DEFAULT_NEW_STAMINA_INC_MAX_DELTA_FACTOR = -6000.0;

// v14
const double PlayerParam::DEFAULT_KICK_POWER_RATE_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_KICK_POWER_RATE_DELTA_MAX = 0.0;
const double PlayerParam::DEFAULT_FOUL_DETECT_PROBABILITY_DELTA_FACTOR = 0.0;

const double PlayerParam::DEFAULT_CATCHABLE_AREA_L_STRETCH_MIN = 1.0;
const double PlayerParam::DEFAULT_CATCHABLE_AREA_L_STRETCH_MAX = 1.3;


PlayerParam &
PlayerParam::instance( rcss::conf::Builder * parent )
{
    static bool parent_set = false;
    if ( parent != NULL || parent_set )
    {
        static PlayerParam rval( parent );
        parent_set = true;
        return rval;
    }
    // hack to allow link testing to call instance without crashing
    // do not used the return value in these situations
    PlayerParam * rval = NULL;
    return *rval;
}

PlayerParam &
PlayerParam::instance()
{
    return PlayerParam::instance( NULL );
}

bool
PlayerParam::init( rcss::conf::Builder * parent )
{
    assert( parent );
    instance( parent );

    if ( ! instance().M_builder->parser() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": internal error: player param could not find configuration parser\n";
        std::cerr << "Please submit a full bug report to sserver-bugs@lists.sf.net\n";
        return false;
    }

    std::string conf_dir = PlayerParam::CONF_DIR;
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
        conf_path /= PlayerParam::PLAYER_CONF;
    }
    catch ( std::exception & e )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " Exception caught! " << e.what()
                  << "\nCould not read config directory '"
                  << tildeExpand( conf_dir ) << "'"
                  << std::endl;
        return false;
    }

    instance().convertOldConf( conf_path.BOOST_FS_FILE_STRING() );

    if ( ! instance().M_builder->parser()->parseCreateConf( conf_path, "player" ) )
    {
        std::cerr << "could not parse configuration file '"
                  << conf_path.BOOST_FS_FILE_STRING()
                  << "'\n";
        return false;
    }

    if ( instance().M_builder->version() != instance().M_builder->parsedVersion() )
    {
        std::cerr << "Version mismatched in the configuration file. "
                  << "Need to regenerate '" << conf_path.BOOST_FS_FILE_STRING() << "'"
                  << " or set '" << instance().M_builder->version() << "' to the 'version' option."
                  << std::endl;
//         std::cerr << "registered version = ["
//                   << instance().m_builder->version() << "]\n"
//                   << "parsed version = ["
//                   << instance().m_builder->parsedVersion() << "]\n"
//                   << std::flush;
        return false;
    }

    return true;
}


void
PlayerParam::convertOldConf( const std::string & new_conf )
{
#ifndef RCSS_WIN
    if ( std::system( ( "ls " + tildeExpand( PlayerParam::OLD_PLAYER_CONF ) + " > /dev/null 2>&1" ).c_str() ) == 0
         && std::system( ( "ls " + tildeExpand( new_conf ) + " > /dev/null 2>&1" ).c_str() ) != 0
         && std::system( "which awk > /dev/null 2>&1" ) == 0 )
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
                instance().M_builder->parser()->parse( filename );
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
#endif // not win32
}


PlayerParam::PlayerParam( rcss::conf::Builder * parent )
    : M_builder( new rcss::conf::Builder( parent, VERSION, "player" ) )
{
    setDefaults();
    addParams();
}

PlayerParam::~PlayerParam()
{

}

template< typename P >
void
PlayerParam::addParam( const std::string & name,
                       P & param,
                       const std::string & desc,
                       int version )
{
    M_builder->addParam( name, param, desc );
    M_ver_map[ name ] = version;
}

template< typename S, typename G >
void
PlayerParam::addParam( const std::string& name,
                       const S& setter,
                       const G& getter,
                       const std::string& desc,
                       int version )
{
    M_builder->addParam( name, setter, getter, desc );
    M_ver_map[ name ] = version;
}

void
PlayerParam::addParams()
{
    addParam( "player_types", M_player_types, "", 7 );
    addParam( "subs_max", M_subs_max, "", 7 );
    addParam( "pt_max", M_pt_max, "", 7 );
    addParam( "player_speed_max_delta_min", M_player_speed_max_delta_min, "", 7 );
    addParam( "player_speed_max_delta_max", M_player_speed_max_delta_max, "", 7 );
    addParam( "stamina_inc_max_delta_factor", M_stamina_inc_max_delta_factor, "", 7 );
    addParam( "player_decay_delta_min", M_player_decay_delta_min, "", 7 );
    addParam( "player_decay_delta_max", M_player_decay_delta_max, "", 7 );
    addParam( "inertia_moment_delta_factor", M_inertia_moment_delta_factor, "", 7 );
    addParam( "dash_power_rate_delta_min", M_dash_power_rate_delta_min, "", 7 );
    addParam( "dash_power_rate_delta_max", M_dash_power_rate_delta_max, "", 7 );
    addParam( "player_size_delta_factor", M_player_size_delta_factor, "", 7 );
    addParam( "kickable_margin_delta_min", M_kickable_margin_delta_min, "", 7 );
    addParam( "kickable_margin_delta_max", M_kickable_margin_delta_max, "", 7 );
    addParam( "kick_rand_delta_factor", M_kick_rand_delta_factor, "", 7 );
    addParam( "extra_stamina_delta_min", M_extra_stamina_delta_min, "", 7 );
    addParam( "extra_stamina_delta_max", M_extra_stamina_delta_max, "", 7 );
    addParam( "effort_max_delta_factor", M_effort_max_delta_factor, "", 7 );
    addParam( "effort_min_delta_factor", M_effort_min_delta_factor, "", 7 );
    addParam( "random_seed", M_random_seed, "", 8 );
    addParam( "new_dash_power_rate_delta_min", M_new_dash_power_rate_delta_min, "", 8 );
    addParam( "new_dash_power_rate_delta_max", M_new_dash_power_rate_delta_max, "", 8 );
    addParam( "new_stamina_inc_max_delta_factor", M_new_stamina_inc_max_delta_factor, "", 8 );
    addParam( "allow_mult_default_type", M_allow_mult_default_type, "", 12 );
    addParam( "kick_power_rate_delta_min", M_kick_power_rate_delta_min, "", 14 );
    addParam( "kick_power_rate_delta_max", M_kick_power_rate_delta_max, "", 14 );
    addParam( "foul_detect_probability_delta_factor", M_foul_detect_probability_delta_factor, "", 14 );
    //addParam( "allow_default_goalie", M_allow_default_goalie, "", 14 );
    addParam( "catchable_area_l_stretch_min", M_catchable_area_l_stretch_min, "", 14 );
    addParam( "catchable_area_l_stretch_max", M_catchable_area_l_stretch_max, "", 14 );
}

void
PlayerParam::setDefaults()
{
    M_player_types = PlayerParam::DEFAULT_PLAYER_TYPES;
    M_subs_max = PlayerParam::DEFAULT_SUBS_MAX;
    M_pt_max = PlayerParam::DEFAULT_PT_MAX;

    M_allow_mult_default_type = false;

    M_player_speed_max_delta_min = PlayerParam::DEFAULT_PLAYER_SPEED_MAX_DELTA_MIN;
    M_player_speed_max_delta_max = PlayerParam::DEFAULT_PLAYER_SPEED_MAX_DELTA_MAX;
    M_stamina_inc_max_delta_factor = PlayerParam::DEFAULT_STAMINA_INC_MAX_DELTA_FACTOR;

    M_player_decay_delta_min = PlayerParam::DEFAULT_PLAYER_DECAY_DELTA_MIN;
    M_player_decay_delta_max = PlayerParam::DEFAULT_PLAYER_DECAY_DELTA_MAX;
    M_inertia_moment_delta_factor = PlayerParam::DEFAULT_INERTIA_MOMENT_DELTA_FACTOR;

    M_dash_power_rate_delta_min = PlayerParam::DEFAULT_DASH_POWER_RATE_DELTA_MIN;
    M_dash_power_rate_delta_max = PlayerParam::DEFAULT_DASH_POWER_RATE_DELTA_MAX;
    M_player_size_delta_factor = PlayerParam::DEFAULT_PLAYER_SIZE_DELTA_FACTOR;

    M_kickable_margin_delta_min = PlayerParam::DEFAULT_KICKABLE_MARGIN_DELTA_MIN;
    M_kickable_margin_delta_max = PlayerParam::DEFAULT_KICKABLE_MARGIN_DELTA_MAX;
    M_kick_rand_delta_factor = PlayerParam::DEFAULT_KICK_RAND_DELTA_FACTOR;

    M_extra_stamina_delta_min = PlayerParam::DEFAULT_EXTRA_STAMINA_DELTA_MIN;
    M_extra_stamina_delta_max = PlayerParam::DEFAULT_EXTRA_STAMINA_DELTA_MAX;
    M_effort_max_delta_factor = PlayerParam::DEFAULT_EFFORT_MAX_DELTA_FACTOR;
    M_effort_min_delta_factor = PlayerParam::DEFAULT_EFFORT_MIN_DELTA_FACTOR;

    M_random_seed = PlayerParam::DEFAULT_RANDOM_SEED;

    M_new_dash_power_rate_delta_min = PlayerParam::DEFAULT_NEW_DASH_POWER_RATE_DELTA_MIN;
    M_new_dash_power_rate_delta_max = PlayerParam::DEFAULT_NEW_DASH_POWER_RATE_DELTA_MAX;
    M_new_stamina_inc_max_delta_factor = PlayerParam::DEFAULT_NEW_STAMINA_INC_MAX_DELTA_FACTOR;

    M_kick_power_rate_delta_min = PlayerParam::DEFAULT_KICK_POWER_RATE_DELTA_MIN;
    M_kick_power_rate_delta_max = PlayerParam::DEFAULT_KICK_POWER_RATE_DELTA_MAX;
    M_foul_detect_probability_delta_factor = PlayerParam::DEFAULT_FOUL_DETECT_PROBABILITY_DELTA_FACTOR;


    //M_allow_default_goalie = true;
    M_catchable_area_l_stretch_min = PlayerParam::DEFAULT_CATCHABLE_AREA_L_STRETCH_MIN;
    M_catchable_area_l_stretch_max = PlayerParam::DEFAULT_CATCHABLE_AREA_L_STRETCH_MAX;
}

player_params_t
PlayerParam::convertToStruct() const
{
    player_params_t tmp;

    tmp.player_types = htons( static_cast< Int16 >( playerTypes() ) );
    tmp.subs_max = htons( static_cast< Int16 >( subsMax() ) );
    tmp.pt_max = htons( static_cast< Int16 >( ptMax() ) );

    tmp.player_speed_max_delta_min = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * playerSpeedMaxDeltaMin() ) ) ) );
    tmp.player_speed_max_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * playerSpeedMaxDeltaMax() ) ) ) );
    tmp.stamina_inc_max_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * staminaIncMaxDeltaFactor() ) ) ) );

    tmp.player_decay_delta_min = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * playerDecayDeltaMin() ) ) ) );
    tmp.player_decay_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * playerDecayDeltaMax() ) ) ) );
    tmp.inertia_moment_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * inertiaMomentDeltaFactor() ) ) ) );

    tmp.dash_power_rate_delta_min = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * dashPowerRateDeltaMin() ) ) ) );
    tmp.dash_power_rate_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * dashPowerRateDeltaMax() ) ) ) );
    tmp.player_size_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * playerSizeDeltaFactor() ) ) ) );

    tmp.kickable_margin_delta_min = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * kickableMarginDeltaMin() ) ) ) );
    tmp.kickable_margin_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * kickableMarginDeltaMax() ) ) ) );
    tmp.kick_rand_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * kickRandDeltaFactor() ) ) ) );

    tmp.extra_stamina_delta_min = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * extraStaminaDeltaMin() ) ) ) );
    tmp.extra_stamina_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * extraStaminaDeltaMax() ) ) ) );
    tmp.effort_max_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * effortMaxDeltaFactor() ) ) ) );
    tmp.effort_min_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * effortMinDeltaFactor() ) ) ) );
    tmp.random_seed = htonl( static_cast< Int32 >( randomSeed() ) );

    tmp.new_dash_power_rate_delta_min = htonl( static_cast< Int32 >( roundint(( SHOWINFO_SCALE2 * newDashPowerRateDeltaMin() ) ) ) );
    tmp.new_dash_power_rate_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * newDashPowerRateDeltaMax() ) ) ) );
    tmp.new_stamina_inc_max_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * newStaminaIncMaxDeltaFactor() ) ) ) );

    tmp.kick_power_rate_delta_min = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * kickPowerRateDeltaMin() ) ) ) );
    tmp.kick_power_rate_delta_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * kickPowerRateDeltaMax() ) ) ) );
    tmp.foul_detect_probability_delta_factor = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * foulDetectProbabilityDeltaFactor() ) ) ) );

    tmp.catchable_area_l_stretch_min = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * catchAreaLengthStretchMin() ) ) ) );
    tmp.catchable_area_l_stretch_max = htonl( static_cast< Int32 >( roundint( ( SHOWINFO_SCALE2 * catchAreaLengthStretchMax() ) ) ) );

    tmp.allow_mult_default_type = htons( static_cast< Int16 >( allowMultDefaultType() ) );
    //tmp.allow_default_goalie = htons( static_cast< Int16 >( allowDefaultGoalie() ) );

    return tmp;
}

bool
PlayerParam::getInt( const std::string & param,
                     int & value ) const
{
    return M_builder->get( param, value );
}

bool
PlayerParam::getBool( const std::string & param,
                      bool & value ) const
{
    return M_builder->get( param, value );
}

bool
PlayerParam::getDouble( const std::string & param,
                        double & value ) const
{
    return M_builder->get( param, value );
}

bool
PlayerParam::getStr( const std::string & param,
                     std::string & value ) const
{
    return M_builder->get( param, value );
}

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

#ifndef RCSSSERVR_PLAYER_PARAM_H
#define RCSSSERVR_PLAYER_PARAM_H

#include "types.h"

#include <rcssbase/conf/builder.hpp>

#include <boost/shared_ptr.hpp>

#include <string>
#include <map>

class PlayerParam {
public:
    typedef std::map< std::string, unsigned int > VerMap;

private:

    PlayerParam(); // not used
    PlayerParam( const PlayerParam & ); // not used
    PlayerParam & operator=( PlayerParam & ); // not used

protected:

    explicit
    PlayerParam( rcss::conf::Builder * parent );

public:

    virtual
    ~PlayerParam();

    static
    PlayerParam & instance();

    static
    PlayerParam & instance( rcss::conf::Builder * parent );

    static
    bool init( rcss::conf::Builder * parent );

protected:

    void addParams();

private:
    boost::shared_ptr< rcss::conf::Builder > M_builder;
    VerMap M_ver_map;

public:

    const VerMap & verMap() const
      {
          return M_ver_map;
      }

    void setRandomSeed( const int seed )
      {
          M_random_seed = seed;
      }

protected:

    void convertOldConf( const std::string & new_conf );

    void setDefaults();

    template< typename P >
    void addParam( const std::string & name,
                   P & param,
                   const std::string & desc,
                   int version );

    template< typename S, typename G >
    void addParam( const std::string & name,
                   const S & setter,
                   const G & getter,
                   const std::string & desc,
                   int version );

public:

    player_params_t convertToStruct() const;


    int playerTypes() const { return M_player_types; }
    int subsMax() const { return M_subs_max; }
    int ptMax() const { return M_pt_max; }

    bool allowMultDefaultType() const { return M_allow_mult_default_type; }

    double playerSpeedMaxDeltaMin() const { return M_player_speed_max_delta_min; }

    double playerSpeedMaxDeltaMax() const { return M_player_speed_max_delta_max; }
    double staminaIncMaxDeltaFactor() const { return M_stamina_inc_max_delta_factor; }

    double playerDecayDeltaMin() const { return M_player_decay_delta_min; }
    double playerDecayDeltaMax() const { return M_player_decay_delta_max; }
    double inertiaMomentDeltaFactor() const { return M_inertia_moment_delta_factor; }

    double dashPowerRateDeltaMin() const { return M_dash_power_rate_delta_min; }
    double dashPowerRateDeltaMax() const { return M_dash_power_rate_delta_max; }
    double playerSizeDeltaFactor() const { return M_player_size_delta_factor; }

    double kickableMarginDeltaMin() const { return M_kickable_margin_delta_min; }
    double kickableMarginDeltaMax() const { return M_kickable_margin_delta_max; }
    double kickRandDeltaFactor() const { return M_kick_rand_delta_factor; }

    double extraStaminaDeltaMin() const { return M_extra_stamina_delta_min; }
    double extraStaminaDeltaMax() const { return M_extra_stamina_delta_max; }
    double effortMaxDeltaFactor() const { return M_effort_max_delta_factor; }
    double effortMinDeltaFactor() const { return M_effort_min_delta_factor; }

    int randomSeed() const { return M_random_seed; }

    double newDashPowerRateDeltaMin() const { return M_new_dash_power_rate_delta_min; }
    double newDashPowerRateDeltaMax() const { return M_new_dash_power_rate_delta_max; }
    double newStaminaIncMaxDeltaFactor() const { return M_new_stamina_inc_max_delta_factor; }

    double kickPowerRateDeltaMin() const { return M_kick_power_rate_delta_min; }
    double kickPowerRateDeltaMax() const { return M_kick_power_rate_delta_max; }
    double foulDetectProbabilityDeltaFactor() const { return M_foul_detect_probability_delta_factor; }

    //bool allowDefaultGoalie() const { return M_allow_default_goalie; }
    double catchAreaLengthStretchMin() const { return M_catchable_area_l_stretch_min; }
    double catchAreaLengthStretchMax() const { return M_catchable_area_l_stretch_max; }

    bool getInt( const std::string & param,
                 int & value ) const;

    bool getBool( const std::string & param,
                  bool & value ) const;

    bool getDouble( const std::string & param,
                    double & value ) const;

    bool getStr( const std::string & param,
                 std::string & value ) const;


private:
    static const std::string CONF_DIR;
    static const std::string PLAYER_CONF;
    static const std::string OLD_PLAYER_CONF;

    static const int DEFAULT_PLAYER_TYPES;
    static const int DEFAULT_SUBS_MAX;
    static const int DEFAULT_PT_MAX;

    static const double DEFAULT_PLAYER_SPEED_MAX_DELTA_MIN;
    static const double DEFAULT_PLAYER_SPEED_MAX_DELTA_MAX;
    static const double DEFAULT_STAMINA_INC_MAX_DELTA_FACTOR;

    static const double DEFAULT_PLAYER_DECAY_DELTA_MIN;
    static const double DEFAULT_PLAYER_DECAY_DELTA_MAX;
    static const double DEFAULT_INERTIA_MOMENT_DELTA_FACTOR;

    static const double DEFAULT_DASH_POWER_RATE_DELTA_MIN;
    static const double DEFAULT_DASH_POWER_RATE_DELTA_MAX;
    static const double DEFAULT_PLAYER_SIZE_DELTA_FACTOR;

    static const double DEFAULT_KICKABLE_MARGIN_DELTA_MIN;
    static const double DEFAULT_KICKABLE_MARGIN_DELTA_MAX;
    static const double DEFAULT_KICK_RAND_DELTA_FACTOR;

    static const double DEFAULT_EXTRA_STAMINA_DELTA_MIN;
    static const double DEFAULT_EXTRA_STAMINA_DELTA_MAX;
    static const double DEFAULT_EFFORT_MAX_DELTA_FACTOR;
    static const double DEFAULT_EFFORT_MIN_DELTA_FACTOR;

    static const int    DEFAULT_RANDOM_SEED;

    static const double DEFAULT_NEW_DASH_POWER_RATE_DELTA_MIN;
    static const double DEFAULT_NEW_DASH_POWER_RATE_DELTA_MAX;
    static const double DEFAULT_NEW_STAMINA_INC_MAX_DELTA_FACTOR;

    // v14
    static const double DEFAULT_KICK_POWER_RATE_DELTA_MIN;
    static const double DEFAULT_KICK_POWER_RATE_DELTA_MAX;
    static const double DEFAULT_FOUL_DETECT_PROBABILITY_DELTA_FACTOR;

    static const double DEFAULT_CATCHABLE_AREA_L_STRETCH_MIN;
    static const double DEFAULT_CATCHABLE_AREA_L_STRETCH_MAX;

    int M_player_types;
    int M_subs_max;
    int M_pt_max;

    bool M_allow_mult_default_type;

    double M_player_speed_max_delta_min;
    double M_player_speed_max_delta_max;
    double M_stamina_inc_max_delta_factor;

    double M_player_decay_delta_min;
    double M_player_decay_delta_max;
    double M_inertia_moment_delta_factor;

    double M_dash_power_rate_delta_min;
    double M_dash_power_rate_delta_max;
    double M_player_size_delta_factor;

    double M_kickable_margin_delta_min;
    double M_kickable_margin_delta_max;
    double M_kick_rand_delta_factor;

    double M_extra_stamina_delta_min;
    double M_extra_stamina_delta_max;
    double M_effort_max_delta_factor;
    double M_effort_min_delta_factor;

    double M_new_dash_power_rate_delta_min;
    double M_new_dash_power_rate_delta_max;
    double M_new_stamina_inc_max_delta_factor;

    int M_random_seed;

    // v14
    double M_kick_power_rate_delta_min;
    double M_kick_power_rate_delta_max;
    double M_foul_detect_probability_delta_factor;

    //bool M_allow_default_goalie;
    double M_catchable_area_l_stretch_min;
    double M_catchable_area_l_stretch_max;
};

#endif

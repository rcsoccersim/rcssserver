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


#ifndef SERVER_PARAM_H
#define SERVER_PARAM_H

#include "types.h"

#include <rcssbase/net/addr.hpp>

#include <boost/shared_ptr.hpp>
#include <map>

#include <cmath>

namespace rcss {
namespace conf {
class Parser;
class Builder;
class StreamStatusHandler;
}
}

class ServerParam {
public:
    typedef std::map< std::string, unsigned int > VerMap;

private:
    static bool S_in_init;
    static std::string S_program_name;

    boost::shared_ptr< rcss::conf::Builder > M_builder;
    boost::shared_ptr< rcss::conf::Parser > M_conf_parser;
    boost::shared_ptr< rcss::conf::StreamStatusHandler > M_err_handler;

    VerMap M_ver_map;

private:

    ServerParam(); // not used
    ServerParam( const ServerParam & ); // not used
    ServerParam & operator=( const ServerParam & ); // not used

protected:

    explicit
    ServerParam ( const std::string & progname );

public:

    static
    ServerParam & instance();

    static
    bool init( const int & argc,
               const char * const * argv );

    virtual
    ~ServerParam();

    void clear();

private:

    void convertOldConf( const std::string & new_conf );

    void setDefaults();
    void addParams();

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

    // default values

    static const int DEFAULT_PORT_NUMBER;
    static const int COACH_PORT_NUMBER;
    static const int OLCOACH_PORT_NUMBER;

    static const int SIMULATOR_STEP_INTERVAL_MSEC; /* milli-sec */
    static const int UDP_RECV_STEP_INTERVAL_MSEC; /* milli-sec */
    static const int UDP_SEND_STEP_INTERVAL_MSEC; /* milli-sec */
    static const int SENSE_BODY_INTERVAL_MSEC; /* milli-sec */
    static const int SEND_VISUALINFO_INTERVAL_MSEC; /* milli-sec */

    static const int HALF_TIME;
    static const int DROP_TIME;

public:
    static const double PITCH_LENGTH;
    static const double PITCH_WIDTH;
    static const double PITCH_MARGIN;
    static const double CENTER_CIRCLE_R;
    static const double PENALTY_AREA_LENGTH;
    static const double PENALTY_AREA_WIDTH;
    static const double GOAL_AREA_LENGTH;
    static const double GOAL_AREA_WIDTH;
    static const double GOAL_WIDTH;
    static const double GOAL_DEPTH;
    static const double PENALTY_SPOT_DIST;
    static const double CORNER_ARC_R;
    static const double KICK_OFF_CLEAR_DISTANCE;

    static const double CORNER_KICK_MARGIN;

private:
    static const double KEEPAWAY_LENGTH;
    static const double KEEPAWAY_WIDTH;

    static const double BALL_SIZE;
    static const double BALL_DECAY;
    static const double BALL_RAND;
    static const double BALL_WEIGHT;
    static const double BALL_T_VEL;
    static const double BALL_SPEED_MAX;
    // th 6.3.00
    static const double BALL_ACCEL_MAX;

    static const double PLAYER_SIZE;
    static const double	PLAYER_WIDGET_SIZE;
    static const double PLAYER_DECAY;
    static const double PLAYER_RAND;
    static const double PLAYER_WEIGHT;
    static const double PLAYER_SPEED_MAX;
    // th 6.3.00
    static const double PLAYER_ACCEL_MAX;
    //
    static const double IMPARAM; /* Inertia-Moment Parameter */

    static const double	STAMINA_MAX;
    static const double	STAMINA_INC_MAX;
    static const double RECOVERY_DEC_THR;
    static const double RECOVERY_DEC;
    static const double RECOVERY_MIN;
    static const double EFFORT_DEC_THR;
    static const double EFFORT_DEC;
    static const double EFFORT_MIN;
    static const double EFFORT_INC_THR;
    static const double EFFORT_INC;

    static const double KICK_RAND;
    static const double PRAND_FACTOR_L;
    static const double PRAND_FACTOR_R;
    static const double KICK_RAND_FACTOR_L;
    static const double KICK_RAND_FACTOR_R;

    static const double GOALIE_CATCHABLE_POSSIBILITY;
    static const double GOALIE_CATCHABLE_AREA_LENGTH;
    static const double GOALIE_CATCHABLE_AREA_WIDTH;
    static const int GOALIE_CATCH_BAN_CYCLE;
    static const int GOALIE_MAX_MOVES;

    static const double VISIBLE_ANGLE;
    static const double VISIBLE_DISTANCE;
    static const double AUDIO_CUT_OFF_DIST;

    static const double DASHPOWERRATE;
    static const double KICKPOWERRATE;
    static const double MAXPOWER;
    static const double MINPOWER;

    static const double KICKABLE_MARGIN;
    static const double CONTROL_RADIUS;

    static const double DIST_QSTEP;
    static const double LAND_QSTEP;
    static const double DIR_QSTEP;

    static const double MAXMOMENT;
    static const double MINMOMENT;

    static const double MAX_NECK_MOMENT;
    static const double MIN_NECK_MOMENT;

    static const double MAX_NECK_ANGLE;
    static const double MIN_NECK_ANGLE;

    static const int DEF_SAY_COACH_MSG_SIZE;
    static const int DEF_SAY_COACH_CNT_MAX;

    static const double	WIND_DIR;
    static const double	WIND_FORCE;
    static const double	WIND_RAND;
    static const double WIND_WEIGHT;

    static const double OFFSIDE_ACTIVE_AREA_SIZE;
    static const double OFFSIDE_KICK_MARGIN;

    static const std::string LANDMARK_FILE;
    static const std::string CONF_DIR;
    static const std::string SERVER_CONF;
    static const std::string OLD_SERVER_CONF;

    static const int SEND_COMMS;
    static const int TEXT_LOGGING;
    static const int GAME_LOGGING;
    static const int GAME_LOG_VERSION;
    static const std::string TEXT_LOG_DIR;
    static const std::string GAME_LOG_DIR;
    static const std::string TEXT_LOG_FIXED_NAME;
    static const std::string GAME_LOG_FIXED_NAME;
    static const int TEXT_LOG_FIXED;
    static const int GAME_LOG_FIXED;
    static const int TEXT_LOG_DATED;
    static const int GAME_LOG_DATED;
    static const std::string LOG_DATE_FORMAT;
    static const int LOG_TIMES;
    static const int RECORD_MESSAGES;
    static const int TEXT_LOG_COMPRESSION;
    static const int GAME_LOG_COMPRESSION;
    static const bool PROFILE;

    static const int KAWAY_LOGGING;
    static const std::string KAWAY_LOG_DIR;
    static const std::string KAWAY_LOG_FIXED_NAME;
    static const int KAWAY_LOG_FIXED;
    static const int KAWAY_LOG_DATED;

    static const int KAWAY_START;

    static const int POINT_TO_BAN;
    static const int POINT_TO_DURATION;

    static const unsigned int SAY_MSG_SIZE;
    static const unsigned int HEAR_MAX;
    static const unsigned int HEAR_INC;
    static const unsigned int HEAR_DECAY;

    static const double TACKLE_DIST;
    static const double TACKLE_BACK_DIST;
    static const double TACKLE_WIDTH;
    static const double TACKLE_EXPONENT;
    static const unsigned int TACKLE_CYCLES;
    static const double TACKLE_POWER_RATE;

    static const int NR_NORMAL_HALFS;
    static const int NR_EXTRA_HALFS;
    static const bool PENALTY_SHOOT_OUTS;

    static const int    PEN_BEFORE_SETUP_WAIT;
    static const int    PEN_SETUP_WAIT;
    static const int    PEN_READY_WAIT;
    static const int    PEN_TAKEN_WAIT;
    static const int    PEN_NR_KICKS;
    static const int    PEN_MAX_EXTRA_KICKS;
    static const bool   PEN_RANDOM_WINNER;
    static const double PEN_DIST_X;
    static const double PEN_MAX_GOALIE_DIST_X;
    static const bool   PEN_ALLOW_MULT_KICKS;
    static const bool   PEN_COACH_MOVES_PLAYERS;

    static const unsigned int FREEFORM_WAIT_PERIOD;
    static const unsigned int FREEFORM_SEND_PERIOD;

    static const bool FREE_KICK_FAULTS;
    static const bool BACK_PASSES;

    static const bool PROPER_GOAL_KICKS;
    static const double STOPPED_BALL_VEL;
    static const int MAX_GOAL_KICKS;

    static const int CLANG_DEL_WIN;
    static const int CLANG_RULE_WIN;

    static const bool S_AUTO_MODE;
    static const int S_KICK_OFF_WAIT;
    static const int S_CONNECT_WAIT;
    static const int S_GAME_OVER_WAIT;

    static const std::string S_TEAM_L_START;
    static const std::string S_TEAM_R_START;

    //static const char S_MODULE_DIR[];
    // 11.0.0
    static const double BALL_STUCK_AREA;
    // 12.0.0
    static const double MAX_TACKLE_POWER;
    static const double MAX_BACK_TACKLE_POWER;
    static const double PLAYER_SPEED_MAX_MIN;
    static const double EXTRA_STAMINA;
    static const int SYNCH_SEE_OFFSET;
    // 12.1.3
    static const int EXTRA_HALF_TIME;
    // 13.0.0
    static const double	STAMINA_CAPACITY;
    static const double MAX_DASH_ANGLE;
    static const double MIN_DASH_ANGLE;
    static const double DASH_ANGLE_STEP;
    static const double SIDE_DASH_RATE;
    static const double BACK_DASH_RATE;
    static const double MAX_DASH_POWER;
    static const double MIN_DASH_POWER;
    // 14.0.0
    static const double TACKLE_RAND_FACTOR;
    static const double FOUL_DETECT_PROBABILITY;
    static const double FOUL_EXPONENT;
    static const int FOUL_CYCLES;
    static const bool GOLDEN_GOAL;
    // 15.0.0
    static const double RED_CARD_PROBABILITY;
    // XXX
    static const double LONG_KICK_POWER_FACTOR;
    static const int LONG_KICK_DELAY;

    double M_goal_width; /* goal width */
    double M_inertia_moment; /* intertia moment for turn */
    double M_player_size; /* player size */
    double M_player_decay; /* player decay */
    double M_player_rand; /* player rand */
    double M_player_weight; /* player weight */
    double M_player_speed_max; /* player speed max */
    // th 6.3.00
    double M_player_accel_max; /* player acceleration max */
    //
    double M_stamina_max; /* player stamina max */
    double M_stamina_inc; /* player stamina inc */
    double M_recover_init; /* player recovery init */
    double M_recover_dec_thr; /* player recovery decriment threshold */
    double M_recover_min; /* player recovery min */
    double M_recover_dec; /* player recovery decriment */
    double M_effort_init; /* player dash effort init */
    double M_effort_dec_thr; /* player dash effort decriment threshold */
    double M_effort_min; /* player dash effrot min */
    double M_effort_dec; /* player dash effort decriment */
    double M_effort_inc_thr; /* player dash effort incriment threshold */
    double M_effort_inc; /* player dash effort incriment */
    // pfr 8/14/00: for RC2000 evaluation
    double M_kick_rand; /* noise added directly to kicks */
    bool M_team_actuator_noise; /* flag whether to use team specific actuator noise */
    double M_player_rand_factor_l; /* factor to multiple prand for left team */
    double M_player_rand_factor_r; /* factor to multiple prand for right team */
    double M_kick_rand_factor_l; /* factor to multiple kick_rand for left team */
    double M_kick_rand_factor_r; /* factor to multiple kick_rand for right team */

    double M_ball_size; /* ball size */
    double M_ball_decay; /* ball decay */
    double M_ball_rand; /* ball rand */
    double M_ball_weight; /* ball weight */
    double M_ball_speed_max; /* ball speed max */
    // th 6.3.00
    double M_ball_accel_max; /* ball acceleration max */
    //
    double M_dash_power_rate; /* dash power rate */
    double M_kick_power_rate; /* kick power rate */
    double M_kickable_margin; /* kickable margin */
    double M_control_radius; /* control radius */
    double M_control_radius_width; /* (control radius) - (plyaer size) */

    double M_max_power; /* max power */
    double M_min_power; /* min power */
    double M_max_moment; /* max moment */
    double M_min_moment; /* min moment */
    double M_max_neck_moment; /* max neck moment */
    double M_min_neck_moment; /* min neck moment */
    double M_max_neck_angle; /* max neck angle */
    double M_min_neck_angle; /* min neck angle */
    double M_visible_angle; /* visible angle */
    double M_visible_distance; /* visible distance */

    double M_wind_dir; /* wind direction */
    double M_wind_force; /* wind force */
    double M_wind_angle; /* wind angle for rand */
    double M_wind_rand; /* wind force for force */
    bool M_wind_none; /* wind factor is none */
    bool M_wind_random; /* wind factor is random */

    double M_kickable_area; /* kickable_area */

    double M_catchable_area_l; /* goalie catchable area length */
    double M_catchable_area_w; /* goalie catchable area width */
    double M_catch_probability; /* goalie catchable possibility */
    int M_goalie_max_moves; /* goalie max moves after a catch */
    bool M_keepaway; /* keepaway mode on/off */
    double M_keepaway_length; /* keepaway region length */
    double M_keepaway_width; /* keepaway region width */
    double M_corner_kick_margin; /* corner kick margin */
    double M_offside_active_area_size; /* offside active area size */
    int M_port; /* port number */
    int M_coach_port; /* coach port number */
    int M_olcoach_port; /* online coach port number */
    int M_say_cnt_max; /* max count of coach SAY (freeform) */
    int M_say_coach_msg_size; /* max length of coach SAY (freeform) */
    int M_clang_win_size; /* coach language: time window size */
    int M_clang_define_win; /* coach language: define messages per window */
    int M_clang_meta_win; /* coach language: meta messages per window */
    int M_clang_advice_win; /* coach language: advice messages per window */
    int M_clang_info_win; /* coach language: info messages per window */
    int M_clang_mess_delay; /* coach language: delay between receive and send */
    int M_clang_mess_per_cycle; /* coach language: number of messages to flush per cycle */

    int M_raw_half_time; /* half time */
    int M_half_time; /* half time */

    int M_drop_ball_time; /* cycles for dropping
                             the ball after a free kick,
                             corner kick message and
                             noone kicking the ball.
                             0 means don't drop
                             automatically  */
    int M_nr_normal_halfs; /* nr of normal halfs: default 2)      */
    int M_nr_extra_halfs; /* nr of extra halfs: -1 is infinite)  */
    bool M_penalty_shoot_outs;/* penalty shoot outs after extra halfs*/

    int M_pen_before_setup_wait;/* cycles waited before penalty setup*/
    int M_pen_setup_wait; /* cycles waited to setup penalty    */
    int M_pen_ready_wait; /* cycles waited to take penalty     */
    int M_pen_taken_wait; /* cycles waited to finish penalty   */
    int M_pen_nr_kicks; /* number of penalty kicks           */
    int M_pen_max_extra_kicks; /* max. nr of extra penalty kicks    */
    double M_pen_dist_x; /* distance from goal to place ball  */
    bool M_pen_random_winner; /* random winner when draw?          */
    bool M_pen_allow_mult_kicks; /* can attacker kick mult. times */
    double M_pen_max_goalie_dist_x; /*max distance from goal for goalie*/
    bool M_pen_coach_moves_players; /*coach moves players when positioned wrongly */

    int M_simulator_step; /* simulator step interval msec */
    int M_sense_body_step; /* sense_body interval step msec */
    int M_send_vi_step; /* online coach's look interval step */
    int M_send_step; /* udp send step interval msec for player's default view mode*/
    int M_lcm_step; /* lcm of all the above steps msec */
    int M_recv_step; /* udp recv step interval msec */
    int M_catch_ban_cycle; /* goalie catch ban cycle */
    int M_slow_down_factor; /* factor to slow down simulator and send intervals */
    bool M_use_offside; /* flag for using off side rule */
    bool M_forbid_kick_off_offside; /* flag for permit kick off offside */
    double M_offside_kick_margin; /* offside kick margin */
    double M_audio_cut_dist; /* audio cut off distance */
    double M_quantize_step; /* quantize step of distance */
    double M_landmark_quantize_step; /* quantize step of distance for landmark */
#ifdef NEW_QSTEP
    double dir_qstep; /* quantize step of direction */
    //private:
    double dist_qstep_l; /* team right quantize step of distance */
    double dist_qstep_r; /* team left quantize step of distance */
    double land_qstep_l; /* team right quantize step of distance for landmark */
    double land_qstep_r; /* team left quantize step of distance for landmark */
    double dir_qstep_l; /* team left quantize step of direction */
    double dir_qstep_r; /* team right quantize step of direction */
    bool defined_qstep_l;
    bool defined_qstep_r;
    bool defined_qstep_l_l;
    bool defined_qstep_l_r;
    bool defined_qstep_dir_l;
    bool defined_qstep_dir_r;
#endif
    bool M_verbose; /* flag for verbose mode */

    bool M_coach_mode; /* coach mode */
    bool M_coach_with_referee_mode; /* coach with referee mode */
    bool M_old_coach_hear; /* old format for hear command (coach) */

    bool M_synch_mode; /* pfr:SYNCH whether to run in synchronized mode */
    int M_synch_offset; /* pfr:SYNCH the offset from the start of the cycle to tell players to run */
    int M_synch_micro_sleep; /* pfr:SYNCH the # of microseconds to sleep while waiting for players */

    int M_start_goal_l; /* The starting score of the left team */
    int M_start_goal_r; /* The starting score of the right team */

    bool M_fullstate_l; /* flag to send fullstate messages to left team; supresses visual info */
    bool M_fullstate_r; /* flag to send fullstate messages to right team; supresses visual info */

    double M_slowness_on_top_for_left_team; /* Kinda self explanatory */
    double M_slowness_on_top_for_right_team; /* ditto */

    std::string M_landmark_file;

    bool M_send_comms;
    bool M_text_logging;
    bool M_game_logging;
    int M_game_log_version;
    std::string M_text_log_dir;
    std::string M_game_log_dir;
    std::string M_text_log_fixed_name;
    std::string M_game_log_fixed_name;
    bool M_text_log_fixed;
    bool M_game_log_fixed;
    bool M_text_log_dated;
    bool M_game_log_dated;
    std::string M_log_date_format;
    bool M_log_times;
    bool M_record_messages;
    int M_text_log_compression;
    int M_game_log_compression;
    bool M_profile;

    bool M_keepaway_logging;
    std::string M_keepaway_log_dir;
    std::string M_keepaway_log_fixed_name;
    bool M_keepaway_log_fixed;
    bool M_keepaway_log_dated;

    int M_keepaway_start;

    int M_point_to_ban;
    int M_point_to_duration;

    int M_say_msg_size;	/* string size of say message */
    int M_hear_max; /* player hear_capacity_max */
    int M_hear_inc; /* player hear_capacity_inc */
    int M_hear_decay; /* player hear_capacity_decay */

    double M_tackle_dist;
    double M_tackle_back_dist;
    double M_tackle_width;
    double M_tackle_exponent;
    int M_tackle_cycles;
    double M_tackle_power_rate;

    int M_freeform_wait_period;
    int M_freeform_send_period;

    bool M_free_kick_faults;
    bool M_back_passes;

    bool M_proper_goal_kicks;
    double M_stopped_ball_vel;
    int M_max_goal_kicks;

    int M_clang_del_win;
    int M_clang_rule_win;

    bool M_auto_mode;
    int M_kick_off_wait;
    int M_connect_wait;
    int M_game_over_wait;

    std::string M_team_l_start;
    std::string M_team_r_start;

    // 11.0.0
    double M_ball_stuck_area; /* threshold distance checked by BallStuckRef */
    std::string M_coach_msg_file; /* file name that contains the messages sent to coaches */

    // 12.0.0
    double M_max_tackle_power;
    double M_max_back_tackle_power;
    double M_player_speed_max_min; // minumum value of player speed max
    double M_extra_stamina;
    int M_max_monitors; //!< The maximum number of monitor client connection.

    int M_synch_see_offset; //!< synch see offset

    // 12.1.3
    int M_raw_extra_half_time;
    int M_extra_half_time;

    // 13.0.0
    double M_stamina_capacity;
    double M_max_dash_angle;
    double M_min_dash_angle;
    double M_dash_angle_step;
    double M_side_dash_rate;
    double M_back_dash_rate;
    double M_max_dash_power;
    double M_min_dash_power;

    // 14.0.0
    double M_tackle_rand_factor;
    double M_foul_detect_probability;
    double M_foul_exponent;
    int M_foul_cycles;

    int M_random_seed;
    bool M_golden_goal;

    // 15.0.0
    double M_red_card_probability;

    // XXX
    double M_long_kick_power_factor;
    int M_long_kick_delay;

private:

    // setters & getters

    void setCTLRadius( double value );
    void setKickMargin( double value );
    void setBallSize( double value );
    void setPlayerSize( double value );

    double getHalfTimeScaler() const;
    void setHalfTime( int value );
    int getRawHalfTime() const;
    void setExtraHalfTime( int value );
    int getRawExtraHalfTime() const;
    void setNrNormalHalfs( int value );
    void setNrExtraHalfs( int value );

    int getRawSimStep() const;
    int getRawSenseBodyStep() const;
    int getRawCoachVisualStep() const;
    int getRawSendStep() const;
    int getRawSynchOffset() const;

    void setSynchMode( bool value );

    void setTeamLeftStart( std::string start );
    void setTeamRightStart( std::string start );

    void setTextLogDir( std::string str );
    void setGameLogDir( std::string str );
    void setKAwayLogDir( std::string str );

    void setCoachMsgFile( std::string str );

    void setFoulDetectProbability( double value );
    void setRedCardProbability( double value );

    void setSlowDownFactor();

public:

    const VerMap & verMap() const
      {
          return M_ver_map;
      }

    void setRandomSeed( const int seed )
      {
          M_random_seed = seed;
      }

    server_params_t convertToStruct() const;

    bool getInt( const std::string & param,
                 int & value ) const;
    bool getBool( const std::string & param,
                  bool & value ) const;
    bool getDouble( const std::string & param,
                    double & value ) const;
    bool getStr( const std::string & param,
                 std::string & value ) const;


    // access methods

    double goalWidth() const { return M_goal_width; }
    double inertiaMoment() const { return M_inertia_moment; }
    double playerSize() const { return M_player_size; }
    double playerDecay() const { return M_player_decay; }
    double playerRand() const { return M_player_rand; }
    double playerWeight() const { return M_player_weight; }
    double playerSpeedMax() const { return M_player_speed_max; }
    double playerAccelMax() const { return M_player_accel_max; }
    double staminaMax() const { return M_stamina_max; }
    double staminaInc() const { return M_stamina_inc; }
    double recoverInit() const { return M_recover_init; }
    double recoverDecThr() const { return M_recover_dec_thr; }
    double recoverMin() const { return M_recover_min; }
    double recoverDec() const { return M_recover_dec; }
    double effortInit() const { return M_effort_init; }
    double effortDecThr() const { return  M_effort_dec_thr; }
    double effortMin() const { return M_effort_min; }
    double effortDec() const { return M_effort_dec; }
    double effortIncThr() const { return  M_effort_inc_thr; }
    double effortInc() const { return M_effort_inc; }
    double kickRand() const { return M_kick_rand; }
    bool teamActuatorNoise() const { return M_team_actuator_noise; }
    double playerRandFactorLeft() const { return M_player_rand_factor_l; }
    double playerRandFactorRight() const { return M_player_rand_factor_r; }
    double kickRandFactorLeft() const { return M_kick_rand_factor_l; }
    double kickRandFactorRight() const { return M_kick_rand_factor_r; }

    double ballSize() const { return M_ball_size; }
    double ballDecay() const { return M_ball_decay; }
    double ballRand() const { return M_ball_rand; }
    double ballWeight() const { return M_ball_weight; }
    double ballSpeedMax() const { return M_ball_speed_max; }
    double ballAccelMax() const { return M_ball_accel_max; }
    double dashPowerRate() const { return M_dash_power_rate; }
    double kickPowerRate() const { return M_kick_power_rate; }
    double kickableMargin() const { return M_kickable_margin; }
    double controlRadius() const { return M_control_radius; }
    double controlRadiusWidth() const { return M_control_radius_width; }

    double maxPower() const { return M_max_power; }
    double minPower() const { return M_min_power; }
    double maxMoment() const { return M_max_moment; }
    double minMoment() const { return M_min_moment; }
    double maxNeckMoment() const { return M_max_neck_moment; }
    double minNeckMoment() const { return M_min_neck_moment; }
    double maxNeckAngle() const { return M_max_neck_angle; }
    double minNeckAngle() const { return M_min_neck_angle; }
    //double visibleAngle() const { return Deg2Rad( M_visible_angle ); }
    double visibleAngleDegree() const { return M_visible_angle; }
    double visibleDistance() const{ return M_visible_distance; }

    double windDir() const { return M_wind_dir; }
    double windForce() const { return M_wind_force; }
    double windAngle() const { return M_wind_angle; }
    double windRand() const { return M_wind_rand; }
    double windWeight() const { return WIND_WEIGHT; }
    bool windNone() const { return M_wind_none; }
    bool windRandom() const { return M_wind_random; }

    double kickableArea() const { return M_kickable_area; }

    double catchAreaLength() const { return M_catchable_area_l; }
    double catchAreaWidth() const { return M_catchable_area_w; }
    double catchProbability() const { return M_catch_probability; }
    int goalieMaxMoves() const { return M_goalie_max_moves; }

    bool keepAwayMode() const { return M_keepaway; }
    double keepAwayLength() const { return M_keepaway_length; }
    double keepAwayWidth() const { return M_keepaway_width; }

    double cornerKickMargin() const { return M_corner_kick_margin; }
    double offsideActiveArea() const { return M_offside_active_area_size; }

    rcss::net::Addr::PortType playerPort() const { return M_port; }
    rcss::net::Addr::PortType offlineCoachPort() const { return M_coach_port; }
    rcss::net::Addr::PortType onlineCoachPort() const { return M_olcoach_port; }

    unsigned int freeformCountMax() const { return (unsigned int)M_say_cnt_max; }
    unsigned int freeformMsgSize() const { return (unsigned int)M_say_coach_msg_size; }

    int clangWinSize() const { return M_clang_win_size; }
    int clangDefineWin() const { return M_clang_define_win; }
    int clangMetaWin() const { return M_clang_meta_win; }
    int clangAdviceWin() const { return M_clang_advice_win; }
    int clangInfoWin() const { return M_clang_info_win; }
    int clangMessDelay() const { return M_clang_mess_delay; }
    int clangMessPerCycle() const{ return M_clang_mess_per_cycle; }

    int halfTime() const { return M_half_time; }
    int dropTime() const { return M_drop_ball_time; }
    int nrNormalHalfs() const { return M_nr_normal_halfs; }
    int nrExtraHalfs() const { return M_nr_extra_halfs; }
    bool penaltyShootOuts() const { return M_penalty_shoot_outs; }

    int penBeforeSetupWait() const { return M_pen_before_setup_wait; }
    int penSetupWait() const { return M_pen_setup_wait; }
    int penReadyWait() const { return M_pen_ready_wait; }
    int penTakenWait() const { return M_pen_taken_wait; }
    int penNrKicks() const { return M_pen_nr_kicks; }
    int penMaxExtraKicks() const { return M_pen_max_extra_kicks; }
    double penDistX() const { return M_pen_dist_x; }
    bool penRandomWinner() const { return M_pen_random_winner; }
    bool penAllowMultiKicks() const { return M_pen_allow_mult_kicks; }
    double penMaxGoalieDistX() const { return M_pen_max_goalie_dist_x; }
    bool penCoachMovesPlayers() const { return M_pen_coach_moves_players; }

    int simStep() const { return M_simulator_step; }
    int senseBodyStep() const { return M_sense_body_step; }
    int coachVisualStep() const { return M_send_vi_step; }
    int sendStep() const { return M_send_step; }
    int lcmStep() const { return M_lcm_step; }
    int recvStep() const { return M_recv_step; }

    int catchBanCycle() const { return M_catch_ban_cycle; }
    int slowDownFactor() const { return M_slow_down_factor; }
    bool useOffside() const { return M_use_offside; }
    bool kickOffOffside() const { return M_forbid_kick_off_offside; }
    double offsideKickMargin() const { return M_offside_kick_margin; }

    double audioCutDist() const { return M_audio_cut_dist; }

    double quantizeStep() const { return M_quantize_step; }
    double landmarkQuantizeStep() const { return M_landmark_quantize_step; }

#ifdef NEW_QSTEP
    const Vlaue & dirQStep() const { return dir_qstep; }
    double distQStepL() const
      {
          return ( ! defined_qstep_l ? dist_qstep : dist_qstep_l );
      }
    double distQStepR() const
      {
          return ( ! defined_qstep_r ? dist_qstep : dist_qstep_r );
      }
    double landQStepL() const
      {
          return ( ! defined_qstep_l_l ? land_qstep : land_qstep_l );
      }
    double landQStepR() const
      {
          return ( ! defined_qstep_r_l ? land_qstep : land_qstep_r );
      }
    double dirQStepL() const
      {
          return ( ! defined_qstep_dir_l ? dir_qstep : dir_qstep_l );
      }
    double dirQStepR() const
      {
          return ( ! defined_qstep_dir_r ? dir_qstep : dir_qstep_r );
      }
#endif

    bool verboseMode() const { return M_verbose; }
    bool coachMode() const { return M_coach_mode; }
    bool coachWithRefereeMode() const { return M_coach_with_referee_mode; }
    bool coachOldHear() const { return M_old_coach_hear; }

    bool synchMode() const { return M_synch_mode; }
    int synchOffset() const { return M_synch_offset; }
    unsigned int synchMicroSleep() const { return static_cast< unsigned int >( M_synch_micro_sleep ); }

    int startGoalLeft() const { return M_start_goal_l; }
    int startGoalRight() const { return M_start_goal_r; }

    bool fullstateLeft() const { return M_fullstate_l; }
    bool fullstateRight() const { return M_fullstate_r; }

    double slownessOnTopForLeft() const { return M_slowness_on_top_for_left_team; }
    double slownessOnTopForRight() const { return M_slowness_on_top_for_right_team; }

    const std::string & landmarkFile() const { return M_landmark_file; }

    bool sendComms() const { return M_send_comms; }

    bool textLogging() const { return M_text_logging; }
    bool gameLogging() const { return M_game_logging; }
    int gameLogVersion() const { return M_game_log_version; }
    const std::string & textLogDir() const { return M_text_log_dir; }
    const std::string & gameLogDir() const { return M_game_log_dir; }
    const std::string & textLogFixedName() const { return M_text_log_fixed_name; }
    const std::string & gameLogFixedName() const { return M_game_log_fixed_name; }
    int textLogFixed() const { return M_text_log_fixed; }
    int gameLogFixed() const { return M_game_log_fixed; }
    int textLogDated() const { return M_text_log_dated; }
    int gameLogDated() const { return M_game_log_dated; }
    const std::string & logDateFormat() const { return M_log_date_format; }
    int logTimes() const { return M_log_times; }
    int recordMessages() const { return M_record_messages; }
    int textLogCompression() const { return M_text_log_compression; }
    int gameLogCompression() const { return M_game_log_compression; }

    bool profile() const { return M_profile; }

    bool kawayLogging() const { return M_keepaway_logging; }
    const std::string & kawayLogDir() const { return M_keepaway_log_dir; }
    const std::string & kawayLogFixedName() const { return M_keepaway_log_fixed_name; }
    bool kawayLogFixed() const { return M_keepaway_log_fixed; }
    bool kawayLogDated() const { return M_keepaway_log_dated; }

    int kawayStart() const { return M_keepaway_start; }

    unsigned int pointToBan() const { return static_cast< unsigned int >( M_point_to_ban ); }
    unsigned int pointToDuration() const { return static_cast< unsigned int >( M_point_to_duration ); }

    unsigned int sayMsgSize() const { return static_cast< unsigned int >( M_say_msg_size ); }
    unsigned int hearMax() const { return static_cast< unsigned int >( M_hear_max ); }
    unsigned int hearInc() const { return static_cast< unsigned int >( M_hear_inc ); }
    unsigned int hearDecay() const { return static_cast< unsigned int >( M_hear_decay ); }

    double tackleDist() const { return M_tackle_dist; }
    double tackleBackDist() const { return M_tackle_back_dist; }
    double tackleWidth() const { return M_tackle_width; }
    double tackleExponent() const { return M_tackle_exponent; }
    int tackleCycles() const { return M_tackle_cycles; }
    double tacklePowerRate() const { return M_tackle_power_rate; }

    unsigned int freeformWaitPeriod() const { return static_cast< unsigned int >( M_freeform_wait_period ); }
    unsigned int freeformSendPeriod() const { return static_cast< unsigned int >( M_freeform_send_period ); }

    bool freeKickFaults() const { return M_free_kick_faults; }
    bool backPasses() const { return M_back_passes; }
    bool properGoalKicks() const { return M_proper_goal_kicks; }
    double stoppedBallVel() const { return M_stopped_ball_vel; }
    int maxGoalKicks() const { return M_max_goal_kicks; }

    int clangDelWin() const { return M_clang_del_win; }
    int clangRuleWin() const { return M_clang_rule_win; }

    bool autoMode() const { return M_auto_mode; }
    int kickOffWait() const { return M_kick_off_wait; }
    int connectWait() const { return M_connect_wait; }
    int gameOverWait() const { return M_game_over_wait; }
    const std::string & teamLeftStart() const { return M_team_l_start; }
    const std::string & teamRightStart() const { return M_team_r_start; }

    double goalPostRadius() const { return 0.06; }

    // v.11
    double ballStuckArea() const { return M_ball_stuck_area; }
    const std::string & coachMsgFile() const { return M_coach_msg_file; }

    // v.12
    double maxTacklePower() const { return M_max_tackle_power; }
    double maxBackTacklePower() const { return M_max_back_tackle_power; }
    double playerSpeedMaxMin() const { return M_player_speed_max_min; }
    double extraStamina() const { return M_extra_stamina; }
    int maxMonitors() const { return M_max_monitors; }
    int synchSeeOffset() const { return M_synch_see_offset; }
    // v12.1.3
    int extraHalfTime() const { return M_extra_half_time; }

    // v13
    double staminaCapacity() const { return M_stamina_capacity; }
    double maxDashAngle() const { return M_max_dash_angle; }
    double minDashAngle() const { return M_min_dash_angle; }
    double dashAngleStep() const { return M_dash_angle_step; }
    double sideDashRate() const { return M_side_dash_rate; }
    double backDashRate() const { return M_back_dash_rate; }
    double maxDashPower() const { return M_max_dash_power; }
    double minDashPower() const { return M_min_dash_power; }

    // v14
    double tackleRandFactor() const { return M_tackle_rand_factor; }
    double foulDetectProbability() const { return M_foul_detect_probability; }
    double foulExponent() const { return M_foul_exponent; }
    int foulCycles() const { return M_foul_cycles; }

    int randomSeed() const { return M_random_seed; }
    bool goldenGoal() const { return M_golden_goal; }

    // v15
    double redCardProbability() const { return M_red_card_probability; }

    // XXX
    double longKickPowerFactor() const { return M_long_kick_power_factor; }
    int longKickDelay() const { return M_long_kick_delay; }

};

#endif

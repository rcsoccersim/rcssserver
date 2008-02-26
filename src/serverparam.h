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


#ifndef SERVER_PARAM_H
#define SERVER_PARAM_H

#include "rcssserverconfig.hpp"

#include "utility.h"
#include "types.h"

#include <rcssbase/net/udpsocket.hpp>

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
    static bool s_in_init;
    static std::string S_program_name;

    boost::shared_ptr< rcss::conf::Builder > m_builder;
    boost::shared_ptr< rcss::conf::Parser > m_conf_parser;
    boost::shared_ptr< rcss::conf::StreamStatusHandler > m_err_handler;

    VerMap m_ver_map;

private:

    ServerParam(); // not used
    ServerParam( const ServerParam & ); // not used
    ServerParam & operator=( const ServerParam & ); // not used

protected:

    RCSSSERVER_API
    ServerParam ( const std::string & progname );

public:

    RCSSSERVER_API
    static
    ServerParam & instance();

    RCSSSERVER_API
    static
    bool init( const int & argc,
               const char * const * argv );

    virtual
    ~ServerParam ();

    void clear();

private:

    void setDefaults();
    void addParams();

    template< typename P >
    void addParam(  const std::string & name,
                    P & param,
                    const std::string & desc,
                    int version );

    template< typename S, typename G >
    void addParam(  const std::string & name,
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
    static const double PLAYER_SPEED_MAX_MIN; // [12.0.0]
    // th 6.3.00
    static const double PLAYER_ACCEL_MAX;
    //
    static const double IMPARAM; /* Inertia-Moment Parameter */

    static const double	STAMINA_MAX;
    static const double EXTRA_STAMINA;
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

    static const double VisibleAngle;
    static const double VisibleDistance;
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

    static const char LANDMARK_FILE[];
    static const char SERVER_CONF[];
    static const char OLD_SERVER_CONF[];

    static const int SEND_COMMS;
    static const int TEXT_LOGGING;
    static const int GAME_LOGGING;
    static const int GAME_LOG_VERSION;
    static const char TEXT_LOG_DIR[];
    static const char GAME_LOG_DIR[];
    static const char TEXT_LOG_FIXED_NAME[];
    static const char GAME_LOG_FIXED_NAME[];
    static const int TEXT_LOG_FIXED;
    static const int GAME_LOG_FIXED;
    static const int TEXT_LOG_DATED;
    static const int GAME_LOG_DATED;
    static const char LOG_DATE_FORMAT[];
    static const int LOG_TIMES;
    static const int RECORD_MESSAGES;
    static const int TEXT_LOG_COMPRESSION;
    static const int GAME_LOG_COMPRESSION;
    static const bool PROFILE;

    static const int KAWAY_LOGGING;
    static const char KAWAY_LOG_DIR[];
    static const char KAWAY_LOG_FIXED_NAME[];
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
    static const double MAX_TACKLE_POWER;
    static const double MAX_BACK_TACKLE_POWER;

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

    static const char S_TEAM_L_START[];
    static const char S_TEAM_R_START[];

    static const char S_MODULE_DIR[];

    static const double BALL_STUCK_AREA;

		double gwidth; /* goal width */
		double inertia_moment; /* intertia moment for turn */
		double psize; /* player size */
		double pdecay; /* player decay */
		double prand; /* player rand */
		double pweight; /* player weight */
		double pspeed_max; /* player speed max */
		// th 6.3.00
		double paccel_max; /* player acceleration max */
		//
		double stamina_max; /* player stamina max */
		double stamina_inc; /* player stamina inc */
		double recover_init; /* player recovery init */
		double recover_dthr; /* player recovery decriment threshold */
		double recover_min; /* player recovery min */
		double recover_dec; /* player recovery decriment */
		double effort_init; /* player dash effort init */
		double effort_dthr; /* player dash effort decriment threshold */
		double effort_min; /* player dash effrot min */
		double effort_dec; /* player dash effort decriment */
		double effort_ithr; /* player dash effort incriment threshold */
		double effort_inc; /* player dash effort incriment */
		// pfr 8/14/00: for RC2000 evaluation
		double kick_rand; /* noise added directly to kicks */
		bool team_actuator_noise; /* flag whether to use team specific actuator noise */
		double prand_factor_l; /* factor to multiple prand for left team */
		double prand_factor_r; /* factor to multiple prand for right team */
		double kick_rand_factor_l; /* factor to multiple kick_rand for left team */
		double kick_rand_factor_r; /* factor to multiple kick_rand for right team */

		double bsize; /* ball size */
		double bdecay; /* ball decay */
		double brand; /* ball rand */
		double bweight; /* ball weight */
		double bspeed_max; /* ball speed max */
		// th 6.3.00
		double baccel_max; /* ball acceleration max */
		//
		double dprate; /* dash power rate */
		double kprate;	/* kick power rate */
		double kmargin; /* kickable margin */
		double ctlradius; /* control radius */
		double ctlradius_width; /* (control radius) - (plyaer size) */
    //public:
		double maxp; /* max power */
		double minp; /* min power */
		double maxm;	/* max moment */
		double minm;	/* min moment */
		double maxnm; /* max neck moment */
		double minnm; /* min neck moment */
		double maxn; /* max neck angle */
		double minn; /* min neck angle */
    //private:
		double visangle; /* visible angle */
    //public:
		double visdist; /* visible distance */
		double windir; /* wind direction */
		double winforce; /* wind force */
		double winang; /* wind angle for rand */
		double winrand; /* wind force for force */
    //private:
		double kickable_area; /* kickable_area */
    //public:
		double catch_area_l; /* goalie catchable area length */
		double catch_area_w; /* goalie catchable area width */
		double catch_prob; /* goalie catchable possibility */
    int goalie_max_moves; /* goalie max moves after a catch */
    bool kaway; /* keepaway mode on/off */
    double ka_length; /* keepaway region length */
    double ka_width; /* keepaway region width */
		double ckmargin; /* corner kick margin */
		double offside_area; /* offside active area size */
		bool win_no; /* wind factor is none */
		bool win_random; /* wind factor is random */
		int portnum; /* port number */
		int coach_pnum; /* coach port number */
		int olcoach_pnum; /* online coach port number */
		int say_cnt_max; /* max count of coach SAY (freeform) */
		int SayCoachMsgSize; /* max length of coach SAY (freeform) */
		int clang_win_size; /* coach language: time window size */
		int clang_define_win; /* coach language: define messages per window */
		int clang_meta_win; /* coach language: meta messages per window */
		int clang_advice_win; /* coach language: advice messages per window */
		int clang_info_win; /* coach language: info messages per window */
		int clang_mess_delay; /* coach language: delay between receive and send */
		int clang_mess_per_cycle; /* coach language: number of messages to flush per cycle */
    //private:
		int half_time; /* half time */
    //public:
		int drop_time; /* cycles for dropping
                      the ball after a free kick,
                      corner kick message and
                      noone kicking the ball.
                      0 means don't drop
                      automatically  */
    int nr_normal_halfs; /* nr of normal halfs: default 2)      */
    int nr_extra_halfs; /* nr of extra halfs: -1 is infinite)  */
    bool penalty_shoot_outs;/* penalty shoot outs after extra halfs*/

    int pen_before_setup_wait;/* cycles waited before penalty setup*/
    int pen_setup_wait; /* cycles waited to setup penalty    */
    int pen_ready_wait; /* cycles waited to take penalty     */
    int pen_taken_wait; /* cycles waited to finish penalty   */
    int pen_nr_kicks; /* number of penalty kicks           */
    int pen_max_extra_kicks; /* max. nr of extra penalty kicks    */
    double pen_dist_x; /* distance from goal to place ball  */
    bool pen_random_winner; /* random winner when draw?          */
    bool pen_allow_mult_kicks; /* can attacker kick mult. times */
    double pen_max_goalie_dist_x; /*max distance from goal for goalie*/
    bool pen_coach_moves_players; /*coach moves players when positioned wrongly */
    //private:
		int sim_st; /* simulator step interval msec */
		int sb_step; /* sense_body interval step msec */
		int sv_st; /* online coach's look interval step */
		int send_st; /* udp send step interval msec */
		int lcm_st; /* lcm of all the above steps msec */
    //public:
		int recv_st; /* udp recv step interval msec */
		int cban_cycle; /* goalie catch ban cycle */
		int slow_down_factor; /* factor to slow down simulator and send intervals */
		bool useoffside; /* flag for using off side rule */
		bool kickoffoffside; /* flag for permit kick off offside */
		double offside_kick_margin; /* offside kick margin */
		double audio_dist; /* audio cut off distance */
		double dist_qstep; /* quantize step of distance */
		double land_qstep; /* quantize step of distance for landmark */
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
    //public:
		bool verbose; /* flag for verbose mode */

		bool CoachMode; /* coach mode */
		bool CwRMode; /* coach with referee mode */
		bool old_hear; /* old format for hear command (coach) */

		bool synch_mode; /* pfr:SYNCH whether to run in synchronized mode */
    //bool timer_loaded;

		int synch_offset; /* pfr:SYNCH the offset from the start of the cycle to tell players to run */

		int synch_micro_sleep; /* pfr:SYNCH the # of microseconds to sleep while waiting for players */

		int start_goal_l; /* The starting score of the left team */
		int start_goal_r; /* The starting score of the right team */

		bool fullstate_l; /* flag to send fullstate messages to left team; supresses visual info */
		bool fullstate_r; /* flag to send fullstate messages to right team; supresses visual info */

		double slowness_on_top_for_left_team; /* Kinda self explanatory */
		double slowness_on_top_for_right_team; /* ditto */
    //private:
		std::string landmark_file;

		bool send_comms;
		bool text_logging;
		bool game_logging;
		int game_log_version;
		std::string text_log_dir;
		std::string game_log_dir;
		std::string text_log_fixed_name;
		std::string game_log_fixed_name;
		bool text_log_fixed;
		bool game_log_fixed;
		bool text_log_dated;
		bool game_log_dated;
		std::string log_date_format;
		bool log_times;
		bool record_messages;
		int text_log_compression;
		int game_log_compression;
		bool M_profile;

    bool kaway_logging;
    std::string kaway_log_dir;
    std::string kaway_log_fixed_name;
    bool kaway_log_fixed;
    bool kaway_log_dated;

    int kaway_start;

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

    // test parameters for future specification
		double M_reliable_catch_area_l; /* goalie reliable catchable area length */
    double M_min_catch_probability;

private:

    // setters & getters

    void setCTLRadius( double value )
      {
          ctlradius_width -= ctlradius;
          ctlradius = value;
          ctlradius_width += ctlradius;
      }

    void setKickMargin( double value )
      {
          kickable_area -= kmargin;
          kmargin = value;
          kickable_area += kmargin;
      }

    void setBallSize( double value )
      {
          kickable_area -= bsize;
          bsize = value;
          kickable_area += bsize;
      }

    void setPlayerSize( double value )
      {
          ctlradius_width += psize;
          kickable_area -= psize;
          psize = value;
          kickable_area += psize;
          ctlradius_width -= psize;
      }

    double getHalfTimeScaler() const
      {
          double value = 1000.0 / (sim_st / slow_down_factor);
          return ( value != 0.0
                   ? value
                   : EPS );
      }

    //Have to be careful with integer math, see bug # 800540
    void setHalfTime( int value )
      {
          half_time = (int)( value * getHalfTimeScaler() + 0.5 );
      }

    //Have to be careful with integer math, see bug # 800540
    int getRawHalfTime() const
      {
          return (int)( half_time / getHalfTimeScaler() + 0.5 );
      }

    void setSimStep( int value )
      {
          half_time = (int)( half_time / getHalfTimeScaler() + 0.5);
          sim_st = value * slow_down_factor;
          half_time = (int)( half_time * getHalfTimeScaler() + 0.5);
      }

    int getRawSimStep() const
      {
          return sim_st / slow_down_factor;
      }

    void setSenseBodyStep( int value )
      {
          sb_step = value * slow_down_factor;
      }

    int getRawSenseBodyStep() const
      {
          return sb_step / slow_down_factor;
      }

    void setCoachVisualStep( int value )
      {
          sv_st = value * slow_down_factor;
      }

    int getRawCoachVisualStep() const
      {
          return sv_st / slow_down_factor;
      }

    void setSendStep( int value )
      {
          send_st = value * slow_down_factor;
      }

    int getRawSendStep() const
      {
          return send_st / slow_down_factor;
      }

    void setSynchOffset( int value )
      {
          synch_offset = value * slow_down_factor;
      }

    int getRawSynchOffset() const
      {
          return synch_offset / slow_down_factor;
      }

    void setSynchMode( bool value );

    void setTeamLeftStart( std::string start );
    void setTeamRightStart( std::string start );

    void setTextLogDir( std::string str )
      {
          text_log_dir = tildeExpand( str );
      }
    void setGameLogDir( std::string str )
      {
          game_log_dir = tildeExpand( str );
      }
    void setKAwayLogDir( std::string str )
      {
          kaway_log_dir = tildeExpand( str );
      }

    void setCoachMsgFile( std::string str )
      {
          M_coach_msg_file = tildeExpand( str );
      }


    void setSlowDownFactor();

public:

    const
    VerMap & verMap() const
      {
          return m_ver_map;
      }

    server_params_t convertToStruct() const;

    bool getInt( const std::string & param, int & value ) const;
    bool getBool( const std::string & param, bool & value ) const;
    bool getDoub( const std::string & param, double & value ) const;
    bool getStr( const std::string & param, std::string & value ) const;


    // access methods

    const double & goalWidth() const { return gwidth; }
    const double & inertiaMoment() const { return inertia_moment; }
    const double & playerSize() const { return psize; }
    const double & playerDecay() const { return pdecay; }
    const double & playerRand() const { return prand; }
    const double & playerWeight() const { return pweight; }
    const double & playerSpeedMax() const { return pspeed_max; }
    const double & playerAccelMax() const { return paccel_max; }
    const double & staminaMax() const { return stamina_max; }
    const double & staminaInc() const { return stamina_inc; }
    const double & recoverInit() const { return recover_init; }
		const double & recoverDecThr() const { return recover_dthr; }
    const double & recoverMin() const { return recover_min; }
		const double & recoverDec() const { return recover_dec; }
		const double & effortInit() const { return effort_init; }
		const double & effortDecThr() const { return  effort_dthr; }
		const double & effortMin() const { return  effort_min; }
		const double & effortDec() const { return effort_dec; }
		const double & effortIncThr() const { return  effort_ithr; }
		const double & effortInc() const { return  effort_inc; }
		const double & kickRand() const { return  kick_rand; }
		bool teamActuatorNoise() const { return team_actuator_noise; }
		const double & playerRandFactorLeft() const { return prand_factor_l; }
		const double & playerRandFactorRight() const { return  prand_factor_r; }
		const double & kickRandFactorLeft() const { return  kick_rand_factor_l; }
		const double & kickRandFactorRight() const { return  kick_rand_factor_r; }

		const double & ballSize() const { return bsize; }
		const double & ballDecay() const { return bdecay; }
		const double & ballRand() const { return brand; }
		const double & ballWeight() const { return bweight; }
		const double & ballSpeedMax() const { return bspeed_max; }
		const double & ballAccelMax() const { return baccel_max; }
		const double & dashPowerRate() const { return dprate; }
		const double & kickPowerRate() const { return kprate; }
		const double & kickableMargin() const { return kmargin; }
		const double & controlRadius() const { return ctlradius; }
    const double & CtlRadiusWidth() const { return ctlradius_width; }

    const double & maxPower() const { return maxp; }
		const double & minPower() const { return minp; }
		const double & maxMoment() const { return maxm; }
		const double & minMoment() const { return minm; }
		const double & maxNeckMoment() const { return maxnm; }
		const double & minNeckMoment() const { return minnm; }
		const double & maxNeckAngle() const { return maxn; }
		const double & minNeckAngle() const { return minn; }

    double visAngle() const { return Deg2Rad(visangle); }
    const double & visAngleDeg() const { return visangle; }
		const double & visibleDistance() const{ return visdist; }
		const double & windDir() const { return windir; }
		const double & windForce() const { return winforce; }
		const double & windAngle() const { return winang; }
		const double & windRand() const { return winrand; }
		const double & windWeight() const { return WIND_WEIGHT; }

    const double & kickableArea() const { return kickable_area; }

		const double & catchAreaLength() const { return catch_area_l; }
		const double & catchAreaWidth() const { return catch_area_w; }
		const double & catchProb() const { return catch_prob; }
    int goalieMaxMoves() const { return goalie_max_moves; }

    bool keepAwayMode() const { return kaway; }
    const double & keepAwayLength() const { return ka_length; }
    const double & keepAwayWidth() const { return ka_width; }

		const double & cornerKickMargin() const { return ckmargin; }
		const double & offsideActiveArea() const { return offside_area; }

		bool windNone() const { return win_no; }
		bool windRandom() const { return win_random; }

    rcss::net::Addr::PortType playerPort() const { return portnum; }
    rcss::net::Addr::PortType offlineCoachPort() const { return coach_pnum; }
    rcss::net::Addr::PortType onlineCoachPort() const { return olcoach_pnum; }

		unsigned int freeformCountMax() const { return (unsigned int)say_cnt_max; }
		unsigned int freeformMsgSize() const { return (unsigned int)SayCoachMsgSize; }

		int clangWinSize() const { return clang_win_size; }
		int clangDefineWin() const { return clang_define_win; }
		int clangMetaWin() const { return clang_meta_win; }
		int clangAdviceWin() const { return clang_advice_win; }
		int clangInfoWin() const { return clang_info_win; }
		int clangMessDelay() const { return clang_mess_delay; }
		int clangMessPerCycle() const{ return clang_mess_per_cycle; }

    int halfTime() const { return half_time; }
    int dropTime() const { return drop_time; }
    int nrNormalHalfs() const { return nr_normal_halfs; }
    int nrExtraHalfs() const { return nr_extra_halfs; }
    bool penaltyShootOuts() const { return penalty_shoot_outs; }

    int penBeforeSetupWait() const { return pen_before_setup_wait; }
    int penSetupWait() const { return pen_setup_wait; }
    int penReadyWait() const { return pen_ready_wait; }
    int penTakenWait() const { return pen_taken_wait; }
    int penNrKicks() const { return pen_nr_kicks; }
    int penMaxExtraKicks() const { return pen_max_extra_kicks; }
    const double & penDistX() const { return  pen_dist_x; }
    bool penRandomWinner() const { return pen_random_winner; }
    bool penAllowMultiKicks() const { return pen_allow_mult_kicks; }
    const double & penMaxGoalieDistX() const { return  pen_max_goalie_dist_x; }
    bool penCoachMovesPlayers() const { return pen_coach_moves_players; }

    int simStep() const { return sim_st; }
    int senseBodyStep() const { return sb_step; }
    int coachVisualStep() const { return sv_st; }
    int sendStep() const { return send_st; }
    int lcmStep() const { return lcm_st; }
    int recvStep() const { return recv_st; }

		int catchBanCycle() const { return cban_cycle; }
		int slowDownFactor() const { return slow_down_factor; }
		bool useOffside() const { return useoffside; }
		bool kickOffOffside() const { return kickoffoffside; }
		const double & offsideKickMargin() const { return offside_kick_margin; }

		const double & audioDist() const { return audio_dist; }

		const double & distQStep() const { return dist_qstep; }
		const double & landQStep() const { return land_qstep; }

#ifdef NEW_QSTEP
    const Vlaue & dirQStep() const { return dir_qstep; }
    const double & distQStepL() const
      {
          return ( ! defined_qstep_l ? dist_qstep : dist_qstep_l );
      }
    const double & distQStepR() const
      {
          return ( ! defined_qstep_r ? dist_qstep : dist_qstep_r );
      }
    const double & landQStepL() const
      {
          return ( ! defined_qstep_l_l ? land_qstep : land_qstep_l );
      }
    const double & landQStepR() const
      {
          return ( ! defined_qstep_r_l ? land_qstep : land_qstep_r );
      }
    const double & dirQStepL() const
      {
          return ( ! defined_qstep_dir_l ? dir_qstep : dir_qstep_l );
      }
    const double & dirQStepR() const
      {
          return ( ! defined_qstep_dir_r ? dir_qstep : dir_qstep_r );
      }
#endif

    bool verboseMode() const { return verbose; }
    bool coachMode() const { return CoachMode; }
		bool coachWithRefereeMode() const { return CwRMode; }
		bool coachOldHear() const { return old_hear; }

    bool synchMode() const { return synch_mode; }
    int synchOffset() const { return synch_offset * slow_down_factor; }
    unsigned int synchMicroSleep() const { return (unsigned int)synch_micro_sleep; }

		int startGoalLeft() const { return start_goal_l; }
		int startGoalRight() const { return start_goal_r; }

		bool fullstateLeft() const { return fullstate_l; }
		bool fullstateRight() const { return fullstate_r; }

		const double & slownessOnTopForLeft() const { return slowness_on_top_for_left_team; }
		const double & slownessOnTopForRight() const { return slowness_on_top_for_right_team; }

    const std::string & landmarkFile() const { return landmark_file; }

		bool sendComms() const { return send_comms; }

		bool textLogging() const { return text_logging; }
		bool gameLogging() const { return game_logging; }
		int gameLogVersion() const { return game_log_version; }
		const std::string & textLogDir() const { return text_log_dir; }
		const std::string & gameLogDir() const { return game_log_dir; }
		const std::string & textLogFixedName() const { return text_log_fixed_name; }
		const std::string & gameLogFixedName() const { return game_log_fixed_name; }
		int textLogFixed() const { return text_log_fixed; }
		int gameLogFixed() const { return game_log_fixed; }
		int textLogDated() const { return text_log_dated; }
		int gameLogDated() const { return game_log_dated; }
		const std::string & logDateFormat() const { return log_date_format; }
		int logTimes() const { return log_times; }
		int recordMessages() const { return record_messages; }
		int textLogCompression() const { return text_log_compression; }
		int gameLogCompression() const { return game_log_compression; }

		bool profile() const { return M_profile; }

    bool kawayLogging() const { return kaway_logging; }
    const std::string & kawayLogDir() const { return kaway_log_dir; }
    const std::string & kawayLogFixedName() const { return kaway_log_fixed_name; }
    bool kawayLogFixed() const { return kaway_log_fixed; }
    bool kawayLogDated() const { return kaway_log_dated; }

    int kawayStart() const { return kaway_start; }

    unsigned int pointToBan() const { return (unsigned int)M_point_to_ban; }
    unsigned int pointToDuration() const { return (unsigned int)M_point_to_duration; }

    unsigned int sayMsgSize() const { return (unsigned int)M_say_msg_size; }
    unsigned int hearMax() const { return (unsigned int)M_hear_max; }
    unsigned int hearInc() const { return (unsigned int)M_hear_inc; }
    unsigned int hearDecay() const { return (unsigned int)M_hear_decay; }

    const double & tackleDist() const { return M_tackle_dist; }
    const double & tackleBackDist() const { return M_tackle_back_dist; }
    const double & tackleWidth() const { return M_tackle_width; }
    const double & tackleExponent() const { return M_tackle_exponent; }
    int tackleCycles() const { return M_tackle_cycles; }
    const double & tacklePowerRate() const { return M_tackle_power_rate; }

    unsigned int freeformWaitPeriod() const { return (unsigned int)M_freeform_wait_period; }
    unsigned int freeformSendPeriod() const { return (unsigned int)M_freeform_send_period; }

    bool freeKickFaults() const { return M_free_kick_faults; }
    bool backPasses() const { return M_back_passes; }
    bool properGoalKicks() const { return M_proper_goal_kicks; }
    const double & stoppedBallVel() const { return M_stopped_ball_vel; }
    int maxGoalKicks() const { return M_max_goal_kicks; }

    int clangDelWin() const { return M_clang_del_win; }
    int clangRuleWin() const { return M_clang_rule_win; }

    bool autoMode() const { return M_auto_mode; }
    int kickOffWait() const { return M_kick_off_wait * sim_st / recv_st; }
    int connectWait() const { return M_connect_wait * sim_st / recv_st; }
    int gameOverWait() const { return M_game_over_wait; }
    const std::string & teamLeftStart() const { return M_team_l_start; }
    const std::string & teamRightStart() const { return M_team_r_start; }

    double goalPostRadius() const { return 0.06; }

    // v.11
    const double & ballStuckArea() const { return M_ball_stuck_area; }
    const std::string & coachMsgFile() const { return M_coach_msg_file; }

    // v.12
    const double & maxTacklePower() const { return M_max_tackle_power; }
    const double & maxBackTacklePower() const { return M_max_back_tackle_power; }
    const double & playerSpeedMaxMin() const { return M_player_speed_max_min; }
    const double & extraStamina() const { return M_extra_stamina; }
    int maxMonitors() const { return M_max_monitors; }
    int synchSeeOffset() const { return M_synch_see_offset; }

    // test
		const double & reliableCatchAreaLength() const { return M_reliable_catch_area_l; }
		const double & minCatchProbability() const { return M_min_catch_probability; }

};

#endif

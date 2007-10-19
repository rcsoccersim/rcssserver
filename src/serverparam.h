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


#ifndef _SERVER_PARAM_H_
#define _SERVER_PARAM_H_

#include "rcssserverconfig.hpp"

#include "utility.h"
#include "types.h"
#include <rcssbase/net/udpsocket.hpp>

#include <boost/shared_ptr.hpp>
#include <map>

#define _USE_MATH_DEFINES
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

    static const int SIMULATOR_STEP_INTERVAL_MSEC; /* milli-sec */
    static const int UDP_RECV_STEP_INTERVAL_MSEC; /* milli-sec */
    static const int UDP_SEND_STEP_INTERVAL_MSEC; /* milli-sec */
    static const int SENSE_BODY_INTERVAL_MSEC; /* milli-sec */
    static const int SEND_VISUALINFO_INTERVAL_MSEC; /* milli-sec */

    static const double IMPARAM; /* Inertia-Moment Parameter */

    static const double BALL_SIZE;
    static const double BALL_DECAY;
    static const double BALL_RAND;
    static const double BALL_WEIGHT;
    static const double BALL_T_VEL;
    static const double BALL_SPEED_MAX;
    // th 6.3.00
    static const double BALL_ACCEL_MAX;

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
    static const double MIN_TACKLE_POWER;
    static const double MAX_TACKLE_POWER;

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

		Value gwidth; /* goal width */
		Value inertia_moment; /* intertia moment for turn */
		Value psize; /* player size */
		Value pdecay; /* player decay */
		Value prand; /* player rand */
		Value pweight; /* player weight */
		Value pspeed_max; /* player speed max */
		// th 6.3.00
		Value paccel_max; /* player acceleration max */
		//
		Value stamina_max; /* player stamina max */
		Value stamina_inc; /* player stamina inc */
		Value recover_init; /* player recovery init */
		Value recover_dthr; /* player recovery decriment threshold */
		Value recover_min; /* player recovery min */
		Value recover_dec; /* player recovery decriment */
		Value effort_init; /* player dash effort init */
		Value effort_dthr; /* player dash effort decriment threshold */
		Value effort_min; /* player dash effrot min */
		Value effort_dec; /* player dash effort decriment */
		Value effort_ithr; /* player dash effort incriment threshold */
		Value effort_inc; /* player dash effort incriment */
		// pfr 8/14/00: for RC2000 evaluation
		Value kick_rand; /* noise added directly to kicks */
		bool team_actuator_noise; /* flag whether to use team specific actuator noise */
		Value prand_factor_l; /* factor to multiple prand for left team */
		Value prand_factor_r; /* factor to multiple prand for right team */
		Value kick_rand_factor_l; /* factor to multiple kick_rand for left team */
		Value kick_rand_factor_r; /* factor to multiple kick_rand for right team */

		Value bsize; /* ball size */
		Value bdecay; /* ball decay */
		Value brand; /* ball rand */
		Value bweight; /* ball weight */
		Value bspeed_max; /* ball speed max */
		// th 6.3.00
		Value baccel_max; /* ball acceleration max */
		//
		Value dprate; /* dash power rate */
		Value kprate;	/* kick power rate */
		Value kmargin; /* kickable margin */
		Value ctlradius; /* control radius */
		Value ctlradius_width; /* (control radius) - (plyaer size) */
    //public:
		Value maxp; /* max power */
		Value minp; /* min power */
		Value maxm;	/* max moment */
		Value minm;	/* min moment */
		Value maxnm; /* max neck moment */
		Value minnm; /* min neck moment */
		Value maxn; /* max neck angle */
		Value minn; /* min neck angle */
    //private:
		Value visangle; /* visible angle */
    //public:
		Value visdist; /* visible distance */
		Angle windir; /* wind direction */
		Value winforce; /* wind force */
		Value winang; /* wind angle for rand */
		Value winrand; /* wind force for force */
    //private:
		Value kickable_area; /* kickable_area */
    //public:
		Value catch_area_l; /* goalie catchable area length */
		Value catch_area_w; /* goalie catchable area width */
		Value catch_prob; /* goalie catchable possibility */
    int goalie_max_moves; /* goalie max moves after a catch */
    bool kaway; /* keepaway mode on/off */
    Value ka_length; /* keepaway region length */
    Value ka_width; /* keepaway region width */
		Value ckmargin; /* corner kick margin */
		Value offside_area; /* offside active area size */
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
		Value offside_kick_margin; /* offside kick margin */
		Value audio_dist; /* audio cut off distance */
		Value dist_qstep; /* quantize step of distance */
		Value land_qstep; /* quantize step of distance for landmark */
#ifdef NEW_QSTEP
		Value dir_qstep; /* quantize step of direction */
    //private:
		Value dist_qstep_l; /* team right quantize step of distance */
		Value dist_qstep_r; /* team left quantize step of distance */
		Value land_qstep_l; /* team right quantize step of distance for landmark */
		Value land_qstep_r; /* team left quantize step of distance for landmark */
		Value dir_qstep_l; /* team left quantize step of direction */
		Value dir_qstep_r; /* team right quantize step of direction */
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
    bool timer_loaded;
    //private:
		int synch_offset; /* pfr:SYNCH the offset from the start of the cycle to tell players to run */
    //public:
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
    double M_min_tackle_power;
    double M_max_tackle_power;

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

    double M_ball_stuck_area; /* threshold distance checked by BallStuckRef */

    std::string M_coach_msg_file; /* file name that contains the messages sent to coaches */

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
          if( value != 0.0 )
              return value;
          else
              return EPS;
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

    void setSlowDownFactor( int value )
      {
          if ( value <= 0 )
          {
              return;
          }
          // undo the effect of the last slow_down_factor
          sim_st /= slow_down_factor;
          sb_step /= slow_down_factor;
          sv_st /= slow_down_factor;
          send_st /= slow_down_factor;
          synch_offset /= slow_down_factor;

          // set the slow_down_factor
          slow_down_factor = value;

          // apply the slow_down_factor to all the depandant varaibles
          sim_st *= slow_down_factor;
          sb_step *= slow_down_factor;
          sv_st *= slow_down_factor;
          send_st *= slow_down_factor;
          synch_offset *= slow_down_factor;
          lcm_st = lcm( sim_st,
                        lcm( send_st,
                             lcm( recv_st,
                                  lcm( sb_step,
                                       lcm( sv_st,
                                            ( synch_mode ? synch_offset : 1 ) ) ) ) ) );
      }

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

public:

    const
    VerMap & verMap() const
      {
          return m_ver_map;
      }

    server_params_t convertToStruct();

    bool getInt( const std::string & param, int & value ) const;
    bool getBool( const std::string & param, bool & value ) const;
    bool getDoub( const std::string & param, double & value ) const;
    bool getStr( const std::string & param, std::string & value ) const;


    // access methods

    const Value & goalWidth() const { return gwidth; }
    const Value & inertiaMoment() const { return inertia_moment; }
    const Value & playerSize() const { return psize; }
    const Value & playerDecay() const { return pdecay; }
    const Value & playerRand() const { return prand; }
    const Value & playerWeight() const { return pweight; }
    const Value & playerSpeedMax() const { return pspeed_max; }
    const Value & playerAccelMax() const { return paccel_max; }
    const Value & staminaMax() const { return stamina_max; }
    const Value & staminaInc() const { return stamina_inc; }
    const Value & recoverInit() const { return recover_init; }
		const Value & recoverDecThr() const { return recover_dthr; }
    const Value & recoverMin() const { return recover_min; }
		const Value & recoverDec() const { return recover_dec; }
		const Value & effortInit() const { return effort_init; }
		const Value & effortDecThr() const { return  effort_dthr; }
		const Value & effortMin() const { return  effort_min; }
		const Value & effortDec() const { return effort_dec; }
		const Value & effortIncThr() const { return  effort_ithr; }
		const Value & effortInc() const { return  effort_inc; }
		const Value & kickRand() const { return  kick_rand; }
		bool teamActuatorNoise() const { return team_actuator_noise; }
		const Value & playerRandFactorLeft() const { return prand_factor_l; }
		const Value & playerRandFactorRight() const { return  prand_factor_r; }
		const Value & kickRandFactorLeft() const { return  kick_rand_factor_l; }
		const Value & kickRandFactorRight() const { return  kick_rand_factor_r; }

		const Value & ballSize() const { return bsize; }
		const Value & ballDecay() const { return bdecay; }
		const Value & ballRand() const { return brand; }
		const Value & ballWeight() const { return bweight; }
		const Value & ballSpeedMax() const { return bspeed_max; }
		const Value & ballAccelMax() const { return baccel_max; }
		const Value & dashPowerRate() const { return dprate; }
		const Value & kickPowerRate() const { return kprate; }
		const Value & kickableMargin() const { return kmargin; }
		const Value & controlRadius() const { return ctlradius; }
    const Value & CtlRadiusWidth() const { return ctlradius_width; }

    const Value & maxPower() const { return maxp; }
		const Value & minPower() const { return minp; }
		const Value & maxMoment() const { return maxm; }
		const Value & minMoment() const { return minm; }
		const Value & maxNeckMoment() const { return maxnm; }
		const Value & minNeckMoment() const { return minnm; }
		const Value & maxNeckAngle() const { return maxn; }
		const Value & minNeckAngle() const { return minn; }

    double visAngle() const { return Deg2Rad(visangle); }
    const Value & visAngleDeg() const { return visangle; }
		const Value & visibleDistance() const{ return visdist; }
		const Angle & windDir() const { return windir; }
		const Value & windForce() const { return winforce; }
		const Value & windAngle() const { return winang; }
		const Value & windRand() const { return winrand; }

    const Value & kickableArea() const { return kickable_area; }

		const Value & catchAreaLength() const { return catch_area_l; }
		const Value & catchAreaWidth() const { return catch_area_w; }
		const Value & catchProb() const { return catch_prob; }
    int goalieMaxMoves() const { return goalie_max_moves; }

    bool keepAwayMode() const { return kaway; }
    const Value & keepAwayLength() const { return ka_length; }
    const Value & keepAwayWidth() const { return ka_width; }

		const Value & cornerKickMargin() const { return ckmargin; }
		const Value & offsideActiveArea() const { return offside_area; }

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
		const Value & offsideKickMargin() const { return offside_kick_margin; }

		const Value & audioDist() const { return audio_dist; }

		const Value & distQStep() const { return dist_qstep; }
		const Value & landQStep() const { return land_qstep; }

#ifdef NEW_QSTEP
    const Vlaue & dirQStep() const { return dir_qstep; }
    const Value & distQStepL() const
      {
          return ( ! defined_qstep_l ? dist_qstep : dist_qstep_l );
      }
    const Value & distQStepR() const
      {
          return ( ! defined_qstep_r ? dist_qstep : dist_qstep_r );
      }
    const Value & landQStepL() const
      {
          return ( ! defined_qstep_l_l ? land_qstep : land_qstep_l );
      }
    const Value & landQStepR() const
      {
          return ( ! defined_qstep_r_l ? land_qstep : land_qstep_r );
      }
    const Value & dirQStepL() const
      {
          return ( ! defined_qstep_dir_l ? dir_qstep : dir_qstep_l );
      }
    const Value & dirQStepR() const
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
    unsigned int tackleCycles() const { return (unsigned int)M_tackle_cycles; }
    const double & tacklePowerRate() const { return M_tackle_power_rate; }
    const double & minTacklePower() const { return M_min_tackle_power; }
    const double & maxTacklePower() const { return M_max_tackle_power; }

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

    const double & ballStuckArea() const { return M_ball_stuck_area; }

    const std::string & coachMsgFile() const { return M_coach_msg_file; }

};

#endif

/*
 *Header:
 *File: typedefs.h
 *Author: Noda Itsuki
 *Date: 1995/02/24
 *EndHeader:
 */

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
#ifndef RCSSSERVER_TYPES_H
#define RCSSSERVER_TYPES_H

#include "param.h"

#include <boost/cstdint.hpp>

typedef boost::int16_t Int16;
typedef boost::int32_t Int32;

enum PlayerState {
    DISABLE =         0x00000000,
    STAND =           0x00000001,
    KICK =            0x00000002,
    KICK_FAULT =      0x00000004,
    GOALIE =          0x00000008,
    CATCH =           0x00000010,
    CATCH_FAULT =     0x00000020,
    BALL_TO_PLAYER =  0x00000040,
    PLAYER_TO_BALL =  0x00000080,
    DISCARD =         0x00000100,
    LOST =            0x00000200, // [I.Noda:00/05/13] added for 3D viewer/commentator/small league
    BALL_COLLIDE =    0x00000400, // player collided with the ball
    PLAYER_COLLIDE =  0x00000800, // player collided with another player
    TACKLE =          0x00001000,
    TACKLE_FAULT =    0x00002000,
    BACK_PASS =       0x00004000,
    FREE_KICK_FAULT = 0x00008000,
    POST_COLLIDE =    0x00010000, // player collided with goal posts
    FOUL_CHARGED =    0x00020000, // player is frozen by intentional tackle foul
    YELLOW_CARD =     0x00040000,
    RED_CARD =        0x00080000,
};

enum Side {
    LEFT = 1,
    NEUTRAL = 0,
    RIGHT = -1
};

#define PLAYMODE_STRINGS {"",                   \
            "before_kick_off",                  \
            "time_over",                        \
            "play_on",                          \
            "kick_off_l",                       \
            "kick_off_r",                       \
            "kick_in_l",                        \
            "kick_in_r",                        \
            "free_kick_l",                      \
            "free_kick_r",                      \
            "corner_kick_l",                    \
            "corner_kick_r",                    \
            "goal_kick_l",                      \
            "goal_kick_r",                      \
            "goal_l",                           \
            "goal_r",                           \
            "drop_ball",                        \
            "offside_l",                        \
            "offside_r",                        \
            "penalty_kick_l",                   \
            "penalty_kick_r",                   \
            "first_half_over",                  \
            "pause",                            \
            "human_judge",                      \
            "foul_charge_l",                    \
            "foul_charge_r",                    \
            "foul_push_l",                      \
            "foul_push_r",                      \
            "foul_multiple_attack_l",           \
            "foul_multiple_attack_r",           \
            "foul_ballout_l",                   \
            "foul_ballout_r",                   \
            "back_pass_l",                      \
            "back_pass_r",                      \
            "free_kick_fault_l",                \
            "free_kick_fault_r",                \
            "catch_fault_l",                    \
            "catch_fault_r",                    \
            "indirect_free_kick_l",             \
            "indirect_free_kick_r",             \
            "penalty_setup_l",                  \
            "penalty_setup_r",                  \
            "penalty_ready_l",                  \
            "penalty_ready_r",                  \
            "penalty_taken_l",                  \
            "penalty_taken_r",                  \
            "penalty_miss_l",                   \
            "penalty_miss_r",                   \
            "penalty_score_l",                  \
            "penalty_score_r"                   \
            }

enum PlayMode {
    PM_Null,
    PM_BeforeKickOff,
    PM_TimeOver,
    PM_PlayOn,
    PM_KickOff_Left,
    PM_KickOff_Right,
    PM_KickIn_Left,
    PM_KickIn_Right,
    PM_FreeKick_Left,
    PM_FreeKick_Right,
    PM_CornerKick_Left,
    PM_CornerKick_Right,
    PM_GoalKick_Left,
    PM_GoalKick_Right,
    PM_AfterGoal_Left,
    PM_AfterGoal_Right,
    PM_Drop_Ball,
    PM_OffSide_Left,
    PM_OffSide_Right,
    // [I.Noda:00/05/13] added for 3D viewer/commentator/small league
    PM_PK_Left,
    PM_PK_Right,
    PM_FirstHalfOver,
    PM_Pause,
    PM_Human,
    PM_Foul_Charge_Left,
    PM_Foul_Charge_Right,
    PM_Foul_Push_Left,
    PM_Foul_Push_Right,
    PM_Foul_MultipleAttacker_Left,
    PM_Foul_MultipleAttacker_Right,
    PM_Foul_BallOut_Left,
    PM_Foul_BallOut_Right,
    PM_Back_Pass_Left,
    PM_Back_Pass_Right,
    PM_Free_Kick_Fault_Left,
    PM_Free_Kick_Fault_Right,
    PM_CatchFault_Left,
    PM_CatchFault_Right,
    PM_IndFreeKick_Left,
    PM_IndFreeKick_Right,
    PM_PenaltySetup_Left,
    PM_PenaltySetup_Right,
    PM_PenaltyReady_Left,
    PM_PenaltyReady_Right,
    PM_PenaltyTaken_Left,
    PM_PenaltyTaken_Right,
    PM_PenaltyMiss_Left,
    PM_PenaltyMiss_Right,
    PM_PenaltyScore_Left,
    PM_PenaltyScore_Right,
    PM_MAX
};

enum MPObjectType {
    MPO_Ball,
    MPO_Player
};


const double SHOWINFO_SCALE = 16.0;
const double SHOWINFO_SCALE2 = 65536.0;

const int REC_OLD_VERSION = 1;
const int REC_VERSION_2 = 2;
const int REC_VERSION_3 = 3;
const int REC_VERSION_4 = 4;
const int REC_VERSION_5 = 5;
const int DEFAULT_REC_VERSION = REC_VERSION_5;

enum DispInfoMode {
    NO_INFO = 0,
    SHOW_MODE = 1,
    MSG_MODE = 2,
    DRAW_MODE = 3,
    BLANK_MODE = 4,
    PM_MODE = 5,
    TEAM_MODE = 6,
    PT_MODE = 7,
    PARAM_MODE = 8,
    PPARAM_MODE = 9,
};

enum DrawMode {
    DrawClear = 0,
    DrawPoint = 1,
    DrawCircle = 2,
    DrawLine = 3,
};

enum BoardType {
    MSG_BOARD = 1,
    LOG_BOARD = 2,
};

struct pos_t {
    Int16	enable;
    Int16	side;
    Int16	unum;
    Int16	angle;
    Int16	x;
    Int16	y;
};

struct team_t {
    char	name[16];
    Int16	score;
};

struct showinfo_t {
    char pmode;
    team_t team[2];
    pos_t	pos[MAX_PLAYER * 2 + 1];
    Int16	time;
};

const int max_message_length_for_display = 2048;

struct msginfo_t {
    Int16	board;
    char	message[max_message_length_for_display];
};

const int COLOR_NAME_MAX = 64;

struct pointinfo_t {
    Int16	x;
    Int16	y;
    char	color[COLOR_NAME_MAX];
};

struct circleinfo_t {
    Int16	x;
    Int16	y;
    Int16	r;
    char	color[COLOR_NAME_MAX];
};

struct lineinfo_t {
    Int16	x1;
    Int16	y1;
    Int16	x2;
    Int16	y2;
    char	color[COLOR_NAME_MAX];
};

struct drawinfo_t {
    Int16 mode;
    union {
        pointinfo_t		pinfo;
        circleinfo_t	cinfo;
        lineinfo_t		linfo;
    } object;
};

struct dispinfo_t {
    Int16	mode;
    union {
        showinfo_t	show;
        msginfo_t	msg;
        drawinfo_t	draw;
    } body;
};

struct ball_t {
    Int32 x;
    Int32 y;
    Int32 deltax;
    Int32 deltay;
};

struct player_t {
    Int16 mode;
    Int16 type;
    Int32 x;
    Int32 y;
    Int32 deltax;
    Int32 deltay;
    Int32 body_angle;
    Int32 head_angle;
    Int32 view_width;
    Int16 view_quality;
    Int32 stamina;
    Int32 effort;
    Int32 recovery;
    Int16 kick_count;
    Int16 dash_count;
    Int16 turn_count;
    Int16 say_count;
    Int16 tneck_count;
    Int16 catch_count;
    Int16 move_count;
    Int16 chg_view_count;
};

struct showinfo_t2 {
    char pmode;
    team_t team[2];
    ball_t ball;
    player_t pos[MAX_PLAYER * 2];
    Int16 time;
};

struct short_showinfo_t2 {
    ball_t ball;
    player_t pos[MAX_PLAYER * 2];
    Int16 time;
};

struct player_type_t {
    Int16 id;
    Int32 player_speed_max;
    Int32 stamina_inc_max;
    Int32 player_decay;
    Int32 inertia_moment;
    Int32 dash_power_rate;
    Int32 player_size;
    Int32 kickable_margin;
    Int32 kick_rand;
    Int32 extra_stamina;
    Int32 effort_max;
    Int32 effort_min;

    Int32 kick_power_rate;
    Int32 foul_detect_probability;
    Int32 catchable_area_l_stretch;

    // spare variables which are to be used for paramenter added in the future
    //Int32 sparelong1;  replaced by kick_power_rate
    //Int32 sparelong2;  replaced by foul_detect_probability
    //Int32 sparelong3;  replaced by catchable_area_l_stretch
    Int32 sparelong4;
    Int32 sparelong5;
    Int32 sparelong6;
    Int32 sparelong7;
    Int32 sparelong8;
    Int32 sparelong9;
    Int32 sparelong10;
};

struct server_params_t {
    Int32 gwidth;					/* goal width */
    Int32 inertia_moment;			/* intertia moment for turn */
    Int32 psize;					/* player size */
    Int32 pdecay;					/* player decay */
    Int32 prand;					/* player rand */
    Int32 pweight;					/* player weight */
    Int32 pspeed_max;				/* player speed max */
    // th 6.3.00
    Int32 paccel_max;				/* player acceleration max */
    //
    Int32 stamina_max;				/* player stamina max */
    Int32 stamina_inc;				/* player stamina inc */
    Int32 recover_init;			/* player recovery init */
    Int32 recover_dthr;			/* player recovery decriment threshold */
    Int32 recover_min;				/* player recovery min */
    Int32 recover_dec;				/* player recovery decriment */
    Int32 effort_init;				/* player dash effort init */
    Int32 effort_dthr;				/* player dash effort decriment threshold */
    Int32 effort_min;				/* player dash effrot min */
    Int32 effort_dec;				/* player dash effort decriment */
    Int32 effort_ithr;				/* player dash effort incriment threshold */
    Int32 effort_inc;				/* player dash effort incriment */
    // pfr 8/14/00: for RC2000 evaluation
    Int32 kick_rand;                                /* noise added directly to kicks */
    Int16 team_actuator_noise;                        /* flag whether to use team specific actuator noise */
    Int32 prand_factor_l;                           /* factor to multiple prand for left team */
    Int32 prand_factor_r;                           /* factor to multiple prand for right team */
    Int32 kick_rand_factor_l;                       /* factor to multiple kick_rand for left team */
    Int32 kick_rand_factor_r;                       /* factor to multiple kick_rand for right team */

    Int32 bsize;					/* ball size */
    Int32 bdecay;					/* ball decay */
    Int32 brand;					/* ball rand */
    Int32 bweight;					/* ball weight */
    Int32 bspeed_max;				/* ball speed max */
    // th 6.3.00
    Int32 baccel_max;				/* ball acceleration max */
    //
    Int32 dprate;					/* dash power rate */
    Int32 kprate;					/* kick power rate */
    Int32 kmargin;					/* kickable margin */
    Int32 ctlradius;				/* control radius */
    Int32 ctlradius_width;			/* (control radius) - (plyaer size) */
    Int32 maxp;					/* max power */
    Int32 minp;					/* min power */
    Int32 maxm;					/* max moment */
    Int32 minm;					/* min moment */
    Int32 maxnm;					/* max neck moment */
    Int32 minnm;					/* min neck moment */
    Int32 maxn;					/* max neck angle */
    Int32 minn;					/* min neck angle */
    Int32 visangle;				/* visible angle */
    Int32 visdist;					/* visible distance */
    Int32 windir;					/* wind direction */
    Int32 winforce;				/* wind force */
    Int32 winang;					/* wind angle for rand */
    Int32 winrand;					/* wind force for force */
    Int32 kickable_area;			/* kickable_area */
    Int32 catch_area_l;			/* goalie catchable area length */
    Int32 catch_area_w;			/* goalie catchable area width */
    Int32 catch_prob;				/* goalie catchable possibility */
    Int16   goalie_max_moves;                 /* goalie max moves after a catch */
    Int32 ckmargin;				/* corner kick margin */
    Int32 offside_area;			/* offside active area size */
    Int16 win_no;					/* wind factor is none */
    Int16 win_random;				/* wind factor is random */
    Int16 say_cnt_max;				/* max count of coach SAY */
    Int16 SayCoachMsgSize;				/* max length of coach SAY */
    Int16 clang_win_size;
    Int16 clang_define_win;
    Int16 clang_meta_win;
    Int16 clang_advice_win;
    Int16 clang_info_win;
    Int16 clang_mess_delay;
    Int16 clang_mess_per_cycle;
    Int16 half_time;					/* half time */
    Int16 sim_st;					/* simulator step interval msec */
    Int16 send_st;					/* udp send step interval msec */
    Int16 recv_st;					/* udp recv step interval msec */
    Int16 sb_step;					/* sense_body interval step msec */
    Int16 lcm_st;		                        /* lcm of all the above steps msec */
    Int16 M_say_msg_size;				/* string size of say message */
    Int16 M_hear_max;					/* player hear_capacity_max */
    Int16 M_hear_inc;					/* player hear_capacity_inc */
    Int16 M_hear_decay;				/* player hear_capacity_decay */
    Int16 cban_cycle;				/* goalie catch ban cycle */
    Int16 slow_down_factor;                          /* factor to slow down simulator and send intervals */
    Int16 useoffside;				/* flag for using off side rule */
    Int16 kickoffoffside;			/* flag for permit kick off offside */
    Int32 offside_kick_margin;		/* offside kick margin */
    Int32 audio_dist;				/* audio cut off distance */
    Int32 dist_qstep;				/* quantize step of distance */
    Int32 land_qstep;				/* quantize step of distance for landmark */
    Int32 dir_qstep;				/* quantize step of direction */
    Int32 dist_qstep_l;			/* team right quantize step of distance */
    Int32 dist_qstep_r;			/* team left quantize step of distance */
    Int32 land_qstep_l;			/* team right quantize step of distance for landmark */
    Int32 land_qstep_r;			/* team left quantize step of distance for landmark */
    Int32 dir_qstep_l;				/* team left quantize step of direction */
    Int32 dir_qstep_r;				/* team right quantize step of direction */
    Int16 CoachMode;				/* coach mode */
    Int16 CwRMode;					/* coach with referee mode */
    Int16 old_hear;					/* old format for hear command (coach) */
    Int16 sv_st;					/* online coach's look interval step */


    // spare variables which are to be used for paramenter added in the future
    Int32 slowness_on_top_for_left_team;
    Int32 slowness_on_top_for_right_team;
    Int32 ka_length;                    /* keepaway region length */
    Int32 ka_width;                     /* keepaway region width */
    // 11.0.0
    Int32 ball_stuck_area; // automatic drop ball threshold while play_on
    // 12.0.0
    Int32 max_tackle_power;
    Int32 max_back_tackle_power;

    Int32 tackle_dist;
    Int32 tackle_back_dist;
    Int32 tackle_width;

    Int16 start_goal_l;
    Int16 start_goal_r;
    Int16 fullstate_l;
    Int16 fullstate_r;
    Int16 drop_time;
    Int16 synch_mode;
    Int16 synch_offset;
    Int16 synch_micro_sleep;
    Int16 point_to_ban;
    Int16 point_to_duration;
    //Int16 kaway; /* keepaway mode on/off */ /* No room in struct */

    // no room in struct
    //  Int16 pen_before_setup_wait;
    //  Int16 pen_setup_wait;
    //  Int16 pen_ready_wait;
    //  Int16 pen_taken_wait;
    //  Int16 pen_nr_kicks;
    //  Int16 pen_max_extra_kicks;
    //  Int32 pen_dist_x;
    //  Int16 pen_random_winner;
    //  Int32 pen_max_goalie_dist_x;
    //  Int16 pen_allow_mult_kicks;


};

struct player_params_t {
    Int16 player_types;
    Int16 subs_max;
    Int16 pt_max;

    Int32 player_speed_max_delta_min;
    Int32 player_speed_max_delta_max;
    Int32 stamina_inc_max_delta_factor;

    Int32 player_decay_delta_min;
    Int32 player_decay_delta_max;
    Int32 inertia_moment_delta_factor;

    Int32 dash_power_rate_delta_min;
    Int32 dash_power_rate_delta_max;
    Int32 player_size_delta_factor;

    Int32 kickable_margin_delta_min;
    Int32 kickable_margin_delta_max;
    Int32 kick_rand_delta_factor;

    Int32 extra_stamina_delta_min;
    Int32 extra_stamina_delta_max;
    Int32 effort_max_delta_factor;
    Int32 effort_min_delta_factor;

    Int32 random_seed;

    Int32 new_dash_power_rate_delta_min;
    Int32 new_dash_power_rate_delta_max;
    Int32 new_stamina_inc_max_delta_factor;

    Int32 kick_power_rate_delta_min;
    Int32 kick_power_rate_delta_max;
    Int32 foul_detect_probability_delta_factor;

    Int32 catchable_area_l_stretch_min;
    Int32 catchable_area_l_stretch_max;

    //Int32 sparelong1;  replaced by seed
    //Int32 sparelong2;  replaced by new_dash_power_rate_delta_min
    //Int32 sparelong3;  replaced by new_dash_power_rate_delta_max
    //Int32 sparelong4;  replaced by new_stamina_inc_max_delta_factor
    //Int32 sparelong5;  replaced by kick_power_rate_delta_min
    //Int32 sparelong6;  replaced by kick_power_rate_delta_max
    //Int32 sparelong7;  replaced by foul_detect_probability_delta_factor
    //Int32 sparelong8;  replaced by catchable_area_l_stretch_min
    //Int32 sparelong9;  replaced by catchable_area_l_stretch_max
    Int32 sparelong10;

    Int16 allow_mult_default_type;
    //Int16 allow_default_goalie;

    //Int16 spareshort1; // replaced by allow_mult_default_type
    Int16 spareshort2; // replaced by allow_default_goalie
    Int16 spareshort3;
    Int16 spareshort4;
    Int16 spareshort5;
    Int16 spareshort6;
    Int16 spareshort7;
    Int16 spareshort8;
    Int16 spareshort9;
    Int16 spareshort10;

};

struct dispinfo_t2 {
    Int16 mode;
    union {
        showinfo_t2 		show;
        msginfo_t 		msg;
        player_type_t		ptinfo;
        server_params_t		sparams;
        player_params_t		pparams;
    } body;
};

#endif

/* -*- Mode: C -*-
 *Header:
 *File: param.h
 *Author: Noda Itsuki
 *Date: 1996/02/23
 *EndHeader:
 */

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

#ifndef _PARAM_H_
#define _PARAM_H_


/*
 *   for Simulator
 */

#define	TIMEDELTA	10					/* polling interval [milli-sec] */

/*
 *   for network
 */
#define DEFAULT_PORT_NUMBER		6000
#define COACH_PORT_NUMBER		6001
#define OLCOACH_PORT_NUMBER		6002
//#define MaxMesg					2048
//pfr 06/14/01: this is probably the maximum UDP size allowed in most applications
#define MaxMesg					8192
//this must be smaller because it will eventually be sent out with other info
#define MaxCoachMesg				(MaxMesg-38)
//We actually compare with one of the times already present
#define MaxCoachMesgWTime			(MaxCoachMesg + 6)
#define SEND 1
#define RECV 2
#define SUBS 4

#define	NO_INFO		0
#define	SHOW_MODE	1
#define	MSG_MODE	2
#define DRAW_MODE	3
#define	BLANK_MODE	4
#define	PM_MODE		5
#define	TEAM_MODE	6
#define PT_MODE		7
#define	PARAM_MODE	8
#define PPARAM_MODE	9


#define DrawClear	0
#define DrawPoint	1
#define DrawCircle	2
#define DrawLine	3

#define	MSG_BOARD	1
#define	LOG_BOARD	2

/*
 *   Objects
 */
#define MaxObject		128

#define GOAL_L_NAME		"(goal l)"
#define GOAL_R_NAME		"(goal r)"

#define LINE_L_NAME 	"(line l)"
#define LINE_R_NAME 	"(line r)"
#define LINE_T_NAME 	"(line t)"
#define LINE_B_NAME		"(line b)"

#define FLAG_C_NAME 	"(flag c)"
#define FLAG_CT_NAME	"(flag c t)"
#define FLAG_CB_NAME	"(flag c b)"
#define FLAG_LT_NAME	"(flag l t)"
#define FLAG_LB_NAME	"(flag l b)"
#define FLAG_RT_NAME	"(flag r t)"
#define FLAG_RB_NAME	"(flag r b)"

#define	FLAG_PLT_NAME	"(flag p l t)"
#define	FLAG_PLC_NAME	"(flag p l c)"
#define	FLAG_PLB_NAME	"(flag p l b)"
#define	FLAG_PRT_NAME	"(flag p r t)"
#define	FLAG_PRC_NAME	"(flag p r c)"
#define	FLAG_PRB_NAME	"(flag p r b)"

#define FLAG_GLT_NAME	"(flag g l t)"
#define FLAG_GLB_NAME	"(flag g l b)"
#define FLAG_GRT_NAME	"(flag g r t)"
#define FLAG_GRB_NAME	"(flag g r b)"

#define FLAG_TL50_NAME	"(flag t l 50)"
#define FLAG_TL40_NAME	"(flag t l 40)"
#define FLAG_TL30_NAME	"(flag t l 30)"
#define FLAG_TL20_NAME	"(flag t l 20)"
#define FLAG_TL10_NAME	"(flag t l 10)"
#define FLAG_T0_NAME	"(flag t 0)"
#define FLAG_TR10_NAME	"(flag t r 10)"
#define FLAG_TR20_NAME	"(flag t r 20)"
#define FLAG_TR30_NAME	"(flag t r 30)"
#define FLAG_TR40_NAME	"(flag t r 40)"
#define FLAG_TR50_NAME	"(flag t r 50)"

#define FLAG_BL50_NAME	"(flag b l 50)"
#define FLAG_BL40_NAME	"(flag b l 40)"
#define FLAG_BL30_NAME	"(flag b l 30)"
#define FLAG_BL20_NAME	"(flag b l 20)"
#define FLAG_BL10_NAME	"(flag b l 10)"
#define FLAG_B0_NAME	"(flag b 0)"
#define FLAG_BR10_NAME	"(flag b r 10)"
#define FLAG_BR20_NAME	"(flag b r 20)"
#define FLAG_BR30_NAME	"(flag b r 30)"
#define FLAG_BR40_NAME	"(flag b r 40)"
#define FLAG_BR50_NAME	"(flag b r 50)"

#define FLAG_LT30_NAME	"(flag l t 30)"
#define FLAG_LT20_NAME	"(flag l t 20)"
#define FLAG_LT10_NAME	"(flag l t 10)"
#define FLAG_L0_NAME	"(flag l 0)"
#define FLAG_LB10_NAME	"(flag l b 10)"
#define FLAG_LB20_NAME	"(flag l b 20)"
#define FLAG_LB30_NAME	"(flag l b 30)"

#define FLAG_RT30_NAME	"(flag r t 30)"
#define FLAG_RT20_NAME	"(flag r t 20)"
#define FLAG_RT10_NAME	"(flag r t 10)"
#define FLAG_R0_NAME	"(flag r 0)"
#define FLAG_RB10_NAME	"(flag r b 10)"
#define FLAG_RB20_NAME	"(flag r b 20)"
#define FLAG_RB30_NAME	"(flag r b 30)"

#define PLAYER_NAME_FORMAT	"(player %s %d)"
#define	PLAYER_NAME_FAR_FORMAT	"(player %s)"
#define	PLAYER_NAME_TOOFAR_FORMAT	"(player)"

#define O_TYPE_FLAG_NAME	"(Flag)"
#define O_TYPE_GOAL_NAME	"(Goal)"
#define O_TYPE_BALL_NAME	"(Ball)"
#define O_TYPE_PLAYER_NAME	"(Player)"


#define GOAL_L_NAME_SHORT	"(g l)"
#define GOAL_R_NAME_SHORT	"(g r)"

#define LINE_L_NAME_SHORT 	"(l l)"
#define LINE_R_NAME_SHORT 	"(l r)"
#define LINE_T_NAME_SHORT 	"(l t)"
#define LINE_B_NAME_SHORT	"(l b)"

#define FLAG_C_NAME_SHORT 	"(f c)"
#define FLAG_CT_NAME_SHORT	"(f c t)"
#define FLAG_CB_NAME_SHORT	"(f c b)"
#define FLAG_LT_NAME_SHORT	"(f l t)"
#define FLAG_LB_NAME_SHORT	"(f l b)"
#define FLAG_RT_NAME_SHORT	"(f r t)"
#define FLAG_RB_NAME_SHORT	"(f r b)"

#define	FLAG_PLT_NAME_SHORT	"(f p l t)"
#define	FLAG_PLC_NAME_SHORT	"(f p l c)"
#define	FLAG_PLB_NAME_SHORT	"(f p l b)"
#define	FLAG_PRT_NAME_SHORT	"(f p r t)"
#define	FLAG_PRC_NAME_SHORT	"(f p r c)"
#define	FLAG_PRB_NAME_SHORT	"(f p r b)"

#define FLAG_GLT_NAME_SHORT	"(f g l t)"
#define FLAG_GLB_NAME_SHORT	"(f g l b)"
#define FLAG_GRT_NAME_SHORT	"(f g r t)"
#define FLAG_GRB_NAME_SHORT	"(f g r b)"

#define FLAG_TL50_NAME_SHORT	"(f t l 50)"
#define FLAG_TL40_NAME_SHORT	"(f t l 40)"
#define FLAG_TL30_NAME_SHORT	"(f t l 30)"
#define FLAG_TL20_NAME_SHORT	"(f t l 20)"
#define FLAG_TL10_NAME_SHORT	"(f t l 10)"
#define FLAG_T0_NAME_SHORT	"(f t 0)"
#define FLAG_TR10_NAME_SHORT	"(f t r 10)"
#define FLAG_TR20_NAME_SHORT	"(f t r 20)"
#define FLAG_TR30_NAME_SHORT	"(f t r 30)"
#define FLAG_TR40_NAME_SHORT	"(f t r 40)"
#define FLAG_TR50_NAME_SHORT	"(f t r 50)"

#define FLAG_BL50_NAME_SHORT	"(f b l 50)"
#define FLAG_BL40_NAME_SHORT	"(f b l 40)"
#define FLAG_BL30_NAME_SHORT	"(f b l 30)"
#define FLAG_BL20_NAME_SHORT	"(f b l 20)"
#define FLAG_BL10_NAME_SHORT	"(f b l 10)"
#define FLAG_B0_NAME_SHORT	"(f b 0)"
#define FLAG_BR10_NAME_SHORT	"(f b r 10)"
#define FLAG_BR20_NAME_SHORT	"(f b r 20)"
#define FLAG_BR30_NAME_SHORT	"(f b r 30)"
#define FLAG_BR40_NAME_SHORT	"(f b r 40)"
#define FLAG_BR50_NAME_SHORT	"(f b r 50)"

#define FLAG_LT30_NAME_SHORT	"(f l t 30)"
#define FLAG_LT20_NAME_SHORT	"(f l t 20)"
#define FLAG_LT10_NAME_SHORT	"(f l t 10)"
#define FLAG_L0_NAME_SHORT	"(f l 0)"
#define FLAG_LB10_NAME_SHORT	"(f l b 10)"
#define FLAG_LB20_NAME_SHORT	"(f l b 20)"
#define FLAG_LB30_NAME_SHORT	"(f l b 30)"

#define FLAG_RT30_NAME_SHORT	"(f r t 30)"
#define FLAG_RT20_NAME_SHORT	"(f r t 20)"
#define FLAG_RT10_NAME_SHORT	"(f r t 10)"
#define FLAG_R0_NAME_SHORT	"(f r 0)"
#define FLAG_RB10_NAME_SHORT	"(f r b 10)"
#define FLAG_RB20_NAME_SHORT	"(f r b 20)"
#define FLAG_RB30_NAME_SHORT	"(f r b 30)"

#define GOALIE_VISUAL_STRING " goalie"

/* pfr: the final string is to denote goalie. We only modify the short string
 because this only affects v6 protocol */
#define PLAYER_NAME_FORMAT_SHORT	"(p \"%s\" %d%s)"
#define	PLAYER_NAME_FAR_FORMAT_SHORT	"(p \"%s\")"
#define	PLAYER_NAME_TOOFAR_FORMAT_SHORT	"(p)"

#define O_TYPE_FLAG_NAME_SHORT		"(F)"
#define O_TYPE_GOAL_NAME_SHORT		"(G)"
#define O_TYPE_BALL_NAME_SHORT		"(B)"
#define O_TYPE_PLAYER_NAME_SHORT     	"(P)"

#define SAY_MESSAGE_SCAN_FORMAT	"(%s %[-0-9a-zA-Z ().+*/?<>_]"
#define SAY_MESSAGE_SCAN_FORMAT_NEW "(%s \"%[-0-9a-zA-Z ().+*/?<>_]\")"

#define TEAM_L_DIRECTION 0
#define TEAM_R_DIRECTION M_PI
#define SideDirection(side)	(((side) == LEFT) ? TEAM_L_DIRECTION : TEAM_R_DIRECTION )

#define BALL_NAME		"(ball)"
#define BALL_NAME_SHORT		"(b)"
//
#define REFEREE_NAME		"referee"
#define REFEREE_NAME_SHORT	"r"
#define OLCOACH_NAME_R		"online_coach_right"
#define OLCOACH_NAME_R_SHORT	"cr"
#define OLCOACH_NAME_L		"online_coach_left"
#define OLCOACH_NAME_L_SHORT	"cl"
#define COACH_NAME		"coach"

#define PLAYER_SIZE		0.3
#define	PLAYER_WIDGET_SIZE	1.0
#define PLAYER_DECAY		0.4
#define PLAYER_RAND		0.1
#define PLAYER_WEIGHT		60.0
#define PLAYER_SPEED_MAX	1.2
// th 6.3.00
#define PLAYER_ACCEL_MAX	1.0
//
#define	STAMINA_MAX		4000.0
#define	STAMINA_INC_MAX		45.0
#define RECOVERY_DEC_THR	0.3
#define RECOVERY_DEC		0.002
#define RECOVERY_MIN		0.5
#define EFFORT_DEC_THR		0.3
#define EFFORT_DEC		0.005
#define EFFORT_MIN		0.6
#define EFFORT_INC_THR		0.6
#define EFFORT_INC		0.01

#define KICK_RAND             0.0
#define PRAND_FACTOR_L        1.0
#define PRAND_FACTOR_R        1.0
#define KICK_RAND_FACTOR_L    1.0
#define KICK_RAND_FACTOR_R    1.0


#define GOALIE_CATCHABLE_POSSIBILITY	1.0
#define GOALIE_CATCHABLE_AREA_LENGTH	2.0
#define GOALIE_CATCHABLE_AREA_WIDTH	1.0
#define GOALIE_CATCH_BAN_CYCLE		5
#define GOALIE_MAX_MOVES        	2

#define VisibleAngle 90.0
#define VisibleDistance 3.0
#define AUDIO_CUT_OFF_DIST 50.0

#define DASHPOWERRATE	0.006
#define KICKPOWERRATE	0.027
#define MAXPOWER	100.0
#define MINPOWER	-100.0

#define KICK_RANDOM_RATE	0.1
#define KICKABLE_MARGIN		0.7
#define CONTROL_RADIUS		2.0

//#define NormalizeDashPower(p)	(std::max(std::min((p),ServerParam::instance().maxp),ServerParam::instance().minp))
//#define ReduceDashPower(p)	(p * player_type->dashPowerRate () / stadium->slowdownOnTop (pos, team->side) )
//#define Sign(x)					(((x) > 0.0) ? 1.0:-1.0)
//#define NormalizeKickPower(p) (std::max(std::min((p),ServerParam::instance().maxp),ServerParam::instance().minp))
/*
#define NormalizeKickPower(p) (std::max(std::min((p),ServerParam::instance().maxp),ServerParam::instance().minp) * ServerParam::instance().kprate)
*/
#define MAXMOMENT	180
#define MINMOMENT	-180
//#define NormalizeMoment(p) Deg2Rad(std::max(std::min((p),ServerParam::instance().maxm),ServerParam::instance().minm))

#define MAX_NECK_MOMENT	180
#define MIN_NECK_MOMENT	-180
//#define NormalizeNeckMoment(p) Deg2Rad(std::max(std::min((p),ServerParam::instance().maxnm),ServerParam::instance().minnm))

#define MAX_NECK_ANGLE	90
#define MIN_NECK_ANGLE	-90
//#define NormalizeNeckAngle(p) (std::max(std::min((p),Deg2Rad(ServerParam::instance().maxn)),Deg2Rad(ServerParam::instance().minn)))


#define LEFT_STR	"l"
#define RIGHT_STR	"r"
#define SideStr(side)	(((side) == LEFT) ? LEFT_STR : RIGHT_STR )


#define DIST_QSTEP	0.1
#define LAND_QSTEP	0.01
#define DIR_QSTEP   0.1

/*
 *===================================================================
 *Part: for COACH
 *===================================================================
 */
#define DEF_SAY_COACH_MSG_SIZE		128
#define DEF_SAY_COACH_CNT_MAX		128
#define BALL_POS_INFO_STRINGS 		{"","in_field","goal_l","goal_r","out_of_field",}
#define BALL_POS_INFO_STRINGS_SHORT 	{"","i","gl","gr","o",}
enum BallPosInfo {
	BPI_Null,
	BPI_InField,
	BPI_GoalL,
	BPI_GoalR,
	BPI_OutOfField,
	BPI_MAX
} ;

/*
 *===================================================================
 *Part: for WEATHER
 *===================================================================
 */
#define	WIND_DIR		0.0
#define	WIND_FORCE		0.0
#define	WIND_RAND		0.0
#define WIND_WEIGHT		10000.0

/*
 *===================================================================
 *Part: Field Parameter
 *===================================================================
 */

#define PITCH_LENGTH 		105.0
#define PITCH_WIDTH 	 	68.0
#define PITCH_MARGIN		 5.0
#define CENTER_CIRCLE_R		9.15
#define PENALTY_AREA_LENGTH	16.5
#define PENALTY_AREA_WIDTH	40.32
#define GOAL_AREA_LENGTH	 5.5
#define GOAL_AREA_WIDTH		18.32
#define GOAL_WIDTH		 14.02
#define GOAL_DEPTH		 2.44
#define PENALTY_SPOT_DIST	11.0
#define CORNER_ARC_R		 1.0
#define KICK_OFF_CLEAR_DISTANCE CENTER_CIRCLE_R

#define KEEPAWAY_LENGTH         20.0
#define KEEPAWAY_WIDTH          20.0

#define CORNER_KICK_MARGIN	1.0

#define LENGTH_MAGNIFY		6.0
#define SHOWINFO_SCALE		16.0
#define SHOWINFO_SCALE2		65536.0

#define MAX_PLAYER			11
#define	HALF_TIME		300
#define DROP_TIME               200
#define AFTER_GOAL_WAIT		50
#define AFTER_OFFSIDE_WAIT	30
#define AFTER_BACKPASS_WAIT	30
#define AFTER_FREE_KICK_FAULT_WAIT	30
#define AFTER_CATCH_FAULT_WAIT	30

//#define MaxStringSize 		4096
#define MaxStringSize 		8192

#define MAX_FILE_LEN		256

#define UNUM_FAR_LENGTH		20.0
#define	UNUM_TOOFAR_LENGTH	40.0
#define TEAM_FAR_LENGTH		40.0
#define TEAM_TOOFAR_LENGTH	60.0

#define	BOARD_ROWS			6

#define VIEW_FOR	1
#define VIEW_REV	-1

#define OFFSIDE_ACTIVE_AREA_SIZE	2.5
#define OFFSIDE_KICK_MARGIN		9.15

#define REC_OLD_VERSION		1
#define REC_VERSION_2		2
#define REC_VERSION_3		3
#define DEFAULT_REC_VERSION	REC_VERSION_3


#endif

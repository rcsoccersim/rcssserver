/* -*- Mode: c++ -*-
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

#ifndef RCSSSERVER_PARAM_H
#define RCSSSERVER_PARAM_H


/*
 *   for Simulator
 */
#define	TIMEDELTA	10					/* polling interval [milli-sec] */

/*
 *   for network
 */

//pfr 06/14/01: this is probably the maximum UDP size allowed in most applications
#define MaxMesg 8192
//this must be smaller because it will eventually be sent out with other info
#define MaxCoachMesg (MaxMesg-38)
//We actually compare with one of the times already present
#define MaxCoachMesgWTime (MaxCoachMesg + 6)

enum TextLogFlag {
    LOG_TEXT = 0,
    SEND = 1,
    RECV = 2,
    SUBS = 4,
};

/*
 *   Objects
 */
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

#define SAY_MESSAGE_SCAN_FORMAT	" ( %s %[-0-9a-zA-Z ().+*/?<>_]"
#define SAY_MESSAGE_SCAN_FORMAT_NEW " ( %s \"%[-0-9a-zA-Z ().+*/?<>_]\" ) "

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


#define LEFT_STR	"l"
#define RIGHT_STR	"r"
#define SideStr(side)	(((side) == LEFT) ? LEFT_STR : RIGHT_STR )

/*
 *===================================================================
 *Part: for COACH
 *===================================================================
 */
#define BALL_POS_INFO_STRINGS 		{"","in_field","goal_l","goal_r","out_of_field",}
#define BALL_POS_INFO_STRINGS_SHORT 	{"","i","gl","gr","o",}
enum BallPosInfo {
	BPI_Null,
	BPI_InField,
	BPI_GoalL,
	BPI_GoalR,
	BPI_OutOfField,
	BPI_MAX
};

#define MAX_PLAYER			11

#endif

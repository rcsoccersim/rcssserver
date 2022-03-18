// -*-c++-*-

/***************************************************************************
                                   referee.h
                              Refereeing module
                             -------------------
    begin                : 16-May-2002
    copyright            : (C) 2001 by The RoboCup Soccer Server
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

#ifndef REFEREE_H
#define REFEREE_H

// later on I want this to produce a library that can be dlopened by
// the server. This way different referring modules could be used
// without having to recompile the server

#include "types.h"
#include "object.h"

#include <set>
#include <vector>

class Stadium;
class Player;
class Team;

class Referee {
private:
    // not used
    Referee() = delete;
    const Referee & operator=( const Referee & ) = delete;

protected:
    Stadium & M_stadium;

    explicit
    Referee( Stadium & stadium )
        : M_stadium( stadium )
      { }
public:

    virtual
    ~Referee()
      { }

    virtual
    void kickTaken( const Player &,
                    const double ) = 0;

    virtual
    void failedKickTaken( const Player & ) = 0;

    virtual
    void tackleTaken( const Player &,
                      const double,
                      const bool ) = 0;

    virtual
    void failedTackleTaken( const Player &,
                            const bool ) = 0;

    virtual
    void ballCaught( const Player & ) = 0;

    virtual
    void ballPunched( const Player & ) = 0;

    virtual
    void ballTouched( const Player & ) = 0;

    virtual
    void analyse() = 0;

    virtual
    void playModeChange( PlayMode ) = 0;

    //
    //
    //

    // static
    // void doAnalyse( Referee * ref )
    //   {
    //       ref->analyse();
    //   }

    //
    //
    //

    // class doKickTaken {
    // private:
    //     const Player & M_kicker;
    //     const double M_accel_r;
    // public:
    //     doKickTaken( const Player & kicker,
    //                  const double accel_r )
    //         : M_kicker( kicker ),
    //           M_accel_r( accel_r )
    //       { }

    //     void operator()( Referee * ref )
    //       {
    //           ref->kickTaken( M_kicker, M_accel_r );
    //       }
    // };

    // class doFailedKickTaken {
    // private:
    //     const Player & M_kicker;
    // public:
    //     doFailedKickTaken( const Player & kicker )
    //         : M_kicker( kicker )
    //       { }

    //     void operator()( Referee * ref )
    //       {
    //           ref->failedKickTaken( M_kicker );
    //       }
    // };

    // class doTackleTaken {
    // private:
    //     const Player & M_tackler;
    //     const double M_accel_r;
    //     const bool M_foul;
    // public:
    //     doTackleTaken( const Player & tackler,
    //                    const double accel_r,
    //                    const bool foul )
    //         : M_tackler( tackler ),
    //           M_accel_r( accel_r ),
    //           M_foul( foul )
    //       { }

    //     void operator()( Referee * ref )
    //       {
    //           ref->tackleTaken( M_tackler, M_accel_r, M_foul );
    //       }
    // };

    // class doFailedTackleTaken {
    // private:
    //     const Player & M_tackler;
    //     const bool M_foul;
    // public:
    //     doFailedTackleTaken( const Player & tackler,
    //                          const bool foul )
    //         : M_tackler( tackler ),
    //           M_foul( foul )
    //       { }

    //     void operator()( Referee * ref )
    //       {
    //           ref->failedTackleTaken( M_tackler, M_foul );
    //       }
    // };

    // class doCaughtBall {
    // private:
    //     const Player & M_catcher;
    // public:
    //     doCaughtBall( const Player & catcher )
    //         : M_catcher( catcher )
    //       { }

    //     void operator()( Referee * ref )
    //       {
    //           ref->ballCaught( M_catcher );
    //       }
    // };

    // class doPunchedBall {
    // private:
    //     const Player & M_goalie;
    // public:
    //     doPunchedBall( const Player & goalie )
    //         : M_goalie( goalie )
    //       { }

    //     void operator()( Referee * ref )
    //       {
    //           ref->ballPunched( M_goalie );
    //       }
    // };

    // class doPlayModeChange {
    // private:
    //     PlayMode M_pm;
    // public:
    //     doPlayModeChange( PlayMode pm )
    //         : M_pm( pm )
    //       { }

    //     void operator()( Referee * ref )
    //       {
    //           ref->playModeChange( M_pm );
    //       }
    // };

    // class doBallTouched {
    // private:
    //     const Player & M_player;
    // public:
    //     doBallTouched( const Player & player )
    //         : M_player( player )
    //       { }

    //     void operator()( Referee * ref )
    //       {
    //           ref->ballTouched( M_player );
    //       }
    // };


protected:

    void awardFreeKick( const Side side,
                        PVector pos );

    void awardGoalKick( const Side side,
                        PVector pos );

    void awardDropBall( PVector pos );

    void awardKickIn( const Side side,
                      PVector pos );

    void awardCornerKick( const Side side,
                          PVector pos );

    bool crossGoalLine( const Side side,
                        const PVector & prev_ball_pos );

    void placePlayersInTheirField();

    void clearPlayersFromBall( const Side side );


    void checkFoul( const Player & tackler,
                    const bool foul,
                    bool * detect_charge,
                    bool * detect_yellow,
                    bool * detect_red );

public:
    static
    PVector truncateToPitch( PVector ball_pos );

    static
    PVector moveOutOfPenalty( const Side side,
                              PVector ball_pos );

    static
    PVector moveOutOfGoalArea( const Side side,
                               PVector ball_pos );

    static
    PVector moveIntoPenalty( const Side side,
                             PVector ball_pos );

    static
    bool inPenaltyArea( const Side side,
                        const PVector & pos );

    static
    bool isPenaltyShootOut( const PlayMode pm,
                            const Side side = NEUTRAL );

};

/*--------------------------------------------------------*/

class TimeRef
    : public Referee {
private:

public:
    explicit
    TimeRef( Stadium & stadium )
        : Referee( stadium )
      { }

    void kickTaken( const Player &,
                    const double )
      { }

    void failedKickTaken( const Player & )
      { }

    void tackleTaken( const Player &,
                      const double,
                      const bool )
      { }

    void failedTackleTaken( const Player &,
                            const bool )
      { }

    void ballCaught( const Player & )
      { }

    void ballPunched( const Player & )
      { }

    void ballTouched( const Player & )
      { }

    void analyse();

    void playModeChange( PlayMode )
      { }
};

/*--------------------------------------------------------*/

class BallStuckRef
    : public Referee {
private:
    PVector M_last_ball_pos;
    int M_counter;

public:
    explicit
    BallStuckRef( Stadium & stadium )
        : Referee( stadium ),
          M_last_ball_pos( 0.0, 0.0 ),
          M_counter( 0 )
      { }

    void kickTaken( const Player &,
                    const double ) override
      { }

    void failedKickTaken( const Player & ) override
      { }

    void tackleTaken( const Player &,
                      const double,
                      const bool ) override
      { }

    void failedTackleTaken( const Player &,
                            const bool ) override
      { }

    void ballCaught( const Player & ) override
      { }

    void ballPunched( const Player & ) override
      { }

    void ballTouched( const Player & ) override
      { }

    void analyse() override;

    void playModeChange( PlayMode ) override
      { }
};

/*--------------------------------------------------------*/

class OffsideRef
    : public Referee {
private:

    struct Candidate {
    private:
        Candidate();
    public:
        const Player * player_;
        PVector pos_;

        Candidate( const Player * player,
                   const double & x,
                   const double & y )
            : player_( player ),
              pos_( x, y )
          { }
    };

    static const int AFTER_OFFSIDE_WAIT;

    int M_last_kick_time;
    int M_last_kick_stoppage_time;
    double M_last_kick_accel_r;
    Side M_last_kicker_side;
    std::vector< Candidate > M_offside_candidates;

    PVector M_offside_pos;
    int M_after_offside_time;

public:
    explicit
    OffsideRef( Stadium & stadium )
        : Referee( stadium ),
          M_last_kick_time( -1 ),
          M_last_kick_stoppage_time( -1 ),
          M_last_kick_accel_r( 0.0 ),
          M_last_kicker_side( NEUTRAL ),
          M_offside_pos(),
          M_after_offside_time( 0 )
      { }

    void kickTaken( const Player & kicker,
                    const double accel_r ) override;

    void failedKickTaken( const Player & kicker ) override;

    void tackleTaken( const Player & tackler,
                      const double accel_r,
                      const bool foul ) override;

    void failedTackleTaken( const Player & kicker,
                            const bool ) override;

    void ballCaught( const Player & ) override
      { }

    void ballPunched( const Player & ) override
      { }

    void ballTouched( const Player & player ) override;

    void analyse() override;

    void playModeChange( PlayMode pm ) override;

private:

    void checkIntentionalAction( const Player & kicker );
    void setOffsideMark( const Player & kicker,
                         const double accel_r );

    void callOffside();

    void checkPlayerAfterOffside();

};

/*--------------------------------------------------------*/

class IllegalDefenseRef
    : public Referee {
private:
    static const int AFTER_ILLEGAL_DEFENSE_WAIT;

    int M_left_illegal_counter;
    int M_right_illegal_counter;
    Side M_last_kicker_side;
    int M_last_kick_time;
    int M_after_illegal_defense_time;

public:
    explicit
    IllegalDefenseRef( Stadium & stadium )
        : Referee( stadium ),
          M_left_illegal_counter( 0 ),
          M_right_illegal_counter( 0 ),
          M_after_illegal_defense_time( 0 )
      { }

    void kickTaken( const Player & kicker,
                    const double accel_r) override;

    void failedKickTaken( const Player & ) override
      { }

    void tackleTaken( const Player & tackler,
                      const double accel_r,
                      const bool foul) override;

    void failedTackleTaken( const Player & ,
                            const bool ) override
      { }

    void ballCaught( const Player & ) override
      { }

    void ballPunched( const Player & ) override
      { }

    void ballTouched( const Player & ) override
      { }

    void analyse() override;

    void playModeChange( PlayMode pm ) override;

private:

    PVector calculateFreeKickPositon(Side side);

};


/*--------------------------------------------------------*/

class FreeKickRef
    : public Referee {
private:
    static const int AFTER_FREE_KICK_FAULT_WAIT;

    int M_timer;

    bool M_kick_taken;
    int M_goal_kick_count;
    const Player * M_kick_taker;
    int M_kick_taker_dashes;

    int M_after_free_kick_fault_time;
public:
    FreeKickRef( Stadium& stadium )
        : Referee( stadium ),
          M_timer( -1 ),
          M_kick_taken( false ),
          M_goal_kick_count( 0 ),
          M_kick_taker( 0 ),
          M_kick_taker_dashes( 0 ),
          M_after_free_kick_fault_time( 0 )
      {}

    virtual
    ~FreeKickRef() override
      { }

    void kickTaken( const Player & kicker,
                    const double accel_r ) override;

    void failedKickTaken( const Player & ) override
      { }

    void tackleTaken( const Player & kicker,
                      const double accel_r,
                      const bool foul ) override;

    void failedTackleTaken( const Player &,
                            const bool ) override
      { }

    void ballCaught( const Player & ) override
      { }

    void ballPunched( const Player & ) override
      { }

    void ballTouched( const Player & player ) override;

    void analyse() override;

    void playModeChange( PlayMode pm ) override;

private:

    void callFreeKickFault( Side side, PVector pos );

    bool goalKick( PlayMode pm );

    bool freeKick( PlayMode pm );

    bool ballStopped();

    //bool ballInPenaltyArea();

    bool tooManyGoalKicks();

    void placePlayersForGoalkick();

};

/*--------------------------------------------------------*/
// touch line referee
class TouchRef
    : public Referee {
private:
    // TODO? move to TimeRef
    static const int AFTER_GOAL_WAIT;

    const Player * M_last_touched;
    int M_last_touched_time;
    double M_last_touched_accel_r;

    const Player * M_last_indirect_kicker;
    bool M_indirect_mode;

    int M_after_goal_time;
    PVector M_prev_ball_pos;

public:
    TouchRef( Stadium& stadium )
        : Referee( stadium ),
          M_last_touched( nullptr ),
          M_last_touched_time( 0 ),
          M_last_touched_accel_r( 0.0 ),
          M_last_indirect_kicker( nullptr ),
          M_indirect_mode( false ),
          M_after_goal_time( 0 ),
          M_prev_ball_pos( 0.0, 0.0 )
      {}

    virtual
    ~TouchRef()
      {}

    void kickTaken( const Player & kicker,
                    const double accel_r ) override;

    void failedKickTaken( const Player & ) override
      { }

    void tackleTaken( const Player & kicker,
                      const double accel_r,
                      const bool foul ) override;

    void failedTackleTaken( const Player &,
                            const bool ) override
      { }

    void ballCaught( const Player & ) override
      { }

    void ballPunched( const Player & ) override
      { }

    void ballTouched( const Player & player ) override;

    void analyse() override;

    void playModeChange( PlayMode pm ) override;

private:

    void analyseImpl();

    bool checkGoal();

    void announceGoal( const Team & team );

    bool indirectFreeKick( const PlayMode pm );

};

/*--------------------------------------------------------*/

class CatchRef
    : public Referee {
private:
    static const int AFTER_BACKPASS_WAIT;
    static const int AFTER_CATCH_FAULT_WAIT;

    struct Kicker {
        int time_;
        Side side_;
        int unum_;
        Kicker( const int time,
                const Side side,
                const int unum )
            : time_( time ),
              side_( side ),
              unum_( unum )
          { }
    };

    int M_last_back_passer_time;
    double M_last_back_passer_accel_r;
    const Player * M_last_back_passer;
    const Player * M_before_last_back_passer;

    bool M_team_l_touched;
    bool M_team_r_touched;
    int M_after_back_pass_time;

    int M_after_catch_fault_time;

public:
    explicit
    CatchRef( Stadium & stadium )
        : Referee( stadium ),
          M_last_back_passer_time( 0 ),
          M_last_back_passer_accel_r( 0.0 ),
          M_last_back_passer( nullptr ),
          M_before_last_back_passer( nullptr ),
          M_team_l_touched( false ),
          M_team_r_touched( false ),
          M_after_back_pass_time( 0 ),
          M_after_catch_fault_time( 0 )
      { }

    virtual
    ~CatchRef()
      { }

    void kickTaken( const Player & kicker,
                    const double accel_r ) override;

    void failedKickTaken( const Player & ) override
      { }

    void tackleTaken( const Player & kicker,
                      const double accel_r,
                      const bool foul ) override;

    void failedTackleTaken( const Player &,
                            const bool ) override
      { }

    void ballCaught( const Player & catcher ) override;

    void ballPunched( const Player & catcher ) override;

    void ballTouched( const Player & player ) override;

    void analyse() override;

    void playModeChange( PlayMode pmode ) override;

private:

    void callBackPass( const Side side );

    void callCatchFault( Side side,
                         PVector pos );

};

/*--------------------------------------------------------*/

class FoulRef
    : public Referee {
private:

    static const int AFTER_FOUL_WAIT;

    int M_after_foul_time;

public:
    explicit
    FoulRef( Stadium & stadium)
        : Referee( stadium ),
          M_after_foul_time( 0 )
      { }

    virtual
    ~FoulRef() override
      { }

    void kickTaken( const Player &,
                    const double ) override
      { }

    void failedKickTaken( const Player & ) override
      { }

    void tackleTaken( const Player & tackler,
                      const double accel_r,
                      const bool foul ) override;

    void failedTackleTaken( const Player &,
                            const bool ) override
      { }

    void ballCaught( const Player & ) override
      { }

    void ballPunched( const Player & ) override
      { }

    void ballTouched( const Player & ) override
      { }

    void analyse() override;

    void playModeChange( PlayMode pm ) override;

private:

    void callFoul( const Player & tackler );
    void callYellowCard( const Player & tackler );
    void callRedCard( const Player & tackler );

};

/*--------------------------------------------------------*/

class KeepawayRef
    : public Referee {
private:
    static const char * trainingMsg;
    static const int TURNOVER_TIME;
    int M_episode;
    int M_keepers, M_takers;
    int M_time;
    int M_take_time;
public:
    KeepawayRef( Stadium & stadium );

    virtual
    ~KeepawayRef() override
      { }

    void kickTaken( const Player &,
                    const double ) override
      { }

    void failedKickTaken( const Player & ) override
      { }

    void tackleTaken( const Player &,
                      const double,
                      const bool ) override
      { }

    void failedTackleTaken( const Player &,
                            const bool ) override
      { }

    void ballCaught( const Player & ) override
      { }

    void ballPunched( const Player & ) override
      { }

    void ballTouched( const Player & ) override
      { }

    void analyse() override;

    void playModeChange( PlayMode pm ) override;

private:
    bool ballInKeepawayArea();

    void logHeader();

    void logEpisode( const char *endCond );

    void resetField();
};

/*--------------------------------------------------------*/

class PenaltyRef
    : public Referee {
private:
    int M_timer;
    int M_pen_nr_taken;
    Side M_pen_side;
    bool M_bDebug;
    Side M_cur_pen_taker;
    std::set< int > M_sLeftPenTaken;
    std::set< int > M_sRightPenTaken;

    const Player * M_last_taker;

    PVector M_prev_ball_pos;

    bool M_timeover;
public:
    PenaltyRef( Stadium& stadium );

    virtual
    ~PenaltyRef() override
      { }

    void kickTaken( const Player & kicker,
                    const double accel_r ) override;

    void failedKickTaken( const Player & ) override
      { }

    void tackleTaken( const Player & tackler,
                      const double accel_r,
                      const bool foul ) override;

    void failedTackleTaken( const Player &,
                            const bool ) override
      { }

    void ballCaught( const Player & catcher ) override;

    void ballPunched( const Player & catcher ) override;

    void ballTouched( const Player & ) override
      { }

    void analyse() override;

    void playModeChange( PlayMode pm ) override;

private:

    void startPenaltyShootout();

    void analyseImpl();
    void handleTimeout( bool left_move_check,
                        bool right_move_check );
    void handleTimer( const bool left_move_check,
                      const bool right_move_check );

    void penalty_init();
    void penalty_foul( const Side side );
    void penalty_check_score( );

    bool penalty_check_players( const Side side );
    void penalty_place_all_players( const Side side );
    void placeTakerTeamPlayers();
    void placeOtherTeamPlayers();

    const Player * getCandidateTaker();

    void penalty_miss( const Side side );
    void penalty_score( const Side side );

};

#endif //REFEREE_H

// -*-c++-*-

/***************************************************************************
                                stdoutsaver.h
                    Class for saving results to the standard output stream
                             -------------------
    begin                : 2008-10-27
    copyright            : (C) 2008 by The RoboCup Soccer Simulator
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

#ifndef RCSS_STDOUTSAVER_H
#define RCSS_STDOUTSAVER_H

#include "resultsaver.hpp"

#include <ctime>

class STDOutSaver
    : public ResultSaver {
public:

    static const std::string NAME;

private:
    STDOutSaver();

public:
    virtual
    ~STDOutSaver();

    static
    ResultSaver::Ptr create();

private:
    virtual
    bool doEnabled() const;

    virtual
    void doSaveStart();

    virtual
    void doSaveTime( const tm & time );

    virtual
    void doSaveTeamName( team_id id,
                         const std::string & name );

    virtual
    void doSaveCoachName( team_id id,
                          const std::string & name );

    virtual
    void doSaveScore( team_id id,
                      unsigned int score );

    virtual
    void doSavePenTaken( team_id id,
                         unsigned int taken );

    virtual
    void doSavePenScored( team_id id,
                          unsigned int scored );

    virtual
    void doSaveCoinTossWinner( team_id id );

    virtual
    bool doSaveComplete();

    virtual
    const char * doGetName() const;


    tm M_time;
    std::string M_team_name[ 2 ];
    std::string M_coach_name[ 2 ];
    unsigned int M_score[ 2 ];
    unsigned int M_pen_taken[ 2 ];
    unsigned int M_pen_scored[ 2 ];
    bool M_left_coin;
    bool M_right_coin;
};

#endif

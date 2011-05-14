// -*-c++-*-

/***************************************************************************
                                csvsaver.h
         Class for saving results to a file as comma seperated values
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

#include "resultsaver.hpp"

#include <rcssbase/conf/builder.hpp>

#include <boost/shared_ptr.hpp>

#include <ctime>
#include <string>
#include <fstream>


namespace rcss {
namespace conf {
class Parser;
class Builder;
}
}

class CSVSaverParam {
private:

    CSVSaverParam(); // not used
    CSVSaverParam( const CSVSaverParam & ); // not used
    CSVSaverParam & operator=( const CSVSaverParam & ); // not used

protected:

    CSVSaverParam( rcss::conf::Builder * parent );

public:
    virtual
    ~CSVSaverParam();


    static
    CSVSaverParam & instance();

    static
    CSVSaverParam & instance( rcss::conf::Builder * parent );


    static
    bool init( rcss::conf::Builder * parent );

private:
    void setDefaults();
    void addParams();

    boost::shared_ptr< rcss::conf::Builder > M_builder;

    bool M_save;
    std::string M_filename;

public:

    bool save() const
      {
          return M_save;
      }

    const
    std::string & filename() const
      {
          return M_filename;
      }

    //static const std::string CONF_DIR;
    //static const std::string CSVSAVER_CONF;

};


class CSVSaver
    : public ResultSaver {
public:

    static const std::string NAME;

private:
    CSVSaver();

public:
    virtual
    ~CSVSaver();

    static
    ResultSaver::Ptr create();

private:

    void openResultsFile();

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

    std::ofstream M_file;

};

// -*-c++-*-

/***************************************************************************
                                resultsaver.hpp
                    Abstract Base Class for saving results
                             -------------------
    begin                : 08-MAY-2003
    copyright            : (C) 2003 by The RoboCup Soccer Simulator
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

#ifndef RCSSRESULTSAVER_HPP
#define RCSSRESULTSAVER_HPP

#include <rcssbase/factory.hpp>

#include <string>
#include <memory>

struct tm;

class ResultSaver {
public:
    enum team_id {
        TEAM_LEFT = 0,
        TEAM_RIGHT = 1
    };

    typedef std::auto_ptr< ResultSaver > Ptr;
    typedef Ptr (*Creator)();
    typedef rcss::Factory< Creator, std::string > FactoryHolder;

    static
    FactoryHolder & factory();

protected:
    ResultSaver()
      { }

public:
    virtual
    ~ResultSaver()
      { }

    bool enabled() const
      {
          return doEnabled();
      }

    void saveStart()
      {
          doSaveStart();
      }

    void saveTime( const tm & time )
      {
          doSaveTime( time );
      }

    void saveTeamName( team_id team,
                       const std::string & name )
      {
          doSaveTeamName( team, name );
      }

    void saveCoachName( team_id team,
                        const std::string & name )
      {
          doSaveCoachName( team, name );
      }

    void saveScore( team_id team,
                    unsigned int score )
      {
          doSaveScore( team, score );
      }

    void savePenTaken( team_id team,
                       unsigned int taken )
      {
          doSavePenTaken( team, taken );
      }

    void savePenScored( team_id team,
                        unsigned int scored )
      {
          doSavePenScored( team, scored );
      }

    void saveCoinTossWinner( team_id team )
      {
          doSaveCoinTossWinner( team );
      }

    bool saveComplete()
      {
          return doSaveComplete();
      }

    const char * getName() const
      {
          return doGetName();
      }

protected:

    virtual
    bool doEnabled() const
      {
          return false;
      }

    virtual
    void doSaveStart()
      { }

    virtual
    void doSaveTime( const tm & )
      { }

    virtual
    void doSaveTeamName( team_id,
                         const std::string & )
      { }

    virtual
    void doSaveCoachName( team_id,
                          const std::string & )
      { }

    virtual
    void doSaveScore( team_id,
                      unsigned int )
      { }

    virtual
    void doSavePenTaken( team_id,
                         unsigned int )
      { }

    virtual
    void doSavePenScored( team_id,
                          unsigned int )
      { }

    virtual
    void doSaveCoinTossWinner( team_id )
      { }

    virtual
    bool doSaveComplete()
      {
          return true;
      }

    virtual
    const char * doGetName() const = 0;

};

#endif

// -*-c++-*-

/***************************************************************************
                                serializermonitor.h
               Classes for serializing data to std v1 monitors
                             -------------------
    begin                : 2007-11-21
    copyright            : (C) 2007 by The RoboCup Soccer Simulator
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/


#ifndef RCSS_SERIALIZER_MONITOR_STD_V1_H
#define RCSS_SERIALIZER_MONITOR_STD_V1_H

#include "serializer.h"

class Ball;
class Player;
class Team;

namespace rcss {

/*!
  \class SerializerMonitor
  \brief base class of the serialization for monitors.
*/
class SerializerMonitor
    : public Serializer {
public:
    typedef const rcss::SerializerMonitor* (*Creator)();
    typedef rcss::lib::Factory< Creator, int > Factory;

public:

    static
    Factory & factory();

protected:

    explicit
    SerializerMonitor( const SerializerCommon & common );

public:

    virtual
    ~SerializerMonitor();

    virtual
    void serializeTeam( std::ostream &,
                        const int,
                        const Team &,
                        const Team & ) const
      { }

    virtual
    void serializePlayMode( std::ostream &,
                            const int,
                            const PlayMode ) const
      { }

    virtual
    void serializeShowBegin( std::ostream &,
                             const int ) const
      { }
    virtual
    void serializeShowEnd( std::ostream & ) const
      { }
    virtual
    void serializePlayModeId( std::ostream &,
                              const PlayMode ) const
      { }
    virtual
    void serializeScore( std::ostream &,
                         const Team &,
                         const Team & ) const
      { }
    virtual
    void serializeBall( std::ostream &,
                        const Ball & ) const
      { }

    virtual
    void serializePlayerBegin( std::ostream &,
                               const Player & ) const
      { }
    virtual
    void serializePlayerEnd( std::ostream & ) const
      { }
    virtual
    void serializePlayerPos( std::ostream &,
                             const Player & ) const
      { }
    virtual
    void serializePlayerArm( std::ostream &,
                             const Player & ) const
      { }
    virtual
    void serializePlayerViewMode( std::ostream &,
                                  const Player & ) const
      { }
    virtual
    void serializePlayerStamina( std::ostream &,
                                 const Player & ) const
      { }
    virtual
    void serializePlayerFocus( std::ostream &,
                               const Player & ) const
      { }
    virtual
    void serializePlayerCounts( std::ostream &,
                                const Player & ) const
      { }

    virtual
    void serializeTeamGraphic( std::ostream &,
                               const int,
                               const int,
                               const char * ) const
      { }
};

/*!
  \class SerializerMonitor
  \brief class of the version 1 serialization for monitors.
*/
class SerializerMonitorStdv1
    : public SerializerMonitor {
protected:

    explicit
    SerializerMonitorStdv1( const SerializerCommon & common );

public:

    virtual
    ~SerializerMonitorStdv1();

    static
    const
    SerializerMonitorStdv1 * instance();

};


/*!
  \class SerializerMonitor
  \brief class of the version 3 serialization for monitors.
*/
class SerializerMonitorStdv3
    : public SerializerMonitorStdv1 {
protected:

    static const double PREC; // precision for positional info
    static const double DPREC; // precision for degree info

    explicit
    SerializerMonitorStdv3( const SerializerCommon & common );

public:

    virtual
    ~SerializerMonitorStdv3();

    static
    const
    SerializerMonitorStdv3 * instance();

    virtual
    void serializeTeam( std::ostream & os,
                        const int time,
                        const Team & team_l,
                        const Team & team_r ) const;

    virtual
    void serializePlayMode( std::ostream & os,
                            const int time,
                            const PlayMode pmode ) const;

    virtual
    void serializeShowBegin( std::ostream & os,
                             const int time ) const;
    virtual
    void serializeShowEnd( std::ostream & ) const;
    virtual
    void serializePlayModeId( std::ostream & os,
                              const PlayMode pmode ) const;
    virtual
    void serializeScore( std::ostream & os,
                         const Team & team_l,
                         const Team & team_r ) const;
    virtual
    void serializeBall( std::ostream & os,
                        const Ball & ball ) const;
    virtual
    void serializePlayerBegin( std::ostream & os,
                               const Player & player ) const;
    virtual
    void serializePlayerEnd( std::ostream & os ) const;
    virtual
    void serializePlayerPos( std::ostream & os,
                             const Player & player ) const;
    virtual
    void serializePlayerArm( std::ostream & os,
                             const Player & player ) const;
    virtual
    void serializePlayerViewMode( std::ostream & os,
                                  const Player & player ) const;
    virtual
    void serializePlayerStamina( std::ostream & os,
                                 const Player & player ) const;
    void serializePlayerFocus( std::ostream & os,
                               const Player & player ) const;

    virtual
    void serializePlayerCounts( std::ostream & os,
                                const Player & player ) const;
    virtual
    void serializeTeamGraphic( std::ostream & os,
                               const int x,
                               const int y,
                               const char * msg ) const;
};


}

#endif

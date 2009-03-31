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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/


#ifndef RCSS_SERIALIZER_MONITOR_STD_V1_H
#define RCSS_SERIALIZER_MONITOR_STD_V1_H

#include "serializer.h"

class Ball;
class Player;
class Team;
class XPMHolder;

namespace rcss {

/*!
  \class SerializerMonitorStdv1
  \brief class of the version 1 serialization for monitors.
*/
class SerializerMonitorStdv1
    : public SerializerMonitor {
protected:

    explicit
    SerializerMonitorStdv1( const SerializerCommon::Ptr common );

public:

    virtual
    ~SerializerMonitorStdv1();

    static
    const
    SerializerMonitor::Ptr create();


    virtual
    void serializeTeamGraphic( std::ostream & os,
                               const Side side,
                               const unsigned int x,
                               const unsigned int y,
                               const XPMHolder * xpm ) const;
};


/*!
  \class SerializerMonitorStdv3
  \brief class of the version 3 serialization for monitors.
*/
class SerializerMonitorStdv3
    : public SerializerMonitorStdv1 {
protected:

    static const double PREC; // precision for positional info
    static const double DPREC; // precision for degree info

    explicit
    SerializerMonitorStdv3( const SerializerCommon::Ptr common );

public:

    virtual
    ~SerializerMonitorStdv3();

    static
    const
    Ptr create();

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
};


/*!
  \class SerializerMonitorStdv4
  \brief class of the version 4 serialization for monitors.
*/
class SerializerMonitorStdv4
    : public SerializerMonitorStdv3 {
protected:

    explicit
    SerializerMonitorStdv4( const SerializerCommon::Ptr common );

public:

    virtual
    ~SerializerMonitorStdv4();

    static
    const
    Ptr create();

    virtual
    void serializePlayerStamina( std::ostream & os,
                                 const Player & player ) const;
};

}

#endif

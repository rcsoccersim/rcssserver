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
    void serializeScore( std::ostream &,
                         const int,
                         const std::string &,
                         const std::string &,
                         const int,
                         const int,
                         const int,
                         const int,
                         const int,
                         const int ) const
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
    void serializeShowPlayMode( std::ostream &,
                                const PlayMode ) const
      { }
    virtual
    void serializeShowScore( std::ostream &,
                             const int,
                             const int,
                             const int,
                             const int,
                             const int,
                             const int ) const
      { }
    virtual
    void serializeShowBall( std::ostream &,
                            const PVector &,
                            const PVector & ) const
      { }

    virtual
    void serializeShowPlayerBegin( std::ostream &,
                                   const Side,
                                   const int,
                                   const int,
                                   const int ) const
      { }
    virtual
    void serializeShowPlayerEnd( std::ostream & ) const
      { }
    virtual
    void serializeShowPlayerPos( std::ostream &,
                                 const PVector &,
                                 const PVector &,
                                 const double &,
                                 const double & ) const
      { }
    virtual
    void serializeShowPlayerArm( std::ostream &,
                                 const double &,
                                 const double & ) const
      { }
    virtual
    void serializeShowPlayerViewMode( std::ostream &,
                                      const bool,
                                      const double & ) const
      { }
    virtual
    void serializeShowPlayerStamina( std::ostream &,
                                     const double &,
                                     const double &,
                                     const double & ) const
      { }
    virtual
    void serializeShowPlayerFocus( std::ostream &,
                                   const Side,
                                   const int ) const
      { }
    virtual
    void serializeShowPlayerCountsBegin( std::ostream & ) const
      { }
    virtual
    void serializeShowPlayerCountsEnd( std::ostream & ) const
      { }
    virtual
    void serializeShowPlayerCounts( std::ostream &,
                                    const int,
                                    const int,
                                    const int,
                                    const int,
                                    const int,
                                    const int,
                                    const int,
                                    const int,
                                    const int,
                                    const int,
                                    const int ) const
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
    void serializeScore( std::ostream & os,
                         const int time,
                         const std::string & left_name,
                         const std::string & right_name,
                         const int left_point,
                         const int right_point,
                         const int left_pen_taken,
                         const int right_pen_taken,
                         const int left_pen_point,
                         const int right_pen_point ) const;

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
    void serializeShowPlayMode( std::ostream & os,
                                const PlayMode pmode ) const;
    virtual
    void serializeShowScore( std::ostream & os,
                             const int score_l,
                             const int score_r,
                             const int pen_taken_l,
                             const int pen_taken_r,
                             const int pen_score_l,
                             const int pen_score_r ) const;
    virtual
    void serializeShowBall( std::ostream & os,
                            const PVector & pos,
                            const PVector & vel ) const;
    virtual
    void serializeShowPlayerBegin( std::ostream & os,
                                   const Side side,
                                   const int unum,
                                   const int type,
                                   const int state ) const;
    virtual
    void serializeShowPlayerEnd( std::ostream & os ) const;
    virtual
    void serializeShowPlayerPos( std::ostream & os,
                                 const PVector & pos,
                                 const PVector & vel,
                                 const double & body,
                                 const double & neck ) const;
    virtual
    void serializeShowPlayerArm( std::ostream & os,
                                 const double & arm_dist,
                                 const double & arm_dir ) const;
    virtual
    void serializeShowPlayerViewMode( std::ostream & os,
                                      const bool high_quality,
                                      const double & vis_angle ) const;
    virtual
    void serializeShowPlayerStamina( std::ostream & os,
                                     const double & stamina,
                                     const double & effort,
                                     const double & recovery ) const;
    virtual
    void serializeShowPlayerFocus( std::ostream & os,
                                   const Side side,
                                   const int unum ) const;
    virtual
    void serializeShowPlayerCounts( std::ostream & os,
                                    const int kick,
                                    const int dash,
                                    const int turn,
                                    const int goalie_catch,
                                    const int move,
                                    const int turn_neck,
                                    const int change_view,
                                    const int say,
                                    const int tackle,
                                    const int pointto,
                                    const int attentionto ) const;

    virtual
    void serializeTeamGraphic( std::ostream &,
                               const int x,
                               const int y,
                               const char * msg ) const;
};


}

#endif

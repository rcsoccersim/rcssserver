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
    void serializeScore( std::ostream & os,
                         const std::string & left_name,
                         const std::string & right_name,
                         const int left_point,
                         const int right_point,
                         const int left_pen_taken,
                         const int right_pen_taken,
                         const int left_pen_point,
                         const int right_pen_point ) const = 0;

    virtual
    void serializePlayMode( std::ostream & os,
                            const PlayMode pmode ) const = 0;

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

    virtual
    void serializeScore( std::ostream & ,
                         const std::string & ,
                         const std::string & ,
                         const int ,
                         const int ,
                         const int ,
                         const int ,
                         const int ,
                         const int  ) const
      { }

    virtual
    void serializePlayMode( std::ostream & ,
                            const PlayMode ) const
      { }

};


/*!
  \class SerializerMonitor
  \brief class of the version 3 serialization for monitors.
*/
class SerializerMonitorStdv3
    : public SerializerMonitorStdv1 {
protected:

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
                            const PlayMode pmode ) const;

};


}

#endif

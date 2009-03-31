// -*-c++-*-

/***************************************************************************
                            serializercoachstdv8.h
               Class for serializing data to std v8 offline coaches
                             -------------------
    begin                : 28-JAN-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
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

#ifndef SERIALIZERCOACHSTDV8_H
#define SERIALIZERCOACHSTDV8_H

#include "serializercoachstdv7.h"

namespace rcss {

class SerializerCoachStdv8
    : public SerializerCoachStdv7 {
protected:
    explicit
    SerializerCoachStdv8( const SerializerCommon::Ptr common );


public:
    virtual
    ~SerializerCoachStdv8();

    static
    const
    Ptr create();

    virtual
    void serializeVisualObject( std::ostream &,
                                const std::string &,
                                const PVector &,
                                const PVector &,
                                const int,
                                const int,
                                const bool ) const;

    virtual
    void serializeVisualObject( std::ostream &,
                                const std::string &,
                                const PVector &,
                                const PVector &,
                                const int,
                                const int,
                                const int,
                                const bool ) const;


    virtual
    void serializeVisualPlayer( std::ostream & strm,
                                const Player & player,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel,
                                const int body,
                                const int neck ) const;

    virtual
    void serializeVisualPlayer( std::ostream & strm,
                                const Player & player,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel,
                                const int body,
                                const int neck,
                                const int point_dir ) const;
};

}

#endif

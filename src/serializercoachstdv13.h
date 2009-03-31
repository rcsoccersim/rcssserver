// -*-c++-*-

/***************************************************************************
                            serializercoachstdv13.h
               Class for serializing data to std v13 offline coaches
                             -------------------
    begin                : 2008-10-28
    copyright            : (C) 2008 by The RoboCup Soccer Server
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

#ifndef SERIALIZERCOACHSTDV13_H
#define SERIALIZERCOACHSTDV13_H

#include "serializercoachstdv8.h"

namespace rcss {

class SerializerCoachStdv13
    : public SerializerCoachStdv8 {
protected:
    SerializerCoachStdv13( const SerializerCommon::Ptr common );


public:
    virtual
    ~SerializerCoachStdv13();

    static
    const
    SerializerCoach::Ptr create();

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

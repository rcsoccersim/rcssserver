// -*-c++-*-

/***************************************************************************
                            serializercoachstdv14.h
               Class for serializing data to std v14 offline coaches
                             -------------------
    begin                : 2009-10-26
    copyright            : (C) 2009 by The RoboCup Soccer Server
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

#ifndef SERIALIZERCOACHSTDV14_H
#define SERIALIZERCOACHSTDV14_H

#include "serializercoachstdv13.h"

namespace rcss {

class SerializerCoachStdv14
    : public SerializerCoachStdv13 {
protected:
    SerializerCoachStdv14( const SerializerCommon::Ptr common );

public:
    virtual
    ~SerializerCoachStdv14();

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

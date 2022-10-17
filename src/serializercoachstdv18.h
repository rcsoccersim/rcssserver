// -*-c++-*-

/***************************************************************************
                            SerializerCoachStdv18.h
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

#ifndef SerializerCoachStdv18_H
#define SerializerCoachStdv18_H

#include "serializercoachstdv14.h"

namespace rcss {

class SerializerCoachStdv18
    : public SerializerCoachStdv14 {
protected:
    SerializerCoachStdv18( const SerializerCommon::Ptr common );

public:
    virtual
    ~SerializerCoachStdv18() override;

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
                                const int neck,
                                const int focus_point_dir,
                                const double focus_point_dist ) const override;

    virtual
    void serializeVisualPlayer( std::ostream & strm,
                                const Player & player,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel,
                                const int body,
                                const int neck,
                                const int focus_point_dir,
                                const double focus_point_dist,
                                const int point_dir ) const override;
};

}

#endif

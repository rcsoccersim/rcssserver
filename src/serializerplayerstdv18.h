// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv14.h
                  Class for serializing data to std v14 players
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

#ifndef SERIALIZERPLAYERSTDV18_H
#define SERIALIZERPLAYERSTDV18_H

#include "serializerplayerstdv14.h"

namespace rcss {

class SerializerPlayerStdv18
    : public SerializerPlayerStdv14 {
protected:
    SerializerPlayerStdv18( const SerializerCommon::Ptr common );

public:
    virtual
    ~SerializerPlayerStdv18() override;

    static
    const
    SerializerPlayer::Ptr create();

    virtual
    void serializeFocusPoint( std::ostream &,
                        const Player & ) const override;

    virtual
    void serializeFSPlayerBegin( std::ostream &,
                                 const char,
                                 const int,
                                 const bool,
                                 const int,
                                 const double &,
                                 const double &,
                                 const double &,
                                 const double &,
                                 const double &,
                                 const double &,
                                 const double &,
                                 const double & ) const override;
};

}

#endif

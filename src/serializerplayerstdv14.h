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

#ifndef SERIALIZERPLAYERSTDV14_H
#define SERIALIZERPLAYERSTDV14_H

#include "serializerplayerstdv13.h"

namespace rcss {

class SerializerPlayerStdv14
    : public SerializerPlayerStdv13 {
protected:
    SerializerPlayerStdv14( const SerializerCommon::Ptr common );

public:
    virtual
    ~SerializerPlayerStdv14();

    static
    const
    SerializerPlayer::Ptr create();


    virtual
    void serializeFoul( std::ostream &,
                        const Player & ) const;

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
                                 const double & ) const;
    virtual
    void serializeFSPlayerState( std::ostream &,
                                 const Player & ) const;
};

}

#endif

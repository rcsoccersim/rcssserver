// -*-c++-*-

/***************************************************************************
                            serializeronlinecoachstdv7.h
               Class for serializing data to std v7 online coaches
                             -------------------
    begin                : 27-MAY-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
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

#ifndef SERIALIZERONLINECOACHSTDV7_H
#define SERIALIZERONLINECOACHSTDV7_H

#include "serializeronlinecoachstdv6.h"

namespace rcss {

class SerializerOnlineCoachStdv7
    : public SerializerOnlineCoachStdv6 {
protected:
    SerializerOnlineCoachStdv7( const SerializerCommon::Ptr common,
                                const SerializerCoach::Ptr coach );

public:
    virtual
    ~SerializerOnlineCoachStdv7();

    static
    const
    Ptr create();

    virtual
    void serializeRefereeAudio( std::ostream & strm,
                                const int time,
                                const std::string & name,
                                const char * msg ) const;

    virtual
    void serializePlayerAudio( std::ostream & strm,
                               const int time,
                               const std::string & name,
                               const char * msg ) const;

    virtual
    void serializeChangedPlayer( std::ostream &,
                                 const int unum,
                                 const int type = -1 ) const;
};

}

#endif

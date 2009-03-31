// -*-c++-*-

/***************************************************************************
                            serializeronlinecoachstdv8.h
               Class for serializing data to std v8 online coaches
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

#ifndef SERIALIZERONLINECOACHSTDV8_H
#define SERIALIZERONLINECOACHSTDV8_H

#include "serializeronlinecoachstdv7.h"

namespace rcss {

class SerializerOnlineCoachStdv8
    : public SerializerOnlineCoachStdv7 {
protected:
    SerializerOnlineCoachStdv8( const SerializerCommon::Ptr common,
                                const SerializerCoach::Ptr coach );

public:
    virtual
    ~SerializerOnlineCoachStdv8();

    static
    const
    Ptr create();

    virtual
    void serializePlayerClangVer( std::ostream & strm,
                                  const std::string & name,
                                  const unsigned int min,
                                  const unsigned int max ) const;
};

}

#endif

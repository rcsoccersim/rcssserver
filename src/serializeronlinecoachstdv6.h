// -*-c++-*-

/***************************************************************************
                            serializeronlinecoachstdv6.h
               Class for serializing data to std v6 online coaches
                             -------------------
    begin                : 24-JAN-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
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

#ifndef SERIALIZERONLINECOACHSTDV6_H
#define SERIALIZERONLINECOACHSTDV6_H

#include "serializeronlinecoachstdv1.h"

namespace rcss {

class SerializerOnlineCoachStdv6
    : public SerializerOnlineCoachStdv1 {
protected:
    SerializerOnlineCoachStdv6( const SerializerCommon & common,
                                const SerializerCoach & coach );

public:
    virtual
    ~SerializerOnlineCoachStdv6();

    static
    const
    SerializerOnlineCoachStdv6 * instance();

    virtual
    void
    serializeInit( std::ostream &,
                   const int side = 0 ) const;
};

}

#endif

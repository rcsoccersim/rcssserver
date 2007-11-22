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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef SERIALIZERCOACHSTDV8_H
#define SERIALIZERCOACHSTDV8_H

#include "serializercoachstdv7.h"

namespace rcss
{
class SerializerCoachStdv8
    : public SerializerCoachStdv7
{
protected:
    SerializerCoachStdv8( const SerializerCommon & common );


public:
    virtual
    ~SerializerCoachStdv8();

    static
    const SerializerCoachStdv8* instance();

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
};
}

#endif

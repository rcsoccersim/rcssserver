// -*-c++-*-

/***************************************************************************
                            serializercommonstdv8.h
                  Class for serializing data to std v8 clients
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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef SERIALIZERCOMMONSTDV8_H
#define SERIALIZERCOMMONSTDV8_H

#include "serializercommonstdv7.h"

namespace rcss {


class SerializerCommonStdv8
    : public SerializerCommonStdv7 {
protected:
    SerializerCommonStdv8();

public:
    virtual
    ~SerializerCommonStdv8();

    static
    const
    Ptr create();

    virtual
    void serializeServerParamBegin( std::ostream & strm ) const;

    virtual
    void serializePlayerParamBegin( std::ostream & strm ) const;

    virtual
    void serializePlayerTypeBegin( std::ostream & strm ) const;

    virtual
    void serializeParam( std::ostream & strm,
                         const std::string & name,
                         const int param ) const;

    virtual
    void serializeParam( std::ostream & strm,
                         const std::string & name,
                         const bool param ) const;

    virtual
    void serializeParam( std::ostream & strm,
                         const std::string & name,
                         const double & param ) const;

    virtual
    void serializeParam( std::ostream & strm,
                         const std::string & name,
                         const std::string & param ) const;

};

}

#endif

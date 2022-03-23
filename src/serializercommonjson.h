// -*-c++-*-

/*!
  \file serializercommonjson.h
  \brief Class for serializing data by JSON format
  \author Hidehisa Akiyama
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Server Maintenance Committee.
 Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef SERIALIZER_COMMON_JSON_H
#define SERIALIZER_COMMON_JSON_H

#include "serializer.h"

namespace rcss {

class SerializerCommonJSON
    : public SerializerCommon {
protected:
    SerializerCommonJSON();

public:
    virtual
    ~SerializerCommonJSON() override;

    static const Ptr create();

    virtual
    void serializeServerParamBegin( std::ostream & strm ) const override;

    virtual
    void serializeServerParamEnd( std::ostream & strm ) const override;

    virtual
    void serializePlayerParamBegin( std::ostream & strm ) const override;

    virtual
    void serializePlayerParamEnd( std::ostream & strm ) const override;

    virtual
    void serializePlayerTypeBegin( std::ostream & strm,
                                   const int id ) const override;
    virtual
    void serializePlayerTypeEnd( std::ostream & strm ) const override;


    virtual
    void serializeParam( std::ostream & strm,
                         const std::string & name,
                         const int param ) const override;

    virtual
    void serializeParam( std::ostream & strm,
                         const std::string & name,
                         const bool param ) const override;

    virtual
    void serializeParam( std::ostream & strm,
                         const std::string & name,
                         const double & param ) const override;

    virtual
    void serializeParam( std::ostream & strm,
                         const std::string & name,
                         const std::string & param ) const override;

};

}

#endif

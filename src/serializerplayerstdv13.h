// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv13.h
                  Class for serializing data to std v13 players
                             -------------------
    begin                : 2008-10-28
    copyright            : (C) 2008 by The RoboCup Soccer Server
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

#ifndef SERIALIZERPLAYERSTDV13_H
#define SERIALIZERPLAYERSTDV13_H

#include "serializerplayerstdv8.h"

namespace rcss {

class SerializerPlayerStdv13
    : public SerializerPlayerStdv8 {
protected:
    SerializerPlayerStdv13( const SerializerCommon::Ptr common );

public:
    virtual
    ~SerializerPlayerStdv13();

    static
    const
    SerializerPlayer::Ptr create();


    virtual
    void serializeVisualPlayer( std::ostream & strm,
                                const Player & player,
                                const std::string & name,
                                const double & dist,
                                const int dir ) const;

    virtual
    void serializeVisualPlayer( std::ostream & strm,
                                const Player & player,
                                const std::string & name,
                                const double & dist,
                                const int dir,
                                const int point_dir ) const;

    virtual
    void serializeVisualPlayer( std::ostream & strm,
                                const Player & player,
                                const std::string & name,
                                const double & dist,
                                const int dir,
                                const double & dist_chg,
                                const double & dir_chg,
                                const int body_dir,
                                const int head_dir ) const;

    virtual
    void serializeVisualPlayer( std::ostream & strm,
                                const Player & player,
                                const std::string & name,
                                const double & dist,
                                const int dir,
                                const double & dist_chg,
                                const double & dir_chg,
                                const int body_dir,
                                const int head_dir,
                                const int point_dir ) const;


    virtual
    void serializeBodyStamina( std::ostream & strm,
                               const double & stamina,
                               const double & effort,
                               const double & stamina_capacity ) const;

    virtual
    void serializeFSPlayerStamina( std::ostream & strm,
                                   const double & stamina,
                                   const double & effort,
                                   const double & recovery,
                                   const double & stamina_capacity ) const;

    virtual
    void serializeFSPlayerState( std::ostream & strm,
                                 const Player & player ) const;
};

}

#endif

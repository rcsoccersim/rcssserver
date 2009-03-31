// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv8.h
                  Class for serializing data to std v8 players
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

#ifndef SERIALIZERPLAYERSTDV8_H
#define SERIALIZERPLAYERSTDV8_H

#include "serializerplayerstdv7.h"

namespace rcss {

class SerializerPlayerStdv8
    : public SerializerPlayerStdv7 {
protected:
    explicit
    SerializerPlayerStdv8( const SerializerCommon::Ptr common );

public:
    virtual
    ~SerializerPlayerStdv8();

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
    void serializeAllyAudioFull( std::ostream & strm,
                                 const int time,
                                 const double & dir,
                                 const int unum,
                                 const char * msg ) const;

    virtual
    void serializeOppAudioFull( std::ostream & strm,
                                const int time,
                                const double & dir,
                                const char * msg ) const;

    virtual
    void serializeAllyAudioShort( std::ostream & strm,
                                  const int time,
                                  const int unum ) const;

    virtual
    void serializeOppAudioShort( std::ostream & strm,
                                 const int time ) const;

    virtual
    void serializeFSBall( std::ostream & strm,
                          const double & x,
                          const double & y,
                          const double & vel_x,
                          const double & vel_y ) const;

    virtual
    void serializeFSPlayerBegin( std::ostream & strm,
                                 const char side,
                                 const int unum,
                                 const bool goalie,
                                 const int type,
                                 const double & x,
                                 const double & y,
                                 const double & vel_x,
                                 const double & vel_y,
                                 const double & body_dir,
                                 const double & neck_dir ) const;

    virtual
    void serializeFSPlayerArm( std::ostream & strm,
                               const double & mag,
                               const double & head ) const;

    virtual
    void serializeFSPlayerStamina( std::ostream & strm,
                                   const double & stamina,
                                   const double & effort,
                                   const double & recovery,
                                   const double & stamina_capacity ) const;

    virtual
    void serializeFSCounts( std::ostream & strm,
                            const int count_kick,
                            const int count_dash,
                            const int count_turn,
                            const int count_catch,
                            const int count_move,
                            const int count_turn_neck,
                            const int count_change_view,
                            const int count_say ) const;

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

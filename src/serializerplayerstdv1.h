// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv1.h
                  Class for serializing data to std v1 players
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

#ifndef SERIALIZERPLAYERSTDV1_H
#define SERIALIZERPLAYERSTDV1_H

#include "serializer.h"

namespace rcss {

class SerializerPlayerStdv1
    : public SerializerPlayer {
protected:
    explicit
    SerializerPlayerStdv1( const SerializerCommon::Ptr common );

public:
    virtual
    ~SerializerPlayerStdv1();

    static
    const
    Ptr create();

    virtual
    void serializeRefereeAudio( std::ostream & strm,
                                const int time,
                                const char * msg ) const;

    virtual
    void serializeCoachAudio( std::ostream & strm,
                              const int time,
                              const std::string & name,
                              const char * msg ) const;

    virtual
    void serializeCoachStdAudio( std::ostream & strm,
                                 const int time,
                                 const std::string & name,
                                 const rcss::clang::Msg & msg ) const;

    virtual
    void serializeSelfAudio( std::ostream & strm,
                             const int time,
                             const char * msg ) const;

    virtual
    void serializePlayerAudio( std::ostream & strm,
                               const int time,
                               const double & dir,
                               const char * msg ) const;

    virtual
    void serializeVisualBegin( std::ostream & strm,
                               const int time ) const;

    virtual
    void serializeVisualEnd( std::ostream & strm ) const;


    virtual
    void serializeBodyBegin( std::ostream & strm,
                             const int time ) const;

    virtual
    void serializeBodyEnd( std::ostream & strm ) const;

    virtual
    void serializeBodyViewMode( std::ostream & strm,
                                const char * qual,
                                const char * width ) const;

    virtual
    void serializeBodyStamina( std::ostream & strm,
                               const double & stamina,
                               const double & effort,
                               const double & stamina_capacity ) const;

    virtual
    void serializeBodyVelocity( std::ostream & strm,
                                const double & mag ) const;

    virtual
    void serializeBodyVelocity( std::ostream & strm,
                                const double & mag,
                                const int head ) const;

    virtual
    void serializeBodyCounts( std::ostream & strm,
                              const int count_kick,
                              const int count_dash,
                              const int count_turn,
                              const int count_say ) const;

    virtual
    void serializeBodyCounts( std::ostream & strm,
                              const int count_catch,
                              const int count_move,
                              const int count_change_view ) const;

    virtual
    void serializeNeckAngle( std::ostream & strm,
                             const int ang ) const;

    virtual
    void serializeNeckCount( std::ostream & strm,
                        const int count_turn_neck ) const;

    virtual
    void serializeArm( std::ostream & strm,
                       const int movable_cycles,
                       const int expires_cycles,
                       const double & dist,
                       const int head,
                       const int count ) const;

    virtual
    void serializeFocus( std::ostream & strm,
                         const char * name,
                         const int count ) const;

    virtual
    void serializeFocus( std::ostream & strm,
                         const char * team,
                         const int unum,
                         const int count ) const;


    virtual
    void serializeTackle( std::ostream & strm,
                          const int cycles,
                          const int count ) const;

    virtual
    void serializeCollision( std::ostream & strm,
                             const bool ball_collide,
                             const bool player_collide,
                             const bool post_collide ) const;


    virtual
    void serializeFSBegin( std::ostream & strm,
                           const int time ) const;

    virtual
    void serializeFSEnd( std::ostream & strm ) const;

    virtual
    void serializeFSPlayMode( std::ostream & strm,
                              const char * mode ) const;

    virtual
    void serializeFSViewMode( std::ostream & strm,
                              const char * qual,
                              const char * width ) const;

    virtual
    void serializeFSScore( std::ostream & strm,
                           const int left,
                           const int right ) const;

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
                                 const bool,
                                 const int,
                                 const double & x,
                                 const double & y,
                                 const double & vel_x,
                                 const double & vel_y,
                                 const double & body_dir,
                                 const double & neck_dir ) const;

    virtual
    void serializeFSPlayerStamina( std::ostream & strm,
                                   const double & stamina,
                                   const double & effort,
                                   const double & recovery,
                                   const double & stamina_capacity ) const;
    virtual
    void serializeFSPlayerEnd( std::ostream & strm ) const;

    virtual
    void serializeInit( std::ostream & strm,
                        const char * side,
                        const int unum,
                        const PlayMode & mode ) const;

    virtual
    void serializeReconnect( std::ostream & strm,
                             const char * side,
                             const PlayMode & mode ) const;


    virtual
    void serializeOKClang( std::ostream & strm,
                           const int min,
                           const int max ) const;

    virtual
    void serializeErrorNoTeamName( std::ostream & strm,
                                   const std::string & team_name ) const;

    virtual
    void serializeScore( std::ostream & strm,
                         const int time,
                         const int our,
                         const int opp ) const;
};

}

#endif

// -*-c++-*-

/***************************************************************************
                             visualsenderplayer.h
                 Classes for building visual messages for players
                             -------------------
    begin                : 2008-02-17
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

#ifndef RCSS_VISUALSENDER_PLAYER_H
#define RCSS_VISUALSENDER_PLAYER_H

#include "visual.h"

#include "observer.h"
#include "player.h"
#include "random.h"

#include <rcssbase/factory.hpp>

#include <boost/shared_ptr.hpp>

class Stadium;

namespace rcss {

class SerializerPlayer;

/*!
//===================================================================
//
//  CLASS: VisualSenderPlayer
//
//  DESC: Base class for the visual protocol for players.
//
//===================================================================
*/

class VisualSenderPlayer
    : public VisualSender {
public:

    class Params {
    public:
        std::ostream & M_transport;
        const Player & M_self;
        const boost::shared_ptr< SerializerPlayer > M_serializer;
        const Stadium & M_stadium;

        Params( std::ostream & transport,
                const Player & self,
                const boost::shared_ptr< SerializerPlayer > serializer,
                const Stadium & stadium )
            : M_transport( transport )
            , M_self( self )
            , M_serializer( serializer )
            , M_stadium( stadium )
          { }
    };


private:
    const boost::shared_ptr< SerializerPlayer > M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      Observer and Observer should have virtual functions for
      stuff like velocity, stamina, etc */
    const Player & M_self;
    const Stadium & M_stadium;

    int M_sendcnt;

public:
    typedef std::auto_ptr< VisualSenderPlayer > Ptr;
    typedef Ptr (*Creator)( const VisualSenderPlayer::Params & );
    typedef rcss::Factory< Creator, int > FactoryHolder;

    static
    FactoryHolder & factory();

    VisualSenderPlayer( const Params & params );

    ~VisualSenderPlayer();

protected:
    const
    SerializerPlayer & serializer() const
      {
          return *M_serializer;
      }

    const
    Player & self() const
      {
          return M_self;
      }

    const
    Stadium & stadium() const
      {
          return M_stadium;
      }

    int sendCount() const
      {
          return M_sendcnt;
      }

    void incSendCount()
      {
          ++M_sendcnt;
      }

    void resetSendCount()
      {
          M_sendcnt = 0;
      }

};


/*!
//===================================================================
//
//  CLASS: ObserverPlayer
//
//  DESC: Interface for players that receives visual information.
//
//===================================================================
*/

class ObserverPlayer
    : protected BaseObserver< VisualSenderPlayer > {
public:

    ObserverPlayer()
      { }

    ObserverPlayer( VisualSenderPlayer & sender )
        : BaseObserver< VisualSenderPlayer >( sender )
      { }

    ObserverPlayer( std::auto_ptr< VisualSenderPlayer > sender )
        : BaseObserver< VisualSenderPlayer >( sender )
      { }

    ~ObserverPlayer()
      { }

    void setVisualSender( VisualSenderPlayer & sender )
      {
          BaseObserver< VisualSenderPlayer >::setSender( sender );
      }

    void setVisualSender( std::auto_ptr< VisualSenderPlayer > sender )
      {
          BaseObserver< VisualSenderPlayer >::setSender( sender );
      }

    void sendVisual()
      {
          BaseObserver< VisualSenderPlayer >::sender().sendVisual();
      }
};


/*!
//===================================================================
//
//  CLASS: VisualSensorPlayerV1
//
//  DESC: Class for the version 1* visual protocol.  This version is
//        completely unused as far as I am aware of, but it is here
//        none the less, just in case there is someone somewhere
//        still using it.
//
//        * It's version 1 to the simualtor in it's current form.
//        From what I know the original simulator was written in
//        lisp and the first C++ version was actually version 3.  I
//        don't know if the protocol was compatible with previous
//        versions, so this may well be the version 3 protocol.
//
//===================================================================
*/

class VisualSenderPlayerV1
    : public VisualSenderPlayer {
public:

    VisualSenderPlayerV1( const Params & params );

    virtual
    ~VisualSenderPlayerV1();

    virtual
    void
    sendVisual();


private:
    void sendFlag( const PObject & obj )
      {
          (this->*M_send_flag)( obj );
      }

    void sendBall( const MPObject & obj )
      {
          (this->*M_send_ball)( obj );
      }

    void sendPlayer( const Player & obj )
      {
          (this->*M_send_player)( obj );
      }

    void serializeLine( const std::string & name,
                        const int dir,
                        const double & sight_2_line_ang,
                        const double & player_2_line )
      {
          (this->*M_serialize_line)( name, dir,
                                     sight_2_line_ang, player_2_line );
      }

    void sendFlags();

    void sendBalls();

    void sendPlayers();

    void sendLines();

    void sendLowFlag( const PObject & flag );

    void sendHighFlag( const PObject & flag );

    void sendLowBall( const MPObject & ball );

    void sendHighBall( const MPObject & ball );

    void sendLowPlayer( const Player & player );

    void sendHighPlayer( const Player & player );

    bool sendLine( const PObject & line );

    void serializeLowLine( const std::string & name,
                           const int dir,
                           const double &,
                           const double & );

    void serializeHighLine( const std::string & name,
                            const int dir,
                            const double & sight_2_line_ang,
                            const double & player_2_line );

    double calcRadDir( const PObject & obj )
      {
          return normalize_angle( self().angleFromBody( obj )
                                  - self().angleNeckCommitted() );
      }

    int calcDegDir( const double & rad_dir ) const
      {
          return rad2Deg( rad_dir );
      }

    double calcLineRadDir( const double & line_normal ) const
      {
          return normalize_angle( line_normal
                                  - self().angleBodyCommitted()
                                  - self().angleNeckCommitted() );
      }

    int calcLineDir( const double & sight_2_line_ang ) const
      {
          if ( sight_2_line_ang > 0 )
              return calcDegDir( sight_2_line_ang - M_PI*0.5 );
          else
              return calcDegDir( sight_2_line_ang + M_PI*0.5 );
      }

    double calcUnQuantDist( const PObject & obj ) const
      {
          return self().pos().distance( obj.pos() );
      }

    double calcQuantDist( const double & dist,
                          const double & qstep ) const
      {
          return Quantize( std::exp( Quantize( std::log( dist + EPS ), qstep ) ), 0.1 );
      }

    double calcLineDist( const double & sight_2_line_ang,
                         const double & player_2_line,
                         const double & qstep ) const
      {
          return Quantize( std::exp( Quantize( std::log( std::fabs( player_2_line
                                                                    / std::cos( sight_2_line_ang ) )
                                                         + EPS ),
                                               qstep ) ), 0.1 );
      }


    void calcVel( const PVector & obj_vel,
                  const PVector & obj_pos,
                  const double & un_quant_dist,
                  const double & quant_dist,
                  double & dist_chg,
                  double & dir_chg ) const;

    bool decide( const double & prob )
      {
          if ( prob >= 1.0 ) return true;
          if ( prob <= 0.0 ) return false;
          return boost::bernoulli_distribution<>( prob )( random::DefaultRNG::instance() );
      }

protected:
    virtual
    void serializePlayer( const Player & player,
                          const std::string & name,
                          const double & dist,
                          const int dir,
                          const double & dist_chg,
                          const double & dir_chg );

    virtual
    void serializePlayer( const Player & player,
                          const std::string & name,
                          const double & dist,
                          const int dir );

    virtual
    int rad2Deg( const double & rad ) const
      {
          return Rad2IDeg( rad );
      }

    virtual
    const
    std::string & calcName( const PObject & obj ) const
      {
          return obj.name();
      }

    virtual
    const
    std::string & calcCloseName( const PObject & obj ) const
      {
          return obj.closeName();
      }

    virtual
    const
    std::string & calcTFarName( const Player & obj ) const
      {
          return obj.nameTooFar();
      }

    virtual
    const
    std::string & calcUFarName( const Player & obj ) const
      {
          return obj.nameFar();
      }

private:
    void (VisualSenderPlayerV1::*M_send_flag)( const PObject & );
    void (VisualSenderPlayerV1::*M_send_ball)( const MPObject & );
    void (VisualSenderPlayerV1::*M_send_player)( const Player & );
    void (VisualSenderPlayerV1::*M_serialize_line)( const std::string &,
                                                    const int,
                                                    const double &,
                                                    const double & );
};

/*!
//===================================================================
//
//  CLASS: VisualSenderPlayerV4
//
//  DESC: Class for the version 4 visual protocol.  This version
//        introduced body directions of players. Everything else is
//        the same.
//
//===================================================================
*/

class VisualSenderPlayerV4
    : public VisualSenderPlayerV1 {
public:
    VisualSenderPlayerV4( const Params & params );

    virtual
    ~VisualSenderPlayerV4();

protected:
    virtual
    void serializePlayer( const Player & player,
                          const std::string & name,
                          const double & dist,
                          const int dir,
                          const double & dist_chg,
                          const double & dir_chg );


    int calcBodyDir( const Player & player ) const
      {
          return rad2Deg( normalize_angle( player.angleBodyCommitted()
                                           - self().angleBodyCommitted()
                                           - self().angleNeckCommitted() ) );
      }
};

/*!
//===================================================================
//
//  CLASS: VisualSensorPlayerV5
//
//  DESC: Class for the version 5 visual protocol.  This version
//        introduced head directions of players. Everything else is
//        the same
//
//===================================================================
*/

class VisualSenderPlayerV5
    : public VisualSenderPlayerV4 {
public:
    VisualSenderPlayerV5( const Params & params );

    virtual
    ~VisualSenderPlayerV5();

protected:
    virtual
    void serializePlayer( const Player & player,
                          const std::string & name,
                          const double & dist,
                          const int dir,
                          const double & dist_chg,
                          const double & dir_chg );

    int calcHeadDir( const Player & player ) const
      {
          return rad2Deg( normalize_angle( player.angleNeckCommitted()
                                           + player.angleBodyCommitted()
                                           - self().angleBodyCommitted()
                                           - self().angleNeckCommitted() ) );
      }

};


/*!
//===================================================================
//
//  CLASS: VisualSensorPlayerV6
//
//  DESC: Class for the version 6 visual protocol.  This version
//        introduced shortened names for all the objects meaning the
//        name calculation must be redefined.
//
//===================================================================
*/

class VisualSenderPlayerV6
    : public VisualSenderPlayerV5 {
public:
    VisualSenderPlayerV6( const Params & params );

    virtual
    ~VisualSenderPlayerV6();

    virtual
    const
    std::string & calcName( const PObject & obj ) const
      {
          return obj.shortName();
      }

    virtual
    const
    std::string & calcCloseName( const PObject & obj ) const
      {
          return obj.shortCloseName();
      }

    virtual
    const
    std::string & calcUFarName( const Player & obj ) const
      {
          return obj.shortNameFar();
      }

    virtual
    const
    std::string & calcTFarName( const Player & obj ) const
      {
          return obj.shortNameTooFar();
      }
};

/*!
//===================================================================
//
//  CLASS: VisualSensorPlayerV7
//
//  DESC: Class for the version 7 visual protocol.  This version
//        fixed a bug in the generation of directions in that they
//        were truncated to int, rather than rounded.  This meant
//        error in the direction pointed was at most times between
//        -0.5 and +0.5 degrees, but occationally between -1.0 and
//        +1.0 degrees and at other times exact.  With the new method
//        of rounding, the error is always between -0.5 and +0.5.
//
//===================================================================
*/

class VisualSenderPlayerV7
    : public VisualSenderPlayerV6 {
public:
    VisualSenderPlayerV7( const Params & params );

    virtual
    ~VisualSenderPlayerV7();

protected:
    virtual
    int rad2Deg( const double & rad ) const
      {
          return Rad2IDegRound( rad );
      }
};

/*!
//===================================================================
//
//  CLASS: VisualSensorPlayerV8
//
//  DESC: Class for the version 8 visual protocol.  This version
//        introduced observation of the a new arm feature, that
//        allows plays to point to different spots, however, on
//        direction the arm is pointing to is visable, not the
//        distance to the spot the arm is pointing to.
//
//===================================================================
*/

class VisualSenderPlayerV8
    : public VisualSenderPlayerV7 {
public:
    VisualSenderPlayerV8( const Params & params );

    virtual
    ~VisualSenderPlayerV8();

protected:
    int calcPointDir( const Player & player );

    virtual
    void serializePlayer( const Player & player,
                          const std::string & name,
                          const double & dist,
                          const int dir,
                          const double & dist_chg,
                          const double & dir_chg );

    virtual
    void serializePlayer( const Player & player,
                          const std::string & name,
                          const double & dist,
                          const int dir );
};


/*!
//===================================================================
//
//  CLASS: VisualSensorPlayerV13
//
//  DESC: Class for the version 13 visual protocol.  This version
//        introduced observation of the kicking state of other players.
//
//===================================================================
*/

class VisualSenderPlayerV13
    : public VisualSenderPlayerV8 {
public:
    VisualSenderPlayerV13( const Params & params );

    virtual
    ~VisualSenderPlayerV13();

};

}

#endif

// -*-c++-*-

/***************************************************************************
                            visualsenderplayer.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "visualsenderplayer.h"

#include "stadium.h"
#include "serializer.h"

namespace rcss {

/**
 * @class NoisyObservation
 * @brief Represents a noisy observation made by an observer player.
 *
 * This class provides a base interface for calculating distances, and velocities
 * for noisy observations. It is meant to be inherited by specific observation classes.
 */
class NoisyObservation {
private:
    const Player & M_observer;

protected:
    NoisyObservation() = delete;

    /**
     * @brief Constructs a NoisyObservation object.
     *
     * This constructor initializes a NoisyObservation object with the given player.
     *
     * @param player The player object to observe.
     */
    explicit
    NoisyObservation( const Player & player )
        : M_observer( player )
      { }

    const Player & observer() const
      {
          return M_observer;
      }

public:
    virtual
    ~NoisyObservation() = default;

    /**
     * Calculates the noisy distance between the observer and the target movable object.
     *
     * @param actual_dist The actual distance between the observer and the target movable object.
     * @param focus_dist The focus distance between the focus point and the target movable object.
     * @return The calculated noisy distance.
     */
    virtual
    double calcDist( const double actual_dist,
                     const double focus_dist ) const = 0;
    /**
     * Calculates the noisy distance between the observer and the target landmark object.
     *
     * @param actual_dist The actual distance between the observer and the target landmark object.
     * @param focus_dist The focus distance between the focus point and the target landmark object.
     * @return The calculated noisy distance.
     */
    virtual
    double calcDistLandmark( const double actual_dist,
                             const double focus_dist ) const = 0;

    /**
     * Calculates the change in distance and direction based on the object's velocity, position, actual distance, and noisy distance.
     *
     * @param obj_vel The velocity of the target object.
     * @param obj_pos The position of the target object.
     * @param actual_dist The actual distance between the observer and the target object.
     * @param noisy_dist The noisy distance between the observer and the target object.
     * @param dist_chg Pointer to a variable that will store the calculated change in distance.
     * @param dir_chg Pointer to a variable that will store the calculated change in direction.
     */
    virtual
    void calcVel( const PVector & obj_vel,
                  const PVector & obj_pos,
                  const double actual_dist,
                  const double noisy_dist,
                  double * dist_chg,
                  double * dir_chg ) const = 0;

};

/**
 * @class QuantizeObservation
 * @brief A class that represents a quantized observation of a player.
 *
 * This class is derived from the NoisyObservation class and provides methods for calculating
 * quantized distances and velocities based on actual distances and positions.
 */
class QuantizeObservation
    : public NoisyObservation {
private:

protected:
    QuantizeObservation() = delete;

public:
    explicit
    QuantizeObservation( const Player & player )
        : NoisyObservation( player )
      { }

    virtual
    ~QuantizeObservation() = default;

private:

    /**
     * Calculates the observed distance based on the actual distance, focus distance, and quantization step.
     *
     * @param actual_dist The actual distance.
     * @param focus_dist The focus distance.
     * @param qstep The quantization step used for quantizing the distances.
     * @return The observed distance calculated based on the given parameters.
     */
    double calcDistImpl( const double actual_dist,
                         const double focus_dist,
                         const double qstep ) const
      {
          const double quant_dist = std::exp( Quantize( std::log( actual_dist + EPS ), qstep ) );
          const double quant_focus_dist = std::exp( Quantize( std::log( focus_dist + EPS ), qstep ) );
          const double observed_dist = std::max( 0.0,
                                                 actual_dist - ( ( focus_dist - quant_focus_dist ) + ( actual_dist - quant_dist ) ) / 2.0 );

          return Quantize( observed_dist, 0.1 );
      }

public:
    double calcDist( const double actual_dist,
                     const double focus_dist ) const override
      {
          return calcDistImpl( actual_dist, focus_dist, observer().distQStep() );
      }

    double calcDistLandmark( const double actual_dist,
                             const double focus_dist ) const override
      {
          return calcDistImpl( actual_dist, focus_dist, observer().landDistQStep() );
      }

    void calcVel( const PVector & obj_vel,
                  const PVector & obj_pos,
                  const double actual_dist,
                  const double noisy_dist,
                  double * dist_chg,
                  double * dir_chg ) const override
      {
          if ( actual_dist != 0.0 )
          {
              const PVector vtmp = obj_vel - observer().vel();
              const PVector etmp = ( obj_pos - observer().pos() ) /= actual_dist;

              *dist_chg = vtmp.x * etmp.x + vtmp.y * etmp.y;
              //         dir_chg = RAD2DEG * ( vtmp.y * etmp.x
              //                               - vtmp.x * etmp.y ) / actual_dist;
              *dir_chg = vtmp.y * etmp.x - vtmp.x * etmp.y;
              *dir_chg /= actual_dist;
              *dir_chg *= RAD2DEG;

              *dir_chg = ( *dir_chg == 0.0
                           ? 0.0
                           : Quantize( *dir_chg, observer().dirQStep() ) );
              *dist_chg = noisy_dist * Quantize( *dist_chg / actual_dist, 0.02 );
          }
          else
          {
              *dir_chg = 0.0;
              *dist_chg = 0.0;
          }
      }
};


/**
 * @class GaussianObservation
 * @brief Represents a Gaussian observation model for player observations.
 *
 * The GaussianObservation class is a concrete implementation of the NoisyObservation class.
 * It calculates the noisy distance and velocity changes based on the actual distance and focus distance.
 * The noise rates are determined by the player's type.
 */
class GaussianObservation
    : public NoisyObservation {
private:

    GaussianObservation() = delete;

public:
    explicit
    GaussianObservation( const Player & player )
        : NoisyObservation( player )
      { }


    virtual
    ~GaussianObservation() = default;

private:
    /**
     * Calculates the distance with noise based on the actual distance, focus distance,
     * distance noise rate, and focus distance noise rate.
     *
     * @param actual_dist The actual distance.
     * @param focus_dist The focus distance.
     * @param dist_noise_rate The distance noise rate.
     * @param focus_dist_noise_rate The focus distance noise rate.
     * @return he observed distance calculated based on the given parameters.
     */
    double calcDistImpl( const double actual_dist,
                         const double focus_dist,
                         const double dist_noise_rate,
                         const double focus_dist_noise_rate ) const
      {
          const double std_dev = actual_dist * dist_noise_rate + focus_dist * focus_dist_noise_rate;

          return std::max( 0.0, ndrand( actual_dist, std_dev ) );
      }

public:
    double calcDist( const double actual_dist,
                     const double focus_dist ) const override
      {
          return calcDistImpl( actual_dist,
                               focus_dist,
                               observer().playerType()->distNoiseRate(),
                               observer().playerType()->focusDistNoiseRate() );
      }

    double calcDistLandmark( const double actual_dist,
                             const double focus_dist ) const override
      {
          return calcDistImpl( actual_dist,
                               focus_dist,
                               observer().playerType()->landDistNoiseRate(),
                               observer().playerType()->landFocusDistNoiseRate() );
      }

    void calcVel( const PVector & obj_vel,
                  const PVector & obj_pos,
                  const double actual_dist,
                  const double noisy_dist,
                  double * dist_chg,
                  double * dir_chg ) const override
      {
          if ( actual_dist != 0.0 )
          {
              const PVector vtmp = obj_vel - observer().vel();
              PVector etmp = ( obj_pos - observer().pos() ) /= actual_dist;

              *dist_chg = vtmp.x * etmp.x + vtmp.y * etmp.y;
              *dir_chg = vtmp.y * etmp.x - vtmp.x * etmp.y;
              *dir_chg /= actual_dist;
              *dir_chg *= RAD2DEG;

              *dir_chg = ( *dir_chg == 0.0
                           ? 0.0
                           : Quantize( *dir_chg, observer().dirQStep() ) );
              //*dist_chg += ( noisy_dist - actual_dist );
              *dist_chg *= ( noisy_dist / actual_dist );
              *dist_chg = Quantize( *dist_chg, 0.01 );
          }
          else
          {
              *dir_chg = 0.0;
              *dist_chg = 0.0;
          }
      }
};


/*!
//===================================================================
//
//  CLASS: VisualSenderPlayer
//
//  DESC: Base class for the visual protocol for players.
//
//===================================================================
*/

VisualSenderPlayer::FactoryHolder &
VisualSenderPlayer::factory()
{
    static FactoryHolder rval;
    return rval;
}

VisualSenderPlayer::VisualSenderPlayer( const Params & params )
    : VisualSender( params.M_transport ),
      M_serializer( params.M_serializer ),
      M_self( params.M_self ),
      M_stadium( params.M_stadium ),
      M_sendcnt( 0 ),
      M_noisy_observation( new QuantizeObservation( M_self ) ),
      M_cached_focus_point( 0.0, 0.0 )
{

    //std::cerr << "create VisualSenderPlayer" << std::endl;
}

VisualSenderPlayer::~VisualSenderPlayer()
{
    //std::cerr << "delete VisualSenderPlayer" << std::endl;
}

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

VisualSenderPlayerV1::VisualSenderPlayerV1( const Params & params )
    : VisualSenderPlayer( params )
{

}

VisualSenderPlayerV1::~VisualSenderPlayerV1()
{

}

void
VisualSenderPlayerV1::sendVisual()
{
    incSendCount();

    if ( sendCount() >= self().visualSendInterval() )
    {
        resetSendCount();
    }
    else
    {
        return;
    }

    if ( self().isGaussianSee() )
    {
        M_noisy_observation = std::shared_ptr< const NoisyObservation >( new GaussianObservation( self() ) );
    }

    updateCache();

    serializer().serializeVisualBegin( transport(), stadium().time() );
    sendFlags();
    sendBalls();
    sendPlayers();
    sendLines();
    serializer().serializeVisualEnd( transport() );
    transport() << std::ends << std::flush;
}

void
VisualSenderPlayerV1::sendFlags()
{
    for ( const PObject * o : stadium().field().landmarks() )
    {
        if ( o->objectVersion() <= self().version() )
        {
            sendFlag( *o );
        }
    }
}

void
VisualSenderPlayerV1::sendBalls()
{
    if ( stadium().ball().objectVersion() <= self().version() )
    {
        sendBall( stadium().ball() );
    }
}

void
VisualSenderPlayerV1::sendPlayers()
{
    for ( Stadium::PlayerCont::const_reference p : stadium().players() )
    {
        if ( p != &self()
             && p->isEnabled()
             && p->objectVersion() <= self().version() )
        {
            sendPlayer( *p );
        }
    }
}


void
VisualSenderPlayerV1::sendLines()
{
    int line_count = 0;
    int max_line_count;
    int min_line_count;
    if ( std::fabs( self().pos().x ) < ServerParam::PITCH_LENGTH*0.5
         && std::fabs( self().pos().y ) < ServerParam::PITCH_WIDTH*0.5 )
    {
        min_line_count = max_line_count = 1;
    }
    else
    {
        max_line_count = 2;
        min_line_count = 0;
    }

    if ( line_count < max_line_count
         && stadium().field().line_l.objectVersion() <= self().version() )
    {
        if ( sendLine( stadium().field().line_l ) )
            ++line_count;
    }
    if ( line_count < max_line_count
         && stadium().field().line_r.objectVersion() <= self().version() )
    {
        if( sendLine( stadium().field().line_r ) )
            ++line_count;
    }
    if ( line_count < max_line_count
         && stadium().field().line_t.objectVersion() <= self().version() )
    {
        if ( sendLine( stadium().field().line_t ) )
            ++line_count;
    }
    if ( line_count < max_line_count
         && stadium().field().line_b.objectVersion() <= self().version() )
    {
        if ( sendLine( stadium().field().line_b ) )
            ++line_count;
    }
    if ( line_count < min_line_count )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": Error rendering visual: not enough lines\n"
                  << "Player = " << self() << std::endl;
    }
}

void
VisualSenderPlayerV1::sendLowFlag( const PObject & flag )
{
    const double ang = calcRadDir( flag );
    const double un_quant_dist = calcUnQuantDist( flag );

    if ( std::fabs( ang ) < self().visibleAngle() * 0.5
         && un_quant_dist < self().playerType()->landMaxObservationLength() )
    {
        serializer().serializeVisualObject( transport(),
                                            calcName( flag ),
                                            calcDegDir( ang ) );
    }
    else if ( un_quant_dist <= self().VISIBLE_DISTANCE )
    {
        serializer().serializeVisualObject( transport(),
                                            calcCloseName( flag ),
                                            calcDegDir( ang ) );
    }
}

void
VisualSenderPlayerV1::sendHighFlag( const PObject & flag )
{
    const double ang = calcRadDir( flag );
    const double actual_dist = calcUnQuantDist( flag );
    const double noisy_dist = calcNoisyDistLandmark( flag, actual_dist );

    if ( std::fabs( ang ) < self().visibleAngle() * 0.5
         && actual_dist < self().playerType()->landMaxObservationLength() )
    {
        if ( decide( calcNoFlagVelProb( noisy_dist ) ) )
        {
            serializer().serializeVisualObject( transport(),
                                                calcName( flag ),
                                                noisy_dist,
                                                calcDegDir( ang ) );
        }
        else
        {
            double dist_chg, dir_chg;
            calcNoisyVel( PVector(), flag.pos(), actual_dist, noisy_dist, &dist_chg, &dir_chg );

            serializer().serializeVisualObject( transport(),
                                                calcName( flag ),
                                                noisy_dist,
                                                calcDegDir( ang ),
                                                dist_chg,
                                                dir_chg );
        }
    }
    else if ( actual_dist <= self().VISIBLE_DISTANCE )
    {
        serializer().serializeVisualObject( transport(),
                                            calcCloseName( flag ),
                                            noisy_dist,
                                            calcDegDir( ang ) );
    }
}


void
VisualSenderPlayerV1::sendLowBall( const MPObject & ball )
{
    const double ang = calcRadDir( ball );
    const double un_quant_dist = calcUnQuantDist( ball );

    if( std::fabs( ang ) < self().visibleAngle() * 0.5
        && un_quant_dist < self().playerType()->ballMaxObservationLength())
    {
        serializer().serializeVisualObject( transport(),
                                            calcName( ball ),
                                            calcDegDir( ang ) );
    }
    else if( un_quant_dist <= self().VISIBLE_DISTANCE )
    {
        serializer().serializeVisualObject( transport(),
                                            calcCloseName( ball ),
                                            calcDegDir( ang ) );
    }
}


void
VisualSenderPlayerV1::sendHighBall( const MPObject & ball )
{
    const double ang = calcRadDir( ball );
    const double actual_dist = calcUnQuantDist( ball );
    const double noisy_dist = noisyObservation().calcDist( actual_dist, ball.pos().distance( cachedFocusPoint() ) );

    if ( std::fabs( ang ) < self().visibleAngle() * 0.5
         && actual_dist < self().playerType()->ballMaxObservationLength() )
    {
        if ( decide( calcNoBallVelProb( noisy_dist ) ) )
        {
            serializer().serializeVisualObject( transport(),
                                                calcName( ball ),
                                                noisy_dist,
                                                calcDegDir( ang ) );
        }
        else
        {
            double dist_chg, dir_chg;
            calcNoisyVel( ball.vel(), ball.pos(), actual_dist, noisy_dist, &dist_chg, &dir_chg );

            serializer().serializeVisualObject( transport(),
                                                calcName( ball ),
                                                noisy_dist,
                                                calcDegDir( ang ),
                                                dist_chg,
                                                dir_chg );
        }
    }
    else if ( actual_dist <= self().VISIBLE_DISTANCE )
    {
        serializer().serializeVisualObject( transport(),
                                            calcCloseName( ball ),
                                            noisy_dist,
                                            calcDegDir( ang ) );
    }
}

void
VisualSenderPlayerV1::sendLowPlayer( const Player & player )
{
    const double ang = calcRadDir( player );
    const double actual_dist = calcUnQuantDist( player );

    if ( std::fabs( ang ) < self().visibleAngle() * 0.5
         && actual_dist < self().playerType()->playerMaxObservationLength() )
    {
        const double noisy_dist = calcNoisyDist( player, actual_dist );

        if ( decide( calcNoTeamProb( noisy_dist ) ) )
        {
            serializer().serializeVisualObject( transport(),
                                                calcTFarName( player ),
                                                calcDegDir( ang ) );
        }
        else
        {
            if ( decide( calcNoUnumProb( noisy_dist ) ) )
            {
                serializer().serializeVisualObject( transport(),
                                                    calcUFarName( player ),
                                                    calcDegDir( ang ) );
            }
            else
            {
                serializer().serializeVisualObject( transport(),
                                                    calcPlayerName( player ),
                                                    calcDegDir( ang ) );
            }
        }
    }
    else if ( actual_dist <= self().VISIBLE_DISTANCE )
    {
        serializer().serializeVisualObject( transport(),
                                            calcCloseName( player ),
                                            calcDegDir( ang ) );
    }
}

void
VisualSenderPlayerV1::sendHighPlayer( const Player & player )
{
    const double ang = calcRadDir( player );
    const double actual_dist = self().pos().distance( player.pos() );
    const double noisy_dist = calcNoisyDist( player, actual_dist );

    if ( std::fabs( ang ) < self().visibleAngle() * 0.5
         && actual_dist < self().playerType()->playerMaxObservationLength() )
    {
        if ( decide( calcNoTeamProb( noisy_dist ) ) )
        {
            // no team information
            serializer().serializeVisualObject( transport(),
                                                calcTFarName( player ),
                                                noisy_dist,
                                                calcDegDir( ang ) );
        }
        else
        {
            if ( decide( calcNoUnumProb( noisy_dist ) ) )
            {
                // no unum information
                serializePlayer( player,
                                 calcUFarName( player ),
                                 noisy_dist,
                                 calcDegDir( ang ) );
            }
            else
            {
                double dist_chg, dir_chg;
                calcNoisyVel( player.vel(), player.pos(), actual_dist, noisy_dist, &dist_chg, &dir_chg );

                serializePlayer( player,
                                 calcPlayerName( player ),
                                 noisy_dist,
                                 calcDegDir( ang ),
                                 dist_chg,
                                 dir_chg );
            }
        }
    }
    else if ( actual_dist <= player.VISIBLE_DISTANCE )
    {
        serializer().serializeVisualObject( transport(),
                                            calcCloseName( player ),
                                            noisy_dist,
                                            calcDegDir( ang ) );
    }
}

bool
VisualSenderPlayerV1::sendLine( const PObject & line )
{
    /*! the angle of an outward pointing normal ( 90degs to ) to
      the line */
    double line_normal;

    //! perp distance of the player from the line
    double player_2_line;

    //! the x of y value of where the line starts
    double line_start;

    //! the x or y value of where the line stops
    double line_stop;

    //! a flag to specify if the line is vertical or horizontal
    bool vert;

    /*! be very carefull here.  The lines pos.x is actually it's
      distance from the center of the field, not neccesarily it's
      x position. */

    //! left line
    if ( line.pos().x == - ServerParam::PITCH_LENGTH*0.5 )
    {
        line_normal = M_PI;
        if ( self().pos().x < line.pos().x )
            line_normal = 0.0;
        player_2_line = line.pos().x - self().pos().x;
        line_start = - ServerParam::PITCH_WIDTH*0.5;
        line_stop = ServerParam::PITCH_WIDTH*0.5;
        vert = true;
    }
    //! right line
    else if ( line.pos().x == ServerParam::PITCH_LENGTH*0.5 )
    {
        line_normal = 0.0;
        if( self().pos().x > line.pos().x )
            line_normal = M_PI;
        player_2_line = line.pos().x - self().pos().x;
        line_start = - ServerParam::PITCH_WIDTH*0.5;
        line_stop = ServerParam::PITCH_WIDTH*0.5;
        vert = true;
    }
    //! top line
    else if ( line.pos().x == - ServerParam::PITCH_WIDTH*0.5 )
    {
        line_normal = -M_PI*0.5;
        if ( self().pos().y < line.pos().x )
            line_normal = M_PI*0.5;
        player_2_line = line.pos().x - self().pos().y;
        line_start = - ServerParam::PITCH_LENGTH*0.5;
        line_stop = ServerParam::PITCH_LENGTH*0.5;
        vert = false;
    }
    //! bottom line
    else if ( line.pos().x == ServerParam::PITCH_WIDTH*0.5 )
    {
        line_normal = M_PI*0.5;
        if ( self().pos().y > line.pos().x )
            line_normal = -M_PI*0.5;
        player_2_line = line.pos().x - self().pos().y;
        line_start = - ServerParam::PITCH_LENGTH*0.5;
        line_stop = ServerParam::PITCH_LENGTH*0.5;
        vert = false;
    }
    else
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": Error, unknown line: " << line << std::endl;
        return false;
    }

    //! angle between the players line of sight and the line's normal
    double sight_2_line_ang = calcLineRadDir( line_normal );

    /*! if the angle between the line of sight and the line's norm
      is not within -90.0 and 90 degrees then the player is
      looking parallel or away from the line, thus it cannot be
      visible. */
    if ( std::fabs( sight_2_line_ang ) >= M_PI*0.5 )
    {
        return false;
    }

    /*! this gives us the x or y offset from the player for where
      their line of sight intersects the line */
    double line_intersect = player_2_line * std::tan( sight_2_line_ang );

    /*! this calculates the actual x or y value for where the line
      of sight intersects the line.  Because the y axis is
      inverted, we need to use -line_intersect if the line is
      vertical. */
    if ( vert )
        line_intersect = self().pos().y - line_intersect;
    else
        line_intersect += self().pos().x;

    /*! If the point that the players line of sight intersects the
      line beyond it's beginning or end then the player wont see
      this line */
    if ( line_intersect < line_start
         || line_intersect > line_stop )
    {
        return false;
    }

    serializeLine( calcName( line ),
                   calcLineDir( sight_2_line_ang ),
                   sight_2_line_ang, player_2_line );
    return true;
}

void
VisualSenderPlayerV1::serializeLowLine( const std::string & name,
                                        const int dir,
                                        const double &, const double & )
{
    serializer().serializeVisualObject( transport(), name, dir );
}

void
VisualSenderPlayerV1::serializeHighLine( const std::string & name,
                                         const int dir,
                                         const double & sight_2_line_ang,
                                         const double & player_2_line )
{
    double dist = calcLineDist( sight_2_line_ang, player_2_line,
                                self().landDistQStep() );
    serializer().serializeVisualObject( transport(), name, dist, dir );
}


double
VisualSenderPlayerV1::calcNoisyDist( const PObject & obj,
                                     const double dist ) const
{
    return noisyObservation().calcDist( dist, obj.pos().distance( cachedFocusPoint() ) );
}

double
VisualSenderPlayerV1::calcNoisyDistLandmark( const PObject & obj,
                                             const double dist ) const
{
    return noisyObservation().calcDistLandmark( dist, obj.pos().distance( cachedFocusPoint() ) );
}

void
VisualSenderPlayerV1::calcNoisyVel( const PVector & obj_vel,
                                    const PVector & obj_pos,
                                    const double actual_dist,
                                    const double noisy_dist,
                                    double * dist_chg,
                                    double * dir_chg ) const
{
    noisyObservation().calcVel( obj_vel, obj_pos, actual_dist, noisy_dist, dist_chg, dir_chg );
}

// void
// VisualSenderPlayerV1::calcVel( const PVector & obj_vel,
//                                const PVector & obj_pos,
//                                const double & actual_dist,
//                                const double & quant_dist,
//                                double& dist_chg,
//                                double& dir_chg ) const
// {
//     if ( actual_dist != 0.0 )
//     {
//         const PVector vtmp = obj_vel - self().vel();
//         PVector etmp = obj_pos - self().pos();
//         etmp /= actual_dist;

//         dist_chg = vtmp.x * etmp.x + vtmp.y * etmp.y;
//         //         dir_chg = RAD2DEG * ( vtmp.y * etmp.x
//         //                               - vtmp.x * etmp.y ) / actual_dist;
//         dir_chg = vtmp.y * etmp.x - vtmp.x * etmp.y;
//         dir_chg /= actual_dist;
//         dir_chg *= RAD2DEG;

//         dir_chg = ( dir_chg == 0.0
//                     ? 0.0
//                     : Quantize( dir_chg, self().dirQStep() ) );
//         dist_chg = ( quant_dist
//                      * Quantize( dist_chg
//                                  / actual_dist, 0.02 ) );
//     }
//     else
//     {
//         dir_chg = 0.0;
//         dist_chg = 0.0;
//     }
// }

void
VisualSenderPlayerV1::serializePlayer( const Player &,
                                       const std::string & name,
                                       const double & dist,
                                       const int dir,
                                       const double & dist_chg,
                                       const double & dir_chg )
{
    serializer().serializeVisualObject( transport(),
                                        name,
                                        dist, dir,
                                        dist_chg, dir_chg );
}

void
VisualSenderPlayerV1::serializePlayer( const Player &,
                                       const std::string & name,
                                       const double & dist,
                                       const int dir )
{
    serializer().serializeVisualObject( transport(),
                                        name,
                                        dist,
                                        dir );
}

// const
// std::string &
// VisualSenderPlayerV1::calcName( const PObject & obj ) const
// {
//     return obj.name();
// }

// const
// std::string &
// VisualSenderPlayerV1::calcCloseName( const PObject & obj ) const
// {
//     return obj.closeName();
// }

// const
// std::string &
// VisualSenderPlayerV1::calcTFarName( const Player & obj ) const
// {
//     return obj.nameTooFar();
// }

// const
// std::string &
// VisualSenderPlayerV1::calcUFarName( const Player & obj ) const
// {
//     return obj.nameFar();
// }

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

VisualSenderPlayerV4::VisualSenderPlayerV4( const Params & params )
    : VisualSenderPlayerV1( params )
{

}

VisualSenderPlayerV4::~VisualSenderPlayerV4()
{

}

void
VisualSenderPlayerV4::serializePlayer( const Player & player,
                                       const std::string & name,
                                       const double & dist,
                                       const int dir,
                                       const double & dist_chg,
                                       const double & dir_chg )
{
    serializer().serializeVisualObject( transport(),
                                        name,
                                        dist, dir, dist_chg, dir_chg,
                                        calcBodyDir( player ) );
}

// int
// VisualSenderPlayerV4::calcBodyDir( const Player & player ) const
// {
//     return rad2Deg( normalize_angle( player.angleBodyCommitted()
//                                      - self().angleBodyCommitted()
//                                      - self().angleNeckCommitted() ) );
// }

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


VisualSenderPlayerV5::VisualSenderPlayerV5( const Params & params )
    : VisualSenderPlayerV4( params )
{

}


VisualSenderPlayerV5::~VisualSenderPlayerV5()
{

}

void
VisualSenderPlayerV5::serializePlayer( const Player & player,
                                       const std::string & name,
                                       const double & dist,
                                       const int dir,
                                       const double & dist_chg,
                                       const double & dir_chg )
{
    serializer().serializeVisualObject( transport(),
                                        name,
                                        dist, dir, dist_chg, dir_chg,
                                        calcBodyDir( player ),
                                        calcHeadDir( player ) );
}

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

VisualSenderPlayerV6::VisualSenderPlayerV6( const Params & params )
    : VisualSenderPlayerV5( params )
{

}

VisualSenderPlayerV6::~VisualSenderPlayerV6()
{

}

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

VisualSenderPlayerV7::VisualSenderPlayerV7( const Params & params )
    : VisualSenderPlayerV6( params )
{

}

VisualSenderPlayerV7::~VisualSenderPlayerV7()
{

}


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

VisualSenderPlayerV8::VisualSenderPlayerV8( const Params & params )
    : VisualSenderPlayerV7( params )
{

}

VisualSenderPlayerV8::~VisualSenderPlayerV8()
{

}

int
VisualSenderPlayerV8::calcPointDir( const Player & player )
{
    double dir = 0.0;
    //if ( player.getArmDir( dir ) )
    if ( player.arm().getRelDir( rcss::geom::Vector2D( player.pos().x, player.pos().y ),
                                 player.angleBodyCommitted() + player.angleNeckCommitted(),
                                 dir ) )
    {
        dir += player.angleNeckCommitted()
            + player.angleBodyCommitted()
            - self().angleBodyCommitted()
            - self().angleNeckCommitted();

        // add random noise

        // With a normal distribution, 95% of the values are within 2
        // std deviations of the mean, so if we set sigma to 1/2 our
        // desired range, then 95% of values will be in that range.
        // NOTE: This means that 5% of the time it will be outside
        // of this range.
        double sigma = self().pos().distance( player.pos() )
            / self().playerType()->teamTooFarLength();
        //            / 60.0; // this should be replaced by the line below.
        //        //        / ServerParam::instance().getTeamTooFarLength ();
        sigma = std::pow( sigma, 4 ); // 4 should be parameterized
        // sigma is now a range between 0 and 1.0

        double min_noise_level = Deg2Rad( 2.5 ) * 0.5;
        // 5.0 should be parameterized.  It is halved for the same
        // reason we want sigma to be half the desired range
        sigma *= M_PI - min_noise_level;
        sigma += min_noise_level;

        //sigma is now in a range of 2.5 to 180 degrees, dependant on
        //the distance of the player.  95% of the returned random values
        //will be within +- 2*sigma of dir
        std::normal_distribution<> dst( dir, sigma );
        return rad2Deg( normalize_angle( dst( DefaultRNG::instance() ) ) );
    }
    else
    {
        return 0;
    }
}

// void
// VisualSenderPlayerV8::serializePlayer( const Player & player,
//                                        const std::string & name,
//                                        const double & dist,
//                                        const int dir,
//                                        const double & dist_chg,
//                                        const double & dir_chg )
// {
//     if ( player.arm().isPointing() )
//     {
//         int point_dir = calcPointDir( player );
//         serializer().serializeVisualObject( transport(),
//                                             name,
//                                             dist, dir,
//                                             dist_chg, dir_chg,
//                                             calcBodyDir( player ),
//                                             calcHeadDir( player ),
//                                             point_dir,
//                                             player.isTackling() );
//     }
//     else
//     {
//         serializer().serializeVisualObject( transport(),
//                                             name,
//                                             dist, dir,
//                                             dist_chg, dir_chg,
//                                             calcBodyDir( player ),
//                                             calcHeadDir( player ),
//                                             player.isTackling() );
//     }
// }

// void
// VisualSenderPlayerV8::serializePlayer( const Player & player,
//                                        const std::string & name,
//                                        const double & dist,
//                                        const int dir )
// {
//     if ( player.arm().isPointing() )
//     {
//         int point_dir = calcPointDir( player );
//         serializer().serializeVisualObject( transport(),
//                                             name,
//                                             dist,
//                                             dir,
//                                             point_dir,
//                                             player.isTackling() );
//     }
//     else
//     {
//         serializer().serializeVisualObject( transport(),
//                                             name,
//                                             dist,
//                                             dir,
//                                             player.isTackling() );
//     }
// }

void
VisualSenderPlayerV8::serializePlayer( const Player & player,
                                       const std::string & name,
                                       const double & dist,
                                       const int dir,
                                       const double & dist_chg,
                                       const double & dir_chg )
{
    if ( player.arm().isPointing() )
    {
        int point_dir = calcPointDir( player );
        serializer().serializeVisualPlayer( transport(),
                                            player,
                                            name,
                                            dist, dir,
                                            dist_chg, dir_chg,
                                            calcBodyDir( player ),
                                            calcHeadDir( player ),
                                            point_dir );
    }
    else
    {
        serializer().serializeVisualPlayer( transport(),
                                            player,
                                            name,
                                            dist, dir,
                                            dist_chg, dir_chg,
                                            calcBodyDir( player ),
                                            calcHeadDir( player ) );
    }
}

void
VisualSenderPlayerV8::serializePlayer( const Player & player,
                                       const std::string & name,
                                       const double & dist,
                                       const int dir )
{
    if ( player.arm().isPointing() )
    {
        int point_dir = calcPointDir( player );
        serializer().serializeVisualPlayer( transport(),
                                            player,
                                            name,
                                            dist,
                                            dir,
                                            point_dir );
    }
    else
    {
        serializer().serializeVisualPlayer( transport(),
                                            player,
                                            name,
                                            dist,
                                            dir );
    }
}


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

VisualSenderPlayerV13::VisualSenderPlayerV13( const Params & params )
    : VisualSenderPlayerV8( params )
{

}

VisualSenderPlayerV13::~VisualSenderPlayerV13()
{

}

/*!
//===================================================================
//
//  CLASS: VisualSensorPlayerV18
//
//  DESC: Class for the version 13 visual protocol.  This version
//        introduced the focus point.
//
//===================================================================
*/

VisualSenderPlayerV18::VisualSenderPlayerV18( const Params & params )
    : VisualSenderPlayerV13( params )
{
}

VisualSenderPlayerV18::~VisualSenderPlayerV18()
{

}

void
VisualSenderPlayerV18::updateCache()
{
    setCachedFocusPoint( self().focusPoint() );
}

/*!
//===================================================================
//
//  Register senders for different versions
//
//===================================================================
*/

namespace visual {
template< typename Sender >
VisualSenderPlayer::Ptr
create( const VisualSenderPlayer::Params & params )
{
    return VisualSenderPlayer::Ptr( new Sender( params ) );
}

RegHolder vp1 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV1 >, 1 );
RegHolder vp2 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV1 >, 2 );
RegHolder vp3 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV1 >, 3 );
RegHolder vp4 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV4 >, 4 );
RegHolder vp5 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV5 >, 5 );
RegHolder vp6 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV6 >, 6 );
RegHolder vp7 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV7 >, 7 );
RegHolder vp8 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV8 >, 8 );
RegHolder vp9 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV8 >, 9 );
RegHolder vp10 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV8 >, 10 );
RegHolder vp11 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV8 >, 11 );
RegHolder vp12 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV8 >, 12 );
RegHolder vp13 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV13 >, 13 );
RegHolder vp14 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV13 >, 14 );
RegHolder vp15 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV13 >, 15 );
RegHolder vp16 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV13 >, 16 );
RegHolder vp17 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV13 >, 17 );
RegHolder vp18 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV18 >, 18 );
RegHolder vp19 = VisualSenderPlayer::factory().autoReg( &create< VisualSenderPlayerV18 >, 19 );
}

}

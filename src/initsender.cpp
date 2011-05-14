// -*-c++-*-

/***************************************************************************
                                initsender.cpp
                  Classes for sending init messages
                             -------------------
    begin                : 22-AUG-2002
    copyright            : (C) 2002 by The RoboCup Soccer Simulator
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

#include "initsender.h"

#include "stadium.h"
#include "serializer.h"
#include "serverparam.h"
#include "playerparam.h"
#include "heteroplayer.h"

namespace rcss {

/*
//===================================================================
//
//  CLASS: InitSender
//
//===================================================================
*/

InitSender::InitSender( std::ostream & transport,
                        const boost::shared_ptr< InitSenderCommon > & common )
    : Sender( transport ),
      m_common_sender( common )
{

}

InitSender::~InitSender()
{

}


/*
//===================================================================
//
//  CLASS: InitSenderCommonV7
//
//===================================================================
*/

void
InitSenderCommonV7::sendServerParams()
{
    serializer().serializeServerParamBegin( transport() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().goalWidth() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().inertiaMoment() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerSize() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerDecay() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerRand() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerWeight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerSpeedMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerAccelMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().staminaMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().staminaInc() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().recoverInit() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().recoverDecThr() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().recoverMin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().recoverDec() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().effortInit() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().effortDecThr() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().effortMin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().effortDec() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().effortIncThr() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().effortInc() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickRand() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().teamActuatorNoise() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerRandFactorLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerRandFactorRight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickRandFactorLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickRandFactorRight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().ballSize() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().ballDecay() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().ballRand() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().ballWeight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().ballSpeedMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().ballAccelMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().dashPowerRate() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickPowerRate() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickableMargin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().controlRadius() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().controlRadiusWidth() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().maxPower() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().minPower() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().maxMoment() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().minMoment() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().maxNeckMoment() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().minNeckMoment() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().maxNeckAngle() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().minNeckAngle() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().visibleAngleDegree() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().visibleDistance() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().windDir() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().windForce() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().windAngle() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().windRand() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickableArea() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().catchAreaLength() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().catchAreaWidth() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().catchProbability() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().goalieMaxMoves() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().cornerKickMargin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().offsideActiveArea() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().windNone() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().windRandom() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().freeformCountMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().freeformMsgSize() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangWinSize() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangDefineWin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangMetaWin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangAdviceWin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangInfoWin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangMessDelay() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangMessPerCycle() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().halfTime() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().simStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().sendStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().recvStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().senseBodyStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().lcmStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().sayMsgSize() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().hearMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().hearInc() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().hearDecay() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().catchBanCycle() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().slowDownFactor() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().useOffside() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickOffOffside() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().offsideKickMargin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().audioCutDist() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().quantizeStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().landmarkQuantizeStep() );
#ifdef NEW_QSTEP
    serializer().serializeParam( transport(),
                                 ServerParam::instance().dirQStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().distQStepLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().distQStepRight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().landQStepLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().landQStepRight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().dirQStepLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().dirQStepRight() );
#else
    serializer().serializeParam( transport(),
                                 -1 );
    serializer().serializeParam( transport(),
                                 -1 );
    serializer().serializeParam( transport(),
                                 -1 );
    serializer().serializeParam( transport(),
                                 -1 );
    serializer().serializeParam( transport(),
                                 -1 );
    serializer().serializeParam( transport(),
                                 -1 );
    serializer().serializeParam( transport(),
                                 -1 );
#endif
    serializer().serializeParam( transport(),
                                 ServerParam::instance().coachMode() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().coachWithRefereeMode() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().coachOldHear() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().coachVisualStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().startGoalLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().startGoalRight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().fullstateLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().fullstateRight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().dropTime() );
    serializer().serializeServerParamEnd( transport() );
    if ( newLine() )
    {
        transport() << std::endl;
    }
    else
    {
        transport() << std::ends << std::flush;
    }
}

void
InitSenderCommonV7::sendPlayerParams()
{
    serializer().serializePlayerParamBegin( transport() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().playerTypes() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().subsMax() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().ptMax() );

    serializer().serializeParam( transport(),
                                 PlayerParam::instance().playerSpeedMaxDeltaMin() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().playerSpeedMaxDeltaMax() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().staminaIncMaxDeltaFactor() );

    serializer().serializeParam( transport(),
                                 PlayerParam::instance().playerDecayDeltaMin() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().playerDecayDeltaMax() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().inertiaMomentDeltaFactor() );

    serializer().serializeParam( transport(),
                                 PlayerParam::instance().dashPowerRateDeltaMin() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().dashPowerRateDeltaMax() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().playerSizeDeltaFactor() );

    serializer().serializeParam( transport(),
                                 PlayerParam::instance().kickableMarginDeltaMin() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().kickableMarginDeltaMax() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().kickRandDeltaFactor() );

    serializer().serializeParam( transport(),
                                 PlayerParam::instance().extraStaminaDeltaMin() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().extraStaminaDeltaMax() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().effortMaxDeltaFactor() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().effortMinDeltaFactor() );
    serializer().serializePlayerParamEnd( transport() );
    if ( newLine() )
    {
        transport() << std::endl;
    }
    else
    {
        transport() << std::ends << std::flush;
    }
}

void
InitSenderCommonV7::sendPlayerTypes()
{
    for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
    {
        const HeteroPlayer * type = stadium().playerType( i );
        if ( type != NULL )
        {
            serializer().serializePlayerTypeBegin( transport() );

            serializePlayerType( i, *type );

            serializer().serializePlayerTypeEnd( transport() );
            if ( newLine() )
            {
                transport() << std::endl;
            }
            else
            {
                transport() << std::ends << std::flush;
            }
        }
    }
}

void
InitSenderCommonV7::serializePlayerType( const int id,
                                         const HeteroPlayer & type )
{
    serializer().serializeParam( transport(),
                                 id );
    serializer().serializeParam( transport(),
                                 type.playerSpeedMax() );
    serializer().serializeParam( transport(),
                                 type.staminaIncMax() );
    serializer().serializeParam( transport(),
                                 type.playerDecay() );
    serializer().serializeParam( transport(),
                                 type.inertiaMoment() );
    serializer().serializeParam( transport(),
                                 type.dashPowerRate() );
    serializer().serializeParam( transport(),
                                 type.playerSize() );
    serializer().serializeParam( transport(),
                                 type.kickableMargin() );
    serializer().serializeParam( transport(),
                                 type.kickRand() );
    serializer().serializeParam( transport(),
                                 type.extraStamina() );
    serializer().serializeParam( transport(),
                                 type.effortMax() );
    serializer().serializeParam( transport(),
                                 type.effortMin() );
}

//-------------------------------------

void
InitSenderCommonV8::sendServerParams()
{
    serializer().serializeServerParamBegin( transport() );
    std::for_each( ServerParam::instance().verMap().begin(),
                   ServerParam::instance().verMap().end(),
                   std::bind1st( std::mem_fun( &rcss::InitSenderCommonV8::sendServerParam ),
                                 this ) );
    serializer().serializeServerParamEnd( transport() );
    if ( newLine() )
    {
        transport() << std::endl;
    }
    else
    {
        transport() << std::ends << std::flush;
    }
}

void
InitSenderCommonV8::doSendServerParam( ServerParam::VerMap::value_type param )
{
    if ( param.second <= version() )
    {

        int ivalue;
        if ( ServerParam::instance().getInt( param.first, ivalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         ivalue );
            return;
        }

        bool bvalue;
        if ( ServerParam::instance().getBool( param.first, bvalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         bvalue );
            return;
        }

        double dvalue;
        if ( ServerParam::instance().getDouble( param.first, dvalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         dvalue );
            return;
        }

        std::string svalue;
        if ( ServerParam::instance().getStr( param.first, svalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         svalue );
            return;
        }
    }
}

void
InitSenderCommonV8::sendPlayerParams()
{
    serializer().serializePlayerParamBegin( transport() );
    std::for_each( PlayerParam::instance().verMap().begin(),
                   PlayerParam::instance().verMap().end(),
                   std::bind1st( std::mem_fun( &rcss::InitSenderCommonV8::sendPlayerParam ),
                                 this ) );
    serializer().serializePlayerParamEnd( transport() );
    if ( newLine() )
    {
        transport() << std::endl;
    }
    else
    {
        transport() << std::ends << std::flush;
    }
}


void
InitSenderCommonV8::doSendPlayerParam( PlayerParam::VerMap::value_type param )
{
    if ( param.second <= version() )
    {
        int ivalue;
        if ( PlayerParam::instance().getInt( param.first, ivalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         ivalue );
            return;
        }

        bool bvalue;
        if ( PlayerParam::instance().getBool( param.first, bvalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         bvalue );
            return;
        }

        double dvalue;
        if ( PlayerParam::instance().getDouble( param.first, dvalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         dvalue );
            return;
        }

        std::string svalue;
        if ( PlayerParam::instance().getStr( param.first, svalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         svalue );
            return;
        }
    }
}

void
InitSenderCommonV8::sendPlayerTypes()
{
    for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
    {
        const HeteroPlayer * type = stadium().playerType( i );
        if ( type != NULL )
        {
            serializer().serializePlayerTypeBegin( transport() );

            serializePlayerType( i, *type );

            serializer().serializePlayerTypeEnd( transport() );
            if ( newLine() )
            {
                transport() << std::endl;
            }
            else
            {
                transport() << std::ends << std::flush;
            }
        }
    }
}

void
InitSenderCommonV8::serializePlayerType( const int id,
                                         const HeteroPlayer & type )
{
    type.serializeParams( transport(),
                          version(),
                          id );
}

}

// -*-c++-*-

/***************************************************************************
                               clangbuilder.cc  
                      Interface for building a clang message
                             -------------------
    begin                : 25-FEB-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server 
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

#include "clangbuilder.h"

namespace rcss
{
  namespace clang
  {
      Builder::Builder()
      {}
  
      
      Builder::~Builder()
      {}

      void
      Builder::setVer( const unsigned int& min, const unsigned int& max )
      {}
      

      void
      Builder::setTime( const int& time )
      {}

      void
      Builder::setSide( const int& side )
      {}


      void
      Builder::newMetaMsg() 
      {}

      
      void
      Builder::newMetaTokenVer( const double& ver ) 
      {}

      
      void
      Builder::doneMetaMsg() 
      {}

      
      void
      Builder::newDefineMsg()
      {}

      
      void
      Builder::doneDefineMsg()
      {}

      
      void
      Builder::newDefineCond()
      {}

      
      void
      Builder::doneDefineCond( const char* name )
      {}

      
      void
      Builder::newDefineDir()
      {}

      
      void
      Builder::doneDefineDir( const char* name )
      {}

      
      void
      Builder::newDefineReg()
      {}

      
      void
      Builder::doneDefineReg( const char* name )
      {}

      
      void
      Builder::newDefineAct()
      {}

      
      void
      Builder::doneDefineAct( const char* name )
      {}

      
      void
      Builder::newFreeformMsg( const char* str )
      {}

      void
      Builder::newUnsuppMsg()
      {}
      
      void  
      Builder::newInfoMsg()
      {}
 
      
      void
      Builder::doneInfoMsg()
      {}

      
      void
      Builder::newAdviceMsg()
      {}

      
      void
      Builder::doneAdviceMsg()
      {}

      
      void
      Builder::newTokenRule()
      {}

      
      void
      Builder::doneTokenRule( const int& ttl )
      {}

      
      void
      Builder::newTokenClear()
      {}

      
      void
      Builder::newActPos()
      {}

      
      void
      Builder::doneActPos()
      {}

      
      void
      Builder::newActHome()
      {}

      
      void
      Builder::doneActHome()
      {}

      
      void
      Builder::newActBallToReg()
      {}

      
      void
      Builder::doneActBallToReg()
      {}

      
      void
      Builder::newActBallToPlayer()
      {}

      
      void
      Builder::doneActBallToPlayer()
      {}

      
      void
      Builder::newActMark()
      {}

      
      void
      Builder::doneActMark()
      {}

      
      void
      Builder::newActMarkLinePlayer()
      {}

      
      void
      Builder::doneActMarkLinePlayer()
      {}

      
      void
      Builder::newActMarkLineReg()
      {}

      
      void
      Builder::doneActMarkLineReg()
      {}

      
      void
      Builder::newActOffsideLine()
      {}

      
      void
      Builder::doneActOffsideLine()
      {}

      
      void
      Builder::newActHetType( const int& type )
      {}

      
      void
      Builder::newActNamed( const char* name )
      {}

      
      void
      Builder::newDirComm()
      {}

      
      void
      Builder::doneDirComm( const bool& our_side )
      {}

      
      void
      Builder::newDirNamed( const char* name )
      {}

      
      void
      Builder::newCondTrue()
      {}

      
      void
      Builder::newCondFalse()
      {}

      
      void
      Builder::newCondPlayerPos()
      {}

      
      void
      Builder::doneCondPlayerPos( const bool& our_side, 
                         const int& min, 
                         const int& max )
      {}

      
      void
      Builder::newCondBallPos()
      {}

      
      void
      Builder::doneCondBallPos()
      {}

      
      void
      Builder::newCondBallOwner()
      {}

      
      void
      Builder::doneCondBallOwner( const bool& our_side )
      {}

      
      void
      Builder::newCondPlayMode( const PlayMode& play_mode )
      {}

      
      void
      Builder::newCondAnd()
      {}

      
      void
      Builder::doneCondAnd()
      {}

      
      void
      Builder::newCondOr()
      {}

      
      void
      Builder::doneCondOr()
      {}

      
      void
      Builder::newCondNot()
      {}

      
      void
      Builder::doneCondNot()
      {}

      
      void
      Builder::newCondNamed( const char* name )
      {}

      
      void
      Builder::newRegNull()
      {}

      
      void
      Builder::newRegQuad()
      {}

      
      void
      Builder::doneRegQuad()
      {}

      
      void
      Builder::newRegArc()
      {}

      
      void
      Builder::doneRegArc( const double& start_rad,
                  const double& end_rad,
                  const double& start_ang,
                  const double& span_ang )
      {}

      
      void
      Builder::newRegUnion()
      {}

      
      void
      Builder::doneRegUnion()
      {}

      
      void
      Builder::newRegNamed( const char* name )
      {}

      
      void
      Builder::newPointSimple( const double& x, const double& y )
      {}

      
      void
      Builder::newPointRel()
      {}

      
      void
      Builder::donePointRel( const double& x, const double& y )
      {}

      
      void
      Builder::newPointBall()
      {}

      
      void
      Builder::newPointPlayer( const bool& our_side, const int& unum )
      {}

      
      void
      Builder::addUnum( const int& unum )
      {}

      
      void
      Builder::addBallMoveToken( const BallMoveToken& bmt )
      {}


  }
}

// -*-c++-*-

/***************************************************************************
                                 clangmsg.cc 
                    Abstract base class for CLang messages
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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include "clangmsg.h"

namespace rcss
{
  namespace clang
  {
//     void
//     Msg::TypeExtractor::visit( MetaMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::TypeExtractor::visit( FreeformMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::TypeExtractor::visit( InfoMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::TypeExtractor::visit( AdviceMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::TypeExtractor::visit( DefineMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::TypeExtractor::visit( UnsuppMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::TypeExtractor::visit( RuleMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::TypeExtractor::visit( DelMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::ConstTypeExtractor::visit( const MetaMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::ConstTypeExtractor::visit( const FreeformMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::ConstTypeExtractor::visit( const InfoMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::ConstTypeExtractor::visit( const AdviceMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::ConstTypeExtractor::visit( const DefineMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::ConstTypeExtractor::visit( const UnsuppMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::ConstTypeExtractor::visit( const RuleMsg* msg )
//     { setValue( msg ); }
    
//     void
//     Msg::ConstTypeExtractor::visit( const DelMsg* msg )
//     { setValue( msg ); }
    
    Msg::Msg()
      : M_time_recv( -1 ),
        M_min_ver( (unsigned int)-1 ),
        M_max_ver( 0 ),
        M_time_send( -1 ),
        M_side( 0 )
    {}
    
    Msg::Msg( const int& time_recv )
      : M_time_recv( time_recv ),
        M_min_ver( (unsigned int)-1 ),
        M_max_ver( 0 ),
        M_time_send( -1 ),
        M_side( 0 )
    {}
    
    Msg::~Msg()
    {}
    
    
    void
    Msg::setVer( const unsigned int& min, const unsigned int& max )
    {
      if( min == max )
        {
          // Then the version is restriced to min/max
          if( min < M_max_ver )
            M_min_ver = min;
          else if( min > M_min_ver )
            M_max_ver = min;
          else if( M_max_ver < M_min_ver )
            M_min_ver = M_max_ver = min;
        }
      else
        {
          if( min < M_min_ver )
            M_min_ver = min;
          if( max > M_max_ver )
            M_max_ver = max;
          if( min > max 
              && M_max_ver < M_min_ver 
              && min > M_max_ver 
              && max < M_min_ver )
            {
              M_min_ver = min;
              M_max_ver = max;
            }
        }
    }

    unsigned int
    Msg::getMinVer() const
    { return M_min_ver; }
    
    unsigned int
    Msg::getMaxVer() const
    { return M_max_ver; }
    
    bool
    Msg::isSupported( const unsigned int& min, const unsigned int& max ) const
    {
      // ino this could be just one big return statement, but it's confusing
      // enough as it is.
      if( M_min_ver >= M_max_ver )
        {
          // then the message can be heard by any client that supports
          // clang message inbetween and including M_min_ver and M_max_ver
          if( min <= M_max_ver
              && max >= M_max_ver )
            return true;
          if( min <= M_min_ver
              && max >= M_min_ver )
            return true;
        }
      else // M_min_ver < M_max_ver
        {
          // then the message can only be heard by clients that support the
          // range M_min_ver to M_max_ver
          if( min <= M_min_ver
              && max >= M_max_ver )
            return true;
        }
      return false;
    }
    
    int 
    Msg::getTimeRecv() const
    { return M_time_recv; }
    
    void
    Msg::setTimeRecv( const int& time_recv )
    { M_time_recv = time_recv; }
    
    int 
    Msg::getTimeSend() const
    { return M_time_send; }
    
    void
    Msg::setTimeSend( const int& time_send )
    { M_time_send = time_send; }
    
    int 
    Msg::getSide() const
    { return M_side; }
    
    void
    Msg::setSide( const int& side )
    { M_side = side; }
  }
}
    
std::ostream& 
operator<<( std::ostream & os, const rcss::clang::Msg& m )
{ return m.print( os ); }
    
    

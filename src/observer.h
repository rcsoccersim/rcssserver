// -*-c++-*-

/***************************************************************************
                                observer.h
                           Base for observer classes
                             -------------------
    begin                : 2002-10-07
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


#ifndef RCSSOBSERVER_H
#define RCSSOBSERVER_H

//#include "rcssexceptions.h"
#include <memory>
#include <stdexcept>

namespace rcss {

/*!
//===================================================================
//
//  CLASS: Observer
//
//  DESC: Base observer.
//
//===================================================================
*/

template < class S >
class BaseObserver {
public:
    typedef S Sender;
private:
    std::shared_ptr< Sender > M_sender;

public:
    BaseObserver()
      { }

    BaseObserver( std::shared_ptr< Sender > sender )
        : M_sender( sender )
      { }

    virtual
    ~BaseObserver()
      { }

    void setSender( std::shared_ptr< Sender > sender )
      {
          M_sender = sender;
      }

    Sender & sender()
      {
          if ( ! M_sender )
          {
              // throw util::NullErr( __FILE__, __LINE__,
              //                      "Sender is null" );
              throw std::logic_error( "Sender is null" );
          }
          return *M_sender;
      }

    const Sender & sender() const
      {
          if ( ! M_sender )
          {
              // throw util::NullErr( __FILE__, __LINE__,
              //                      "Sender is null" );
              throw std::logic_error( "Sender is null" );
          }
          return *M_sender;
      }

private:

    BaseObserver( const BaseObserver & ) = delete; // not used;
    BaseObserver & operator=( const BaseObserver & ) = delete; // not used;
};

}

#endif

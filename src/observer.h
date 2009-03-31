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

#include "rcssexceptions.h"
#include <memory>

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
    Sender * M_sender;
    bool M_owns_sender;

public:
    BaseObserver()
        : M_sender( NULL ),
          M_owns_sender( false )
      { }

    BaseObserver( Sender & sender )
        : M_sender( &sender ),
          M_owns_sender( false )
      { }

    BaseObserver( std::auto_ptr< Sender > sender )
        : M_sender( sender.release() ),
          M_owns_sender( true )
      { }


    ~BaseObserver()
      {
          clear();
      }

    void setSender( Sender & sender )
      {
          clear();
          M_sender = &sender;
          M_owns_sender = false;
      }

    void setSender( std::auto_ptr< Sender > sender )
      {
          clear();
          M_sender = sender.release();
          M_owns_sender = true;
      }

    Sender & sender()
      {
          if ( M_sender == NULL )
          {
              throw util::NullErr( __FILE__, __LINE__,
                                   "Sender is null" );
          }
          return *M_sender;
      }

    const Sender & sender() const
      {
          if ( M_sender == NULL )
          {
              throw util::NullErr( __FILE__, __LINE__,
                                   "Sender is null" );
          }
          return *M_sender;
      }

private:
    void clear()
      {
          if ( M_owns_sender )
          {
              delete M_sender;
              M_sender = NULL;
          }
      }

    BaseObserver( const BaseObserver & ); // not used;
    BaseObserver & operator=( const BaseObserver & ); // not used;
};

}

#endif

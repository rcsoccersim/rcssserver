// -*-c++-*-

/***************************************************************************
                               paramsetter.hpp
                Interface for setting parameter values
                             -------------------
    begin                : 14-MAY-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
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

#ifndef PARAMSETTER_HPP
#define PARAMSETTER_HPP

#include <boost/shared_ptr.hpp>

namespace rcss {
namespace conf {
namespace priv {

template< typename Arg >
class SetterBase {
public:
    virtual
    ~SetterBase()
      {}

    virtual
    void operator()( Arg arg ) = 0;

};

template< typename Arg, typename C >
class SetterMFun
    : public SetterBase< Arg > {
public:
    SetterMFun( C * c,
                void(C::*fun)( Arg ) )
        : m_class( c )
        , m_fun( fun )
      { }

    void operator()( Arg arg )
      {
          (m_class->*m_fun)( arg );
      }

private:
    C * m_class;
    void(C::*m_fun)( Arg );
};

template< typename Arg >
class SetterPFun
    : public SetterBase< Arg > {
public:
    SetterPFun( void(*fun)( Arg ) )
        : m_fun( fun )
      { }

    void operator()( Arg arg )
      {
          (*m_fun)( arg );
      }

private:
    void(*m_fun)( Arg );
};

template< typename Arg >
class SetterArg
    : public SetterBase< Arg > {
public:
    SetterArg( Arg & arg )
        : m_arg( arg )
      { }

    void operator()( Arg arg )
      {
          m_arg = arg;
      }

private:
    Arg & m_arg;
};

}


template< typename Arg >
class Setter {
public:
    Setter( const Setter< Arg > & s )
        : m_setter( s.m_setter )
      { }

    template< typename C >
    Setter( C * c,
            void(C::*fun)( Arg ) )
        : m_setter( new rcss::conf::priv::SetterMFun< Arg, C >( c, fun ) )
      { }

    Setter( void(*fun)( Arg ))
        : m_setter( new rcss::conf::priv::SetterPFun< Arg >( fun ) )
      { }

    Setter( Arg & arg )
        : m_setter( new rcss::conf::priv::SetterArg< Arg >( arg ) )
      { }

    void operator()( Arg arg )
      {
          m_setter->operator()( arg );
      }

private:
    boost::shared_ptr< rcss::conf::priv::SetterBase< Arg > > m_setter;
};

template< typename Arg >
static
inline
Setter< Arg >
makeSetter( Arg & arg )
{
    return Setter< Arg >( arg );
}

template< typename Arg >
static
inline
Setter< Arg >
makeSetter( void(*&pfun)( Arg ) )
{
    return Setter< Arg >( pfun );
}

template< typename Arg, typename C >
static
inline
Setter< Arg >
makeSetter( C* c, void(C::*mfun)( Arg ) )
{
    return Setter< Arg >( c, mfun );
}

template< typename Arg, typename C >
static
inline
Setter< Arg >
makeSetter( C& c, void(C::*mfun)( Arg ) )
{
    return Setter< Arg >( &c, mfun );
}

}
}

#endif

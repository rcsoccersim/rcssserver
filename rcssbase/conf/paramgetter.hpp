// -*-c++-*-

/***************************************************************************
                               paramgetter.hpp
                Interface for getting parameter values
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

#ifndef PARAMGETTER_HPP
#define PARAMGETTER_HPP

#include <boost/shared_ptr.hpp>

namespace rcss {
namespace conf {
namespace priv {

template< typename RVal >
class GetterBase {
public:
    virtual
    ~GetterBase()
      {}

    virtual
    RVal operator()() const = 0;

};

template< typename RVal, typename C >
class GetterMFun
    : public GetterBase< RVal >{
public:
    GetterMFun( const C* c,
                RVal(C::*fun)() const )
        : m_class( c )
        , m_fun( fun )
      {}

    RVal operator()() const
      {
          return (m_class->*m_fun)();
      }


private:
    const C * m_class;
    RVal(C::*m_fun)() const;
};

template< typename RVal >
class GetterPFun
    : public GetterBase< RVal >
{
public:
    GetterPFun( RVal(*fun)() )
    : m_fun( fun )
      { }

    RVal operator()() const
      {
          return (*m_fun)();
      }


private:
    RVal(*m_fun)();
};

template< typename RVal >
class GetterRVal
    : public GetterBase< RVal > {
public:
    GetterRVal( const RVal & rval )
        : m_rval( rval )
      { }

    RVal operator()() const
      {
          return m_rval;
      }

private:
    const RVal & m_rval;
};
}

template< typename RVal >
class Getter {
public:
    template< typename C >
    Getter( const C* c,
            RVal(C::*fun)() const )
        : m_getter( new rcss::conf::priv::GetterMFun< RVal, C >( c, fun ) )
      { }

    Getter( RVal(*fun)() )
    : m_getter( new rcss::conf::priv::GetterPFun< RVal >( fun ) )
      { }

    Getter( const RVal & rval )
        : m_getter( new rcss::conf::priv::GetterRVal< RVal >( rval ) )
      { }

    RVal operator()() const
      {
          return (*m_getter)();
      }

private:
    boost::shared_ptr< rcss::conf::priv::GetterBase< RVal > > m_getter;
};

template< typename RVal >
static
inline
Getter< RVal >
makeGetter( const RVal & rval )
{
    return Getter< RVal >( rval );
}

template< typename RVal >
static
inline
Getter< RVal >
makeGetter( RVal(*&pfun)() )
{
    return Getter< RVal >( pfun );
}


template< typename RVal, typename C >
static
inline
Getter< RVal >
makeGetter( C* c,
            RVal(C::*mfun)() const )
{
    return Getter< RVal >( c, mfun );
}

template< typename RVal, typename C >
static
inline
Getter< RVal >
makeGetter( const C& c,
            RVal(C::*mfun)() const )
{
    return Getter< RVal >( &c, mfun );
}

}
}

#endif

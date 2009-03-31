// -*-c++-*-

/***************************************************************************
                                  random.h
                            Random Number Generator
                             -------------------
    begin                : 17-JAN-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
                           Maintainance Group.
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


#ifndef RCSSSERVER_RANDOM_H
#define RCSSSERVER_RANDOM_H

#include <boost/random.hpp>

#include <algorithm>
#include <memory>
#include <cstdlib>

namespace rcss {
namespace random {

class DefaultRNG
    : public boost::mt19937 {
public:
    static
    DefaultRNG & instance()
      {
          static DefaultRNG the_instance;
          return the_instance;
      }

    static
    DefaultRNG &
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x520)
    // Work around overload resolution problem (Gennadiy E. Rozental)
    instance( const boost::mt19937::result_type & value )
#else
        instance( boost::mt19937::result_type value )
#endif
      {
          instance().seed( value );
          return instance();
      }

    // For GCC, moving this function out-of-line prevents inlining, which may
    // reduce overall object code size.  However, MSVC does not grok
    // out-of-line definitions of member function templates.
    template< class Generator >
    static
    DefaultRNG & instance( Generator & gen )
      {
          instance().seed( gen );
          return instance();
      }
private:
    DefaultRNG()
        : boost::mt19937()
      { }
};

// class UniformRNG
//     : public boost::random_number_generator< rcss::random::DefaultRNG >
// {
// public:
//     static
//     UniformRNG&
//     instance ()
//       { static UniformRNG the_instance; return the_instance; }
//
// private:
//     UniformRNG()
//         : boost::random_number_generator< rcss::random::DefaultRNG >( DefaultRNG::instance() )
//       {}
// };

} // namespace random
} // namespace rcss


// old random code
//#define RANDOMBASE		1000
//#define IRANDOMBASE		31

//#define drand(h,l)	(((((h)-(l)) * ((double)(random()%RANDOMBASE) / (double)RANDOMBASE))) + (l))
//#define	irand(x)	((random() / IRANDOMBASE) % (x))

inline
int
irand( int x )
{
    if ( x <= 1 ) return 0;
    //return boost::uniform_smallint<>( 0, x - 1 )( rcss::random::DefaultRNG::instance() );
    //return (int)((double)x * std::rand() / (RAND_MAX + 1.0));
    return rcss::random::DefaultRNG::instance()() % x;
}

inline
double
drand( double low, double high )
{
    if ( low > high ) std::swap( low, high );
    if ( high - low < 1.0e-10 ) return (low + high) * 0.5;
    //return boost::uniform_real<>( low, high )( rcss::random::DefaultRNG::instance() );
    boost::uniform_real<> rng( low, high );
    boost::variate_generator< rcss::random::DefaultRNG &, boost::uniform_real<> >
      gen( rcss::random::DefaultRNG::instance(), rng );
    return gen();
}

#endif

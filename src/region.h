/* -*- Mode: C++ -*- */
/*
 *Copyright:

 Copyright (C) 2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Itsuki Noda,	Mikhail Prokopenko, Jan Wendler
 Copyright (C) 2002- RoboCup Soccer Simulator Maintainance Group.

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

/* This files defines a class region to specify regions for the coach messages */

#ifndef CLANGREGION_H
#define CLANGREGION_H

#include "vector.h"
#include "clangutil.h"
#include "arithop.h"

#include <memory>
#include <string>
#include <list>

namespace rcss {
namespace clang {

class Point {
protected:
    Point()
      { }
public:
    virtual
    ~Point()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const = 0;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const = 0;

    virtual
    std::auto_ptr< Point > deepCopy() const = 0;
};

inline
std::ostream &
operator<<( std::ostream & os,
            const rcss::clang::Point & r )
{
    return r.print( os );
}


class PointSimple
    : public Point {
private:
    PointSimple(); // not used

public:
    PointSimple( const double & x,
                 const double & y )
        : Point(),
          M_vec( x, y )
      { }

    PointSimple( const geom::Vector2D & vec )
        : Point(),
          M_vec( vec )
      { }

    ~PointSimple()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Point > deepCopy() const
      {
          return std::auto_ptr< Point >( new PointSimple( *this ) );
      }

    const geom::Vector2D & getVec() const
      {
          return M_vec;
      }

private:
    geom::Vector2D M_vec;
};

class PointRel
    : public Point {
private:
    PointRel(); // not used
    PointRel( const PointRel & ); // not used
    PointRel & operator=( const PointRel & ); // not used
public:

    PointRel( const double & x,
              const double & y,
              std::auto_ptr< Point > origin )
        : Point(),
          M_origin( origin ),
          M_offset( x, y )
      { }

    PointRel( const PointSimple offset,
              std::auto_ptr< Point > origin )
        : Point(),
          M_origin( origin ),
          M_offset( offset )
      { }

    ~PointRel()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Point > deepCopy() const;


    PointSimple getOffset() const
      {
          return M_offset;
      }

    const Point * getOrigin() const
      {
          return M_origin.get();
      }

private:
	std::auto_ptr< Point > M_origin;
    PointSimple M_offset;
};


class PointBall
    : public Point {
public:
    PointBall()
        : Point()
      { }

    ~PointBall()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Point > deepCopy() const
      {
          return std::auto_ptr< Point >( new PointBall( *this ) );
      }

};

class PointPlayer
    : public Point {
private:
    PointPlayer();
public:
    PointPlayer( const bool our_side,
                 const UNum & unum )
        : Point(),
          M_our_side( our_side ),
          M_unum( unum )
      { }

    ~PointPlayer()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Point > deepCopy() const
      {
          return std::auto_ptr< Point >( new PointPlayer( *this ) );
      }

private:
    bool M_our_side;
    UNum M_unum;
};



class PointArith
    : public Point {
private:
    PointArith(); // not used
public:
    PointArith( std::auto_ptr< Point > pt1,
                std::auto_ptr< Point > pt2,
                const util::ArithOp & arith_op );
    PointArith( const PointArith & pt );

    ~PointArith()
      { }

    PointArith & operator=( const PointArith & pt );

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Point > deepCopy() const
      {
          return std::auto_ptr< Point >( new PointArith( *this ) );
      }

private:
    std::auto_ptr< Point > M_points[2];
    const util::ArithOp * M_arith_op;
    unsigned int M_idx;
};

class Region {
protected:
    Region()
      { }
public:
    virtual
    ~Region()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const = 0;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const = 0;

    virtual
    std::auto_ptr< Region > deepCopy() const = 0;
};

inline
std::ostream &
operator<<( std::ostream & os,
            const rcss::clang::Region & r )
{
    return r.print( os );
}


class RegNull
    : public Region {
public:
    RegNull()
        : Region()
      { }

    ~RegNull()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Region > deepCopy() const
      {
          return std::auto_ptr< Region >( new RegNull( *this ) );
      }
};

class RegQuad
    : public Region {
private:
	RegQuad(); // not used
    RegQuad( const RegQuad & ); // not used
    RegQuad & operator=( const RegQuad & ); // not used
public:
	RegQuad( std::auto_ptr< Point > pt0,
             std::auto_ptr< Point > pt1,
             std::auto_ptr< Point > pt2,
             std::auto_ptr< Point > pt3 );
	~RegQuad()
      { }

    virtual
	std::ostream & print( std::ostream & out ) const;

    virtual
	std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Region > deepCopy() const;

private:
	std::auto_ptr< Point > M_points[ 4 ];
};


class RegArc
    : public Region {
private:
	RegArc(); // not used
public:
    RegArc( std::auto_ptr< Point > center,
            const double & start_rad,
            const double & end_rad,
            const double & start_ang,
            const double & span_ang );
    ~RegArc()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Region > deepCopy() const;

private:
    /* start rad <= end_rad */
    double M_start_rad, M_end_rad;
    double M_start_ang, M_span_ang;
	std::auto_ptr< Point > M_center;
};


class RegUnion
    : public Region {
public:
	typedef std::list< Region * > Storage;

    RegUnion()
        : Region()
      { }

    RegUnion( const Storage & regs )
        : Region(),
          M_regs( regs )
      { }

    ~RegUnion()
      {
          deleteAll();
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Region > deepCopy() const;

    Storage & getRegions()
      {
          return M_regs;
      }

private:
	Storage M_regs;

	void deleteAll();
};


class RegNamed
    : public Region {
private:
    RegNamed(); // not used
public:
    RegNamed( const std::string & name )
        : Region(),
          M_name( name )
      { }

    ~RegNamed()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Region > deepCopy() const
      {
          return std::auto_ptr< Region >( new RegNamed( *this ) );
      }

private:
    std::string M_name;
};


class RegPoint
    : public Region {
private:
    RegPoint(); // not used
    RegPoint( const RegPoint & point ) ;
    RegPoint & operator=( const RegPoint & point ); // not used
public:

    RegPoint( std::auto_ptr< Point > point );
    ~RegPoint();

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Region > deepCopy() const
      {
          return std::auto_ptr< Region >( new RegPoint( *this ) );
      }

private:
    std::auto_ptr< Point > M_point;
};


class RegTri
    : public Region {
private:
    RegTri(); // not used
public:
    RegTri( std::auto_ptr< Point > pt0,
            std::auto_ptr< Point > pt1,
            std::auto_ptr< Point > pt2 );

    ~RegTri()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

	virtual
    std::auto_ptr< Region > deepCopy() const;

private:
	std::auto_ptr< Point > m_points[ 3 ];
};



class RegRec
    : public Region {
private:
    RegRec(); // not used

public:
    RegRec( std::auto_ptr< Point > pt0,
            std::auto_ptr< Point > pt1 );

    ~RegRec()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Region > deepCopy() const;

private:
	std::auto_ptr< Point > M_points[ 2 ];
};

}
}

#endif

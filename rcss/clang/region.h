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

#ifndef RCSS_CLANGREGION_H
#define RCSS_CLANGREGION_H

#include <rcss/clang/clangutil.h>
#include <rcss/clang/arithop.h>
#include <rcss/vector.h>

#include <memory>
#include <string>
#include <list>

namespace rcss {
namespace clang {

class Point {
protected:
    Point() = default;

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
    std::shared_ptr< Point > deepCopy() const = 0;
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
    PointSimple() = delete; // not used

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

    ~PointSimple() override
      { }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Point > deepCopy() const override
      {
          return std::shared_ptr< Point >( new PointSimple( *this ) );
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
    PointRel() = delete; // not used
    PointRel( const PointRel & ) = delete; // not used
    PointRel & operator=( const PointRel & ) = delete; // not used
public:

    PointRel( const double & x,
              const double & y,
              std::shared_ptr< Point > origin )
        : Point(),
          M_origin( origin ),
          M_offset( x, y )
      { }

    PointRel( const PointSimple offset,
              std::shared_ptr< Point > origin )
        : Point(),
          M_origin( origin ),
          M_offset( offset )
      { }

    ~PointRel() override
      { }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Point > deepCopy() const override;


    PointSimple getOffset() const
      {
          return M_offset;
      }

    const Point * getOrigin() const
      {
          return M_origin.get();
      }

private:
	std::shared_ptr< Point > M_origin;
    PointSimple M_offset;
};


class PointBall
    : public Point {
public:
    PointBall()
        : Point()
      { }

    ~PointBall() override
      { }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Point > deepCopy() const override
      {
          return std::shared_ptr< Point >( new PointBall( *this ) );
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

    ~PointPlayer() override
      { }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Point > deepCopy() const override
      {
          return std::shared_ptr< Point >( new PointPlayer( *this ) );
      }

private:
    bool M_our_side;
    UNum M_unum;
};



class PointArith
    : public Point {
private:
    PointArith() = delete; // not used
public:
    PointArith( std::shared_ptr< Point > pt1,
                std::shared_ptr< Point > pt2,
                const util::ArithOp & arith_op );
    PointArith( const PointArith & pt );

    ~PointArith() override
      { }

    PointArith & operator=( const PointArith & pt );

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Point > deepCopy() const override
      {
          return std::shared_ptr< Point >( new PointArith( *this ) );
      }

private:
    std::shared_ptr< Point > M_points[2];
    const util::ArithOp * M_arith_op;
    unsigned int M_idx;
};

class Region {
protected:
    Region() = default;

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
    std::shared_ptr< Region > deepCopy() const = 0;
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
    RegNull() = default;

    ~RegNull() override
      { }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Region > deepCopy() const override
      {
          return std::shared_ptr< Region >( new RegNull( *this ) );
      }
};

class RegQuad
    : public Region {
private:
	RegQuad() = delete; // not used
    RegQuad( const RegQuad & ) = delete; // not used
    RegQuad & operator=( const RegQuad & ) = delete; // not used
public:
	RegQuad( std::shared_ptr< Point > pt0,
             std::shared_ptr< Point > pt1,
             std::shared_ptr< Point > pt2,
             std::shared_ptr< Point > pt3 );
	~RegQuad() override
      { }

    virtual
	std::ostream & print( std::ostream & out ) const override;

    virtual
	std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Region > deepCopy() const override;

private:
	std::shared_ptr< Point > M_points[ 4 ];
};


class RegArc
    : public Region {
private:
	RegArc() = delete;
public:
    RegArc( std::shared_ptr< Point > center,
            const double & start_rad,
            const double & end_rad,
            const double & start_ang,
            const double & span_ang );
    ~RegArc() override
      { }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Region > deepCopy() const override;

private:
    /* start rad <= end_rad */
    double M_start_rad, M_end_rad;
    double M_start_ang, M_span_ang;
	std::shared_ptr< Point > M_center;
};


class RegUnion
    : public Region {
public:
	typedef std::list< std::shared_ptr< Region > > Storage;

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
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Region > deepCopy() const override;

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
    RegNamed() = delete; // not used
public:
    RegNamed( const std::string & name )
        : Region(),
          M_name( name )
      { }

    ~RegNamed() override
      { }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Region > deepCopy() const
      {
          return std::shared_ptr< Region >( new RegNamed( *this ) );
      }

private:
    std::string M_name;
};


class RegPoint
    : public Region {
private:
    RegPoint() = delete; // not used
    RegPoint( const RegPoint & point );
    RegPoint & operator=( const RegPoint & point ) = delete; // not used
public:

    RegPoint( std::shared_ptr< Point > point );
    ~RegPoint() override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Region > deepCopy() const override
      {
          return std::shared_ptr< Region >( new RegPoint( *this ) );
      }

private:
    std::shared_ptr< Point > M_point;
};


class RegTri
    : public Region {
private:
    RegTri() = delete; // not used
public:
    RegTri( std::shared_ptr< Point > pt0,
            std::shared_ptr< Point > pt1,
            std::shared_ptr< Point > pt2 );

    ~RegTri() override
      { }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

	virtual
    std::shared_ptr< Region > deepCopy() const override;

private:
	std::shared_ptr< Point > m_points[ 3 ];
};



class RegRec
    : public Region {
private:
    RegRec() = delete; // not used

public:
    RegRec( std::shared_ptr< Point > pt0,
            std::shared_ptr< Point > pt1 );

    ~RegRec()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Region > deepCopy() const override;

private:
	std::shared_ptr< Point > M_points[ 2 ];
};

}
}

#endif

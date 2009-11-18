/* -*- Mode: C++ -*- */
/*
 *Copyright:

 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "region.h"

#include <iostream>

namespace rcss {
namespace clang {

/*** PointSimple ***/

std::ostream &
PointSimple::print( std::ostream & out ) const
{
    return out << "(pt " << M_vec.getX() << " " << M_vec.getY() << ")";
}

std::ostream &
PointSimple::printPretty( std::ostream & out,
                          const std::string & line_header ) const
{
    return out << line_header << "Point("
               << M_vec.getX() << ", " << M_vec.getY() << ")"
               << std::endl;
}

/*** PointRel ***/

std::ostream &
PointRel::print( std::ostream & out ) const
{
    out << "(pt " << M_offset.getVec().getX() << " "
        << M_offset.getVec().getY() << " ";
    if ( getOrigin() == NULL )
    {
        out << "(null)";
    }
    else
    {
        out << *getOrigin();
    }
    return out << ")";
}

std::ostream &
PointRel::printPretty( std::ostream & out,
                       const std::string & line_header ) const
{
    out << line_header << "Point-Relative(" << M_offset.getVec().getX()
        << ", " << M_offset.getVec().getY() << " ";
    if ( getOrigin() == NULL )
    {
        out << "(null)";
    }
    else
    {
        getOrigin()->printPretty( out, line_header + " " );
    }
    return out << ")" << std::endl;
}

std::auto_ptr< Point >
PointRel::deepCopy() const
{
    if ( getOrigin() )
    {
        return std::auto_ptr< Point >( new PointRel( M_offset, M_origin->deepCopy() ) );
    }
    else
    {
        return std::auto_ptr< Point >( new PointRel( M_offset, std::auto_ptr< Point >() ) );
    }
}

/*** PointBall ***/

std::ostream &
PointBall::print( std::ostream & out ) const
{
    return out << "(pt ball)";
}

std::ostream &
PointBall::printPretty( std::ostream & out,
                        const std::string & line_header ) const
{
    return out << line_header << "Point-Ball" << std::endl;
}


/*** PointPlayer ***/

// PointPlayer::PointPlayer()
//     : Point(),
//       M_our_side( false ),
//       M_unum()
// {
// }

std::ostream &
PointPlayer::print( std::ostream & out ) const
{
    return out << "(pt " << ( M_our_side ? "our" : "opp" )
               << " " << M_unum << ")";
}

std::ostream &
PointPlayer::printPretty( std::ostream & out,
                          const std::string & line_header ) const
{
    return out << line_header << "Point-Player("
               << ( M_our_side ? "our team" : "opponent" )
               << " " << M_unum << ")"
               << std::endl;
}

/*** PointArith ***/

// PointArith::PointArith()
//     : Point(),
//       M_arith_op( NULL ),
//       M_idx( 0 )
// {
// }

PointArith::PointArith( std::auto_ptr< Point > pt1,
                        std::auto_ptr< Point > pt2,
                        const util::ArithOp & arith_op )
    : Point(),
      M_arith_op( &arith_op ),
      M_idx( 0 )
{
    M_points[ 0 ] = pt1;
    M_points[ 1 ] = pt2;
}

PointArith::PointArith( const PointArith & pt )
    : Point(),
      M_arith_op( pt.M_arith_op ),
      M_idx( 0 )
{
    std::auto_ptr< Point > pt_copy1 = pt.M_points[ 0 ]->deepCopy();
    M_points[ 0 ] = pt_copy1;
    std::auto_ptr< Point > pt_copy2 = pt.M_points[ 1 ]->deepCopy();
    M_points[ 1 ] = pt_copy2;
}

PointArith &
PointArith::operator=( const PointArith & pt )
{
    std::auto_ptr< Point > pt_copy1 = pt.M_points[ 0 ]->deepCopy();
    M_points[ 0 ] = pt_copy1;
    std::auto_ptr< Point > pt_copy2 = pt.M_points[ 1 ]->deepCopy();
    M_points[ 1 ] = pt_copy2;
    M_idx = pt.M_idx;
    return *this;
}

std::ostream &
PointArith::print( std::ostream & out ) const
{
    out << "(";

    if ( M_points[ 0 ].get() == NULL )
    {
        out << "(null)";
    }
    else
    {
        out << *(M_points[ 0 ]);
    }

    out << " ";

    if ( M_arith_op == NULL )
    {
              out << "(null)";
    }
    else
    {
        out << *M_arith_op;
    }

    out << " ";

    if ( M_points[ 1 ].get() == NULL )
    {
              out << "(null)";
    }
    else
    {
        out << *(M_points[ 1 ]);
    }

    return out << ")";
}

std::ostream &
PointArith::printPretty( std::ostream & out,
                         const std::string & line_header ) const
{
    out << line_header << "Point-Arith" << std::endl;

    if ( M_points[ 0 ].get() == NULL )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        M_points[ 0 ]->printPretty( out, line_header + " " );
    }

    if ( M_points[ 1 ].get() == NULL )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        M_points[ 1 ]->printPretty( out, line_header + " " );
    }

    if ( M_arith_op == NULL )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        out << line_header << " operation: " << *M_arith_op << std::endl;
    }

    return out;
}

/*** RegNull ***/

std::ostream &
RegNull::print( std::ostream & out ) const
{
    return out << "(null)";
}

std::ostream &
RegNull::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    return out << line_header << "null region" << std::endl;
}



/*** RegQuad ***/

RegQuad::RegQuad( std::auto_ptr< Point > pt0,
                  std::auto_ptr< Point > pt1,
                  std::auto_ptr< Point > pt2,
                  std::auto_ptr< Point > pt3 )
    : Region()
{
	M_points[ 0 ] = pt0;
	M_points[ 1 ] = pt1;
	M_points[ 2 ] = pt2;
	M_points[ 3 ] = pt3;
}

std::ostream &
RegQuad::print( std::ostream & out ) const
{
    out << "(quad";
    for ( unsigned int i = 0; i < 3; ++i )
    {
	    if ( M_points[ i ].get() == NULL )
        {
            out << " (null)";
        }
	    else
        {
            out << " " << *M_points[ i ];
        }
    }
    return out << ")";
}

std::ostream &
RegQuad::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    out << line_header
        << "Quadrangle: ";
    for ( unsigned int i = 0; i < 4; ++i )
    {
        if ( M_points[ i ].get() == NULL )
        {
            out << "(null) ";
        }
        else
        {
            M_points[ i ]->printPretty( out, line_header + " " );
            out << " ";
        }
    }
    return out << std::endl;
}

std::auto_ptr< Region >
RegQuad::deepCopy() const
{
    std::auto_ptr< Point > pts[ 4 ];
    for ( int i = 0; i < 4; ++i )
    {
        if ( M_points[ i ].get() )
        {
            pts[ i ] = M_points[ i ]->deepCopy();
        }
    }
    return std::auto_ptr< Region >( new RegQuad( pts[ 0 ],
                                                 pts[ 1 ],
                                                 pts[ 2 ],
                                                 pts[ 3 ] ) );
}


/*** RegArc ***/

RegArc::RegArc()
    : M_start_rad( 0.0 ),
      M_end_rad( 0.0 ),
      M_start_ang( 0.0 ),
      M_span_ang( 0.0 )
{

}

RegArc::RegArc( std::auto_ptr< Point > center,
                const double & start_rad,
                const double & end_rad,
                const double & start_ang,
                const double & span_ang )
    : M_start_rad( start_rad ),
      M_end_rad( end_rad ),
      M_start_ang( start_ang ),
      M_span_ang( span_ang ),
      M_center( center )
{

}

std::ostream &
RegArc::print( std::ostream & out ) const
{
    out << "(arc";
    if ( M_center.get() == NULL )
    {
        out << " (null)";
    }
    else
    {
        out << " " << *M_center;
    }

    return out << " " << M_start_rad
               << " " << M_end_rad
               << " " << M_start_ang
               << " " << M_span_ang
               << ")";
}

std::ostream &
RegArc::printPretty( std::ostream & out,
                     const std::string & line_header ) const
{
    out << line_header
        << "Arc: "
        << "center=";
    if ( M_center.get() == NULL )
    {
        out << "(null)";
    }
    else
    {
        M_center->printPretty( out, line_header + " " );
    }

    return out << "\tradius="
               << M_start_rad
               << " to "
               << M_end_rad
               << "\tangle="
               << M_start_ang
               << " for "
               << M_span_ang
               << std::endl;
}

std::auto_ptr< Region >
RegArc::deepCopy() const
{
    if ( M_center.get() )
    {
        return std::auto_ptr< Region >( new RegArc( M_center->deepCopy(),
                                                    M_start_rad, M_end_rad,
                                                    M_start_ang, M_span_ang ) );
    }
    else
    {
        return std::auto_ptr< Region >( new RegArc( std::auto_ptr< Point >(),
                                                    M_start_rad, M_end_rad,
                                                    M_start_ang, M_span_ang ) );
    }
}


/*** RegUnion ***/

std::ostream &
RegUnion::print( std::ostream & out ) const
{
    out << "(reg";
    for ( Storage::const_iterator iter = M_regs.begin();
          iter != M_regs.end();
          ++iter )
    {
	    if ( *iter == NULL )
        {
            out << " (null)";
        }
	    else
        {
            out << " " << **iter;
        }
    }
    return out << ")";
}

std::ostream &
RegUnion::printPretty( std::ostream & out,
                       const std::string & line_header ) const
{
    out << line_header << "Region Union:" << std::endl;
    for ( Storage::const_iterator iter = M_regs.begin();
          iter != M_regs.end();
          ++iter )
    {
        if ( *iter == NULL )
        {
            out << line_header << "o (null)";
        }
        else
        {
            (*iter)->printPretty( out, line_header + "o " );
        }
    }
    return out;
}

std::auto_ptr< Region >
RegUnion::deepCopy() const
{
    Storage regs;
    for ( Storage::const_iterator i = M_regs.begin();
          i != M_regs.end();
          ++i )
    {
        regs.push_front( (*i)->deepCopy().release() );
    }
    return std::auto_ptr< Region >( new RegUnion( regs ) );
}

void
RegUnion::deleteAll()
{
    for ( Storage::iterator i = M_regs.begin();
          i != M_regs.end();
          ++i )
    {
        delete *i;
    }
    M_regs.clear();
}


/*** RegNamed ***/

std::ostream &
RegNamed::print( std::ostream & out ) const
{
    return out << "\"" << M_name << "\"";
}

std::ostream &
RegNamed::printPretty( std::ostream & out,
                       const std::string & line_header ) const
{
    return out << line_header << "region named \""
               << M_name << "\""
               << std::endl;
}


/*** RegPoint ***/

// RegPoint::RegPoint()
//     : Region(),
//       M_point( NULL )
// {

// }

RegPoint::RegPoint( std::auto_ptr< Point > point )
    : Region(),
      M_point( point )
{

}

RegPoint::RegPoint( const RegPoint & point )
    : Region(),
      M_point( point.M_point->deepCopy() )
{

}

RegPoint::~RegPoint()
{

}

// RegPoint &
// RegPoint::operator=( const RegPoint & point )
// {
//     std::auto_ptr< Point > pt_copy( point.M_point->deepCopy() );
//     M_point = pt_copy;
//     return *this;
// }

std::ostream &
RegPoint::print( std::ostream & out ) const
{
    if ( M_point.get() == NULL )
    {
        return out << "(null)";
    }
    else
    {
        return M_point->print( out );
    }
}

std::ostream &
RegPoint::printPretty( std::ostream & out,
                       const std::string & line_header ) const
{
    out << line_header << "region point\n";
    if ( M_point.get() == NULL )
    {
        return out << line_header << " (null)\n";
    }
    else
    {
        return M_point->printPretty( out, line_header );
    }
}


RegTri::RegTri( std::auto_ptr< Point > pt0,
                std::auto_ptr< Point > pt1,
                std::auto_ptr< Point > pt2 )
    : Region()
{
	m_points[ 0 ] = pt0;
	m_points[ 1 ] = pt1;
	m_points[ 2 ] = pt2;
}

std::ostream &
RegTri::print( std::ostream & out ) const
{
    out << "(tri";
    for ( unsigned int i = 0; i < 3; ++i )
    {
        if ( m_points[ i ].get() == NULL )
        {
            out << " (null)";
        }
        else
        {
            out << " " << *m_points[ i ];
        }
    }

    return out << ")";
}

std::ostream &
RegTri::printPretty( std::ostream & out,
                     const std::string & line_header ) const
{
    out << line_header
        << "Triangle: ";
    for ( unsigned int i = 0; i < 3; ++i )
    {
        if ( m_points[ i ].get() == NULL )
        {
            out << "(null) ";
        }
        else
        {
            m_points[ i ]->printPretty( out, line_header + " " );
            out << " ";
        }
    }
    return out << std::endl;
}

std::auto_ptr< Region >
RegTri::deepCopy() const
{
    std::auto_ptr< Point > pts[ 3 ];
    for ( int i = 0; i < 3; ++i )
    {
        if ( m_points[ i ].get() )
        {
            pts[ i ] = m_points[ i ]->deepCopy();
        }
    }
    return std::auto_ptr< Region >( new RegTri( pts[ 0 ],
                                                pts[ 1 ],
                                                pts[ 2 ] ) );
}


RegRec::RegRec( std::auto_ptr< Point > pt0,
                std::auto_ptr< Point > pt1 )
    : Region()
{
	M_points[ 0 ] = pt0;
	M_points[ 1 ] = pt1;
}

std::ostream &
RegRec::print( std::ostream & out ) const
{
    out << "(rec";
    for ( unsigned int i = 0; i < 2; ++i )
    {
        if ( M_points[ i ].get() == NULL )
        {
            out << " (null)";
        }
        else
        {
            out << " " << *M_points[ i ];
        }
    }
    return out << ")";
}

std::ostream &
RegRec::printPretty( std::ostream & out,
                     const std::string & line_header ) const
{
    out << line_header
        << "Rectangle: ";
    for ( unsigned int i = 0; i < 2; ++i )
    {
	    if ( M_points[ i ].get() == NULL )
        {
            out << "(null) ";
        }
        else
        {
            M_points[ i ]->printPretty( out, line_header + " " );
            out << " ";
        }
    }
    return out << std::endl;
}

std::auto_ptr< Region >
RegRec::deepCopy() const
{
    std::auto_ptr< Point > pts[ 2 ];
    for ( int i = 0; i < 2; ++i )
    {
        if ( M_points[ i ].get() )
        {
            pts[ i ] = M_points[ i ]->deepCopy();
        }
    }
    return std::auto_ptr< Region >( new RegRec( pts[ 0 ], pts[ 1 ] ) );
}

}
}

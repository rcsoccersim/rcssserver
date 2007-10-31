/* -*- Mode: C++ -*- */
/*
 *Copyright:

    Copyright (C) 2000, 2001 RoboCup Soccer Server Maintenance Group.
    	Patrick Riley, Tom Howard, Itsuki Noda,
	Mikhail Prokopenko, Jan Wendler 

    This file is a part of SoccerServer.

    This code is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

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

#include "region.h"

/*** RegQuad ***/

namespace rcss
{
  namespace clang
  {
    RegQuad::RegQuad() 
	: Region()
    {}
 
    RegQuad::RegQuad( std::auto_ptr< Point > pt0,
                      std::auto_ptr< Point > pt1, 
                      std::auto_ptr< Point > pt2,
                      std::auto_ptr< Point > pt3 ) 
        : Region()
    {
	m_points[ 0 ] = pt0;
	m_points[ 1 ] = pt1;
	m_points[ 2 ] = pt2;
	m_points[ 3 ] = pt3;
    }

    RegQuad::~RegQuad()
    {}

    void 
    RegQuad::setAllPts( std::auto_ptr< Point > pt0,
                        std::auto_ptr< Point > pt1,
                        std::auto_ptr< Point > pt2,
                        std::auto_ptr< Point > pt3 )
    {
	m_points[ 0 ] = pt0;
	m_points[ 1 ] = pt1;
	m_points[ 2 ] = pt2;
	m_points[ 3 ] = pt3;
    }

    std::ostream& 
    RegQuad::print( std::ostream& out ) const
    {
      out << "(quad";
      for( unsigned int i = 0; i < 3; ++i )
        {
	    if( m_points[ i ].get() == NULL )
		out << " (null)";
	    else
		out << " " << *m_points[ i ];
        }
      return out << ")";
    }

    std::ostream&
    RegQuad::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      out << line_header
          << "Quadrangle: ";
      for( unsigned int i = 0; i < 4; ++i )
        {
          if( m_points[ i ].get() == NULL )
	      out << "(null) ";
          else
	  {
              m_points[ i ]->printPretty( out, line_header + " " );
              out << " ";
	  }
        }
      return out << std::endl;
    }

    /*** RegArc ***/

    RegArc::RegArc()
      : M_start_rad( 0.0 ),
        M_end_rad( 0.0 ),
        M_start_ang( 0.0 ),
        M_span_ang( 0.0 )
    {}

    RegArc::RegArc( std::auto_ptr< Point > center, 
                    const double& start_rad, const double& end_rad, 
                    const double& start_ang, const double& span_ang )
      : M_start_rad( start_rad ),
        M_end_rad( end_rad ),
        M_start_ang( start_ang ),
        M_span_ang( span_ang ),
	m_center( center )        
    {}

    RegArc::~RegArc()
    {}

    std::ostream&
    RegArc::print( std::ostream& out ) const
    {
      out << "(arc";
      if( m_center.get() == NULL )
        out << " (null)";
      else
        out << " " << *m_center;
      return out << " " << M_start_rad 
                 << " " << M_end_rad 
                 << " " << M_start_ang 
                 << " " << M_span_ang
                 << ")";
    }

    std::ostream&
    RegArc::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      out << line_header
          << "Arc: "
          << "center=";
      if( m_center.get() == NULL )
        out << "(null)";
      else
	  m_center->printPretty( out, line_header + " " );
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

    bool 
    RegArc::setRad( const double& start_rad, const double& end_rad)
    {
      if (start_rad > end_rad)
        return false;
      M_start_rad = start_rad;
      M_end_rad = end_rad;
      return true;
    }

    bool
    RegArc::setAng( const double& start_ang, const double& span_ang )
    {
      if ( start_ang > 180 || start_ang < -180 )
        return false;
      M_start_ang = start_ang;
      M_span_ang = span_ang;
      return true;
    }

/*** RegUnion ***/

    std::ostream& 
    RegUnion::print( std::ostream& out ) const
    {
      out << "(reg";
      for( Storage::const_iterator iter = m_regs.begin();
           iter != m_regs.end(); ++iter )
        {
	    if( *iter == NULL )
		out << " (null)";
	    else
		out << " " << **iter;
        }
      return out << ")";
    }

    std::ostream& 
    RegUnion::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      out << line_header << "Region Union:" << std::endl;
      for( Storage::const_iterator iter = m_regs.begin(); 
           iter != m_regs.end(); ++iter)
        {
          if( *iter == NULL )
            out << line_header << "o (null)";
          else
            (*iter)->printPretty( out, line_header + "o " );
        }
      return out;
    }

    RegPoint::RegPoint() 
      : Region(),
        M_point( NULL )
    {}
      
    RegPoint::RegPoint( std::auto_ptr< Point > point ) 
      : Region(),
        M_point( point )
    {}
      
    RegPoint::RegPoint( const RegPoint& point ) 
      : Region(),
        M_point( point.M_point->deepCopy() )
    {}
      
    RegPoint::~RegPoint()
    {}

    RegPoint&
    RegPoint::operator=( const RegPoint& point )
    {
      std::auto_ptr< Point > pt_copy( point.M_point->deepCopy() );
      M_point = pt_copy;
      return *this;
    }
    
    std::ostream&
    RegPoint::print( std::ostream& out ) const
    { 
      if( M_point.get() == NULL )
        return out << "(null)";
      else
        return M_point->print( out );
    }
  
    std::ostream&
    RegPoint::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      out << line_header << "region point\n";
      if( M_point.get() == NULL )
        return out << line_header << " (null)\n";
      else
        return M_point->printPretty( out, line_header ); 
    }
    
    Point*
    RegPoint::getPoint()
    { return M_point.get(); }

    const Point*
    RegPoint::getPoint() const
    { return M_point.get(); }

    std::auto_ptr< Point >
    RegPoint::detatchPoint()
    { return M_point; }

    void
    RegPoint::setPoint( std::auto_ptr< Point > point )
    { M_point = point; }




    RegTri::RegTri() 
      : Region() 
    {}
 
    RegTri::RegTri( std::auto_ptr< Point > pt0,
                    std::auto_ptr< Point > pt1, 
                    std::auto_ptr< Point > pt2 ) 
      : Region()
    {
	m_points[ 0 ] = pt0;
	m_points[ 1 ] = pt1;
	m_points[ 2 ] = pt2;
    }

    RegTri::~RegTri()
    {}

    void 
    RegTri::setAllPts( std::auto_ptr< Point > pt0,
                       std::auto_ptr< Point > pt1,
                       std::auto_ptr< Point > pt2 )
    {
	m_points[ 0 ] = pt0;
	m_points[ 1 ] = pt1;
	m_points[ 2 ] = pt2;
    }

    std::ostream& 
    RegTri::print( std::ostream& out ) const
    {
      out << "(tri";
      for( unsigned int i = 0; i < 3; ++i )
        {
          if( m_points[ i ].get() == NULL )
	      out << " (null)";
          else
	      out << " " << *m_points[ i ];
        }
      return out << ")";
    }

    std::ostream&
    RegTri::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      out << line_header
          << "Triangle: ";
      for( unsigned int i = 0; i < 3; ++i )
        {
          if( m_points[ i ].get() == NULL )
            out << "(null) ";
          else
            {
		m_points[ i ]->printPretty( out, line_header + " " );
              out << " ";
            }
        }
      return out << std::endl;
    }

    RegRec::RegRec() 
      : Region()
    {}
 
    RegRec::RegRec( std::auto_ptr< Point > pt0,
                    std::auto_ptr< Point > pt1 ) 
        : Region()
    {
	m_points[ 0 ] = pt0;
	m_points[ 1 ] = pt1;
    }

    RegRec::~RegRec()
    {}
    
    void 
    RegRec::setAllPts( std::auto_ptr< Point > pt0,
                       std::auto_ptr< Point > pt1 )
    {
	m_points[ 0 ] = pt0;
	m_points[ 1 ] = pt1;
    }

    std::ostream& 
    RegRec::print( std::ostream& out ) const
    {
      out << "(rec";
      for( unsigned int i = 0; i < 2; ++i )
        {
          if( m_points[ i ].get() == NULL )
            out << " (null)";
          else
            out << " " << *m_points[ i ];
        }
      return out << ")";
    }

    std::ostream&
    RegRec::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      out << line_header
          << "Rectangle: ";
      for( unsigned int i = 0; i < 2; ++i )
        {
	    if( m_points[ i ].get() == NULL )
            out << "(null) ";
          else
            {
              m_points[ i ]->printPretty( out, line_header + " " );
              out << " ";
            }
        }
      return out << std::endl;
    }
  }
}

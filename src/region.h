/* -*- Mode: C++ -*- */
/*
 *Copyright:

    Copyright (C) 2001 RoboCup Soccer Server Maintainance Group.
    	Patrick Riley, Tom Howard, Itsuki Noda,	Mikhail Prokopenko, Jan Wendler 

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

#ifndef _H_REGION
#define _H_REGION

#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include "vector.h"
#include "visitor.h"
#include "clangutil.h"
#include "arithop.h"
#include "hasa.h"
#include <list>

namespace rcss
{
  namespace clang
  {

    class PointSimple;
    class PointRel;
    class PointBall;
    class PointPlayer;
    class PointArith;

    class Point
    {
    public:
 //      typedef util::Visitor10< PointSimple*,
//         PointRel*,
//         PointBall*,
//         PointPlayer*,
//         PointArith* > Visitor;

//       typedef util::Visitor10< const PointSimple*,
//         const PointRel*,
//         const PointBall*,
//         const PointPlayer*,
//         const PointArith* > ConstVisitor;

//       class TypeExtractor
//         : public util::TypeExtractor10< PointSimple*,
//         PointRel*,
//         PointBall*,
//         PointPlayer*,
//         PointArith* >
//       {
//       protected:
//         void
//         visit( PointSimple* pt )
//         { setValue( pt ); }

//         void
//         visit( PointRel* pt )
//         { setValue( pt ); }

//         void
//         visit( PointBall* pt )
//         { setValue( pt ); }

//         void
//         visit( PointPlayer* pt )
//         { setValue( pt ); }

//         void
//         visit( PointArith* pt )
//         { setValue( pt ); }
//       }; 

//       class ConstTypeExtractor
//         : public util::TypeExtractor10< const PointSimple*,
//         const PointRel*,
//         const PointBall*,
//         const PointPlayer*,
//         const PointArith* >
//       {
//       protected:
//         void
//         visit( const PointSimple* pt )
//         { setValue( pt ); }

//         void
//         visit( const PointRel* pt )
//         { setValue( pt ); }

//         void
//         visit( const PointBall* pt )
//         { setValue( pt ); }

//         void
//         visit( const PointPlayer* pt )
//         { setValue( pt ); }

//         void
//         visit( const PointArith* pt )
//         { setValue( pt ); }
//       }; 

      Point()
      {}

      virtual 
      ~Point() 
      {}
      
      virtual
      std::ostream& 
      print( std::ostream& out ) const = 0;

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const = 0;
  
//       virtual
//       void
//       accept( Visitor& v ) = 0;

//       virtual
//       void
//       accept( ConstVisitor& v ) const = 0;

      virtual
      std::auto_ptr< Point >
      deepCopy() const = 0;
    };

      inline
      std::ostream&
      operator <<(std::ostream & os, const rcss::clang::Point& r )
      { return r.print(os); } 
      

    class PointSimple
      : public Point
    {
    private:
      geom::Vector2D M_vec;
    public:
      PointSimple() 
        : Point(),
          M_vec()
      {}

      PointSimple( const double& x,
                   const double& y ) 
        : Point(),
          M_vec( x, y )
      {}

      PointSimple( const geom::Vector2D& vec ) 
        : Point(),
          M_vec( vec )
      {}

      ~PointSimple() {}

      std::ostream&
      print( std::ostream& out ) const
      { return out << "(pt " << M_vec.getX() << " " << M_vec.getY() << ")"; }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        return out << line_header << "Point(" 
                   << M_vec.getX() << ", " << M_vec.getY() << ")" << std::endl; 
      }
      
      geom::Vector2D&
      getVec()
      { return M_vec; }

      const geom::Vector2D&
      getVec() const
      { return M_vec; }

      geom::Vector2D&
      setVec( const geom::Vector2D& vec )
      { M_vec = vec; return M_vec; }

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }
  
      std::auto_ptr< Point >
      deepCopy() const
      { return std::auto_ptr< Point >( new PointSimple( *this ) ); }
   };

    class PointRel
      : public Point
    {
    public:

      PointRel()
	    {}
      
      PointRel( const double& x,
                const double& y,
                std::auto_ptr< Point > origin ) 
        : Point(),
          m_origin( origin ),
          m_offset( x, y )
      {}
      
      PointRel( const PointSimple offset,
                std::auto_ptr< Point > origin ) 
        : Point(),
          m_origin( origin ),
          m_offset( offset )
      {}

      virtual
      ~PointRel()
      {}
      
      std::ostream& 
      print( std::ostream& out ) const
      {
        out << "(pt " << m_offset.getVec().getX() << " " 
            << m_offset.getVec().getY() << " ";
        if( getOrigin() == NULL )
	    out << "(null)";
        else
	    out << *getOrigin();
        return out << ")";
      }
      
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      {
        out << line_header << "Point-Relative(" << m_offset.getVec().getX() 
            << ", " << m_offset.getVec().getY() << " ";
        if( getOrigin() == NULL )
          out << "(null)";
        else
	    getOrigin()->printPretty( out, line_header + " " );
        return out << ")" << std::endl;
      } 
     
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }
      
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }
      
      virtual
      std::auto_ptr< Point >
      deepCopy() const
      {
	  if( getOrigin() )
	      return std::auto_ptr< Point >( new PointRel( m_offset, m_origin->deepCopy() ) ); 
	  else
	      return std::auto_ptr< Point >( new PointRel( m_offset, std::auto_ptr< Point >() ) ); 
      }

      PointSimple
      getOffset() const
      { return m_offset; }

      void
      set( const PointSimple& offset )
      { m_offset = offset; }

      const Point*
      getOrigin() const
      { return m_origin.get(); }

    private:
	std::auto_ptr< Point > m_origin;
      PointSimple m_offset;
    };


    class PointBall
      : public Point
    {
    public:
      PointBall() 
        : Point()
      {}
      
      ~PointBall()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { return out << "(pt ball)"; }
      
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return out << line_header << "Point-Ball" << std::endl; }
 
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Point >
      deepCopy() const
      { return std::auto_ptr< Point >( new PointBall( *this ) ); }

    };

    class PointPlayer
      : public Point
    {
    public:
      PointPlayer() 
        : Point(),
          M_our_side( false ),
          M_unum()
      {}

      PointPlayer( const bool& our_side, const UNum& unum ) 
        : Point(),
          M_our_side( our_side ),
          M_unum( unum )
      {}

      ~PointPlayer()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { 
        return out << "(pt " << (M_our_side ? "our" : "opp") 
                   << " " << M_unum << ")";
      }
      
      std::ostream& 
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        return out << line_header << "Point-Player(" 
                   << (M_our_side ? "our team" : "opponent")
                   << " " << M_unum << ")" << std::endl;
      }

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Point >
      deepCopy() const
      { return std::auto_ptr< Point >( new PointPlayer( *this ) ); }
 
      /* class specific */
      bool 
      isOurSide() const
      { return M_our_side; }
      
      bool
      isTheirSide() const
      { return !M_our_side; }
      
      UNum
      getUNum() const
      { return M_unum; }
      
      void 
      setOurSide( const bool& our_side )
      { M_our_side = our_side; }
      
      void
      setUNum( const UNum& num )
      { M_unum = num; }
      
    private:
      bool M_our_side;
      UNum M_unum;
    };



    class PointArith
      : public Point
    {
    public:
      PointArith() 
        : Point(),
          M_arith_op( NULL ),
          M_idx( 0 )
      {}
      
      PointArith( std::auto_ptr< Point > pt1, std::auto_ptr< Point > pt2,
                  const util::ArithOp& arith_op ) 
        : Point(),
          M_arith_op( &arith_op ),
          M_idx( 0 )
      {
        M_points[ 0 ] = pt1;
        M_points[ 1 ] = pt2;
      }
      
      PointArith( const PointArith& pt ) 
        : Point(),
          M_arith_op( pt.M_arith_op ),
          M_idx( 0 )
      {
        std::auto_ptr< Point > pt_copy1 = pt.M_points[ 0 ]->deepCopy();
        M_points[ 0 ] = pt_copy1;
        std::auto_ptr< Point > pt_copy2 = pt.M_points[ 1 ]->deepCopy();
        M_points[ 1 ] = pt_copy2;
      }
      
      ~PointArith()
      {}

      PointArith&
      operator=( const PointArith& pt )
      {
        std::auto_ptr< Point > pt_copy1 = pt.M_points[ 0 ]->deepCopy();
        M_points[ 0 ] = pt_copy1;
        std::auto_ptr< Point > pt_copy2 = pt.M_points[ 1 ]->deepCopy();
        M_points[ 1 ] = pt_copy2;        
        M_idx = pt.M_idx;
        return *this;
      }

      std::ostream&
      print( std::ostream& out ) const
      {
        out << "(";

        if( M_points[ 0 ].get() == NULL )
          out << "(null)";
        else
          out << *(M_points[ 0 ]);

        out << " ";

        if( M_arith_op == NULL )
          out << "(null)";
        else
        {
          out << *M_arith_op;
        }

        out << " ";

        if( M_points[ 1 ].get() == NULL )
          out << "(null)";
        else
          out << *(M_points[ 1 ]);

        return out << ")";
      }
      
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        out << line_header << "Point-Arith" << std::endl;

        if( M_points[ 0 ].get() == NULL )
          out << line_header << " (null)\n";
        else
          M_points[ 0 ]->printPretty( out, line_header + " " );

        if( M_points[ 1 ].get() == NULL )
          out << line_header << " (null)\n";
        else
          M_points[ 1 ]->printPretty( out, line_header + " " );

        if( M_arith_op == NULL )
          out << line_header << " (null)\n";
        else
          out << line_header << " operation: " << *M_arith_op << std::endl;
        
        return out;
      }
 
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Point >
      deepCopy() const
      { return std::auto_ptr< Point >( new PointArith( *this ) ); }

      bool 
      setPoint( const int& i, std::auto_ptr< Point >& pt )
      {
        if (i < 0 || i > 1)
          return false;
        M_points[i] = pt;
        return true;
      }

      void 
      setAllPoints( std::auto_ptr< Point >& pt0,
                 std::auto_ptr< Point >& pt1 )
      {
        M_points[0] = pt0;
        M_points[1] = pt1;
      }

      bool      
      setPoint( std::auto_ptr< Point >& pt )
      {
        M_points[ M_idx ] = pt;
        M_idx = (M_idx + 1) % 2;
        return true;
      }

      Point* 
      getPoint( const int& i )
      { return (i>=0 && i<=1) ? M_points[i].get() : NULL; }

      const util::ArithOp*
      getOp() const
      { return M_arith_op; }

      const util::ArithOp&
      setOp( const util::ArithOp& arith_op )
      { return *(M_arith_op = &arith_op); }
      
    private:
      std::auto_ptr< Point > M_points[2];
      const util::ArithOp* M_arith_op;
      unsigned int M_idx;
    };



    class RegNull;
    class RegQuad;
    class RegArc;
    class RegUnion;
    class RegNamed;
    class RegPoint;
    class RegTri;
    class RegRec;

    class Region 
    {
    public:
//       typedef util::Visitor10< RegNull*,
//         RegQuad*,
//         RegArc*,
//         RegUnion*,
//         RegNamed*,
//         RegPoint*,
//         RegTri*,
//         RegRec* > Visitor;

//       typedef util::Visitor10< const RegNull*,
//         const RegQuad*,
//         const RegArc*,
//         const RegUnion*,
//         const RegNamed*,
//         const RegPoint*,
//         const RegTri*,
//         const RegRec*  > ConstVisitor;

//       class TypeExtractor
//         : public util::TypeExtractor10< RegNull*,
//         RegQuad*,
//         RegArc*,
//         RegUnion*,
//         RegNamed*,
//         RegPoint*,
//         RegTri*,
//         RegRec* >
//       {
//       protected:
//         void
//         visit( RegNull* reg )
//         { setValue( reg ); }

//         void
//         visit( RegQuad* reg )
//         { setValue( reg ); }

//         void
//         visit( RegArc* reg )
//         { setValue( reg ); }

//         void
//         visit( RegUnion* reg )
//         { setValue( reg ); }

//         void
//         visit( RegNamed* reg )
//         { setValue( reg ); }

//         void
//         visit( RegPoint* reg )
//         { setValue( reg ); }

//         void
//         visit( RegTri* reg )
//         { setValue( reg ); }

//         void
//         visit( RegRec* reg )
//         { setValue( reg ); }

//       };

//       class ConstTypeExtractor
//         : public util::TypeExtractor10< const RegNull*,
//         const RegQuad*,
//         const RegArc*,
//         const RegUnion*,
//         const RegNamed*,
//         const RegPoint*,
//         const RegTri*,
//         const RegRec* >
//       {
//       protected:
//         void
//         visit( const RegNull* reg )
//         { setValue( reg ); }

//         void
//         visit( const RegQuad* reg )
//         { setValue( reg ); }

//         void
//         visit( const RegArc* reg )
//         { setValue( reg ); }

//         void
//         visit( const RegUnion* reg )
//         { setValue( reg ); }

//         void
//         visit( const RegNamed* reg )
//         { setValue( reg ); }

//         void
//         visit( const RegPoint* reg )
//         { setValue( reg ); }

//         void
//         visit( const RegTri* reg )
//         { setValue( reg ); }

//         void
//         visit( const RegRec* reg )
//         { setValue( reg ); }
//       };

      Region()
      {}

      virtual 
      ~Region() 
      {}
      
      virtual
      std::ostream& 
      print( std::ostream& out ) const = 0;

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const = 0;
  
//       virtual
//       void
//       accept( Visitor& v ) = 0;

//       virtual
//       void
//       accept( ConstVisitor& v ) const = 0;

      virtual
      std::auto_ptr< Region >
      deepCopy() const = 0;
    };

      inline
      std::ostream&
      operator <<(std::ostream & os, const rcss::clang::Region& r )
      { return r.print(os); } 


    class RegNull
      : public Region
    {
    public:
      RegNull() 
        : Region()
      {}
      
      ~RegNull()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { return out << "(null)"; }
      
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return out << line_header << "null region" << std::endl; }

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Region >
      deepCopy() const
      { return std::auto_ptr< Region >( new RegNull( *this ) ); }
    };

    class RegQuad
      : public Region
    {
    public:
	RegQuad();
      
	RegQuad( std::auto_ptr< Point > pt0,
		 std::auto_ptr< Point > pt1, 
		 std::auto_ptr< Point > pt2,
		 std::auto_ptr< Point > pt3 );

	virtual
	~RegQuad();

	std::ostream&
	print( std::ostream& out ) const;
	
	std::ostream& 
	printPretty( std::ostream& out, const std::string& line_header ) const;

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Region >
      deepCopy() const
      { 
	  std::auto_ptr< Point > pts[ 4 ];
	  for( int i = 0; i < 4; ++i )
	  {
	      if( m_points[ i ].get() )
		  pts[ i ] = m_points[ i ]->deepCopy();
	  }
	  return std::auto_ptr< Region >( new RegQuad( pts[ 0 ],
						       pts[ 1 ],
						       pts[ 2 ],
						       pts[ 3 ] ) ); 
      }

  /* class specific */
      const Point* 
      getPt( unsigned int i ) const
      {
	  if( i < 4 )
	      return m_points[ i ].get();
	  else
	      return NULL;
      }
      
      void
      setAllPts( std::auto_ptr< Point > pt0,
                 std::auto_ptr< Point > pt1,
                 std::auto_ptr< Point > pt2,
                 std::auto_ptr< Point > pt3 );

    private:
	std::auto_ptr< Point > m_points[ 4 ];
    };

    class RegArc
      : public Region
    {
    public:
	RegArc();
      
      RegArc( std::auto_ptr< Point > center, 
              const double& start_rad, 
              const double& end_rad, 
              const double& start_ang,
              const double& span_ang );

      virtual
      ~RegArc();
       
      std::ostream& 
      print( std::ostream& out ) const;
      
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const;

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Region >
      deepCopy() const
      {
	  if( m_center.get() )
	      return std::auto_ptr< Region >( new RegArc( m_center->deepCopy(),
							  M_start_rad, M_end_rad,
							  M_start_ang, M_span_ang ) ); 
	  else
	      return std::auto_ptr< Region >( new RegArc( std::auto_ptr< Point >(),
							  M_start_rad, M_end_rad,
							  M_start_ang, M_span_ang ) ); 
	  
      }

      const Point*
      getCenter() const 
      { return m_center.get(); }
  
      double    
      getStartRad() const
      { return M_start_rad; }

      double
      getEndRad() const
      { return M_end_rad; }
      
      double
      getStartAng() const
      { return M_start_ang; }

      double 
      getSpanAng() const
      { return M_span_ang; }
      
      bool
      setRad( const double& start_rad, const double& end_rad );
      
      bool
      setAng( const double& start_ang, const double& span_ang);
  
    private:
      /* start rad <= end_rad */
      double M_start_rad, M_end_rad;
      double M_start_ang, M_span_ang;
	std::auto_ptr< Point > m_center;
    };


    class RegUnion
      : public Region
    {
    public:
	typedef std::list< Region* > Storage;

      RegUnion() 
        : Region()
      {}
      
      RegUnion( const Storage& regs) 
	  : Region(),
	    m_regs( regs )
      {}

      virtual
      ~RegUnion()
      {
	  deleteAll();
      }

      std::ostream& 
      print( std::ostream& out ) const;
      
      std::ostream& 
      printPretty( std::ostream& out, const std::string& line_header ) const;

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Region >
      deepCopy() const
      { 
	  Storage regs;
	  for( Storage::const_iterator i = m_regs.begin(); i != m_regs.end(); ++i )
	  {
	      regs.push_front( (*i)->deepCopy().release() );
	  }
	  return std::auto_ptr< Region >( new RegUnion( regs ) ); 
      }

      const Storage&
      getRegions() const
      { return m_regs; }
	
      Storage&
      getRegions()
      { return m_regs; }
	
    private:
	Storage m_regs;

	void
	deleteAll()
	    {
		for( Storage::iterator i = m_regs.begin();
		     i != m_regs.end(); ++i )
		    delete *i;
		m_regs.clear();
	    }
    };


    class RegNamed
      : public Region
    {
    public:
      RegNamed( const std::string& name = "" ) 
        : Region(),
          M_name(name)
      {}

      ~RegNamed()
      {}
      
      std::ostream&
      print( std::ostream& out ) const
      { return out << "\"" << M_name << "\""; }
  
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        return out << line_header << "region named \"" 
                   << M_name << "\"" << std::endl;
      }

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Region >
      deepCopy() const
      { return std::auto_ptr< Region >( new RegNamed( *this ) ); }

      /* class specific */
      std::string&
      getName()
      { return M_name; }
  
      const std::string&
      getName() const
      { return M_name; }
  
      void
      setName( const std::string& name )
      { M_name = name; }

    private:
      std::string M_name;
    };

    /**** RegPoint ****/
    class RegPoint
      : public Region
    {
    public:
      RegPoint();
      RegPoint( std::auto_ptr< Point > point );
      RegPoint( const RegPoint& point ) ;
      ~RegPoint();

      RegPoint&
      operator=( const RegPoint& point );

      std::ostream&
      print( std::ostream& out ) const;
  
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const;
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Region >
      deepCopy() const
      { return std::auto_ptr< Region >( new RegPoint( *this ) ); }

      Point*
      getPoint();

      const Point*
      getPoint() const;

      std::auto_ptr< Point >
      detatchPoint();

      void
      setPoint( std::auto_ptr< Point > point );

    private:
      std::auto_ptr< Point > M_point;
    };


    class RegTri
      : public Region
    {
    public:

      RegTri();
      
      RegTri( std::auto_ptr< Point > pt0,
              std::auto_ptr< Point > pt1,
              std::auto_ptr< Point > pt2 );
      
      virtual
      ~RegTri();

      std::ostream&
      print( std::ostream& out ) const;
      
      std::ostream& 
      printPretty( std::ostream& out, const std::string& line_header ) const;

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

	virtual
	std::auto_ptr< Region >
	deepCopy() const
	    {
		std::auto_ptr< Point > pts[ 3 ];
		for( int i = 0; i < 3; ++i )
		{
		    if( m_points[ i ].get() )
			pts[ i ] = m_points[ i ]->deepCopy();
		}
		return std::auto_ptr< Region >( new RegTri( pts[ 0 ],
							    pts[ 1 ],
							    pts[ 2 ] ) ); 
	    }
	
      const Point* 
      getPt( unsigned int i ) const
      { 
	  if( i < 3 )
	      return m_points[ i ].get();
	  else
	      return NULL;
      }
            
      void
      setAllPts( std::auto_ptr< Point > pt0,
                 std::auto_ptr< Point > pt1,
                 std::auto_ptr< Point > pt2 );

    private:
	std::auto_ptr< Point > m_points[ 3 ];
    };



   class RegRec
      : public Region
    {
    public:
      RegRec();
      
      RegRec( std::auto_ptr< Point > pt0,
              std::auto_ptr< Point > pt1 );
      
      virtual
      ~RegRec();

      std::ostream&
      print( std::ostream& out ) const;
      
      std::ostream& 
      printPretty( std::ostream& out, const std::string& line_header ) const;

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }
      
      virtual
      std::auto_ptr< Region >
      deepCopy() const
      { 
	  std::auto_ptr< Point > pts[ 2 ];
	  for( int i = 0; i < 2; ++i )
	  {
	      if( m_points[ i ].get() )
		  pts[ i ] = m_points[ i ]->deepCopy();
	  }
	  return std::auto_ptr< Region >( new RegRec( pts[ 0 ], pts[ 1 ] ) ); 
      }

	const Point* 
	getPt( unsigned int i ) const
	    {
		if( i < 2 )
		    return m_points[ i ].get();
		else
		    return NULL;
	    }
      
      void
      setAllPts( std::auto_ptr< Point > pt0,
                 std::auto_ptr< Point > pt1 );
 
    private:
	std::auto_ptr< Point > m_points[ 2 ];
   };

  }
}

#endif


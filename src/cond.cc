#include "cond.h"

namespace rcss
{
  namespace clang
  {    

    const char* CondPlayMode::MODE_STRINGS[] = {
      "",
      "bko",
      "time_over",
      "play_on",
      "ko_our",
      "ko_opp",
      "ki_our",
      "ki_opp",
      "fk_our",
      "fk_opp",
      "ck_our",
      "ck_opp",
      "gk_our",
      "gk_opp",
      "gc_our",
      "gc_opp",
      "ag_our",
      "ag_opp" };

    std::ostream&
    CondPlayerPos::print( std::ostream& out ) const
    {
      out << "(ppos"
          << " " << (M_our_side ? "our" : "opp")
          << " " << m_players
          << " " << M_min_match
          << " " << M_max_match
          << " ";
      if( m_reg.get() == NULL )
        out << "(null)";
      else
        out << *m_reg;
      out << ")";
      return out;
    }

    std::ostream&
    CondPlayerPos::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      out << line_header
          << "are "
          << M_min_match << "-" << M_max_match << " "
          << (M_our_side ? "our team" : "opponent") << " "
          << m_players << " "
          << "in:" << std::endl;
      if( m_reg.get() == NULL )
        out << line_header << " (null)\n";
      else
        m_reg->printPretty( out, line_header + " " );
      return out;
    }

    std::ostream&
    CondBallOwner::print( std::ostream& out ) const
    {
      return out << "(bowner"
                 << " " << (M_our_side ? "our" : "opp") 
                 << " " << M_players
                 << ")";
    }

    std::ostream&
    CondBallOwner::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      return out << line_header
                 << "is ball controlled by someone in " 
                 << (M_our_side ? "our team" : "opponent") << " "
                 << M_players << std::endl;
    }

    std::ostream&
    CondAnd::print( std::ostream& out ) const
    {
      out << "(and";
      for( Storage::const_iterator iter = getConds().begin();
           iter != getConds().end(); ++iter )
        {
          if( *iter == NULL )
            out << " (null)";
          else
            out << " " << **iter;
        }
      return out << ")";
    }

    std::ostream&
    CondAnd::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      out << line_header << "and" << std::endl;
      for( Storage::const_iterator iter = getConds().begin(); 
           iter != getConds().end(); ++iter )
        {
          if( *iter == NULL )
            out << line_header << " +(null)\n";
          else
            (*iter)->printPretty( out, line_header + " +" );
        }
      return out;
    }

    bool
    CondAnd::eval( const Context& context ) const
    {
      for( Storage::const_iterator iter = getConds().begin(); 
           iter != getConds().end(); ++iter )
        {
          if( *iter == NULL )
            throw util::NullErr( __FILE__, __LINE__,
                                 "Null condition in CondAnd\n" );
          if( !(*iter)->eval( context ) )
            return false;
        }
      return true;
    }


    std::ostream&
    CondOr::print( std::ostream& out ) const
    {
      out << "(or";
      for( Storage::const_iterator iter = getConds().begin(); 
           iter != getConds().end(); ++iter) 
        {
          if( *iter == NULL )
            out << " (null)";
          else
            out << " " << **iter;
        }
      return out << ")";
    }

    std::ostream&
    CondOr::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      out << line_header << "or" << std::endl;
      for( Storage::const_iterator iter = getConds().begin();
           iter != getConds().end(); ++iter )
        {
          if( *iter == NULL )
            out << line_header << " +(null)\n";
          else
            (*iter)->printPretty( out, line_header + " +" );
        }
      return out;
    }

    bool
    CondOr::eval( const Context& context ) const
    {
      for( Storage::const_iterator iter = getConds().begin();
           iter != getConds().end(); ++iter )
        {
          if( *iter == NULL )
            throw util::NullErr( __FILE__, __LINE__, 
                                 "Null condition in CondOr\n" );
          if( (*iter)->eval( context ) )
            return true;
        }
      return false;
    }
  }
}


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>;
#endif

#include "clangparser.h"

namespace rcss {
namespace clang {

Parser::Parser( Builder & builder )
  : M_param( *this, builder ),
    M_parser( &RCSS_CLANG_parse )
{

}

int
Parser::parse( const char * msg )
{
#ifdef HAVE_SSTREAM
    std::istringstream strm( msg );
#else
    std::istrstream strm( msg );
#endif
    return ( rcss::Parser::parse( strm ) ? 0 : 1 );
}

}
}

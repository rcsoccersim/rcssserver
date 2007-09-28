#include "clangparser.h"

rcss::clang::Parser::Parser( Builder& builder )
  : M_param( *this, builder ),
    M_parser( &RCSS_CLANG_parse )
{
    
}

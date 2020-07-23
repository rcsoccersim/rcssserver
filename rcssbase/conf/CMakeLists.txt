add_library(RCSSConfParser SHARED
    parser.cpp 
    builder.cpp 
    statushandler.cpp 
    streamstatushandler.cpp
    paramgetter.hpp
    paramsetter.hpp
)
add_library(RCSS::ConfParser ALIAS RCSSConfParser)

set_target_properties(RCSSConfParser
  PROPERTIES
    VERSION "3.0.0"
)


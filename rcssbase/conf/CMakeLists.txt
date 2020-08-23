add_library(RCSSConfParser SHARED
    parser.cpp 
    builder.cpp 
    statushandler.cpp 
    streamstatushandler.cpp
    paramgetter.hpp
    paramsetter.hpp
)
add_library(RCSS::ConfParser ALIAS RCSSConfParser)

target_compile_definitions(RCSSConfParser
  PUBLIC
    HAVE_CONFIG_H
)

target_include_directories(RCSSConfParser
  PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

set_target_properties(RCSSConfParser
  PROPERTIES
    SOVERSION 3
    VERSION 3.0.0
)

install(TARGETS RCSSConfParser
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

add_library(RCSSConfParser SHARED
    parser.cpp
    builder.cpp
    statushandler.cpp
    streamstatushandler.cpp
    paramgetter.hpp
    paramsetter.hpp
)
add_library(RCSS::ConfParser ALIAS RCSSConfParser)

target_link_libraries(RCSSConfParser
  PUBLIC
    Boost::boost
)

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
    LIBRARY_OUTPUT_NAME "rcssconfparser"
)

set_property(TARGET RCSSConfParser PROPERTY
  PUBLIC_HEADER
    builder.hpp
    parser.hpp
    paramgetter.hpp
    paramsetter.hpp
    statushandler.hpp
    streamstatushandler.hpp
)

install(TARGETS RCSSConfParser
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT Libraries
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rcss/conf
)

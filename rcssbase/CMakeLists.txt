add_subdirectory(net)
add_subdirectory(conf)
add_subdirectory(gzip)

add_library(RCSSBase SHARED
    factory.hpp
    parser.h
)
add_library(RCSS::Base ALIAS RCSSBase)

target_compile_definitions(RCSSBase
  PUBLIC
    HAVE_CONFIG_H
)

target_include_directories(RCSSBase
  PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

set_target_properties(RCSSBase
  PROPERTIES
    LINKER_LANGUAGE CXX
)

target_link_libraries(RCSSBase
  PRIVATE
    Boost::system
    Boost::filesystem
)

install(TARGETS RCSSBase
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

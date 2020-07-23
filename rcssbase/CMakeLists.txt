add_subdirectory(net)
add_subdirectory(conf)
add_subdirectory(gzip)

add_library(RCSSBase SHARED
    factory.hpp
    parser.h
)
add_library(RCSS::Base ALIAS RCSSBase)

set_target_properties(RCSSBase
  PROPERTIES
    LINKER_LANGUAGE CXX
)

target_link_libraries(RCSSBase
  PRIVATE
    Boost::system
    Boost::filesystem
)


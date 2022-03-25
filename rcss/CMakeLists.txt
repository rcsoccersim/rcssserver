add_subdirectory(net)
add_subdirectory(conf)
add_subdirectory(gzip)
add_subdirectory(clang)

add_library(RCSSBase INTERFACE)
add_library(RCSS::Base ALIAS RCSSBase)

target_compile_definitions(RCSSBase
  INTERFACE
    HAVE_CONFIG_H
)

target_include_directories(RCSSBase
  INTERFACE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

#set_property(TARGET RCSSBase PROPERTY
#  PUBLIC_HEADER
#    factory.hpp
#    parser.h
#)

target_link_libraries(RCSSBase
  INTERFACE
    Boost::boost
    Boost::system
    Boost::filesystem
)

#install(TARGETS RCSSBase
#    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rcssbase
#)

install(FILES
  factory.hpp
  parser.h
  vector.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rcss
)

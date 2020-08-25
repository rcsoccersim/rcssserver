add_library(RCSSGZ SHARED
    gzfstream.cpp
    gzstream.cpp
)
add_library(RCSS::GZ ALIAS RCSSGZ)

target_compile_definitions(RCSSGZ
  PUBLIC
    HAVE_CONFIG_H
)

target_include_directories(RCSSGZ
  PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

set_target_properties(RCSSGZ
  PROPERTIES
    SOVERSION 1
    VERSION 1.0.0
    LIBRARY_OUTPUT_NAME "rcssgz"
)

set_property(TARGET RCSSGZ PROPERTY
  PUBLIC_HEADER
    gzstream.hpp
    gzfstream.hpp
)

target_link_libraries(RCSSGZ
  PRIVATE
    ZLIB::ZLIB
)

install(TARGETS RCSSGZ
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rcssbase/gzip
)

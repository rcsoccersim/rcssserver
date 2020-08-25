add_library(RCSSNet SHARED
    addr.cpp
    handler.cpp
    socket.cpp
    socketstreambuf.cpp
    tcpsocket.cpp
    udpsocket.cpp
    isocketstream.hpp
    osocketstream.hpp
    iosocketstream.hpp
)
add_library(RCSS::Net ALIAS RCSSNet)

target_compile_definitions(RCSSNet
  PUBLIC
    HAVE_CONFIG_H
)

target_include_directories(RCSSNet
  PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

set_target_properties(RCSSNet
  PROPERTIES
    SOVERSION 1
    VERSION 1.0.1
    LIBRARY_OUTPUT_NAME "rcssnet"
)

install(TARGETS RCSSNet
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

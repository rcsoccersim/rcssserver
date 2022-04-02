add_library(RCSSNet SHARED
    addr.cpp
    socket.cpp
    socketstreambuf.cpp
    tcpsocket.cpp
    udpsocket.cpp
    isocketstream.hpp
    osocketstream.hpp
    iosocketstream.hpp
)
add_library(RCSS::Net ALIAS RCSSNet)

target_link_libraries(RCSSNet
  PUBLIC
    Boost::boost
)

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

set_property(TARGET RCSSNet PROPERTY
  PUBLIC_HEADER
    addr.hpp
    socket.hpp
    udpsocket.hpp
    tcpsocket.hpp
    socketstreambuf.hpp
    isocketstream.hpp
    osocketstream.hpp
    iosocketstream.hpp
)

install(TARGETS RCSSNet
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT Libraries
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rcss/net
)

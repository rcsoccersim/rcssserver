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

set_target_properties(RCSSNet
  PROPERTIES
    VERSION "1.1.0"
)


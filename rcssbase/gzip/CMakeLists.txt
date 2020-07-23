add_library(RCSSGZ SHARED
    gzfstream.cpp
    gzstream.cpp
)
add_library(RCSS::GZ ALIAS RCSSGZ)

set_target_properties(RCSSGZ
  PROPERTIES
    VERSION "1.0.0"
)

target_link_libraries(RCSSGZ
  PRIVATE
    ZLIB::ZLIB
)


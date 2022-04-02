
bison_target(coach_lang_parser
    coach_lang_parser.ypp
    ${CMAKE_CURRENT_BINARY_DIR}/coach_lang_parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/coach_lang_parser.hpp
)
flex_target(coach_lang_tokenizer
    coach_lang_tok.lpp
    ${CMAKE_CURRENT_BINARY_DIR}/raw_coach_lang_tok.cpp
)
add_flex_bison_dependency(coach_lang_tokenizer coach_lang_parser)
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/coach_lang_tok.cpp"
  COMMAND ${CMAKE_COMMAND}
  ARGS
    "-DGENERATED_FILE_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/raw_coach_lang_tok.cpp\""
    "-DCORRECT_HEADER_NAME=coach_lang_tok.h"
    "-DOUTPUT_FILE_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/coach_lang_tok.cpp\""
    "-P" "${CMAKE_CURRENT_SOURCE_DIR}/fix_lexer_file.cmake"
  MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}/raw_coach_lang_tok.cpp"
)

add_library(RCSSCLangParser SHARED
  clangbuilder.cpp
  clangmsgbuilder.cpp
  clangparser.cpp
  clangmsg.cpp
  clangmetamsg.cpp
  clangfreeformmsg.cpp
  clangunsuppmsg.cpp
  clangrulemsg.cpp
  clangdelmsg.cpp
  clanginfomsg.cpp
  clangadvicemsg.cpp
  clangdefmsg.cpp
  clangaction.cpp
  clangutil.cpp
  coach_lang_comp.cpp
  arithop.cpp
  cond.cpp
  compop.cpp
  region.cpp
  rule.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/coach_lang_parser.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/coach_lang_tok.cpp
  )

add_library(RCSS::CLangParser ALIAS RCSSCLangParser)


target_link_libraries(RCSSCLangParser
  PUBLIC
    Boost::boost
  )

target_compile_definitions(RCSSCLangParser
  PUBLIC
     HAVE_CONFIG_H
  )

target_include_directories(RCSSCLangParser
  PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
	)

set_target_properties(RCSSCLangParser
  PROPERTIES
    SOVERSION 2
    VERSION 2.1.0
    LIBRARY_OUTPUT_NAME "rcssclangparser"
	)


set_property(TARGET RCSSCLangParser PROPERTY
  PUBLIC_HEADER
    clangparser.h
    coach_lang_tok.h
    clangbuilder.h
    clangmsgbuilder.h
    clangmsg.h
    clangmetamsg.h
    clangfreeformmsg.h
    clangunsuppmsg.h
    clangrulemsg.h
    clangdelmsg.h
    clanginfomsg.h
    clangadvicemsg.h
    clangdefmsg.h
    clangaction.h
    clangutil.h
    coach_lang_comp.h
    ${CMAKE_CURRENT_BINARY_DIR}/coach_lang_parser.hpp
    arithop.h
    compop.h
    cond.h
    region.h
    rule.h
	)

install(TARGETS RCSSCLangParser
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT Libraries
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rcss/clang
)

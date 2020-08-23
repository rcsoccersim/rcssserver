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
    rcssexceptions.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/coach_lang_parser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/coach_lang_tok.cpp
)
add_library(RCSS::CLangParser ALIAS RCSSCLangParser)

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
    SOVERSION 3
    VERSION 3.0.1
)


bison_target(player_command_parser
    player_command_parser.ypp
    ${CMAKE_CURRENT_BINARY_DIR}/player_command_parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/player_command_parser.hpp
)
flex_target(player_command_tokenizer
    player_command_tok.lpp
    ${CMAKE_CURRENT_BINARY_DIR}/raw_player_command_tok.cpp
)
add_flex_bison_dependency(player_command_tokenizer player_command_parser)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/player_command_tok.cpp"
  COMMAND ${CMAKE_COMMAND}
  ARGS
    "-DGENERATED_FILE_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/raw_player_command_tok.cpp\""
    "-DCORRECT_HEADER_NAME=player_command_tok.h"
    "-DOUTPUT_FILE_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/player_command_tok.cpp\""
    "-P" "${CMAKE_CURRENT_SOURCE_DIR}/fix_lexer_file.cmake"
  MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}/raw_player_command_tok.cpp"
)

add_executable(RCSSServer
    audio.cpp
    bodysender.cpp
    coach.cpp
    csvsaver.cpp
    dispsender.cpp
    field.cpp
    fullstatesender.cpp
    heteroplayer.cpp
    initsender.cpp
    initsendercoach.cpp
    initsenderlogger.cpp
    initsendermonitor.cpp
    initsenderonlinecoach.cpp
    initsenderplayer.cpp
    landmarkreader.cpp
    logger.cpp
    main.cpp
    monitor.cpp
    pcombuilder.cpp
    pcomparser.cpp
    player.cpp
    playerparam.cpp
    object.cpp
    referee.cpp
    remoteclient.cpp
    resultsaver.cpp
    serializer.cpp
    serializercoachstdv1.cpp
    serializercoachstdv7.cpp
    serializercoachstdv8.cpp
    serializercoachstdv13.cpp
    serializercoachstdv14.cpp
    serializercommonstdv1.cpp
    serializercommonstdv7.cpp
    serializercommonstdv8.cpp
    serializermonitor.cpp
    serializeronlinecoachstdv1.cpp
    serializeronlinecoachstdv6.cpp
    serializeronlinecoachstdv7.cpp
    serializeronlinecoachstdv8.cpp
    serializeronlinecoachstdv13.cpp
    serializeronlinecoachstdv14.cpp
    serializerplayerstdv1.cpp
    serializerplayerstdv7.cpp
    serializerplayerstdv8.cpp
    serializerplayerstdv13.cpp
    serializerplayerstdv14.cpp
    serverparam.cpp
    stadium.cpp
    stdoutsaver.cpp
    stdtimer.cpp
    synctimer.cpp
    team.cpp
    utility.cpp
    visualsendercoach.cpp
    visualsenderplayer.cpp
    weather.cpp
    xmlreader.cpp
    xpmholder.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/player_command_parser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/player_command_tok.cpp
)

target_link_libraries(RCSSServer
  PRIVATE
    RCSS::CLangParser
    RCSS::ConfParser
    RCSS::Net
    RCSS::GZ
    Boost::filesystem
    Boost::system
    ZLIB::ZLIB
)

set_target_properties(RCSSServer
  PROPERTIES
    RUNTIME_OUTPUT_NAME "rcssserver"
)


add_executable(RCSSClient
    client.cpp
)

target_link_libraries(RCSSClient
  PRIVATE
    RCSS::Net
    RCSS::GZ
    Boost::filesystem
    Boost::system
    ZLIB::ZLIB
)

set_target_properties(RCSSClient
  PROPERTIES
    RUNTIME_OUTPUT_NAME "rcssclient"
)

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(libdir ${CMAKE_INSTALL_FULL_LIBDIR})
configure_file(rcsoccersim.in rcsoccersim @ONLY)

install(TARGETS RCSSServer RCSSClient RCSSCLangParser
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rcsoccersim DESTINATION ${CMAKE_INSTALL_BINDIR})

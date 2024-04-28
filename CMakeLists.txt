cmake_minimum_required(VERSION 3.5.1)

project(RCSSServer VERSION 18.1.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
  set(HAVE_LIBZ TRUE)
endif()
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
#find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)

include(GNUInstallDirs)
include(CheckIncludeFileCXX)

check_include_file_cxx("sys/socket.h" HAVE_SYS_SOCKET_H)
check_include_file_cxx("sys/param.h" HAVE_SYS_PARAM_H)
check_include_file_cxx("sys/time.h" HAVE_SYS_TIME_H)
check_include_file_cxx("netinet/in.h" HAVE_NETINET_IN_H)
check_include_file_cxx("arpa/inet.h" HAVE_ARPA_INET_H)
check_include_file_cxx("netdb.h" HAVE_NETDB_H)
check_include_file_cxx("unistd.h" HAVE_UNISTD_H)
check_include_file_cxx("poll.h" HAVE_POLL_H)
check_include_file_cxx("pwd.h" HAVE_PWD_H)



configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake config.h)

add_subdirectory(rcss)
add_subdirectory(src)

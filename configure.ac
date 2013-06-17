#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
LT_PREREQ([2.2])
AC_INIT([RCSSServer],[15.2.2],[sserver-admin@users.sf.net],[rcssserver])

#AM_INIT_AUTOMAKE([gnu 1.7.2 check-news dist-bzip2 dist-zip])
AM_INIT_AUTOMAKE([gnu 1.7.2 check-news])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

##################################################
# Checks for programs.
##################################################

AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

#ACX_PTHREAD([LIBS="$PTHREAD_LIBS $LIBS"
#             CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
#             CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
#             CC="$PTHREAD_CC"])

##################################################
# Checks for libraries.
##################################################
AC_CHECK_LIB([m], [cos])
dnl AC_CHECK_LIB(expat, XML_Parse)
#AC_CHECK_LIB([z], [deflate])
AC_SUBST(GZ_LIBS)
AC_CHECK_LIB([z], [deflate],
             [AC_DEFINE([HAVE_LIBZ], [1],
                        [Define to 1 if you have the `z' library (-lz).])
              GZ_LIBS="-lz"])

AC_ARG_WITH(sstream,
        AS_HELP_STRING(--with-sstream,use sstream instead of strstream (default=yes)),
                       use_sstream=$withval, use_sstream=yes)
if test "$use_sstream" = "yes"; then
   AC_CXX_HAVE_SSTREAM
fi

##################################################
# Checks for header files.
##################################################

AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h])
AC_CHECK_HEADERS([inttypes.h libintl.h libintl.h malloc.h netdb.h])
AC_CHECK_HEADERS([netinet/in.h poll.h pwd.h stddef.h stdlib.h sys/param.h])
AC_CHECK_HEADERS([sys/socket.h sys/time.h sys/types.h unistd.h])
#AC_CHECK_HEADERS([winsock2.h])

##################################################
# Checks for typedefs, structures, and compiler characteristics.
##################################################

AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

AC_CHECK_TYPES([socklen_t], [], [], [
#include <sys/types.h>
#include <sys/socket.h>
])
AC_CHECK_TYPES([size_t], [], [], [
#include <sys/types.h>
#include <sys/socket.h>
])


##################################################
# Checks for library functions.
##################################################

AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_DIAGNOSE([obsolete],[your code may safely assume C89 semantics that RETSIGTYPE is void.
Remove this warning and the `AC_CACHE_CHECK' when you adjust the code.])dnl
AC_CACHE_CHECK([return type of signal handlers],[ac_cv_type_signal],[AC_COMPILE_IFELSE(
[AC_LANG_PROGRAM([#include <sys/types.h>
#include <signal.h>
],
		 [return *(signal (0, 0)) (0) == 1;])],
		   [ac_cv_type_signal=int],
		   [ac_cv_type_signal=void])])
AC_DEFINE_UNQUOTED([RETSIGTYPE],[$ac_cv_type_signal],[Define as the return type of signal handlers
		    (`int' or `void').])

AC_FUNC_STRFTIME
AC_CHECK_FUNCS([memset], [], [
  echo "************** ERROR ****************"
  echo "Could not find memset function."
  echo "Please upgrade you system"
  exit 1
])
AC_CHECK_FUNCS([floor gethostbyname gettimeofday inet_ntoa memset mkdir pow rint])
AC_CHECK_FUNCS([socket sqrt strdup strerror])

##################################################
# check flex
##################################################

AC_SUBST([FLEX])
AC_SUBST([FLEXFLAGS])
AC_SUBST([AM_FLEXFLAGS])
#AC_ARG_WITH([flex],
#            [  --with-flex=FLEX_PATH   set the flex path. it is possible to specify a
#                          certain flex to generate lexer sources.])
#if test x$with_flex != x && test -x "$with_flex" ; then
#  FLEX="$with_flex"
AC_ARG_VAR(FLEX, [name and location of the fast lexical analyser generator.
If it is not set, configure looks for flex in your PATH])
if test -z "$FLEX" ; then
#  AC_CHECK_PROG([FLEX],[flex],flex)
  AX_PROG_FLEX([FLEX="$LEX"],[FLEX=""])
fi
AM_FLEXFLAGS="-olex.yy.c"

if test -z "$FLEX" ; then
  AC_MSG_ERROR([
    \`flex' is missing on your system.  Please install it first.]);
fi

##################################################
# check bison
##################################################

#AC_PROG_YACC
AX_PROG_BISON([],
              [AC_MSG_ERROR([
     \`bison' is missing on your system.  Please install it first.]);])

##################################################
# libtool settings
##################################################

AC_ENABLE_SHARED
AC_DISABLE_STATIC
#LT_INIT([shared disable-static])
#LT_LANG([C++])
AC_PROG_LIBTOOL
#AC_ENABLE_SHARED
#AC_DISABLE_STATIC
AC_SUBST(LIBTOOL_DEPS)
##LIBS="$LIBS $lt_cv_dlopen_libs"

#AC_ARG_VAR(SED, [name and location of sed executable.  If it is not
#set, configure looks for sed in your PATH])
#
#grep "SED=" ./libtool >& /dev/null
#if test "$?" -ne 0; then
#    if test -z "$SED" ; then
#      AC_PATH_PROG(SED, sed)
#    fi
#    if test -z "$SED"; then
#        AC_MSG_ERROR([
#            sed cannot be found on your system.  Please install it in
#            your PATH, or specify it's location to configure.  e.g.
#            `./configure SED=/bin/sed`], 1)
#    fi
#    mv libtool libtool.tmp
#    ${AWK} -v SED="$SED" '/# libtool/ \
#    { if( done != "true" ) { print "SED=\"" SED "\""; done = "true"; } } \
#    { print; }' libtool.tmp > libtool
#    chmod +x libtool
#fi
#
#echo `pwd`
#mv libtool libtool.tmp
#${AWK} -v EGREP="${EGREP}" '/# libtool/ \
#{ if( done != "true" ) { print "EGREP=\"" EGREP "\""; done = "true"; } } \
#{ print; }' libtool.tmp > libtool
#chmod +x libtool
#rm -f libtool.tmp

##################################################
# check boost
##################################################

AX_BOOST_BASE([1.32.0])
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
#OLDLIBS="$LIBS"
#LIBS="$LIBS $BOOST_FILESYSTEM_LIB"

##################################################
# enable/disable rcssclient
##################################################

AC_SUBST(RCSSCLIENT)
AC_ARG_ENABLE(rcssclient,
[  --enable-rcssclient[=ARG] build rcssclient, a sample client [ARG=yes]],
if test -n $enableval; then
  if test $enableval = yes; then
    RCSSCLIENT=rcssclient$EXEEXT
    echo enabling rcssclient
  elif test $enableval = no; then
    RCSSCLIENT=
    echo disabling rcssclient
  fi
fi
, RCSSCLIENT=rcssclient$EXEEXT)


if ! test -z "$RCSSCLIENT"; then
  AC_CHECK_FUNCS([select])
  AC_FUNC_SELECT_ARGTYPES
#  AC_ARG_WITH(ncurses,
#        AS_HELP_STRING(--with-ncurses,use ncurses for rcssclient (default=yes)),
#                       use_ncurses=$withval, use_ncurses=yes)
#  if test "$use_ncurses" = "yes"; then
#    AC_CHECK_LIB(ncurses, initscr,[
#      AC_DEFINE(HAVE_LIBNCURSES, 1, [
#        "Define to 1 if you have the `ncurses' library (-lncurses)."] )
      AC_ARG_VAR(RCSSCLIENT_LIBS, The libs rcssclient needs)
#      RCSSCLIENT_LIBS="-lncurses"
#    ])
#  fi
fi

##################################################
# generated files
##################################################

AC_CONFIG_FILES([Makefile
                rcssbase/Makefile
                rcssbase/net/Makefile
                rcssbase/conf/Makefile
                rcssbase/gzip/Makefile
                src/Makefile
                src/rcsoccersim],
                [test -f src/rcsoccersim && chmod +x src/rcsoccersim])
AC_OUTPUT

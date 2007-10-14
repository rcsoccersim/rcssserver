dnl Process this file with autoconf to produce a configure script.
AC_INIT([RCSSServer], 11.1.2, sserver-admin@users.sf.net, rcssserver)

AC_PREREQ(2.59)

AC_CONFIG_SRCDIR([src/main.C])
AM_INIT_AUTOMAKE([gnu 1.7.2 check-news dist-bzip2 dist-zip])
AC_CONFIG_HEADERS([config.h])
AC_PROG_MAKE_SET

AC_PROG_AWK
AC_EXEEXT
AC_LANG(C++)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
ACX_PTHREAD([LIBS="$PTHREAD_LIBS $LIBS"
             CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
             CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
             CC="$PTHREAD_CC"])

dnl Checks for libraries.
AC_CHECK_LIB([m], [cos])
dnl AC_CHECK_LIB(expat, XML_Parse)
AC_CHECK_LIB([z], [deflate])

AC_ARG_WITH(sstream,
        AS_HELP_STRING(--with-sstream,use sstream instead of strstream (default=yes)),
                       use_sstream=$withval, use_sstream=yes)
if test "$use_sstream" = "yes"; then
   AC_CXX_HAVE_SSTREAM
fi

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(pwd.h)
AC_CHECK_HEADERS(sys/time.h)
AC_CHECK_HEADERS(sys/types.h)
AC_CHECK_HEADERS(netinet/in.h)
AC_HEADER_TIME

AC_CHECK_FUNCS([memset], [], [
  echo "************** ERROR ****************"
  echo "Could not find memset function."
  echo "Please upgrade you system"
  exit 1
])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

dnl Checks for library functions.

#AC_ARG_WITH(mysql,
#        AS_HELP_STRING(--with-mysql,use mysql to store results in a database (default=yes)),
#                       use_mysql=$withval, use_mysql=yes)
#
#if test "$use_mysql" = "yes"; then
#   AC_CHECK_LIB(mysqlclient, mysql_real_connect,
#                [AC_CHECK_HEADERS(mysql/mysql.h,
#                                 [AC_SUBST(MYSQLLIB,"-lmysqlclient")],
#                                 [use_mysql=no] )],
#                [use_mysql=no] )
#fi
#
#AM_CONDITIONAL(BUILD_MYSQL_SAVER, [test "$use_mysql" = "yes"])


AC_SUBST([FLEX])
AC_SUBST([FLEXFLAGS])
AC_SUBST([AM_FLEXFLAGS])
AC_CHECK_PROG([FLEX],
              [flex],
              flex,
              $srcdir/missingflex)
AM_FLEXFLAGS="-olex.yy.c"

AC_PROG_YACC

AC_DISABLE_STATIC
AC_LIBTOOL_DLOPEN
# due to a problem with libtool 1.4.2 and autoconf 2.53 and
# automake 1.6.3, I need to use the following line to
# enable dlopen support. I don't know how good or bad this is
AC_DEFUN([AC_PROVIDE_AC_LIBTOOL_DLOPEN])
# AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)
LIBS="$LIBS $lt_cv_dlopen_libs"

AC_ARG_VAR(SED, [name and location of sed executable.  If it is not
set, it configure looks for sed in your PATH])

grep "SED=" ./libtool >& /dev/null
if test "$?" -ne 0; then
    AC_PATH_PROG(SED, sed)
    if test -z "$SED"; then
        AC_MSG_ERROR([
            sed cannot be found on your system.  Please install it in
            your PATH, or specify it's location to configure.  e.g.
            `./configure SED=/bin/sed`], 1)
    fi
    mv libtool libtool.tmp
    ${AWK} -v SED="$SED" '/# libtool/ \
    { if( done != "true" ) { print "SED=\"" SED "\""; done = "true"; } } \
    { print; }' libtool.tmp > libtool
    chmod +x libtool
fi

echo `pwd`
mv libtool libtool.tmp
${AWK} -v EGREP="${EGREP}" '/# libtool/ \
{ if( done != "true" ) { print "EGREP=\"" EGREP "\""; done = "true"; } } \
{ print; }' libtool.tmp > libtool
chmod +x libtool

AC_ARG_VAR(RCSSBASE, [The prefix that was used when installing rcssbase])
if test "$RCSSBASE"; then
  if test -z "$PATH"; then
    PATH="$RCSSBASE/bin"
  else
    PATH="$PATH:$RCSSBASE/bin"
  fi
  if test -z "$LDFLAGS"; then
    LDFLAGS="-L$RCSSBASE/lib"
  else
    LDFLAGS="$LDFLAGS -L$RCSSBASE/lib"
  fi
  if test -z "$CPPFLAGS"; then
    CPPFLAGS="-I$RCSSBASE/include"
  else
    CPPFLAGS="$CPPFLAGS -I$RCSSBASE/include"
  fi
fi

AX_BOOST_BASE([1.32.0])
AX_BOOST_FILESYSTEM
CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
OLDLIBS="$LIBS"
LIBS="$LIBS $BOOST_FILESYSTEM_LIB"

AC_CHECK_LIB([rcssnet], [main],
             [],
             [AC_MSG_ERROR([
Could not find the rcssbase/net library or it is out of
date. Please (re)install rcssbase before proceeding, or
specify where it was installed with the RCSSBASE
environment variable.
])])
AC_CHECK_LIB([rcsslib], [main],
             [],
             [AC_MSG_ERROR([
Could not find the rcssbase/lib library or it is out of
date. Please (re)install rcssbase before proceeding, or
specify where it was installed with the RCSSBASE
environment variable.
])])
AC_CHECK_LIB([rcssconfparser], [main],
             [],
             [AC_MSG_ERROR([
Could not find the rcssbase/conf library or it is out of
date. Please (re)install rcssbase before proceeding, or
specify where it was installed with the RCSSBASE
environment variable.
])])
AC_CHECK_LIB([rcssgz], [main],
             [AC_DEFINE([HAVE_RCSSGZ], [1],
                        [Define to 1 if you have the `rcssgz' library (-lrcssgz).])],
             [AC_MSG_ERROR([
Could not find the rcssbase/gzip library or it is out of
date. Please (re)install rcssbase before proceeding, or
specify where it was installed with the RCSSBASE
environment variable.
])])
AC_CHECK_LIB([rcssbase], [main],
             [],
             [AC_MSG_ERROR([
Could not find the rcssbase library or it is out of
date. Please (re)install rcssbase before proceeding, or
specify where it was installed with the RCSSBASE
environment variable.
])])


LIBS="$OLDLIBS"

AC_SUBST(RCSSLIBIMPORTER)
AC_SUBST(RCSSMODTEST)

AC_CHECK_PROGS(RCSSLIBIMPORTER,[rcsslibimporter])
if test -z "${RCSSLIBIMPORTER}"; then
  AC_MSG_ERROR([
Could not find the rcsslibimporter executable or it is out of
date. Please (re)install rcssbase before proceeding, or
specify where it was installed with the RCSSBASE
environment variable.
])
fi

AC_CHECK_PROGS(RCSSMODTEST,[rcssmodtest])
if test -z "${RCSSMODTEST}"; then
  AC_MSG_ERROR([
Could not find the rcssmodtest executable or it is out of
date. Please (re)install rcssbase before proceeding, or
specify where it was installed with the RCSSBASE
environment variable.
])
fi

AC_ARG_VAR([MODULES], [Libraries that rcssserver could possibly open
with rcsslib.  This is only needed if you are not building shared
libraries or dlopen is not supported on your system])

./libtool --features | grep '^enable shared libraries$' >& /dev/null
ENABLE_SHARED=$?
AM_CONDITIONAL(NODLOPEN, [test $ENABLE_SHARED -ne 0])

#AC_CHECK_HEADERS(sys/filio.h,[], [
#  AC_CHECK_HEADERS(sys/ioctl.h, [], [
#    echo ""
#    echo "****************** ERROR *****************"
#    echo "Could not find sys/filio.h or sys/ioctl.h."
#    echo "RCSServer needs one of these headers to"
#    echo "compile."
#    echo ""
#    exit 1
#  ])
#])

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
  AC_ARG_WITH(ncurses,
        AS_HELP_STRING(--with-ncurses,use ncurses for rcssclient (default=yes)),
                       use_ncurses=$withval, use_ncurses=yes)
  if test "$use_ncurses" = "yes"; then
    AC_CHECK_LIB(ncurses, initscr,[
      AC_DEFINE(HAVE_LIBNCURSES, 1, [
        "Define to 1 if you have the `ncurses' library (-lncurses)."] )
      AC_ARG_VAR( RCSSCLIENT_LIBS, "the libs rcssclient needs" )
      RCSSCLIENT_LIBS="-lncurses"
    ])
  fi
fi


AX_EXTRA_DIST
AX_UPLOAD([ncftpput -v upload.sourceforge.net /incoming %%s])
AX_RPM_GEN_SPEC([spec.tmpl],
	    [The RoboCup Soccer Simulator Server],
            [LGPL],
	    [Applications/Engineering],
	    [http://sserver.sf.net],
	    [http://prdownloads.sourceforge.net/sserver/],
[The RoboCup Soccer Simulator Server (rcssserver) is a research and\neducational tool for mutli-agent systems and artificial\nintelligence. It allows 11 simulated autonomous robotic players to\nplay soccer (football).])
AX_DIST_RPM
#AX_DIST_MSI([$srcdir/Setup/Release/${PACKAGE_NAME}Setup.msi])
AX_CVS([:pserver:anonymous@cvs.sourceforge.net:/cvsroot/sserver])

AX_PKG_GEN_PLIST([Info.plist], [Description.plist],
	         [RoboCup], [com.robocup.${PACKAGE}])
AX_DIST_DMG



AC_CONFIG_FILES([Makefile
		 src/Makefile])
AC_OUTPUT

# AC_LIB_RCSSCLANGPARSER([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ---------------------------------------------------------
# Checks for the rcssclangparser library
AC_DEFUN([AC_LIB_RCSSCLANGPARSER],
[AS_VAR_PUSHDEF([ac_lib_rcssclangparser], [ac_cv_lib_rcssclangparser])dnl
AC_CACHE_CHECK(whether the rcssclangparser library is available, ac_cv_lib_rcssclangparser,
               [AC_LANG_PUSH(C++)
                OLD_LDFLAGS="$LDFLAGS"
                LDFLAGS="$LDFLAGS -lrcssclangparser"
                AC_LINK_IFELSE([@%:@include <rcssserver/clangrulemsg.h>
                                int main()
                                {
                                    rcss::clang::RuleMsg rule;
                                    return 0;
                                }],
                                [AS_VAR_SET(ac_lib_rcssclangparser, yes)], 
                                [AS_VAR_SET(ac_lib_rcssclangparser, no) 
                                 LDFLAGS="$OLD_LDFLAGS"
                                ])
                AC_LANG_POP(C++)
                ])
AS_IF([test AS_VAR_GET(ac_lib_rcssclangparser) = yes], [$1], [$2])
AS_VAR_POPDEF([ac_lib_rcssclangparser])dnl
])# AC_LIB_RCSSCLANGPARSER

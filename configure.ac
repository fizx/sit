AC_INIT([sit], [0.1.0])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC_C99
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_LIBTOOL

AC_CHECK_LIB([pthread], [pthread_create])

AC_CONFIG_SUBDIRS([contrib/pcre contrib/libev src])

AC_CONFIG_FILES([Makefile
                 src/Makefile])

AM_INIT_AUTOMAKE([1.9 foreign])

AC_OUTPUT
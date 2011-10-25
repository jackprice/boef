AC_INIT([Buffer Overflow Exploitation Framework], [0.1], [boef@quetuo.net], [boef], [http://www.quetuo.net/boef])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_CONFIG_SUBDIRS([modules])
AC_CHECK_FUNCS([ptrace])
AC_CHECK_HEADERS([sys/types.h sys/ptrace.h sys/param.h], , AC_MSG_ERROR[required header file missing])
AC_CHECK_HEADERS([machine/reg.h],,)
AC_HEADER_STDC
AC_OUTPUT

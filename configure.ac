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

## CHECK PREREQUISITES ##

# Check C Standard Library
AC_CHECK_HEADERS([stdio.h stdlib.h string.h errno.h stdint.h stdarg.h], , AC_MSG_ERROR[Required C stdlib header missing])

# Check C POSIX Library
AC_CHECK_HEADERS([unistd.h sys/types.h dirent.h sys/wait.h], , AC_MSG_ERROR[Required C POSIX header missing])

# Check for ptrace
AC_CHECK_FUNCS([ptrace])
AC_CHECK_HEADERS([machine/reg.h sys/user.h sys/ptrace.h],,)

# Check for dl
AC_CHECK_LIB([dl], [dlopen], , , )
AC_CHECK_HEADERS([dlfcn.h],,)

# Check for sched
AC_CHECK_HEADERS([sched.h])

# Check for param
AC_CHECK_HEADERS([sys/param.h])

# Check for cstd headers
AC_HEADER_STDC
AC_INCLUDES_DEFAULT
AC_OUTPUT

AC_INIT([Buffer Overflow Exploitation Framework], [0.1], [boef@quetuo.net], [boef], [http://www.quetuo.net/boef])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
#AC_CONFIG_SUBDIRS([modules])

## CHECK FEATURES ##

# SQLite
AC_ARG_ENABLE([sqlite3],
              AS_HELP_STRING([--disable-sqlite3], [Disable SQLite]))
AS_IF([test "x$enable_sqlite3" != "xno"], [
    AC_CHECK_HEADERS([sqlite3.h], , AC_MSG_ERROR[SQLite headers required, use --disable-sqlite3])
    AC_CHECK_LIB([sqlite3], [sqlite3_open], , , )
    AC_DEFINE([WITH_SQLITE3], [1], [Define if SQLite is enabled])
])

## CHECK PREREQUISITES ##

# Check C Standard Library
AC_CHECK_HEADERS([stdio.h stdlib.h string.h errno.h stdint.h stdarg.h], , AC_MSG_ERROR[Required C stdlib header missing])

# Check C POSIX Library
AC_CHECK_HEADERS([unistd.h sys/types.h dirent.h sys/wait.h fcntl.h], , AC_MSG_ERROR[Required C POSIX header missing])
AC_CHECK_FUNCS([posix_openpt])

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

# Check for pwd
AC_CHECK_HEADERS([pwd.h])

# Check for stat
AC_CHECK_HEADERS([sys/stat.h])

# Check for signal
AC_CHECK_HEADERS([signal.h])

# Check for backtrace
AC_CHECK_FUNCS([backtrace])
AC_CHECK_HEADERS([execinfo.h])
AC_CHECK_LIB([execinfo], [backtrace], , , ) # On BSD at least, libexecinfo is needed

# Check for binutils
AC_CHECK_HEADERS([bfd.h])
AC_CHECK_LIB([bfd], [bfd_openr], , , )

# Check for libiberty
AC_CHECK_HEADERS([libiberty.h])
AC_CHECK_LIB([iberty], [xmalloc], , , )

# Check for libopcodes
AC_CHECK_HEADERS([dis-asm.h])
AC_CHECK_LIB([opcodes], [buffer_read_memory], , , )

# Check for GTK
PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.2)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

# Check for VTE
PKG_CHECK_MODULES(VTE, vte)
AC_SUBST(VTE_CFLAGS)
AC_SUBST(VTE_LIBS)

# Check for wx
#AM_OPTIONS_WXCONFIG
#reqwx=2.4.0
#AM_PATH_WXCONFIG($reqwx, wxWin=1)
#if test "$wxWin" != 1; then
#	AC_MSG_ERROR([
#		wxWidgets must be installed on your system.
 #
#		Please check that wx-config is in path, the directory
#		where wxWidgets libraries are installed (returned by
#		'wx-config --libs' or 'wx-config --static --libs' command)
#		is in LD_LIBRARY_PATH or equivalent variable and
#		wxWidgets version is $reqwx or above.
#		])
#fi
#CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
#CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
#CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
#LIBS="$LIBS $WX_LIBS"

# Check for something else, I can't remember what though O_o

# Check for cstd headers
AC_HEADER_STDC
AC_INCLUDES_DEFAULT
AC_OUTPUT

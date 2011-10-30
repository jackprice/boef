#include "config.h"

// The following headers are required!

// C Standard Library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>

// C POSIX Library
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>

// C++ Standard Library
#include <iostream>
#include <sstream>

// C++ STL
#include <vector>
#include <map>

// The following headers are platform-dependant

#ifdef HAVE_MACHINE_REG_H
	#include <machine/reg.h>
#endif
#ifdef HAVE_SYS_USER_H
	#include <sys/user.h>
#endif
#ifdef HAVE_DLFCN_H
	#include <dlfcn.h>
#endif
#ifdef HAVE_SCHED_H
	#include <sched.h>
#endif
#ifdef HAVE_SYS_PARAM_H
	#include <sys/param.h>
#endif
#ifdef HAVE_SYS_PTRACE_H
	#include <sys/ptrace.h>
#endif
#ifdef HAVE_PWD_H
	#include <pwd.h>
#endif
#ifdef HAVE_SYS_STAT_H
	#include <sys/stat.h>
#endif
#ifdef HAVE_SIGNAL_H
	#include <signal.h>
#endif
#ifdef HAVE_EXECINFO_H
	#include <execinfo.h>
#endif
#ifdef HAVE_BFD_H
	#include <bfd.h>
#endif
#ifdef HAVE_LIBIBERTY_H
	//#include <libiberty.h>
#endif
#ifdef HAVE_DIS_ASM_H
	#include <dis-asm.h>
#endif

// SQLite3

#ifdef WITH_SQLITE3
	#include <sqlite3.h>
#endif

#include "main.h"
#include "modules.h"
#include "host.h"
#include "workspace.h"
#include "interface.h"
#include "fuzz.h"
#include "debug.h"

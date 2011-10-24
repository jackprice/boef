/******************************************************************************/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/******************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sched.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdint.h>
#include <sys/user.h>
#include <sys/ptrace.h>

#include "main.h"
#include "modules.h"
#include "host.h"
#include "debug.h"

debug_process::debug_process () {
	pipe (pfds);
}

debug_process::debug_process (char * _exec) {
	pipe (pfds);
	pid = fork_exec (_exec);
}

debug_process::debug_process (pid_t _pid) {
	pipe (pfds);
	pid = attach_pid (_pid);
}

debug_process::~debug_process () {
}

pid_t debug_process::fork_exec (char * _exec) {
	int waitval;
	switch (pid = fork ()) {
		case -1:
			return -1;
			break;
		case 0:
			close (0);
			dup2 (0, pfds [0]);
			close (pfds [1]);
			#ifdef linux
				ptrace (PTRACE_TRACEME, 0, 0, 0);
			#endif
			execl (_exec, NULL, NULL);
			break;
		case 1:
			wait (&waitval);
			break;
	}
	exec = _exec;
	return pid;
}

pid_t debug_process::attach_pid (pid_t _pid) {
	#ifdef linux
		if (ptrace (PTRACE_ATTACH, _pid, 0, 0) == -1) {
			return -1;
		}
		pid = _pid;
		return pid;
	#endif
}

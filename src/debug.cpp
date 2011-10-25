/******************************************************************************/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/******************************************************************************/
/** Copyright (C) 2011 Quetuo (http://www.quetuo.net)                        **/
/**                                                                          **/
/** This program is free software: you can redistribute it and/or modify     **/
/** it under the terms of the GNU General Public License as published by     **/
/** the Free Software Foundation, either version 3 of the License, or        **/
/** (at your option) any later version.
/**                                                                          **/
/** This program is distributed in the hope that it will be useful,          **/
/** but WITHOUT ANY WARRANTY; without even the implied warranty of           **/
/** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            **/
/** GNU General Public License for more details.                             **/
/**                                                                          **/
/** You should have received a copy of the GNU General Public License        **/
/** along with this program.  If not, see <http://www.gnu.org/licenses/>.    **/
/******************************************************************************/

#include "include.h"

int debug_ptrace_traceme () {
	#ifdef __linux__
		return (int) ptrace (PTRACE_TRACEME, 0, 0, 0);
	#endif
	#ifdef BSD
		return (int) ptrace (PT_TRACE_ME, 0, 0, 0);
	#endif
}

int debug_ptrace_getregs (pid_t pid, void * data) {
	#ifdef __linux__
		return (int) ptrace (PTRACE_GETREGS, pid, NULL, data);
	#endif
	#ifdef BSD
		return (int) ptrace (PT_GETREGS, pid, NULL, (int) data);
	#endif
}

debug_process::debug_process () {
	pid = -1;
	pipe (pfds);
}

debug_process::debug_process (char * _exec) {
	pid = -1;
	pipe (pfds);
	pid = fork_exec (_exec);
}

debug_process::debug_process (pid_t _pid) {
	pid = -1;
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
			debug_ptrace_traceme ();
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
	printf ("Attaching to process %i...\n", _pid);
	#ifdef linux
		if (ptrace (PTRACE_ATTACH, _pid, 0, 0) == -1) {
			return -1;
		}
		pid = _pid;
		return pid;
	#endif
	#ifdef BSD
		if (ptrace (PT_ATTACH, _pid, 0, 0) == -1) {
			return -1;
		}
		pid = _pid;
		return pid;
	#endif
}

pid_t debug_process::getpid () {
	return pid;
}

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
/** (at your option) any later version.                                      **/
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

using namespace std;

int masterfd = -1;
char * slavedevice = NULL;
int slavefd = -1;
pid_t childpid = -1;
int ptraceing = -1;
int childrunning = 0;
s_process proc;
#ifdef BSD
	struct reg regs;
#endif
#ifdef __linux__
	struct user_regs_struct regs;
#endif

#ifndef HAVE_POSIX_OPENPT
	int posix_openpt (int flags) {
		return open ("/dev/ptmax", flags);
	}
#endif

void host_childsig (int sig, siginfo_t * info, void * ptr) {
	if (info -> si_pid == childpid) {
		if (info -> si_code == CLD_EXITED) {
			printf ("Child exited with status %i\n", info -> si_status);
			childpid = -1;
			return;
		}
		if (info -> si_code == CLD_TRAPPED) {
			childrunning = 1;
			return;
		}
		
		if (info -> si_status == 5) {
			//printf ("Debugger attached\n");
			ptraceing = 0;
			return;
		}
		printf ("\n");
		host_dumpregs ();
		psignal (info -> si_status, "\nChild Signal");
		printf ("Child Signal: %i\n", info -> si_status);
	}
	/*pid_t _pid = wait (NULL);
	if (_pid == childpid) {
		psignal (sig, "\nRecieved");
		printf ("Host disconnected because child exited\n");
		childpid = -1;
		return;
	}*/
}

int host_init () {
	printf ("Initalising host (%s)...\n", ttyname (0));
	struct sigaction act;
	memset (&act, 0, sizeof (act));
	act.sa_sigaction = host_childsig;
	act.sa_flags = SA_SIGINFO;
	sigaction (SIGCHLD, &act, NULL);
	return 0;
}

int host_init_pts () {
	printf ("Creating slave pts for %s...\n", ttyname (0));
	if (slavefd > -1) {
		close (slavefd);
		slavefd = -1;
	}
	if (masterfd > -1) {
		close (masterfd);
		masterfd = -1;
	}
	masterfd = posix_openpt (O_RDWR | O_NOCTTY);
	if (masterfd == -1 || 
		grantpt (masterfd) == -1 || 
		unlockpt (masterfd) == -1 ||
		(slavedevice = ptsname (masterfd)) == NULL) {
		printf ("Failed to open pty: %s\n", strerror (errno));
		exit (1);
	}
	printf ("Opened %s\n", slavedevice);
	slavefd = open (ptsname (masterfd), O_RDWR);
}

void host_cleanup () {
	printf ("Cleaning up host...");
	if (masterfd != -1) {
		close (masterfd);
		masterfd = -1;
	}
	if (slavefd != -1) {
		close (slavefd);
		slavefd = -1;
	}
	if (childpid != -1) {
		kill (childpid, 9);
		childpid = -1;
	}
	interface_printok (true);
	return;
}

void host_attach (pid_t pid) {
	return;
}

void host_exec (char * exec) {
	if (childpid > -1) {
		printf ("Host currently attached - use host kill or host detach\n");
		return;
	}
	printf ("Executing %s...\n", exec);
	vector <string> args;
	explode_string (exec, &args);
	exec = (char *) args [0].c_str ();
	char ** cargs = (char **) malloc (args.size () + 1);
	int i;
	for (i = 0; i < args.size (); i ++) {
		*(cargs + i) = strdup (args [i].c_str ());
	}
	*(cargs + i + 1) = NULL;
	
	if (debug_open (cargs [0]) == -1) {
		printf ("Could not debug %s\n", cargs [0]);
		free (cargs);
		return;
	}
	host_init_pts ();
	
	switch (childpid = fork ()) {
		case -1:
			printf ("Failed to fork\n");
			return;
		case 0:
			// Child
			#ifdef __linux__
				if (setpgrp() == -1) {
					printf ("Failed to detatch\n");
					exit (1);
				}
			#endif
			#ifdef BSD
				if (setpgrp(getpid (), getpid ()) == -1) {
					printf ("Failed to detatch\n");
					exit (1);
				}
			#endif
			close (masterfd);
			close (0);
			close (1);
			close (2);
			dup (slavefd);
			dup (slavefd);
			dup (slavefd);
			debug_ptrace_traceme ();
			execv (exec, cargs);
			printf ("Failed to execute\n");
			exit (1);
			break;
		default:
			// Parent
			close (slavefd);
			wait (NULL);
			printf ("Child started with PID %i\n", childpid);
			process_load (childpid, &proc);
			//host_rununtilfault ();
			sleep (1);
			free (cargs);
			break;
	}
	return;
}

void host_kill () {
	if (childpid != -1) {
		kill (childpid, 9);
		childpid = -1;
	}
	return;
}

void host_run () {
	host_rununtilfault ();
	return;
	#ifdef linux
		if (ptrace (PTRACE_CONT, childpid, 1, 0) == 0) {
			
		}
		else {
			printf ("Could not run: %.2X\n", errno);
		}
		return;
	#endif
	#ifdef BSD
		if (ptrace (PT_CONTINUE, childpid, (caddr_t) 1, 0) == 0) {
			
		}
		else {
			printf ("Could not run: %.2X\n", errno);
		}
		return;
	#endif
	return;
}

void host_printinfo () {
	printf ("Child PID: %i\n", childpid);
	host_dumpregs ();
	debug_printinfo ();
	return;
}

void host_write (void * buffer, size_t len) {
	printf ("Writing %i bytes to %i\n", len, childpid);
	if (childpid == -1) {
		printf ("No child attached\n");
		return;
	}
	write (masterfd, buffer, len);
	write (masterfd, "\n\0", 2);
	return;
}

void host_readline () {
	char * buf = (char *) malloc (256);
	memset (buf, 0, 256);
	read (masterfd, buf, 255);
	printf ("%s\n", buf);
	free (buf);
}

int host_read (void * buffer, size_t len) {
	return read (masterfd, buffer, len);
}

int host_getregs () {
	return ptrace (PTRACE_GETREGS, childpid, 0, (int) &regs);
}

void host_dumpregs () {
	host_getregs ();
	#ifdef linux
		printf ("EAX %.8lX\tEBX %.8lX\tECX %.8lX\tEDX %.8lX\n", regs.eax, regs.ebx, regs.ecx, regs.edx);
		printf ("ESP %.8lX\tEBP %.8lX\tESI %.8lX\tEDI %.8lX\n", regs.esp, regs.ebp, regs.esi, regs.edi);
		printf ("EIP %.8lX\n", regs.eip);
	#endif
	#ifdef BSD
		printf ("EAX %.8lX\tEBX %.8lX\tECX %.8lX\tEDX %.8lX\n", regs.r_eax, regs.r_ebx, regs.r_ecx, regs.r_edx);
		printf ("ESP %.8lX\tEBP %.8lX\tESI %.8lX\tEDI %.8lX\n", regs.r_esp, regs.r_ebp, regs.r_esi, regs.r_edi);
		printf ("EIP %.8lX\n", regs.r_eip);
	#endif
}

void host_rununtilfault () {
	s_process prevproc;
	#ifdef BSD
		struct reg prevregs;
	#endif
	#ifdef __linux__
		struct user_regs_struct prevregs;
	#endif
	if (ptrace (PTRACE_SINGLESTEP, childpid, 0, 0) == -1) {
		printf ("Failed to step\n");
		return;
	}
	process_load (childpid, &proc);
	host_getregs ();
	printf ("\n");
	prevregs.eip = 0;
	while (1) {
		ptrace (PTRACE_SINGLESTEP, childpid, 0, 0);
		//process_load (childpid, &proc);
		prevregs = regs;
		//if (host_getregs () == -1) {break;}
		printf ("EIP: %08.8lX \033[00G", regs.eip);
		usleep (100);
	}
	printf ("\nProcess no longer running");
}

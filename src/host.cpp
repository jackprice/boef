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

int masterfd = -1;
char * slavedevice = NULL;
int slavefd = -1;
pid_t childpid = -1;

#ifndef HAVE_POSIX_OPENPT
	int posix_openpt (int flags) {
		return open ("/dev/ptmax", flags);
	}
#endif

void host_childsig (int sig) {
	wait (NULL);
	printf ("Host disconnected because child exited\n");
	childpid = -1;
}

int host_init () {
	printf ("Initalising host (%s)...\n", ttyname (0));
	signal (SIGCHLD, host_childsig);
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
	char ** cargs = (char **) malloc (4);
	*(cargs) = "/bin/sh";
	*(cargs + 1) = "-c";
	*(cargs + 2) = exec;
	*(cargs + 3) = NULL;
	
	host_init_pts ();
	
	switch (childpid = fork ()) {
		case -1:
			printf ("Failed to fork\n");
			return;
		case 0:
			// Child
			close (masterfd);
			close (0);
			close (1);
			close (2);
			dup (slavefd);
			dup (slavefd);
			dup (slavefd);
			execv ("/bin/sh", cargs);
			printf ("Failed to execute\n");
			return;
			break;
		case 1:
			// Parent
			close (slavefd);
			wait (NULL);
			printf ("Child started with PID %i\n", childpid);
			break;
	}
	return;
}

void host_kill () {
	return;
}

void host_run () {
	return;
}

void host_printinfo () {
	return;
}

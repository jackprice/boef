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

debug_process * debug = NULL;

void host_attach (pid_t pid) {
	if (debug != NULL) {
		printf ("Host is currently attached, use 'host kill' or 'host detach' to free\n");
	}
	debug = new debug_process (pid);
	if (debug -> getpid () < 0) {
		printf ("Failed to attach: 0x%.2X\n", errno);
		delete debug;
		debug = NULL;
		return;
	}
	return;
}

void host_exec (char * exec) {
	if (debug != NULL) {
		printf ("Host is currently attached, use 'host kill' or 'host detach' to free\n");
	}
	printf ("Executing \"%s\"\n", exec);
	debug = new debug_process (exec);
	return;
}

void host_printinfo () {
	if (debug == NULL) {
		printf ("Host not currently attached\n");
	}
	return;
}

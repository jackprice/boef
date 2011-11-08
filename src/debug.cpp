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

bfd * bfdf = NULL;
char * target = NULL;
std::map <std::string, asymbol> symbols;
std::map <std::string, bfd_section> sections;
std::vector <std::string> vulnfunctions;

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

void debug_init () {
	#ifdef HAVE_LIBBFD
		bfd_init ();
	#endif
	target = getenv ("GNUTARGET");
	printf ("Initialising debugging...\n");
	vulnfunctions.push_back ("gets");
	vulnfunctions.push_back ("strcpy");
	vulnfunctions.push_back ("strcat");
	vulnfunctions.push_back ("sprintf");
	printf ("Initialising debugging\n");
}

void debug_cleanup () {
	printf ("Cleaning up debugging...");
	if (bfdf != NULL) {
		#ifdef HAVE_LIBBFD
			bfd_close (bfdf);
			bfdf = NULL;
		#endif
	}
	interface_printok (true);
}

void debug_loadsections () {
	if (bfdf == NULL) {
		return;
	}
	sections.clear ();
	#ifdef HAVE_LIBBFD
		bfd_section * p;
		for (p = bfdf -> sections; p != NULL; p = p -> next) {
			sections [p -> name] = *p;
		}
	#endif
}

void debug_printsections () {
	if (bfdf == NULL) {
		return;
	}
	if (sections.size () == 0) {
		debug_loadsections ();
	}
	if (sections.size () == 0) {
		return;
	}
	printf ("Sections: (%i)\n", sections.size ());
	std::map <std::string, bfd_section> :: iterator it;
	for (it = sections.begin (); it != sections.end (); it ++) {
		printf ("\t%s\n", (*it).first.c_str ());
	}
}

void debug_loadsymbols () {
	if (bfdf == NULL) {
		return;
	}
	symbols.clear ();
	#ifdef HAVE_LIBBFD
		long storage_needed;
		asymbol ** symbol_table;
		long lsymbols;
		long i;
		storage_needed = bfd_get_symtab_upper_bound (bfdf);
		if (storage_needed <= 0) {
			return;
		}
		symbol_table = (asymbol **) malloc (storage_needed);
		if (symbol_table == NULL) {
			return;
		}
		lsymbols = bfd_canonicalize_symtab (bfdf, symbol_table);
		if (lsymbols < 0) {
			return;
		}
		for (i = 0; i < lsymbols; i ++) {
			//printf ("%02.2i: %08.8p\t %s \n", i, symbol_table [i] -> value, symbol_table [i] -> name);
			symbols [symbol_table [i] -> name] = *(symbol_table [i]);
		}
		free (symbol_table);
	#endif
}

void debug_printsymbols () {
	if (bfdf == NULL) {
		return;
	}
	if (symbols.size () == 0) {
		debug_loadsymbols ();
	}
	if (symbols.size () == 0) {
		return;
	}
	printf ("Symbols: (%i)\n", symbols.size ());
	std::map <std::string, asymbol> :: iterator it;
	for (it = symbols.begin (); it != symbols.end (); it ++) {
		printf ("\t%s\n", (*it).first.c_str ());
	}
}

void debug_printstack () {
}

int debug_open (char * fn) {
	if (bfdf != NULL) {
		return -1;
	}
	#ifdef HAVE_LIBBFD
		if ((bfdf = bfd_openr (fn, target)) == NULL) {
			return -1;
		}
		if (!bfd_check_format (bfdf, bfd_object)) {
			bfd_perror (NULL);
			return -1;
		}
	#endif
	debug_printsections ();
	debug_printsymbols ();
	return 0;
}

void debug_printinfo () {
	debug_printsections ();
	debug_printsymbols ();
}

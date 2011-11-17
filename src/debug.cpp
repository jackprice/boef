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

#ifdef HAVE_LIBBFD
	bfd * bfdf = NULL;
	char * target = NULL;
	std::map <std::string, asymbol *> symbols;
	std::map <std::string, bfd_section *> sections;
#endif
std::vector <std::string> vulnfunctions;
disassembler_ftype disassemble_fn = NULL;
struct disassemble_info disasm_info;

struct ASM_INSN {
	char mnemonic [16];
	char src [32];
	char dest [32];
	char arg [32];
} curr_insn;

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
		target = getenv ("GNUTARGET");
	#endif
	interface_log ("Initialising debugging...\n");
	vulnfunctions.push_back ("gets");
	vulnfunctions.push_back ("strcpy");
	vulnfunctions.push_back ("strcat");
	vulnfunctions.push_back ("sprintf");
}

void debug_cleanup () {
	interface_log ("Cleaning up debugging...");
	#ifdef HAVE_LIBBFD
	if (bfdf != NULL) {
			bfd_close (bfdf);
			bfdf = NULL;
	}
	#endif
	interface_printok (true);
}

void debug_loadsections () {
	#ifdef HAVE_LIBBFD
		if (bfdf == NULL) {
			return;
		}
		sections.clear ();
		bfd_section * p;
		for (p = bfdf -> sections; p != NULL; p = p -> next) {
			sections [p -> name] = p;
			interface_section_add ((char *) p -> name, p -> vma);
		}
	#endif
}

void debug_printsections () {
	#ifdef HAVE_LIBBFD
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
		std::map <std::string, bfd_section *> :: iterator it;
		for (it = sections.begin (); it != sections.end (); it ++) {
			printf ("\t%s\n", (*it).first.c_str ());
		}
	#endif
}

void debug_loadsymbols () {
	#ifdef HAVE_LIBBFD
		if (bfdf == NULL) {
			return;
		}
		symbols.clear ();
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
			symbols [symbol_table [i] -> name] = (symbol_table [i]);
			if (symbol_table [i] -> flags & BSF_FUNCTION) {
				interface_symbol_add ((char *) symbol_table [i] -> name, symbol_table [i] -> section -> vma + symbol_table [i] -> value);
			}
		}
		free (symbol_table);
	#endif
}

void debug_printsymbols () {
	#ifdef HAVE_LIBBFD
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
		std::map <std::string, asymbol *> :: iterator it;
		for (it = symbols.begin (); it != symbols.end (); it ++) {
			if ((*it).second -> flags & BSF_FUNCTION) {
				printf ("\t%s\n", (*it).first.c_str ());
				printf ("\t\tRelative to %s (%lX)\n", (*it).second -> section -> name, (*it).second -> section -> vma);
			}
		}
	#endif
}

asymbol * debug_get_symbol_from_address (long addr) {
	std::map <std::string, asymbol *> :: iterator it;
	for (it = symbols.begin (); it != symbols.end (); it ++) {
		if ((*it).second -> flags & BSF_FUNCTION) {
			if (addr == it -> second -> value + it -> second -> section -> vma) {
				return it -> second;
			}
		}
	}
	return NULL;
}

void debug_printstack () {
}

int debug_open (char * fn) {
	interface_set_progress (-1.0);
	#ifdef HAVE_LIBBFD
		if (bfdf != NULL) {
			return -1;
		}
		if ((bfdf = bfd_openr (fn, target)) == NULL) {
			return -1;
		}
		if (!bfd_check_format (bfdf, bfd_object)) {
			bfd_perror (NULL);
			return -1;
		}
		disassemble_fn = disassembler (bfdf);
		disasm_info.arch = bfd_get_arch (bfdf);
		disasm_info.mach = bfd_get_mach (bfdf);
		if (bfd_big_endian (bfdf)) {
			disasm_info.endian = BFD_ENDIAN_BIG;
		}
		else if (bfd_little_endian (bfdf)) {
			disasm_info.endian = BFD_ENDIAN_LITTLE;
		}
	#endif
	debug_loadsections ();
	debug_loadsymbols ();
	interface_set_progress (0);
	return 0;
}

void debug_printinfo () {
	debug_printsections ();
	debug_printsymbols ();
}

void debug_print_function_disasm (char * function) {
	std::map <std::string, asymbol *> :: iterator it;
	for (it = symbols.begin (); it != symbols.end (); it ++) {
		if ((*it).first == function) {
			asection * s = it -> second -> section;
			printf ("Disassembling %s\n", s -> name);
			long size = bfd_section_size (bfdf, s);
			void * buf = malloc (s -> lma);
			if (bfd_get_section_contents (bfdf, s, buf, 0, size) == false) {
				bfd_perror (NULL);
				interface_error ("Could not read section contents");
				return;
			}
			struct disassemble_info info;
			char * ins = (char *) malloc (30);
			init_disassemble_info (&info, ins, (fprintf_ftype) sprintf);
			info.mach = bfd_get_mach (bfdf);
			info.endian = BFD_ENDIAN_LITTLE;
			info.buffer = (bfd_byte *) buf;
			info.buffer_length = s -> lma;
			long vma = s -> vma;
			info.buffer += it -> second -> value;
			vma += it -> second -> value;
			while ((void *)info.buffer < buf + size) {
				asymbol * ss = debug_get_symbol_from_address (vma);
				if (ss != NULL) {
					printf ("<%s>\n", ss -> name);
				}
				printf ("\t0x%lX:%s\n", vma, ins);
				int b = disassemble_fn (0, &info);
				if (*(info.buffer) == 0xC3 || *(info.buffer) == 0xC2 || *(info.buffer) == 0xCA || *(info.buffer) == 0xCB) {
					break;
				}
				info.buffer += b;
				vma += b;
				if (b == 0) {
					info.buffer += 1;
					vma += 1;
				}
			}
			free (ins);
			printf ("\n");
		}
	}
}

void debug_print_section_disasm (char * section) {
	std::map <std::string, bfd_section *> :: iterator it;
	for (it = sections.begin (); it != sections.end (); it ++) {
		if ((*it).first == section) {			
			asection * s = bfd_get_section_by_name (bfdf, (const char *) section);
			printf ("Disassembling %s\n", s -> name);
			long size = bfd_section_size (bfdf, s);
			void * buf = malloc (s -> lma);
			if (bfd_get_section_contents (bfdf, s, buf, 0, size) == false) {
				bfd_perror (NULL);
				interface_error ("Could not read section contents");
				return;
			}
			struct disassemble_info info;
			init_disassemble_info (&info, stdout, (fprintf_ftype) fprintf);
			info.mach = bfd_get_mach (bfdf);
			info.endian = BFD_ENDIAN_LITTLE;
			info.buffer = (bfd_byte *) buf;
			info.buffer_length = s -> lma;
			long vma = s -> vma;
			while ((void *)info.buffer < buf + size) {
				printf ("\n");
				asymbol * ss = debug_get_symbol_from_address (vma);
				if (ss != NULL) {
					printf ("\n<%s>\n", ss -> name);
				}
				printf ("\t0x%lX:\t", vma);
				int b = disassemble_fn (0, &info);
				info.buffer += b;
				vma += b;
				if (b == 0) {
					info.buffer += 1;
					vma += 1;
				}
			}
			printf ("\n");
		}
	}
}

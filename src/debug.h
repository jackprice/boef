#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __linux__
#endif
#ifdef BSD
#endif

struct stack_frame {
	void * s_temps;
	void * s_params;
	void * s_return;
	void * s_fp;
	void * s_locals;
};

int debug_ptrace_traceme ();
void debug_init ();
void debug_cleanup ();
int debug_open (char * fn);
void debug_printinfo ();
void debug_print_function_disasm (char * function);
void debug_print_section_disasm (char * section);

#endif

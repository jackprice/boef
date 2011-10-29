#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __linux__
#endif
#ifdef BSD
#endif

int debug_ptrace_traceme ();
void debug_init ();
void debug_cleanup ();
int debug_open (char * fn);

#endif

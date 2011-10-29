#ifndef __HOST_H__
#define __HOST_H__

#ifndef HAVE_POSIX_OPENPT
	int posix_openpt (int flags);
#endif

int host_init ();
void host_cleanup ();
void host_exec (char * exec);
void host_printinfo ();
void host_attach (pid_t pid);
void host_kill ();
void host_run ();
void host_write (void * buffer, size_t len);
int host_read (void * buffer, size_t len);
void host_readline ();
void host_getregs ();
void host_dumpregs ();

#endif

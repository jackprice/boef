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

#endif

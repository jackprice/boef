#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#ifdef WITH_GTK

#endif

static void interface_exit ();
void interface_set_status (char * status);
void interface_message (char * err);
void interface_error (char * err);
void interface_init (int argc, char * argv []);
void interface_loop ();
void interface_printok (bool ok);
void interface_log (char * err);
void interface_log_warn (char * err);
void interface_log_error (char * err);
void interface_childpty_setup ();
void interface_pty_setup (int fd);
void interface_set_progress (double frac);
void interface_symbol_add (char * name, unsigned int address);

#endif

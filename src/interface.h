#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#ifdef WITH_GTK

#endif

void interface_set_status (char * status);
void interface_message (char * err);
void interface_error (char * err);
void interface_init (int argc, char * argv []);
void interface_loop ();
void interface_printok (bool ok);
void interface_log (char * err);
void interface_log_warn (char * err);
void interface_log_error (char * err);

#endif

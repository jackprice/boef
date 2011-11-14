#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__

union workspace_settings {
	const char * fn;
	const char * arguments;
};

extern workspace_settings settings;
int workspace_choose (char * ws);
void workspace_init ();
void workspace_cleanup ();
char * workspace_getname ();
void workspace_log (char * msg);

#endif

#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__

int workspace_choose (char * ws);
void workspace_init ();
void workspace_cleanup ();
char * workspace_getname ();
void workspace_log (char * msg);

#endif

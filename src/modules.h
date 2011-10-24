#ifndef __MODULES_H__
#define __MODULES_H__

struct module_args {
	int argc;
	char ** argv;
	void * additional;
};
typedef void * (*module_func)(module_args *);

char * module_geterror ();
int module_load (std::string name);
module_func module_getfunction (std::string name);
void module_cleanup ();
module_args * module_makeargs (int count, ...);
void module_cleanargs (module_args * args);
void module_printhelp (std::string name);
void module_listmodules ();

#endif

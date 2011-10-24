#ifndef __MODULE_H__
#define __MODULE_H__

struct module_args {
	int argc;
	char ** argv;
	void * additional;
};
typedef void * (*module_func)(module_args *);

#endif

#ifndef __MODULES_H__
#define __MODULES_H__

typedef void * (*module_func)(void *);

int module_load (std::string name);
module_func module_getfunction (std::string name);

#endif

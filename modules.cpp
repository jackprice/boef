#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <dlfcn.h>
#include <string.h>

#include "main.h"
#include "modules.h"

using namespace std;

map <string, void *> dl_handles;
map <string, module_func> dl_functions;

int module_load (string name) {
	void * dl_handle = dlopen (name.c_str (), RTLD_LAZY);
	if (!dl_handle) {
		return -1;
	}
	dl_handles [name] = dl_handle;
	return 0;
}

void * module_gethandle (string name) {
	if (dl_handles.count (name) > 0) {
		return dl_handles [name];
	}
	if (module_load (name) == 0) {
		return dl_handles [name];
	}
	return NULL;
}

module_func module_getfunction (std::string name) {
	if (dl_functions.count (name) > 0) {
		return dl_functions [name];
	}
	map <string, void *> :: iterator it;
	module_func func;
	for (it = dl_handles.begin (); it != dl_handles.end (); it ++) {
		func = (module_func) dlsym ((*it).second, name.c_str ());
		if (dlerror () == NULL) {
			dl_functions [name] = func;
			return func;
		}
	}
	return NULL;
}

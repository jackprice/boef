/******************************************************************************/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/******************************************************************************/
/** Copyright (C) 2011 Quetuo (http://www.quetuo.net)                        **/
/**                                                                          **/
/** This program is free software: you can redistribute it and/or modify     **/
/** it under the terms of the GNU General Public License as published by     **/
/** the Free Software Foundation, either version 3 of the License, or        **/
/** (at your option) any later version.
/**                                                                          **/
/** This program is distributed in the hope that it will be useful,          **/
/** but WITHOUT ANY WARRANTY; without even the implied warranty of           **/
/** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            **/
/** GNU General Public License for more details.                             **/
/**                                                                          **/
/** You should have received a copy of the GNU General Public License        **/
/** along with this program.  If not, see <http://www.gnu.org/licenses/>.    **/
/******************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <dlfcn.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <dirent.h>

#include "main.h"
#include "modules.h"
#include "host.h"

using namespace std;

map <string, void *> dl_handles;
map <string, module_func> dl_functions;

char * module_geterror () {
	return dlerror ();
}

/* module_load
 * Load a module into memory
 * Takes the path to the library (*.so.0.1) and returns 0 on success and -1
 * on failure
*/
int module_load (string name) {
	const char * cname = ("modules/" + name + ".so.1.0").c_str ();
	if (dl_handles.count (name) > 0) {
		return -1;
	}
	void * dl_handle = dlopen (cname, RTLD_LAZY);
	if (!dl_handle) {
		return -1;
	}
	dl_handles [name] = dl_handle;
	module_func func = (module_func) dlsym (dl_handle, "module_init");
	if (dlerror () == NULL) {
		module_args * args = module_makeargs (1, name.c_str ());
		func (args);
		module_cleanargs (args);
	}
	else {
		printf ("Failed to initialise: %s\n", dlerror ());
	}
	return 0;
}

/* module_gethandle
 * Return the handle to a loaded module, or loads a new module and returns the
 * handle to it
 * Returns a void pointer on success and NULL on failure
*/
void * module_gethandle (string name) {
	if (dl_handles.count (name) > 0) {
		return dl_handles [name];
	}
	if (module_load (name) == 0) {
		return dl_handles [name];
	}
	return NULL;
}

/* module_getfunction
 * Returns a pointer to a function in a loaded module
 * Function names must be unique to modules, all loaded modules are searched to
 * find the function
 * Function is of the prototype void * (*) (void *)
*/
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

/* module_cleanup ()
 * Calls cleanup functions for all loaded modules and unloads them all
*/

void module_cleanup () {
	map <string, void *> :: iterator it;
	module_func func;
	for (it = dl_handles.begin (); it != dl_handles.end (); it ++) {
		func = (module_func) dlsym ((*it).second, "module_cleanup");
		if (dlerror () == NULL) {
			func (NULL);
		}
		dlclose ((*it).second);
	}
	dl_handles.clear ();
	dl_functions.clear ();
}

/* module_makeargs () 
 * Returns a pointer to a module_args structure
*/

module_args * module_makeargs (int count, ...) {
	module_args * ret = new module_args;
	char * arg;
	va_list args;
	va_start (args, count);
	ret -> argc = count;
	ret -> argv = (char **) malloc (count);
	for (int i = 0; i < count; i ++) {
		arg = va_arg (args, char *);
		*(ret -> argv + i) = arg;
	}
	return ret;
}

void module_cleanargs (module_args * args) {
	if (args != NULL) {
		delete args;
	}
}

void module_printhelp (std::string name) {
	if (dl_handles.count (name) == 0) {
		printf ("Module %s not loaded\n", name.c_str ());
		return;
	}
	module_func func = (module_func) dlsym (dl_handles [name], "module_help");
	if (dlerror () == NULL) {
		func (NULL);
		return;
	}
	printf ("No help available for %s\n", name.c_str ());
	return;
}

void module_listmodules () {
	printf ("Loaded modules:\n");
	map <string, void *> :: iterator it;
	for (it = dl_handles.begin (); it != dl_handles.end (); it ++) {
		printf ("\t%s\n", it -> first.c_str ());
	}
	DIR * dp;
	struct dirent * dirp;
	if ((dp = opendir ("modules/")) == NULL) {
		printf ("Failed to open module directory!\n");
		return;
	}
	printf ("Available modules:\n");
	while ((dirp = readdir (dp)) != NULL) {
		if (strstr (dirp -> d_name, ".so.1.0") != NULL) {
			*(strstr (dirp -> d_name, ".so.1.0")) = 0x00;
			if (dl_handles.count (dirp -> d_name) == 0) {
				printf ("\t%s\n", dirp -> d_name);
			}
		}
	}
	closedir (dp);
	return;
}

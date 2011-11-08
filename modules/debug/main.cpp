#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string.h>

#include "../module.h"

extern "C" const char * module_name = "debug";
extern "C" const char * module_version = "0.0.1";
extern "C" const char * module_credits = "By Quetuo";
extern "C" const char * module_about = "This is a test module";

extern "C" void * module_init (module_args * args) {
	printf ("Module %s loaded\n", args -> argv [0]);
	return NULL;
}

extern "C" void * module_help (module_args * args) {
	printf ("Debug Module\n"
			"Written to test module functionality\n");
			return NULL;
}

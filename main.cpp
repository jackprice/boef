#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string.h>

#include "main.h"
#include "modules.h"

int main (int argc, char * argv []) {
	printf ("Loading...\n");
	if (module_load ("modules/debug.so.1.0") == 0) {
		printf ("Woo!\n");
	}
	module_func func;
	func = module_getfunction ("debug_init");
	if (func != NULL) {
		printf ("Woo!\n");
	}
	func (NULL);
	return 0;
}

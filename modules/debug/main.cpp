#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string.h>

extern "C" void * debug_init (void * ptr) {
	printf ("This is a test module!");
	return NULL;
}

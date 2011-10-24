/******************************************************************************/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/**                                                                          **/
/******************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <vector>
#include <sstream>

#include "main.h"
#include "modules.h"
#include "host.h"

bool running = true;

using namespace std;

void explode_string (string str, vector <string> * results) {
	stringstream data (str);
	string line;
	while (getline (data, line, ' ')) {
		results -> push_back (line);
	}
}

void printhelp () {
	printf ("Help\n");
}

int main (int argc, char * argv []) {
	char input [256];
	vector <string> args;
	while (running) {
		args.clear ();
		printf ("boef > ");
		fgets (input, 256, stdin);
		if (strlen (input) > 0) {
			input [strlen (input) - 1] = 0;
			explode_string (input, &args);
		
			if (args [0] == "quit") {
				printf ("Cleaning up...\n");
				module_cleanup ();
				exit (0);
			}
			else if (args [0] == "load") {
				if (args.size () < 3) {
					printf ("Invalid arguments\n");
				}
				else {
					if (args [1] == "module") {
						if (module_load (args [2]) == -1) {
							printf ("Failed to load %s!\n", args [2].c_str ());
						}
					}
				}
			}
			else if (args [0] == "list") {
				if (args.size () < 2) {
					printf ("Invalide arguments\n");
				}
				else {
					if (args [1] == "modules") {
						module_listmodules ();
					}
				}
			}
			else if (args [0] == "host") {
				if (args.size () < 3) {
					printf ("Invalid arguments\n");
				}
				else {
					if (args [1] == "exec") {
						
					}
				}
			}
			else if (args [0] == "help") {
				if (args.size () == 2) {
					module_printhelp (args [1]);
				}
				else {
					printhelp ();
				}
			}
			else {
				printf ("Unknown function %s\n", args [0].c_str ());
			}
		}
	}
	return 0;
}

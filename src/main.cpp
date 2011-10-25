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
#include <string.h>
#include <vector>
#include <sstream>

#include "config.h"
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
	printf ("%s Copyright (C) 2011 Quetuo\n"
			"URL: %s\n\n"
			"This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'.\n"
			"This is free software, and you are welcome to redistribute it under certain \n"
			"conditions; type 'show c' for details\n\n",
			PACKAGE_STRING, PACKAGE_URL);
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
					printf ("Invalid arguments\n");
				}
				else {
					if (args [1] == "modules") {
						module_listmodules ();
					}
				}
			}
			else if (args [0] == "host") {
				if (args.size () == 1) {
					host_printinfo ();
				}
				else if (args.size () > 2) {
					if (args [1] == "exec") {
						
					}
				}
				else {
					printf ("Invalid arguments\n");
				}
			}
			else if (args [0] == "help") {
				if (args.size () == 2) {
					if (args [1] == "version") {
						printf ("%s\n", PACKAGE_STRING);
					}
					else {
						module_printhelp (args [1]);
					}
				}
				else {
					printhelp ();
				}
			}
			else if (args [0] == "show") {
				if (args.size () == 2) {
					if (args [1] == "w") {
						printf ("This program is distributed in the hope that it will be useful, but WITHOUT ANY \n"
								"WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A \n"
								"PARTICULAR PURPOSE. See the GNU General Public License for more details.\n");
					}
					else if (args [1] == "c") {
						printf ("This program is free software: you can redistribute it and/or modify it under \n"
								"the terms of the GNU General Public License as published by the Free Software \n"
								"Foundation, either version 3 of the License, or (at your option) any later \n"
								"version.\n");
					}
				}
				else {
					printf ("Invalid arguments\n");
				}
			}
			else {
				printf ("Unknown function %s\n", args [0].c_str ());
			}
		}
	}
	return 0;
}

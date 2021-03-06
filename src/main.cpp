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
/** (at your option) any later version.                                      **/
/**                                                                          **/
/** This program is distributed in the hope that it will be useful,          **/
/** but WITHOUT ANY WARRANTY; without even the implied warranty of           **/
/** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            **/
/** GNU General Public License for more details.                             **/
/**                                                                          **/
/** You should have received a copy of the GNU General Public License        **/
/** along with this program.  If not, see <http://www.gnu.org/licenses/>.    **/
/******************************************************************************/

#include "include.h"

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

void printtrace () {
	#ifdef HAVE_EXECINFO_H
		printf ("\033[31mBacktrace:\n");
		void * array [10];
		size_t size;
		char ** strings;
		size_t i;
		size = backtrace (array, 20);
		strings = backtrace_symbols (array, size);
		for (i = 0; i < size; i ++) {
			printf ("    %s\n", strings [i]);
		}
		free (strings);
		printf ("\033[0m\n");
	#endif
}

void handler (int sig, siginfo_t * info, void * ptr) {
	#ifdef HAVE_SIGNAL_H
		//printf ("SIG %i\n", sig);
		if (sig == SIGINT || sig == SIGQUIT) {
			printf ("\n");
			module_cleanup ();
			//workspace_cleanup (); // Damn it, why won't you stop segfaulting on me!
			host_cleanup ();
			debug_cleanup ();
			exit (0);
		}
	#endif
}

int main (int argc, char * argv []) {
	printf ("\n"
			"\033[31m8     \033[0m \033[33m      \033[0m \033[32m      \033[0m \033[34m d'b\033[0m \n"
			"\033[31m8     \033[0m \033[33m      \033[0m \033[32m      \033[0m \033[34m 8  \033[0m \n"
			"\033[31m8oPYo.\033[0m \033[33m.oPYo.\033[0m \033[32m.oPYo.\033[0m \033[34mo8P \033[0m \n"
			"\033[31m8    8\033[0m \033[33m8    8\033[0m \033[32m8oooo8\033[0m \033[34m 8  \033[0m \n"
			"\033[31m8    8\033[0m \033[33m8    8\033[0m \033[32m8.    \033[0m \033[34m 8  \033[0m    \033[35mBy Quetuo\033[0m\n"
			"\033[31m`YooP'\033[0m \033[33m`YooP'\033[0m \033[32m`Yooo'\033[0m \033[34m 8  \033[0m \n\n"
			"\033[40m%s Copyright (C) 2011 Quetuo\033[0m\n"
			"URL: %s\n\n"
			"\033[0mThis program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'.\n"
			"This is free software, and you are welcome to redistribute it under certain \n"
			"conditions; type 'show c' for details\033[0m\n\n",
			PACKAGE_STRING, PACKAGE_URL);
	
	#ifdef HAVE_SIGNAL_H
		struct sigaction act;
		memset (&act, 0, sizeof (act));
		act.sa_sigaction = handler;
		act.sa_flags = SA_SIGINFO;
		sigaction (SIGINT, &act, NULL);
		sigaction (SIGABRT, &act, NULL);
		sigaction (SIGFPE, &act, NULL);
		sigaction (SIGILL, &act, NULL);
		sigaction (SIGTERM, &act, NULL);
		sigaction (SIGSEGV, &act, NULL);
	#endif
	
	interface_init (argc, argv);
	host_init ();
	workspace_init ();
	workspace_choose ("default");
	debug_init ();
		
	interface_set_status ("Idle");
	interface_loop ();
	
	char input [256];
	vector <string> args;
	while (running) {
		args.clear ();
		memset (input, 0, 256);
		printf ("boef > ");
		fgets (input, 256, stdin);
		if (strlen (input) > 0) {
			input [strlen (input) - 1] = 0;
			explode_string (input, &args);
		
			if (args [0] == "quit") {
				if (raise (2) != 0) {
					abort ();
				}
			}
			else if (args [0] == "load") {
				if (args.size () < 3) {
					printf ("Invalid arguments\n");
				}
				else {
					if (args [1] == "module") {
						module_load (args [2]);
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
				else if (args.size () == 2) {
					if (args [1] == "kill") {
						host_kill ();
					}
					else if (args [1] == "run") {
						host_run ();
					}
					else if (args [1] == "read") {
						host_readline ();
					}
					else {
						printf ("Invalid arguments\n");
					}
				}
				else if (args.size () > 2) {
					if (args [1] == "exec") {
						string exec = "";
						int i;
						for (i = 2; i < args.size (); i ++) {
							exec += args [i] + " ";
						}
						host_exec ((char *) exec.c_str ());
					}
					else if (args [1] == "attach" && args.size () == 3) {
						pid_t pid = atoi (args [2].c_str ());
						if (pid > 0) {
							host_attach (pid);
						}
						else {
							printf ("Invalid arguments\n");
						}
					}
					else {
						printf ("Invalid arguments\n");
					}
				}
				else {
					printf ("Invalid arguments\n");
				}
			}
			else if (args [0] == "workspace") {
				if (args.size () == 1) {
					printf ("Workspace %s selected\n", workspace_getname ());
				}
				else if (args.size () == 2) {
					if (workspace_choose ((char *) args [1].c_str ()) == -1) {
						printf ("Could not select workspace %s\n", args [1].c_str ());
					}
				}
				else {
					printf ("Invalid arguments\n");
				}
			}
			else if (args [0] == "fuzz") {
				if (args.size () == 3) {
					if (args [1] == "send") {
						int len = atoi (args [2].c_str ());
						if (len > 0) {
							host_write (fuzz_makestring (len, 'A'), len);
						}
						else {
							printf ("Invalid arguments\n");
						}
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
					else if (args [1] == "error") {
						printf ("Last error: \"%s\" (0x%.2X)\n", strerror (errno), errno);
					}
					else if (args [1] == "credits") {
						printf ("%s\n"
								"By Quetuo (http://www.quetuo.net)\n",
								PACKAGE_NAME);
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
						printf ("This program is distributed in the hope that it will be useful, but WITHOUT ANY\n"
								"WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n"
								"PARTICULAR PURPOSE. See the GNU General Public License for more details.\n");
					}
					else if (args [1] == "c") {
						printf ("This program is free software: you can redistribute it and/or modify it under\n"
								"the terms of the GNU General Public License as published by the Free Software\n"
								"Foundation, either version 3 of the License, or (at your option) any later\n"
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

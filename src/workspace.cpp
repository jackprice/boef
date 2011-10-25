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

#include "include.h"

char * workspace_home = NULL;
char * workspace_dir = NULL;

#ifdef WITH_SQLITE3
	sqlite3 * db;
	char * workspace_db = NULL;
#endif

void workspace_init () {
	// Get path to workspace (~/.boef)
	workspace_home = getpwuid (getuid ()) -> pw_dir;
	workspace_dir = (char *) malloc (strlen (workspace_home) + 7);
	sprintf (workspace_dir, "%s/.boef", workspace_home);
	printf ("Opening workspace %s...\n", workspace_dir);
	
	// Check whether it exists and create it if it doesn't
	struct stat buf;
	if (stat ((const char *) workspace_dir, &buf) == -1) {
		if (errno == ENOENT) {
			if (mkdir (workspace_dir, S_IRWXU | S_IRGRP | S_IROTH) == -1) {
				printf ("Could not mkdir(2) %s: %.2X\n", workspace_dir, errno);
				exit (1);
			}
		}
		else {
			printf ("Could not stat(2) %s: %.2X\n", workspace_dir, errno);
			exit (1);
		}
	}
	
	#ifdef WITH_SQLITE3
		// Open database
		workspace_db = (char *) malloc (strlen (workspace_dir) + 7);
		sprintf (workspace_db, "%s/db.sl3", workspace_dir);
		printf ("Opening SQLite database %s...\n", workspace_db);
		if (sqlite3_open (workspace_db, &db)) {
			printf ("Failed to open SQLite database\n");
			exit (1);
		}
		
		
		// Check tables
		sqlite3_stmt * result;
		const char * tail;
		if (sqlite3_prepare_v2 (db, "SELECT name FROM sqlite_master WHERE type='table';", 1000, &result, &tail) != SQLITE_OK) {
			printf ("Hmm");
		}
		bool workspace_table_workspace = false;
		while (sqlite3_step (result) == SQLITE_ROW) {
			char * row = sqlite_column_text (result, 0);
			if (strcmp (row, "workspace") == 0) {
				workspace_table_workspace = true;
			}
		}
		
		// Create tables that don't exist
		if (!workspace_table_workspace) {
			
		}
	#endif
}

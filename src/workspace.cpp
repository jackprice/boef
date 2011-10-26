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
char * workspace = "default";
int workspace_id = 0;

#ifdef WITH_SQLITE3
	sqlite3 * db;
	char * workspace_db = NULL;
#endif

int workspace_choose (char * ws) {
	#ifdef WITH_SQLITE3
		sqlite3_stmt * stmt;
		if (sqlite3_prepare (db, "SELECT id FROM workspace WHERE name=?;", -1, &stmt, 0) != SQLITE_OK) {
			return -1;
		}
		if (sqlite3_bind_text (stmt, 1, (const char *) ws, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
			return -1;
		}
		if (sqlite3_step (stmt) == SQLITE_DONE) {
			sqlite3_finalize (stmt);
			if (sqlite3_prepare (db, "INSERT INTO workspace (name) VALUES (?);", -1, &stmt, 0) != SQLITE_OK) {
				return -1;
			}
			if (sqlite3_bind_text (stmt, 1, (const char *) ws, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
				return -1;
			}
			sqlite3_step (stmt);
			sqlite3_finalize (stmt);
			workspace_choose (ws);
			workspace_log ("Workspace created");
			return 0;
		}
		workspace_id = sqlite3_column_int (stmt, 0);
		sqlite3_finalize (stmt);
		workspace = ws;
		printf ("Workspace %s (%i) selected\n", workspace, workspace_id);
		return 0;
	#else
		return 0;
	#endif
}

void workspace_init () {
	// Get path to workspace (~/.boef)
	workspace_home = getpwuid (getuid ()) -> pw_dir;
	workspace_dir = (char *) malloc (strlen (workspace_home) + 7);
	sprintf (workspace_dir, "%s/.boef", workspace_home);
	printf ("Opening workspace %s in %s...\n", workspace, workspace_dir);
	
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
		if (sqlite3_open (workspace_db, &db)) {
			printf ("Failed to open SQLite database\n");
			exit (1);
		}
		
		int error = sqlite3_exec (db, "CREATE TABLE IF NOT EXISTS workspace(id INTEGER PRIMARY KEY ASC, name TEXT);", 0, 0, 0)
			| sqlite3_exec (db, "CREATE TABLE IF NOT EXISTS log(workspace_id INTEGER, value TEXT);", 0, 0, 0);
		if (error != SQLITE_OK) {
			printf ("Failed to update database\n");
			exit (1);
		}
	#endif
}

char * workspace_getname () {
	return workspace;
}

void workspace_log (char * msg) {
	#ifdef WITH_SQLITE3
		sqlite3_stmt * stmt;
		if (sqlite3_prepare (db, "INSERT INTO log (workspace_id, value) VALUES (?, ?);", -1, &stmt, 0) != SQLITE_OK) {
			return;
		}
		if (sqlite3_bind_int (stmt, 1, workspace_id) != SQLITE_OK) {
			return;
		}
		if (sqlite3_bind_text (stmt, 2, (const char *) msg, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
			return;
		}
		sqlite3_step (stmt);
		sqlite3_finalize (stmt);
	#endif
}

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

char * workspace_home = NULL;
char * workspace_dir = NULL;
char * workspace = "default";
int workspace_id = 0;
extern workspace_settings settings;

#ifdef WITH_SQLITE3
	sqlite3 * db = NULL;
	char * workspace_db = NULL;
#else
	FILE * file = NULL;
	char * workspace_file = NULL;
#endif

int workspace_choose (char * ws) {
	char * buf = (char *) malloc (25 + strlen (ws));
	sprintf (buf, "Opening workspace %s...\n", ws);
	interface_log (buf);
	free (buf);
	#ifdef WITH_SQLITE3
		sqlite3_stmt * stmt;
		if (sqlite3_prepare (db, "SELECT id FROM workspace WHERE name=?;", -1, &stmt, 0) != SQLITE_OK) {
			interface_error ("Failed to SELECT\n");
			return -1;
		}
		if (sqlite3_bind_text (stmt, 1, (const char *) ws, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
			interface_error ("Failed to bind\n");
			return -1;
		}
		if (sqlite3_step (stmt) == SQLITE_DONE) {
			sqlite3_finalize (stmt);
			if (sqlite3_prepare (db, "INSERT INTO workspace (name) VALUES (?);", -1, &stmt, 0) != SQLITE_OK) {
				interface_error ("Failed to INSERT\n");
				return -1;
			}
			if (sqlite3_bind_text (stmt, 1, (const char *) ws, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
				interface_error ("Failed to bind\n");
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
		return 0;
	#else
		if (file) {
			fclose (file);
			file = NULL;
		}
		workspace_file = (char *) malloc (strlen (workspace_dir) + strlen (ws) + 6);
		sprintf (workspace_file, "%s/%s.wsp", workspace_dir, ws);
		file = fopen (workspace_file, "a");
		if (file == NULL) {
			char * buf = (char *) malloc (27 + strlen (workspace_file) + strlen (strerror (errno)));
			sprintf (buf, "Could not fopen(2) %s: %s\n", workspace_file, strerror (errno));
			interface_error (buf);
			free (buf);
			return -1;
		}
		workspace = ws;
		return 0;
	#endif
}

void workspace_init () {
	// Get path to workspace (~/.boef)
	workspace_home = getpwuid (getuid ()) -> pw_dir;
	workspace_dir = (char *) malloc (strlen (workspace_home) + 7);
	sprintf (workspace_dir, "%s/.boef", workspace_home);
	char * buf1 = (char *) malloc (28 + strlen (workspace_dir));
	sprintf (buf1, "Opening workspace in %s...\n", workspace_dir);
	interface_log (buf1);
	free (buf1);
	
	// Check whether it exists and create it if it doesn't
	struct stat buf;
	if (stat ((const char *) workspace_dir, &buf) == -1) {
		if (errno == ENOENT) {
			if (mkdir (workspace_dir, S_IRWXU | S_IRGRP | S_IROTH) == -1) {
				char * buf = (char *) malloc (27 + strlen (workspace_dir) + strlen (strerror (errno)));
				sprintf (buf, "Could not mkdir(2) %s: %s\n", workspace_dir, strerror (errno));
				interface_error (buf);
				free (buf);
				exit (1);
			}
		}
		else {
			char * buf = (char *) malloc (27 + strlen (workspace_dir) + strlen (strerror (errno)));
			sprintf (buf, "Could not stat(2) %s: %s\n", workspace_dir, strerror (errno));
			interface_error (buf);
			free (buf);
			exit (1);
		}
	}
	
	#ifdef WITH_SQLITE3
		// Open database
		workspace_db = (char *) malloc (strlen (workspace_dir) + 7);
		sprintf (workspace_db, "%s/db.sl3", workspace_dir);
		if (sqlite3_open (workspace_db, &db)) {
			interface_error ("Failed to open SQLite database\n");
			exit (1);
		}
		
		int error = sqlite3_exec (db, "CREATE TABLE IF NOT EXISTS workspace(id INTEGER PRIMARY KEY ASC, name TEXT);", 0, 0, 0)
			| sqlite3_exec (db, "CREATE TABLE IF NOT EXISTS log(workspace_id INTEGER, value TEXT);", 0, 0, 0)
			| sqlite3_exec (db, "CREATE TABLE IF NOT EXISTS option(workspace_id INTEGER, name TEXT, value TEXT);", 0, 0, 0)
			| sqlite3_exec (db, "DELETE FROM option WHERE workspace_id=1;", 0, 0, 0);
		if (error != SQLITE_OK) {
			interface_error ("Failed to update database\n");
			exit (1);
		}
	#endif
}

void workspace_cleanup () {
	interface_log ("Cleaning up workspace...");
	#ifdef WITH_SQLITE3
		if (db != NULL) {
			sqlite3_close (db);
			db = NULL;
		}
	#else
		if (file != NULL) {
			fclose (file);
			file = NULL;
		}
	#endif
}

char * workspace_getname () {
	return workspace;
}

char * workspace_getoption (char * option) {
	#ifdef WITH_SQLITE3
		sqlite3_stmt * stmt;
		if (sqlite3_prepare (db, "SELECT value FROM option WHERE name=? AND workspace_id=?;", -1, &stmt, 0) != SQLITE_OK) {
			interface_error ("Failed to SELECT\n");
			return NULL;
		}
		if (sqlite3_bind_text (stmt, 1, (const char *) option, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
			interface_error ("Failed to bind\n");
			return NULL;
		}
		if (sqlite3_bind_int (stmt, 2, workspace_id) != SQLITE_OK) {
			interface_error ("Failed to bind\n");
			return NULL;
		}
		if (sqlite3_step (stmt) == SQLITE_DONE) {
			return NULL;
		}
		char * ret = (char *) sqlite3_column_text (stmt, 0);
		sqlite3_finalize (stmt);
		return ret;
	#endif
}

int workspace_setoption (char * option, char * value) {
	#ifdef WITH_SQLITE3
		sqlite3_stmt * stmt;
		if (workspace_getoption (option) == NULL) {
			if (sqlite3_prepare (db, "INSERT INTO option (workspace_id, name,value) VALUES (?,?,?);", -1, &stmt, 0) != SQLITE_OK) {
				interface_error ("Failed to INSERT\n");
				return -1;
			}
			if (sqlite3_bind_int (stmt, 1, workspace_id) != SQLITE_OK) {
				interface_error ("Failed to bind\n");
				return NULL;
			}
			if (sqlite3_bind_text (stmt, 2, (const char *) option, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
				interface_error ("Failed to bind\n");
				return -1;
			}
			if (sqlite3_bind_text (stmt, 3, (const char *) value, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
				interface_error ("Failed to bind\n");
				return -1;
			}
			sqlite3_step (stmt);
			sqlite3_finalize (stmt);
			return 0;
		}
		else {
			if (sqlite3_prepare (db, "UPDATE option set value=? WHERE name=? AND workspace_id=?;", -1, &stmt, 0) != SQLITE_OK) {
				interface_error ("Failed to INSERT\n");
				return -1;
			}
			if (sqlite3_bind_text (stmt, 1, (const char *) value, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
				interface_error ("Failed to bind\n");
				return -1;
			}
			if (sqlite3_bind_text (stmt, 2, (const char *) option, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
				interface_error ("Failed to bind\n");
				return -1;
			}
			if (sqlite3_bind_int (stmt, 3, workspace_id) != SQLITE_OK) {
				interface_error ("Failed to bind\n");
				return NULL;
			}
			sqlite3_step (stmt);
			sqlite3_finalize (stmt);
			return 0;
		}
	#endif
}

void workspace_log (char * msg) {
	char * _msg = (char *) malloc (strlen (msg) + 23);
	sprintf (_msg + 22, "%s", msg);
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (_msg, 22, "[%d/%m/%Y %H:%M:%S] ", timeinfo);
	#ifdef WITH_SQLITE3
		sqlite3_stmt * stmt;
		if (sqlite3_prepare (db, "INSERT INTO log (workspace_id, value) VALUES (?, ?);", -1, &stmt, 0) != SQLITE_OK) {
			free (_msg);
			return;
		}
		if (sqlite3_bind_int (stmt, 1, workspace_id) != SQLITE_OK) {
			free (_msg);
			return;
		}
		if (sqlite3_bind_text (stmt, 2, (const char *) _msg, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
			free (_msg);
			return;
		}
		sqlite3_step (stmt);
		sqlite3_finalize (stmt);
		free (_msg);
		return;
	#else
		if (file != NULL) {
			fprintf (file, "%s\n", _msg);
			fflush (file);
		}
		free (_msg);
		return;
	#endif
}

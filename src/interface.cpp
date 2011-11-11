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

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "include.h"
#ifdef WITH_VTE
	#include <vte/vte.h>
#endif

char * authors [] = {"Quetuo <quetuo@quetuo.net>", NULL};

GtkWidget * window;
// Status bar
GtkWidget * windowstatus;
GtkWidget * windowstatusprogress;
// Toolbar
GtkWidget * windowtoolbar;
GtkToolItem * windowtoolbarnew;
GtkToolItem * windowtoolbaropen;
GtkToolItem * windowtoolbarrun;
GtkToolItem * windowtoolbarstop;
GtkToolItem * windowtoolbarrestart;
GtkWidget * windowvpaned;
// Sidebar
GtkWidget * windowvpanedhpaned;
GtkWidget * windowvpanedhpanednotebook;
GtkListStore * windowvpanedhpanednotebookliststore1;
GtkWidget * windowvpanedhpanednotebooktreeview1;
GtkListStore * windowvpanedhpanednotebookliststore2;
GtkWidget * windowvpanedhpanednotebooklabel1;
GtkWidget * windowvpanedhpanednotebooklabel2;
// Main
GtkTextBuffer * windowcodebuffer;
GtkWidget * windowvpanedhpanedtextview;
GtkWidget * windowvpanednotebook;
GtkWidget * windowvpanednotebooklabel1;
GtkWidget * windowvpanedhpanednotebookscrolledwindow1;
GtkWidget * windowvpanedhpanednotebookscrolledwindow2;
#ifdef WITH_VTE
	GtkWidget * windowvpanednotebookvte;
	VtePty * windowvpanednotebookpty;
#else
	GtkTextBuffer * windowvpanednotebookvtebuffer;
	GtkWidget * windowvpanednotebookvte;
#endif
GtkWidget * windowvpanednotebooklabel2;
GtkWidget * windowvpanednotebooktextview;
GtkTextBuffer * windowtextbuffer;
GtkTextIter windowtextiter;
GtkWidget * windowmenu;
GtkWidget * windowmenufile;
GtkWidget * windowmenuworkspace;
GtkWidget * windowmenuhelp;
GtkWidget * windowmenuhelpabout;

GtkWidget * workspacewindow;

GtkWidget * aboutwindow;
//GdkPixBuf * logo;

static void interface_exit () {
	gtk_main_quit ();
	if (raise (2) != 0) {
		abort ();
	}
}

static gboolean delete_event (GtkWidget * widget, GdkEvent * event, gpointer data) {
	if (widget == window) {
		//return false; // TODO: remove
		GtkWidget * dialog = gtk_message_dialog_new (GTK_WINDOW (window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Are you sure to quit?");
		gtk_window_set_title(GTK_WINDOW(dialog), "Question");
		if (gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
			gtk_widget_destroy (dialog);
			return false;
		}
		gtk_widget_destroy (dialog);
		return true;
	}
	gtk_widget_hide (widget);
	return true;
}

static void destroy (GtkWidget * widget, GdkEvent * event) {
	interface_exit ();
}

static gboolean windownotepad_focus (GtkNotebook * notebook, gpointer arg1, guint arg2, gpointer data) {
	if (arg2 == 1) {
		gtk_label_set_text (GTK_LABEL (windowvpanednotebooklabel2), (const gchar *) "Log");
	}
	return false;
}

void interface_new (GtkToolButton * button, gpointer user_data) {
	gtk_widget_set_visible (windowvpaned, false);
	gtk_widget_set_sensitive (GTK_WIDGET (windowtoolbaropen), true);
	gtk_widget_set_sensitive (GTK_WIDGET (windowtoolbarrun), false);
}

void interface_open (GtkToolButton * button, gpointer user_data) {
	GtkWidget * dialog = gtk_file_chooser_dialog_new ("Open File",
	                                                  GTK_WINDOW (window),
	                                                  GTK_FILE_CHOOSER_ACTION_OPEN,
	                                                  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                                                  GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
	                                                  NULL);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char * filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		gtk_widget_set_visible (windowvpaned, true);
		gtk_widget_set_sensitive (GTK_WIDGET (windowtoolbaropen), false);
		gtk_widget_set_sensitive (GTK_WIDGET (windowtoolbarrun), true);
		if (debug_open (filename) == -1) {
			interface_error ("Could not load file\n");
		}
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}

void interface_run (GtkToolButton * button, gpointer user_data) {
}

void interface_symbol_add (char * name, unsigned int address) {
	char buf [20];
	sprintf (buf, "0x%0.8X", address);
	GtkTreeIter iter;
	gtk_list_store_append (windowvpanedhpanednotebookliststore1, &iter);
	gtk_list_store_set (windowvpanedhpanednotebookliststore1, &iter,
	                    0, name,
	                    1, buf,
	                    -1);
}

void interface_set_status (char * status) {
	gtk_statusbar_pop (GTK_STATUSBAR (windowstatus), gtk_statusbar_get_context_id (GTK_STATUSBAR (windowstatus), "status"));
	gtk_statusbar_push (GTK_STATUSBAR (windowstatus), gtk_statusbar_get_context_id (GTK_STATUSBAR (windowstatus), "status"), status);
}

void interface_set_progress (double frac) {
	if (frac < 0) {
		//gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (windowstatusprogress), 1.0);
		gtk_progress_bar_pulse (GTK_PROGRESS_BAR (windowstatusprogress));
	}
	else {
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (windowstatusprogress), frac);
	}
}

void interface_log (char * err) {
	if (gtk_notebook_get_current_page (GTK_NOTEBOOK (windowvpanednotebook)) != 1) {
		gtk_label_set_markup (GTK_LABEL (windowvpanednotebooklabel2), (const gchar *) "<b>Log</b>");
	}
	gtk_text_buffer_insert (GTK_TEXT_BUFFER (windowtextbuffer), &windowtextiter, (const gchar *) err, -1);
	return;
}

void interface_log_warn (char * err) {
	if (gtk_notebook_get_current_page (GTK_NOTEBOOK (windowvpanednotebook)) != 1) {
		gtk_label_set_markup (GTK_LABEL (windowvpanednotebooklabel2), (const gchar *) "<b><span foreground=\"orange\">Log</span></b>");
	}
	gtk_text_buffer_insert_with_tags_by_name (GTK_TEXT_BUFFER (windowtextbuffer), &windowtextiter, (const gchar *) err, -1, (const gchar *) "orangefg", NULL);
	return;
}

void interface_log_error (char * err) {
	if (gtk_notebook_get_current_page (GTK_NOTEBOOK (windowvpanednotebook)) != 1) {
		gtk_label_set_markup (GTK_LABEL (windowvpanednotebooklabel2), (const gchar *) "<b><span foreground=\"red\">Log</span></b>");
	}
	gtk_text_buffer_insert_with_tags_by_name (GTK_TEXT_BUFFER (windowtextbuffer), &windowtextiter, (const gchar *) err, -1, (const gchar *) "redfg", NULL);
	return;
}

void interface_message (char * err) {
	interface_log (err);
	printf ("%s\n", err);
	GtkWidget * dialog;
	dialog = gtk_message_dialog_new (GTK_WINDOW (window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, err);
	gtk_window_set_title (GTK_WINDOW (dialog), "Information");
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

void interface_error (char * err) {
	interface_log_error (err);
	printf ("Error: %s\n", err);
	GtkWidget * dialog;
	dialog = gtk_message_dialog_new (GTK_WINDOW (window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, err);
	gtk_window_set_title (GTK_WINDOW (dialog), "Error");
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

void interface_about () {
	gtk_widget_show (aboutwindow);
	gtk_dialog_run (GTK_DIALOG (aboutwindow));
	gtk_widget_hide (aboutwindow);
}

void interface_workspace_chooser () {
	
}

void interface_init (int argc, char * argv []) {
	gtk_init (&argc, &argv);
	
	// Main window
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_signal_connect (window, "delete-event", G_CALLBACK (delete_event), NULL);
	g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);
	gtk_widget_show (window);
	
	GtkWidget * vbox = gtk_vbox_new (false, 0);
	gtk_container_add (GTK_CONTAINER (window), vbox);
	gtk_widget_show (vbox);
	
	windowmenu = gtk_menu_bar_new ();
	gtk_widget_show (windowmenu);
	gtk_box_pack_start (GTK_BOX (vbox), windowmenu, FALSE, TRUE, 0);
	windowmenufile =  gtk_menu_item_new_with_mnemonic ("_File");
	gtk_menu_bar_append (GTK_MENU_BAR (windowmenu), windowmenufile);
	gtk_widget_show (windowmenufile);
	windowmenuworkspace =  gtk_menu_item_new_with_mnemonic ("_Workspace");
	gtk_menu_bar_append (GTK_MENU_BAR (windowmenu), windowmenuworkspace);
	gtk_widget_show (windowmenuworkspace);
	windowmenuhelp =  gtk_menu_item_new_with_mnemonic ("_Help");
	gtk_menu_bar_append (GTK_MENU_BAR (windowmenu), windowmenuhelp);
	gtk_widget_show (windowmenuhelp);
	windowmenuhelpabout = gtk_menu_item_new_with_mnemonic ("_About");
	//gtk_menu_append (GTK_MENU_ITEM (windowmenuhelp), windowmenuhelpabout);
	//gtk_widget_show (windowmenuhelpabout);

	windowtoolbar = gtk_toolbar_new ();
	windowtoolbarnew = gtk_tool_button_new_from_stock ("gtk-new");
	g_signal_connect (windowtoolbarnew, "clicked", G_CALLBACK (interface_new), NULL);
	gtk_toolbar_insert (GTK_TOOLBAR (windowtoolbar), windowtoolbarnew, -1);
	windowtoolbaropen = gtk_tool_button_new_from_stock ("gtk-open");
	g_signal_connect (windowtoolbaropen, "clicked", G_CALLBACK (interface_open), NULL);
	gtk_toolbar_insert (GTK_TOOLBAR (windowtoolbar), windowtoolbaropen, -1);
	windowtoolbarrun = gtk_tool_button_new_from_stock ("gtk-execute");
	g_signal_connect (windowtoolbarrun, "clicked", G_CALLBACK (interface_run), NULL);
	gtk_toolbar_insert (GTK_TOOLBAR (windowtoolbar), windowtoolbarrun, -1);
	gtk_widget_show (windowtoolbar);
	gtk_widget_show (GTK_WIDGET (windowtoolbaropen));
	gtk_widget_show (GTK_WIDGET (windowtoolbarnew));
	gtk_widget_show (GTK_WIDGET (windowtoolbarrun));
	gtk_box_pack_start (GTK_BOX (vbox), windowtoolbar, FALSE, TRUE, 0);
	
	windowvpaned = gtk_vpaned_new ();
	gtk_box_pack_start (GTK_BOX (vbox), windowvpaned, TRUE, TRUE, 0);
	//gtk_widget_show (windowvpaned);
	
	windowvpanedhpaned = gtk_hpaned_new ();
	gtk_paned_add1 (GTK_PANED (windowvpaned), windowvpanedhpaned);
	gtk_widget_show (windowvpanedhpaned);
	
	windowvpanedhpanednotebook = gtk_notebook_new ();
	gtk_paned_add1 (GTK_PANED (windowvpanedhpaned), windowvpanedhpanednotebook);
	gtk_widget_show (windowvpanedhpanednotebook);
	
	windowvpanedhpanednotebooklabel1 = gtk_label_new ((const gchar *) "Functions");
	windowvpanedhpanednotebookliststore1 = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
	windowvpanedhpanednotebooktreeview1 = gtk_tree_view_new_with_model (GTK_TREE_MODEL (windowvpanedhpanednotebookliststore1));
	windowvpanedhpanednotebookscrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_add (GTK_CONTAINER (windowvpanedhpanednotebookscrolledwindow1), windowvpanedhpanednotebooktreeview1);
	gtk_notebook_append_page (GTK_NOTEBOOK (windowvpanedhpanednotebook), windowvpanedhpanednotebookscrolledwindow1, windowvpanedhpanednotebooklabel1);
	gtk_widget_show (windowvpanedhpanednotebooklabel1);
	gtk_widget_show (windowvpanedhpanednotebookscrolledwindow1);
	gtk_widget_show (windowvpanedhpanednotebooktreeview1);
	GtkTreeIter iter;
	/*gtk_list_store_append (windowvpanedhpanednotebookliststore1, &iter);
	gtk_list_store_set (windowvpanedhpanednotebookliststore1, &iter,
	                    0, "Hello, world",
	                    1, 1231312,
	                    -1);*/
	GtkTreeViewColumn * col = gtk_tree_view_column_new ();
	gtk_tree_view_column_set_title (col, "Name");
	GtkCellRenderer * cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (GTK_TREE_VIEW_COLUMN (col), GTK_CELL_RENDERER (cell), true);	
	gtk_tree_view_append_column (GTK_TREE_VIEW (windowvpanedhpanednotebooktreeview1), col);
	gtk_tree_view_column_add_attribute (GTK_TREE_VIEW_COLUMN (col), GTK_CELL_RENDERER (cell), "text", 0);
	col = gtk_tree_view_column_new ();
	gtk_tree_view_column_set_title (col, "Address");
	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (GTK_TREE_VIEW_COLUMN (col), GTK_CELL_RENDERER (cell), true);
	gtk_tree_view_append_column (GTK_TREE_VIEW (windowvpanedhpanednotebooktreeview1), col);
	gtk_tree_view_column_add_attribute (GTK_TREE_VIEW_COLUMN (col), GTK_CELL_RENDERER (cell), "text", 1);

	windowvpanedhpanednotebooklabel2 = gtk_label_new ((const gchar *) "Sections");
	windowvpanedhpanednotebookscrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
	gtk_notebook_append_page (GTK_NOTEBOOK (windowvpanedhpanednotebook), windowvpanedhpanednotebookscrolledwindow2, windowvpanedhpanednotebooklabel2);
	gtk_widget_show (windowvpanedhpanednotebooklabel2);
	gtk_widget_show (windowvpanedhpanednotebookscrolledwindow2);

	windowcodebuffer = gtk_text_buffer_new (NULL);
	windowvpanedhpanedtextview = gtk_text_view_new_with_buffer (windowcodebuffer);
	gtk_paned_add2 (GTK_PANED (windowvpanedhpaned), windowvpanedhpanedtextview);
	gtk_widget_show (windowvpanedhpanedtextview);
	
	windowvpanednotebook = gtk_notebook_new ();
	gtk_paned_add2 (GTK_PANED (windowvpaned), windowvpanednotebook);
	gtk_widget_show (windowvpanednotebook);
	g_signal_connect (windowvpanednotebook, "switch-page", G_CALLBACK (windownotepad_focus), NULL);
	
	#ifdef WITH_VTE
		windowvpanednotebookvte = vte_terminal_new ();
		
	#else
		windowvpanednotebookvtebuffer = gtk_text_buffer_new (NULL);
		windowvpanednotebookvte = gtk_text_view_new_with_buffer (GTK_TEXT_BUFFER (windowvpanednotebookvtebuffer));
	#endif
	windowvpanednotebooklabel1 = gtk_label_new ((const gchar *) "Terminal");
	gtk_notebook_append_page (GTK_NOTEBOOK (windowvpanednotebook), windowvpanednotebookvte, windowvpanednotebooklabel1);
	gtk_widget_show (windowvpanednotebookvte);
	gtk_widget_show (windowvpanednotebooklabel1);
	
	windowtextbuffer = gtk_text_buffer_new (NULL);
	gtk_text_buffer_create_tag (windowtextbuffer, "redfg", "foreground", "red", NULL);
	gtk_text_buffer_create_tag (windowtextbuffer, "orangefg", "foreground", "orange", NULL);
	gtk_text_buffer_get_iter_at_offset (windowtextbuffer, &windowtextiter, 0);
	windowvpanednotebooklabel2 = gtk_label_new ((const gchar *) "Log");
	windowvpanednotebooktextview = gtk_text_view_new_with_buffer (GTK_TEXT_BUFFER (windowtextbuffer));
	gtk_text_view_set_editable (GTK_TEXT_VIEW (windowvpanednotebooktextview), FALSE);
	gtk_notebook_append_page (GTK_NOTEBOOK (windowvpanednotebook), windowvpanednotebooktextview, windowvpanednotebooklabel2);
	gtk_widget_show (windowvpanednotebooktextview);
	gtk_widget_show (windowvpanednotebooklabel2);
	
	windowstatus = gtk_statusbar_new ();
	gtk_widget_show (windowstatus);
	gtk_box_pack_end (GTK_BOX (vbox), windowstatus, FALSE, TRUE, 0);
	gtk_statusbar_push (GTK_STATUSBAR (windowstatus), gtk_statusbar_get_context_id (GTK_STATUSBAR (windowstatus), "status"), "");
	windowstatusprogress = gtk_progress_bar_new ();
	gtk_widget_show (windowstatusprogress);
	gtk_box_pack_start (GTK_BOX (windowstatus), windowstatusprogress, FALSE, TRUE, 0);
	
	// About window
	aboutwindow = gtk_about_dialog_new ();
	gtk_about_dialog_set_name (GTK_ABOUT_DIALOG (aboutwindow), PACKAGE_NAME);
	gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (aboutwindow), VERSION);
	gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (aboutwindow), COPYRIGHT);
	gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (aboutwindow), LICENSE);
	gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (aboutwindow), PACKAGE_URL);
	gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (aboutwindow), (const gchar **) authors);
	gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (aboutwindow), gtk_image_get_pixbuf (GTK_IMAGE (gtk_image_new_from_file ("boef.png"))));
	
	// Workspace chooser
	
	
	interface_workspace_chooser ();
}

void interface_pty_setup (int fd) {
	windowvpanednotebookpty = vte_pty_new_foreign (fd, NULL);
	vte_terminal_set_pty_object (VTE_TERMINAL (windowvpanednotebookvte), windowvpanednotebookpty);
}

void interface_childpty_setup () {
	vte_pty_child_setup (windowvpanednotebookpty);
}

void interface_loop () {
	gtk_main ();
}

void interface_printok (bool ok) {
	if (ok) {
		printf ("\033[73G[  OK  ]\n");
	}
	else {
		printf ("\033[73G[ \033[31mFAIL\033[0m ]\n");
	}
}

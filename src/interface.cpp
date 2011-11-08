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
GtkWidget * windowstatus;
GtkWidget * windowvpaned;
GtkWidget * windowvpanednotebook;
GtkWidget * windowvpanednotebooklabel1;
#ifdef WITH_VTE
	GtkWidget * windowvpanednotebookvte;
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

GtkWidget * aboutwindow;
//GdkPixBuf * logo;

static gboolean delete_event (GtkWidget * widget, GdkEvent * event, gpointer data) {
	if (widget == window) {
		return false; // TODO: remove
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
	gtk_main_quit ();
	if (raise (2) != 0) {
		abort ();
	}
}

static gboolean windownotepad_focus (GtkNotebook * notebook, gpointer arg1, guint arg2, gpointer data) {
	if (arg2 == 1) {
		gtk_label_set_text (GTK_LABEL (windowvpanednotebooklabel2), (const gchar *) "Log");
	}
	return false;
}

void interface_set_status (char * status) {
	gtk_statusbar_pop (GTK_STATUSBAR (windowstatus), gtk_statusbar_get_context_id (GTK_STATUSBAR (windowstatus), "status"));
	gtk_statusbar_push (GTK_STATUSBAR (windowstatus), gtk_statusbar_get_context_id (GTK_STATUSBAR (windowstatus), "status"), status);
}

void interface_log (char * err) {
	gtk_text_buffer_insert (GTK_TEXT_BUFFER (windowtextbuffer), &windowtextiter, (const gchar *) err, -1);
	return;
}

void interface_log_warn (char * err) {
	if (gtk_notebook_get_current_page (GTK_NOTEBOOK (windowvpanednotebook)) != 1) {
		gtk_label_set_markup (GTK_LABEL (windowvpanednotebooklabel2), (const gchar *) "<span foreground=\"orange\">Log</span>");
	}
	gtk_text_buffer_insert_with_tags_by_name (GTK_TEXT_BUFFER (windowtextbuffer), &windowtextiter, (const gchar *) err, -1, (const gchar *) "orangefg", NULL);
	return;
}

void interface_log_error (char * err) {
	if (gtk_notebook_get_current_page (GTK_NOTEBOOK (windowvpanednotebook)) != 1) {
		gtk_label_set_markup (GTK_LABEL (windowvpanednotebooklabel2), (const gchar *) "<span foreground=\"red\">Log</span>");
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
	
	windowvpaned = gtk_vpaned_new ();
	gtk_box_pack_start (GTK_BOX (vbox), windowvpaned, TRUE, TRUE, 0);
	gtk_widget_show (windowvpaned);
	
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
	
	// About window
	aboutwindow = gtk_about_dialog_new ();
	gtk_about_dialog_set_name (GTK_ABOUT_DIALOG (aboutwindow), PACKAGE_NAME);
	gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (aboutwindow), VERSION);
	gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (aboutwindow), COPYRIGHT);
	gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (aboutwindow), LICENSE);
	gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (aboutwindow), PACKAGE_URL);
	gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (aboutwindow), (const gchar **) authors);
	gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (aboutwindow), gtk_image_get_pixbuf (GTK_IMAGE (gtk_image_new_from_file ("boef.png"))));
	
	interface_log ("GUI started\n");
	interface_log_error ("Erm\n");
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

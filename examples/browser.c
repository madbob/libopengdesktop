/*  libopengdesktop
 *  Copyright (C) 2009/2012 Roberto -MadBob- Guido <bob4job@gmail.com>
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ogd.h>
#include <gtk/gtk.h>

GtkWidget *mainStatus;

static gboolean delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    return FALSE;
}

static void add_category (OGDObject *obj, gpointer cat)
{
    GtkWidget *categories;
    GtkListStore *store;
    GtkTreeIter iter;
    OGDCategory *fetched;

    if (obj == NULL) {
        gtk_statusbar_pop (GTK_STATUSBAR (mainStatus), 1);
        return;
    }

    categories = (GtkWidget*) cat;
    fetched = OGD_CATEGORY (obj);

    store = GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX (categories)));
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 0, ogd_category_get_name (fetched), 1, fetched, -1);
}

static void clear_container (GtkWidget *container)
{
    GList *children;
    GList *iter;
    GtkWidget *child;

    children = gtk_container_get_children (GTK_CONTAINER (container));

    for (iter = g_list_first (children); iter; iter = g_list_next (iter)) {
        child = (GtkWidget*) iter->data;
        gtk_container_remove (GTK_CONTAINER (container), child);
        g_object_unref (child);
    }

    g_list_free (children);
}

static void fit_contents_list (OGDObject *obj, gpointer container)
{
    GtkWidget *contents;
    GtkWidget *link;
    OGDContent *con;

    contents = (GtkWidget*) container;

    if (obj == NULL) {
        gtk_statusbar_pop (GTK_STATUSBAR (mainStatus), 2);
        return;
    }

    con = OGD_CONTENT (obj);

    link = gtk_link_button_new_with_label (ogd_content_get_homepage (con), ogd_content_get_name (con));
    gtk_box_pack_end_defaults (GTK_BOX (contents), link);
    gtk_widget_show (link);
}

static void fetch_contents_by_category (GtkComboBox *widget, gpointer to_fill)
{
    GtkWidget *contents;
    GtkTreeIter iter;
    OGDCategory *category;
    OGDIterator *iterator;

    contents = (GtkWidget*) to_fill;
    clear_container (contents);

    if (gtk_combo_box_get_active_iter (widget, &iter))
        gtk_tree_model_get (gtk_combo_box_get_model (widget), &iter, 1, &category, -1);
    else
        return;

    iterator = ogd_category_get_contents (category, OGD_CATEGORY_SORT_NEWEST);
    ogd_iterator_fetch_async (iterator, fit_contents_list, contents);

    gtk_statusbar_push (GTK_STATUSBAR (mainStatus), 2, "Fetching contents...");
}

static GtkWidget* main_window (OGDProvider *provider)
{
    GtkWidget *window;
    GtkWidget *container;
    GtkWidget *categories;
    GtkWidget *contents;
    GtkWidget *scrollable;
    GtkWidget *viewport;
    GtkListStore *store;
    GtkCellRenderer *rend;

    mainStatus = gtk_statusbar_new ();
    gtk_statusbar_push (GTK_STATUSBAR (mainStatus), 1, "Fetching categories...");

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (delete_event), NULL);
    gtk_window_set_default_size (GTK_WINDOW (window), 300, 300);

    container = gtk_vbox_new (FALSE, 5);
    gtk_container_add (GTK_CONTAINER (window), container);

    contents = gtk_vbox_new (TRUE, 2);
    store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_OBJECT);
    categories = gtk_combo_box_new_with_model (GTK_TREE_MODEL (store));
    rend = gtk_cell_renderer_text_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (categories), rend, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (categories), rend, "text", 0);
    g_signal_connect (G_OBJECT (categories), "changed", G_CALLBACK (fetch_contents_by_category), contents);
    ogd_category_fetch_all_async (provider, add_category, categories);
    gtk_container_add (GTK_CONTAINER (container), categories);

    scrollable = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollable), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request (scrollable, 300, 250);
    gtk_container_add (GTK_CONTAINER (container), scrollable);
    viewport = gtk_viewport_new (NULL, NULL);
    gtk_viewport_set_shadow_type (GTK_VIEWPORT (viewport), GTK_SHADOW_NONE);
    gtk_container_add (GTK_CONTAINER (scrollable), viewport);
    gtk_container_add (GTK_CONTAINER (viewport), contents);

    gtk_container_add (GTK_CONTAINER (container), mainStatus);

    return window;
}

int main (int argc, char **argv)
{
    register int i;
    gchar *username;
    gchar *password;
    GtkWidget *window;
    OGDProvider *provider;

    username = NULL;
    password = NULL;

    g_thread_init (NULL);
    gtk_init (&argc, &argv);

    for (i = 0; i < argc; i++) {
        if (strcmp (argv[i], "-u") == 0)
            username = g_strdup (argv[++i]);
        else if (strcmp (argv[i], "-p") == 0)
            password = g_strdup (argv[++i]);
    }

    if (username == NULL || password == NULL) {
        printf ("Usage: %s -u <username> -p <password>\n", argv[0]);

        if (username != NULL)
            g_free (username);
        if (password != NULL)
            g_free (password);

        exit (1);
    }

    provider = ogd_provider_new ("api.opendesktop.org");
    ogd_provider_auth_user_and_pwd (provider, username, password);

    g_set_application_name ("libopengdesktop test");

    window = main_window (provider);
    gtk_widget_show_all (window);

    gtk_main ();
    exit (0);
}

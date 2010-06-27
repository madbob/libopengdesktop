/*  libopengdesktop
 *  Copyright (C) 2009/2010 Roberto -MadBob- Guido <madbob@users.barberaware.org>
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

#include <ogd.h>

static void print_category (OGDObject *obj, gpointer check)
{
    OGDCategory *category;

    if (obj == NULL) {
        *((gboolean*) check) = FALSE;
        g_main_loop_quit (g_main_loop_new (NULL, FALSE));
    }
    else {
        category = OGD_CATEGORY (obj);
        printf ("%s\n", ogd_category_get_name (category));
    }
}

static gboolean busy_print (gpointer check)
{
    if (*((gboolean*) check) == FALSE) {
        return FALSE;
    }
    else {
        printf ("doing something else...\n");
        return TRUE;
    }
}

int main (int argc, char **argv)
{
    register int i;
    gchar *username;
    gchar *password;
    gboolean check;
    OGDProvider *provider;

    username = NULL;
    password = NULL;

    g_type_init ();
    g_thread_init (NULL);

    for (i = 1; i < argc; i++) {
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

    check = TRUE;
    ogd_category_fetch_all_async (provider, print_category, &check);
    g_timeout_add (2000, busy_print, &check);

    g_main_loop_run (g_main_loop_new (NULL, FALSE));
    exit (0);
}

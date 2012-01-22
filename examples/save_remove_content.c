/*  libopengdesktop
 *  Copyright (C) 2012 Roberto -MadBob- Guido <bob4job@gmail.com>
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

static void test_content (OGDProvider *provider)
{
    OGDContent *c;
    OGDCategory *cat;

    /*
        On gtk-apps.org, 360 is the "Development/Environment" category
    */
    cat = ogd_category_new_by_id (provider, "360");
    if (cat == NULL)
        g_error ("Unable to find category");

    c = ogd_content_new (provider);

    /*
        "name" and "category" are mandatory
    */
    ogd_content_set_name (c, "Test");
    ogd_content_set_category (c, cat);

    ogd_content_set_description (c, "This is just a test to verify new libopengdesktop capabilities");
    ogd_content_save (c);

    printf ("New id: %s\n", ogd_content_get_id (c));

    ogd_content_remove (c);
}

int main (int argc, char **argv)
{
    register int i;
    gchar *username;
    gchar *password;
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

    provider = ogd_provider_new ("api.gtk-apps.org");
    ogd_provider_auth_user_and_pwd (provider, username, password);
    test_content (provider);
    exit (0);
}

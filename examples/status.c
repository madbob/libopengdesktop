/*  libopengdesktop 0.3
 *  Copyright (C) 2009 Roberto -MadBob- Guido <madbob@users.barberaware.org>
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

int main (int argc, char **argv)
{
    register int i;
    gchar *username;
    gchar *password;
    gchar *message;
    OGDProvider *provider;
    OGDPerson *myself;

    username = NULL;
    password = NULL;
    message = NULL;

    g_type_init ();
    g_thread_init (NULL);

    for (i = 1; i < argc; i++) {
        if (strcmp (argv[i], "-u") == 0)
            username = g_strdup (argv[++i]);
        else if (strcmp (argv[i], "-p") == 0)
            password = g_strdup (argv[++i]);
        else if (strcmp (argv[i], "-m") == 0)
            message = g_strdup (argv[++i]);
    }

    if (username == NULL || password == NULL || message == NULL) {
        printf ("Usage: %s -u <username> -p <password> -m \"<status message>\"\n", argv[0]);
        i = 1;
        goto finish;
    }

    provider = ogd_provider_new ("api.opendesktop.org");
    ogd_provider_auth_user_and_pwd (provider, username, password);

    myself = (OGDPerson*) ogd_person_get_myself (provider);
    if (ogd_activity_set (myself, message) == FALSE)
        printf ("Unable to set status message\n");

    g_object_unref (myself);
    g_object_unref (provider);

    i = 0;

finish:
    if (username != NULL)
        g_free (username);
    if (password != NULL)
        g_free (password);
    if (message != NULL)
        g_free (message);

    exit (i);
}

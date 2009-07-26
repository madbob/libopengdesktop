/*  libopengdesktop 0.2
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

#include "ogd.h"

guint64 node_to_num (xmlNode *node)
{
    guint64 ret;
    xmlChar *tmp;

    tmp = xmlNodeGetContent (node);
    ret = g_ascii_strtoull ((char*) tmp, NULL, 10);
    xmlFree (tmp);
    return ret;
}

gdouble node_to_double (xmlNode *node)
{
    gdouble ret;
    xmlChar *tmp;

    tmp = xmlNodeGetContent (node);
    ret = g_ascii_strtod ((char*) tmp, NULL);
    xmlFree (tmp);
    return ret;
}

GDate* node_to_date (xmlNode *node)
{
    GTimeVal timeval;
    GDate *ret;
    xmlChar *tmp;

    ret = g_date_new ();
    tmp = xmlNodeGetContent (node);

    if (g_time_val_from_iso8601 ((char*) tmp, &timeval))
        g_date_set_time_val (ret, &timeval);

    xmlFree (tmp);
    return ret;
}

gulong total_items_for_query (xmlNode *package)
{
    gulong ret;
    GHashTable *header;
    GList *header_vals;
    GList *iter;

    ret = 0;
    header = ogd_provider_header_from_raw (package);
    header_vals = g_hash_table_get_keys (header);

    for (iter = g_list_first (header_vals); iter; iter = g_list_next (iter))
        if (strcmp ((gchar*) iter->data, "totalitems") == 0) {
            ret = (gulong) g_ascii_strtoull (g_hash_table_lookup (header, iter->data), NULL, 10);
            break;
        }

    g_list_free (header_vals);
    g_hash_table_unref (header);
    return ret;
}

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

#include "ogd.h"
#include "ogd-provider-private.h"
#include "ogd-private-utils.h"

static GHashTable       *TypesMap           = NULL;

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

    if (header != NULL) {
        header_vals = g_hash_table_get_keys (header);

        for (iter = g_list_first (header_vals); iter; iter = g_list_next (iter))
            if (strcmp ((gchar*) iter->data, "totalitems") == 0) {
                ret = (gulong) g_ascii_strtoull (g_hash_table_lookup (header, iter->data), NULL, 10);
                break;
            }

        g_list_free (header_vals);
        g_hash_table_unref (header);
    }

    return ret;
}

GList* list_of_people (OGDObject *reference, gchar *query)
{
    gulong collected;
    gulong totalitems;
    gint page;
    gchar *complete_query;
    GList *ret;
    xmlChar *friend_id;
    xmlNode *data;
    xmlNode *cursor;
    OGDPerson *obj;
    OGDProvider *provider;

    ret = NULL;
    collected = 0;
    totalitems = 0;
    page = 0;
    provider = ogd_object_get_provider (reference);

    do {
        complete_query = g_strdup_printf ("%s?pagesize=100&page=%d", query, page);
        data = ogd_provider_get_raw (provider, complete_query, NULL);
        g_free (complete_query);

        if (data != NULL) {
            totalitems = total_items_for_query (data);

            for (cursor = data->children; cursor; cursor = cursor->next) {
                friend_id = xmlNodeGetContent (cursor->children);

                if (friend_id != NULL) {
                    obj = g_object_new (OGD_PERSON_TYPE, NULL);
                    ogd_object_set_provider (OGD_OBJECT (obj), provider);

                    if (ogd_object_fill_by_id (OGD_OBJECT (obj), (char*) friend_id, NULL) == TRUE) {
                        ret = g_list_prepend (ret, obj);
                    }
                    else {
                        g_warning ("Unable to retrieve person with ID %s.", (char*) friend_id);
                    }

                    xmlFree (friend_id);
                    collected++;
                }
            }

            xmlFreeDoc (data->doc);
            page++;
        }

    } while (collected < totalitems);

    if (ret != NULL)
        ret = g_list_reverse (ret);

    return ret;
}

void init_types_management ()
{
    GType type;

    if (TypesMap == NULL) {
        TypesMap = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

        type = ogd_activity_get_type ();
        g_hash_table_insert (TypesMap, g_strdup ("activity"), MEMDUP (type));
        type = ogd_category_get_type ();
        g_hash_table_insert (TypesMap, g_strdup ("category"), MEMDUP (type));
        type = ogd_content_get_type ();
        g_hash_table_insert (TypesMap, g_strdup ("content"), MEMDUP (type));
        type = ogd_event_get_type ();
        g_hash_table_insert (TypesMap, g_strdup ("event"), MEMDUP (type));
        type = ogd_folder_get_type ();
        g_hash_table_insert (TypesMap, g_strdup ("folder"), MEMDUP (type));
        type = ogd_message_get_type ();
        g_hash_table_insert (TypesMap, g_strdup ("message"), MEMDUP (type));
        type = ogd_person_get_type ();
        g_hash_table_insert (TypesMap, g_strdup ("person"), MEMDUP (type));
        type = ogd_comment_get_type ();
        g_hash_table_insert (TypesMap, g_strdup ("comment"), MEMDUP (type));
    }
}

void finalize_types_management ()
{
    g_hash_table_destroy (TypesMap);
}

GType retrieve_type (const gchar *xml_name)
{
    void *ptr;

    if (TypesMap == NULL)
        g_warning ("You have to allocate an OGDProvider before any other action");

    ptr = g_hash_table_lookup (TypesMap, xml_name);

    if (ptr == NULL)
        return 0;
    else
        return *((GType*) ptr);
}

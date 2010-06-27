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
#include "ogd-category.h"
#include "ogd-private-utils.h"

#define OGD_FOLDER_GET_PRIVATE(obj)         (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_FOLDER_TYPE, OGDFolderPrivate))

/**
 * SECTION: ogd-folder
 * @short_description:  group of messages
 *
 * #OGDFolder is a meta-object which permits browsing messages. Each permit access to #OGDMessage
 * stored in the specified #OGDProvider for the user inited with ogd_provider_auth_user_and_pwd()
 */

struct _OGDFolderPrivate {
    gchar                   *id;
    gchar                   *name;
    guint                   messagecount;
    OGD_FOLDER_CATEGORY     category;
};

G_DEFINE_TYPE (OGDFolder, ogd_folder, OGD_OBJECT_TYPE);

static void ogd_folder_finalize (GObject *obj)
{
    OGDFolder *folder;

    folder = OGD_FOLDER (obj);

    PTR_CHECK_FREE_NULLIFY (folder->priv->id);
    PTR_CHECK_FREE_NULLIFY (folder->priv->name);
}

static gboolean ogd_folder_fill_by_xml (OGDObject *obj, const xmlNode *xml, GError **error)
{
    xmlNode *cursor;
    OGDFolder *folder;
    xmlChar *tmp;

    folder = OGD_FOLDER (obj);

    if (MYSTRCMP (xml->name, "folder") != 0) {
        g_set_error (error, OGD_PARSING_ERROR_DOMAIN, OGD_XML_ERROR, "Invalid XML for OGDFolder");
        return FALSE;
    }

    ogd_folder_finalize (G_OBJECT (obj));

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        if (MYSTRCMP (cursor->name, "id") == 0)
            folder->priv->id = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "name") == 0)
            folder->priv->name = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "messagecount") == 0)
            folder->priv->messagecount = (guint) node_to_num (cursor);
        else if (MYSTRCMP (cursor->name, "type") == 0) {
            tmp = xmlNodeGetContent (cursor);
            if (MYSTRCMP (tmp, "inbox") == 0)
                folder->priv->category = OGD_FOLDER_INBOX;
            else if (MYSTRCMP (tmp, "send") == 0)
                folder->priv->category = OGD_FOLDER_SEND;
            else if (MYSTRCMP (tmp, "trash") == 0)
                folder->priv->category = OGD_FOLDER_TRASH;
            else
                folder->priv->category = OGD_FOLDER_UNDEFINED;
            xmlFree (tmp);
        }
    }

    return TRUE;
}

static void ogd_folder_class_init (OGDFolderClass *klass)
{
    GObjectClass *gobject_class;
    OGDObjectClass *ogd_object_class;

    g_type_class_add_private (klass, sizeof (OGDFolderPrivate));

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ogd_folder_finalize;

    ogd_object_class = OGD_OBJECT_CLASS (klass);
    ogd_object_class->fill_by_xml = ogd_folder_fill_by_xml;
}

static void ogd_folder_init (OGDFolder *item)
{
    item->priv = OGD_FOLDER_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDFolderPrivate));
}

/**
 * ogd_folder_fetch_all:
 * @provider:       a #OGDProvider
 *
 * To retrieve the list of messages folders managed by a specified provider
 *
 * Return value:    a list of #OGDFolder
 */
GList* ogd_folder_fetch_all (OGDProvider *provider)
{
    return ogd_provider_get (provider, "message");
}

/**
 * ogd_folder_fetch_all_async:
 * @provider:       a #OGDProvider
 * @callback:       async callback to which incoming #OGDFolders are passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_folder_fetch_all()
 */
void ogd_folder_fetch_all_async (OGDProvider *provider, OGDAsyncCallback callback, gpointer userdata)
{
    ogd_provider_get_async (provider, "message", callback, userdata);
}

/**
 * ogd_folder_get_id:
 * @folder:         a #OGDFolder
 *
 * To get the ID of the folder
 *
 * Return value:    ID of the selected folder
 */
const gchar* ogd_folder_get_id (OGDFolder *folder)
{
    return (const gchar*) folder->priv->id;
}

/**
 * ogd_folder_get_name:
 * @folder:         a #OGDFolder
 *
 * To get the name of the folder
 *
 * Return value:    name of the selected folder
 */
const gchar* ogd_folder_get_name (OGDFolder *folder)
{
    return (const gchar*) folder->priv->name;
}

/**
 * ogd_folder_get_category:
 * @folder:         a #OGDFolder
 *
 * To get the category of the folder
 *
 * Return value:    category of the selected folder
 */
OGD_FOLDER_CATEGORY ogd_folder_get_category (OGDFolder *folder)
{
    return folder->priv->category;
}

/**
 * ogd_folder_get_count:
 * @folder:         a #OGDFolder
 *
 * To get the messages count of the folder
 *
 * Return value:    messages count of the selected folder
 */
guint ogd_folder_get_count (OGDFolder *folder)
{
    return folder->priv->messagecount;
}

/**
 * ogd_folder_get_contents:
 * @folder:         the #OGDFolder from which retrieve contents
 *
 * Used to retrieve #OGDMessage s inside a folder
 *
 * Return value:    a #OGDIterator usable to browse contents inside the selected folder
 */
OGDIterator* ogd_folder_get_contents (OGDFolder *folder)
{
    gchar *query;
    OGDIterator *iterator;

    query = g_strdup_printf ("message/%s", folder->priv->id);
    iterator = ogd_iterator_new (ogd_object_get_provider (OGD_OBJECT (folder)), query);
    g_free (query);
    return iterator;
}

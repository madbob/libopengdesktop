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

#include "ogd.h"
#include "ogd-private-utils.h"

#define OGD_CATEGORY_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_CATEGORY_TYPE, OGDCategoryPrivate))

/**
 * SECTION: ogd-category
 * @short_description:  group of contents
 *
 * #OGDCategory is a meta-object which permits browsing contents by their nature. Each
 * #OGDProvider hosts an arbitrary number of arbitrary categories, each identificable by name and
 * containing some #OGDContent
 */

struct _OGDCategoryPrivate {
    gchar       *id;
    gchar       *name;
};

G_DEFINE_TYPE (OGDCategory, ogd_category, OGD_OBJECT_TYPE);

static void ogd_category_finalize (GObject *obj)
{
    OGDCategory *category;

    category = OGD_CATEGORY (obj);

    PTR_CHECK_FREE_NULLIFY (category->priv->id);
    PTR_CHECK_FREE_NULLIFY (category->priv->name);
}

static gboolean ogd_category_fill_by_xml (OGDObject *obj, const xmlNode *xml, GError **error)
{
    xmlNode *cursor;
    OGDCategory *category;

    category = OGD_CATEGORY (obj);

    if (MYSTRCMP (xml->name, "category") != 0) {
        g_set_error (error, OGD_PARSING_ERROR_DOMAIN, OGD_XML_ERROR, "Invalid XML for OGDCategory");
        return FALSE;
    }

    ogd_category_finalize (G_OBJECT (obj));

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        if (MYSTRCMP (cursor->name, "id") == 0)
            category->priv->id = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "name") == 0)
            category->priv->name = MYGETCONTENT (cursor);
    }

    return TRUE;
}

static void ogd_category_class_init (OGDCategoryClass *klass)
{
    GObjectClass *gobject_class;
    OGDObjectClass *ogd_object_class;

    g_type_class_add_private (klass, sizeof (OGDCategoryPrivate));

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ogd_category_finalize;

    ogd_object_class = OGD_OBJECT_CLASS (klass);
    ogd_object_class->fill_by_xml = ogd_category_fill_by_xml;
}

static void ogd_category_init (OGDCategory *item)
{
    item->priv = OGD_CATEGORY_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDCategoryPrivate));
}

/**
 * ogd_category_new_by_id:
 * @provider:       the parent #OGDProvider of the desidered category
 * @id:             ID of the category
 *
 * To retrieve a #OGDCategory given his ID
 *
 * Return value: a #OGDCategory, or %NULL if no category is found for the given ID
 */
OGDCategory* ogd_category_new_by_id (OGDProvider *provider, gchar *id)
{
    GList *list;
    OGDCategory *cat;
    OGDCategory *ret;

    ret = NULL;

    for (list = ogd_category_fetch_all (provider); list != NULL; list = g_list_next (list)) {
        cat = list->data;

        if (strcmp (ogd_category_get_id (cat), id) == 0)
            ret = cat;
        else
            g_object_unref (cat);
    }

    for (; list != NULL; list = g_list_next (list)) {
        g_object_unref (list->data);
    }

    g_list_free (list);
    return ret;
}

/**
 * ogd_category_fetch_all:
 * @provider:       a #OGDProvider
 *
 * To retrieve the list of categories managed by a specified provider
 *
 * Return value:    a list of #OGDCategory
 */
GList* ogd_category_fetch_all (OGDProvider *provider)
{
    return ogd_provider_get (provider, "content/categories");
}

/**
 * ogd_category_fetch_all_async:
 * @provider:       a #OGDProvider
 * @callback:       async callback to which incoming #OGDCategory are passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_category_fetch_all()
 */
void ogd_category_fetch_all_async (OGDProvider *provider, OGDAsyncCallback callback, gpointer userdata)
{
    return ogd_provider_get_async (provider, "content/categories", callback, userdata);
}

/**
 * ogd_category_get_id:
 * @category:       a #OGDCategory
 *
 * To get the ID of the category
 *
 * Return value:    ID of the selected category
 */
const gchar* ogd_category_get_id (OGDCategory *category)
{
    return (const gchar*) category->priv->id;
}

/**
 * ogd_category_get_name:
 * @category:       a #OGDCategory
 *
 * To get the name of the category
 *
 * Return value:    name of the selected category
 */
const gchar* ogd_category_get_name (OGDCategory *category)
{
    return (const gchar*) category->priv->name;
}

/**
 * ogd_category_get_contents:
 * @category:       the #OGDCategory from which retrieve contents
 * @sorting:        type of sorting required
 *
 * Used to retrieve #OGDContent s inside a category
 *
 * Return value:    a #OGDIterator usable to browse contents inside the selected catogory.
 * Attention: #OGDContents from this iterator may not be completely filled, to obtain all details
 * about that it is suggested to call ogd_object_fill_by_id() on the #OGDContent itself and the
 * result of ogd_content_get_id()
 */
OGDIterator* ogd_category_get_contents (OGDCategory *category, OGD_CATEGORY_SORTING sorting)
{
    gchar *query;
    gchar *sort;
    OGDIterator *iterator;

    switch (sorting) {
        case OGD_CATEGORY_SORT_NEWEST:
            sort = "new";
            break;

        case OGD_CATEGORY_SORT_ALPHABETICAL:
            sort = "alpha";
            break;

        case OGD_CATEGORY_SORT_RATING:
            sort = "high";
            break;

        case OGD_CATEGORY_SORT_DOWNLOADS:
            sort = "down";
            break;

        default:
            g_warning ("Invalid sorting type for category");
            return NULL;
    }

    query = g_strdup_printf ("content/data?categories=%s&sortmode=%s", category->priv->id, sort);
    iterator = ogd_iterator_new (ogd_object_get_provider (OGD_OBJECT (category)), query);
    g_free (query);
    return iterator;
}

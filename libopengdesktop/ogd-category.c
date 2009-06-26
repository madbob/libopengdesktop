/*  libopengdesktop 0.1
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
#include "ogd-category.h"
#include "ogd-private-utils.h"

#define OGD_CATEGORY_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_CATEGORY_TYPE, OGDCategoryPrivate))

struct _OGDCategoryPrivate {
    gchar       *id;
    gchar       *name;
};

G_DEFINE_TYPE (OGDCategory, ogd_category, OGD_OBJECT_TYPE);

static void ogd_category_finalize (GObject *obj)
{
    OGDCategory *category;

    category = OGD_CATEGORY (obj);

    PTR_CHECK_FREE_NULLIFY (category->priv->name);
}

static gboolean ogd_category_fill_by_xml (OGDObject *obj, const xmlNode *xml, GError **error)
{
    xmlNode *cursor;
    OGDCategory *category;

    category = OGD_CATEGORY (obj);

    if (strcmp (xml->name, "category") != 0)
        return FALSE;

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        if (strcmp (cursor->name, "id") == 0)
            category->priv->id = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "name") == 0)
            category->priv->name = xmlNodeGetContent (cursor);
    }

    return TRUE;
}

static gboolean ogd_category_fill_by_id (OGDObject *obj, const gchar *id, GError **error)
{
    /*
        Categories cannot be required by ID, so this function is left unimplemented. Anyway it
        would be possible to retrieve the required category fetching them all and search in it:
        if needed this would be used in future
    */
    return FALSE;
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
    ogd_object_class->fill_by_id = ogd_category_fill_by_id;
}

static void ogd_category_init (OGDCategory *item)
{
    item->priv = OGD_CATEGORY_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDCategoryPrivate));
}

/**
 * ogd_category_get_all:
 * @provider:       a #OGDProvider
 *
 * To retrieve the list of categories managed by a specified provider
 *
 * Return value:    a list of #OGDCategory
 */

/*
    TODO    Provide also an async version
*/

GList* ogd_category_get_all (OGDProvider *provider)
{
    return ogd_provider_get (provider, "content/categories", OGD_CATEGORY_TYPE);
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
 * Return value:    a #OGDIterator usable to browse contents inside the selected catogory
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
    iterator = ogd_iterator_new (ogd_object_get_provider (OGD_OBJECT (category)), query, OGD_CONTENT_TYPE);
    g_free (query);
    return iterator;
}

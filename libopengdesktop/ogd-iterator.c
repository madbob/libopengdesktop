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
#include "ogd-iterator.h"
#include "ogd-provider.h"
#include "ogd-private-utils.h"

#define OGD_ITERATOR_DEFAULT_STEP       10

#define OGD_ITERATOR_GET_PRIVATE(obj)     (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OGD_ITERATOR_TYPE, OGDIteratorPrivate))

/**
 * SECTION: ogd-iterator
 * @short_description:  convenience object to iterate multiple contents
 *
 * Since most of the queries which may performed to a #OGDProvider returns long lists of objects,
 * an #OGDIterator may be used to paginate the requests and hide multiple requests, so to
 * maintain a coherent and slim interface on top
 */

struct _OGDIteratorPrivate {
    OGDProvider *provider;
    gchar       *query;
    GType       obj_type;

    gulong      total;
    gulong      step;
    gulong      position;
};

G_DEFINE_TYPE (OGDIterator, ogd_iterator, G_TYPE_OBJECT);

static void ogd_iterator_finalize (GObject *obj)
{
    OGDIterator *iterator;

    iterator = OGD_ITERATOR (obj);
    PTR_CHECK_FREE_NULLIFY (iterator->priv->query);
}

static void ogd_iterator_class_init (OGDIteratorClass *klass)
{
    GObjectClass *gobject_class;

    g_type_class_add_private (klass, sizeof (OGDIteratorPrivate));

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ogd_iterator_finalize;
}

static void ogd_iterator_init (OGDIterator *iter)
{
    iter->priv = OGD_ITERATOR_GET_PRIVATE (iter);
    memset (iter->priv, 0, sizeof (OGDIteratorPrivate));
    iter->priv->step = OGD_ITERATOR_DEFAULT_STEP;
}

static void prefetch_total_count (OGDIterator *iterator)
{
    gchar *query;
    xmlNode *node;

    /*
        Here we provide a simple dry-run of the query, just to obtain information about total
        items involved in the query. Perhaps this check may be delayed at least at the first step
        up of the iterator, or asyncronized
    */

    query = g_strdup_printf ("%s&page=0&pagesize=1", iterator->priv->query);
    node = ogd_provider_get_raw (iterator->priv->provider, query);
    g_free (query);

    iterator->priv->total = total_items_for_query (node);
    xmlFreeDoc (node->doc);
}

/**
 * ogd_iterator_new:
 * @provider:       #OGDProvider from which fetch contents
 * @base_query:     basic query to perform to extract multiple contents
 * @obj_type:       subtype of #OGDObject to build with responses from the server
 * 
 * Init a new #OGDIterator
 *
 * Return value:    a newly allocated #OGDIterator, which initial index is 0
 */
OGDIterator* ogd_iterator_new (OGDProvider *provider, const gchar *base_query, GType obj_type)
{
    OGDIterator *iterator;

    if (g_type_is_a (obj_type, OGD_OBJECT_TYPE) == FALSE) {
        g_warning ("An OGD_OBJECT_TYPE must be specified as 'obj_type'\n");
        return NULL;
    }

    iterator = g_object_new (OGD_ITERATOR_TYPE, NULL);
    iterator->priv->provider = provider;
    iterator->priv->query = g_strdup (base_query);
    iterator->priv->obj_type = obj_type;
    prefetch_total_count (iterator);
    return iterator;
}

/**
 * ogd_iterator_get_slice:
 * @iter:           #OGDIterator to advance
 * @start:          starting index of the request
 * @quantity:       number of elements to be fetched
 * 
 * Provides to fetch contents from index @start to index @end, accordly the query used in
 * initialization. ogd_iterator_next_slice() is a more convenient version of this. Take care this
 * function influences the internal index and next invocation of ogd_iterator_next_slice() will
 * use @start + @quantity as the starting point
 *
 * Return value:    a #GList of #GObject of the type specified at initialization
 *                  ( ogd_iterator_new() ). The list may be at maximum composed of @quantity
 *                  elements, or shorter if not enough objects are available to fill it starting
 *                  at @start index. The list must be freed and internal object unref when no
 *                  longer in use
 */

/*
    TODO    Provide also an async version
*/

GList* ogd_iterator_get_slice (OGDIterator *iter, gulong start, gulong quantity)
{
    gint len;
    gulong page;
    gchar *query;
    GList *ret;

    if (start >= iter->priv->total)
        return NULL;

    page = start / quantity;
    query = g_strdup_printf ("%s&page=%lu&pagesize=%lu", iter->priv->query, page, quantity);
    ret = ogd_provider_get (iter->priv->provider, query, iter->priv->obj_type);
    g_free (query);

    len = g_list_length (ret);
    iter->priv->position += (len < quantity) ? len : quantity;

    return ret;
}

/**
 * ogd_iterator_next_slice:
 * @iter:           #OGDIterator to advance
 * 
 * As ogd_iterator_get_slice(), but uses an internal index internally incremented as starting
 * point. Each invocation get a number of elements equal to the step specified with
 * ogd_iterator_set_step()
 *
 * Return value:    a #GList of #GObject of the type specified at initialization
 *                  ( ogd_iterator_new() ). The list length is between 0 and the number of
 *                  objects specified with ogd_iterator_set_step(). The list must be freed and
 *                  internal object unref when no longer in use
 */

/*
    TODO    Provide also an async version
*/

GList* ogd_iterator_next_slice (OGDIterator *iter)
{
    return ogd_iterator_get_slice (iter, iter->priv->position, iter->priv->step);
}

/**
 * ogd_iterator_set_step:
 * @iter:           #OGDIterator for which change step
 * @step:           number of elements to fetch at any invocation of ogd_iterator_next_slice()
 *
 * This permits to personalize dimension of the slice with is fetched when
 * ogd_iterator_next_slice() is called. OGD_ITERATOR_DEFAULT_STEP is the default step
 */
void ogd_iterator_set_step (OGDIterator *iter, gulong step)
{
    iter->priv->step = step;
}

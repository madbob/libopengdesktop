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
#include "ogd-iterator.h"
#include "ogd-provider.h"
#include "ogd-private-utils.h"

#define OGD_ITERATOR_DEFAULT_STEP       10

#define OGD_ITERATOR_GET_PRIVATE(obj)     (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OGD_ITERATOR_TYPE, OGDIteratorPrivate))

struct _OGDIteratorPrivate {
    OGDProvider *provider;
    gchar       *query;
    GType       obj_type;

    gulong      step;
    gulong      position;
};

G_DEFINE_TYPE (OGDIterator, ogd_iterator, G_TYPE_OBJECT);

static void ogd_iterator_finalize (GObject *obj)
{
    OGDIterator *provider;

    provider = OGD_ITERATOR (obj);

    PTR_CHECK_FREE_NULLIFY (provider->priv->query);
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

OGDIterator* ogd_iterator_new (OGDProvider *provider, const gchar *base_query, GType obj_type)
{
    OGDIterator *iterator;

    /*
        TODO    Verify obj_type is a OGDObject
    */

    iterator = g_object_new (OGD_ITERATOR_TYPE, NULL);
    iterator->priv->provider = provider;
    iterator->priv->query = g_strdup (base_query);
    iterator->priv->obj_type = obj_type;
    return iterator;
}

/*
    TODO    Provide also an async version
*/

GList* ogd_iterator_get (OGDIterator *iter, gulong start, gulong quantity)
{
    gint len;
    gulong page;
    gchar *query;
    GList *ret;

    /*
        TODO    Page selection can not works if step is modified, improve rounding algorithm
    */
    page = start / quantity;
    query = g_strdup_printf ("%s&page=%lu&pagesize=%lu", iter->priv->query, page, quantity);
    ret = ogd_provider_get (iter->priv->provider, query, iter->priv->obj_type);
    g_free (query);

    len = g_list_length (ret);
    iter->priv->position += (len < quantity) ? len : quantity;

    return ret;
}

/*
    TODO    Provide also an async version
*/

GList* ogd_iterator_next (OGDIterator *iter)
{
    return ogd_iterator_get (iter, iter->priv->position, iter->priv->step);
}

void ogd_iterator_set_step (OGDIterator *iter, gulong step)
{
    iter->priv->step = step;
}

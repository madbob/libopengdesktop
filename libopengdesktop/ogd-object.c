/*  libopengdesktop
 *  Copyright (C) 2009/2011 Roberto -MadBob- Guido <bob4job@gmail.com>
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
#include "ogd-object.h"
#include "ogd-provider.h"
#include "ogd-provider-private.h"
#include "ogd-private-utils.h"

#define OGD_OBJECT_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_OBJECT_TYPE, OGDObjectPrivate))

/**
 * SECTION: ogd-object
 * @short_description:  common rappresentation for contents incoming from the web
 *
 * This is just an abstract class for all objects described in the specification, describes a
 * common interface to serialize and deserialize them and some usefull informations shared among
 * all components
 */

struct _OGDObjectPrivate {
    OGDProvider     *provider;
};

G_DEFINE_ABSTRACT_TYPE (OGDObject, ogd_object, G_TYPE_OBJECT);

static void ogd_object_finalize (GObject *obj)
{
    /* dummy */
}

/**
 * ogd_object_fill_by_xml:
 * @obj:            #OGDObject to fill with values from the provided XML
 * @xml:            XML to parse
 * @error:          a #GError filled if the function return %FALSE
 *
 * To fill a #OGDObject with the given XML. Consider this is an abstract method, to be
 * reimplemented by each extending class
 *
 * Return value:    %TRUE if the XML is correctly parsed and @obj is filled with retrieved
 *                  values, %FALSE otherwise
 */
gboolean ogd_object_fill_by_xml (OGDObject *obj, const xmlNode *xml, GError **error)
{
    return OGD_OBJECT_GET_CLASS (obj)->fill_by_xml (obj, xml, error);
}

static inline gchar* has_valid_target_callback (OGDObject *obj, const gchar *id)
{
    if (OGD_OBJECT_GET_CLASS (obj)->target_query == NULL) {
        g_warning ("No target query defined for this object type");
        return NULL;
    }

    return OGD_OBJECT_GET_CLASS (obj)->target_query (id);
}

/**
 * ogd_object_fill_by_id:
 * @obj:            #OGDObject to fill with values from the provided XML
 * @id:             ID of the object to read
 * @error:          a #GError filled if the function return %FALSE
 *
 * To fill a #OGDObject given its ID on the server. Consider this is an abstract method, to be
 * reimplemented by each extending class, but not all classes may be accessed through an ID
 *
 * Return value:    %TRUE if the XML is correctly took and filled due the given @id, %FALSE
 *                  otherwise
 */
gboolean ogd_object_fill_by_id (OGDObject *obj, const gchar *id, GError **error)
{
    gchar *query;
    gboolean ret;
    xmlNode *data;

    if (obj->priv->provider == NULL) {
        g_set_error (error, OGD_HIERARCHY_ERROR_DOMAIN, OGD_HIERARCHY_ERROR,
                     "Object without OGDProvider: have you used ogd_object_set_provider() properly?");
        return FALSE;
    }

    query = has_valid_target_callback (obj, id);
    if (query == NULL) {
        g_set_error (error, OGD_TYPE_ERROR_DOMAIN, OGD_TYPE_ERROR,
                     "This kind of object seems not have a callback to retrieve a single element.");
        return FALSE;
    }

    data = ogd_provider_get_raw (ogd_object_get_provider (obj), query, error);
    g_free (query);

    if (data != NULL) {
        ret = ogd_object_fill_by_xml (obj, data, error);
        xmlFreeDoc (data->doc);
    }
    else {
        ret = FALSE;
    }

    return ret;
}

static void parse_xml_from_async (xmlNode *node, gpointer userdata)
{
    AsyncRequestDesc *req;

    if (node != NULL) {
        req = (AsyncRequestDesc*) userdata;
        ogd_object_fill_by_xml (req->reference, node, NULL);
        req->callback (req->reference, req->userdata);
        xmlFreeDoc (node->doc);
        g_free (req);
    }
}

/**
 * ogd_object_fill_by_id_async:
 * @obj:            #OGDObject to fill with values from the provided XML
 * @id:             ID of the object to read
 * @callback:       async callback to which the filled #OGDObject is passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_object_fill_by_id()
 */
void ogd_object_fill_by_id_async (OGDObject *obj, const gchar *id, OGDAsyncCallback callback, gpointer userdata)
{
    gchar *query;
    AsyncRequestDesc *req;

    query = has_valid_target_callback (obj, id);
    if (query == NULL)
        return;

    req = g_new0 (AsyncRequestDesc, 1);
    req->callback = callback;
    req->userdata = userdata;
    req->reference = obj;

    ogd_provider_get_raw_async (ogd_object_get_provider (obj), query, FALSE, parse_xml_from_async, req);
    g_free (query);
}

static void ogd_object_class_init (OGDObjectClass *klass)
{
    GObjectClass *gobject_class;

    g_type_class_add_private (klass, sizeof (OGDObjectPrivate));

    klass->fill_by_xml = ogd_object_fill_by_xml;

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ogd_object_finalize;
}

static void ogd_object_init (OGDObject *item)
{
    item->priv = OGD_OBJECT_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDObjectPrivate));
}

/**
 * ogd_object_get_provider:
 * @obj:            the #OGDObject to which get relative #OGDProvider
 *
 * To retrieve the repository from which an object has been took
 *
 * Return value:    the #OGDProvider from which the object has been took. Don't free it!
 */
OGDProvider* ogd_object_get_provider (OGDObject *obj)
{
    return obj->priv->provider;
}

/**
 * ogd_object_set_provider:
 * @obj:            #OGDObject to which assign a provider
 * @provider:       reference to the #OGDProvider from which the object has been took
 *
 * This is internally used to explicitely assign a provider to an object. This is usefull to
 * propagate reference to the content repository from an object to another, and avoid specify it
 * each time an action is performed
 */
void ogd_object_set_provider (OGDObject *obj, OGDProvider *provider)
{
    obj->priv->provider = (OGDProvider*) provider;
}

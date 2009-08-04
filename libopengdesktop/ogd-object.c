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

#include "ogd.h"
#include "ogd-object.h"
#include "ogd-provider.h"

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

/*
    TODO    Provide also an async version
*/

gboolean ogd_object_fill_by_id (OGDObject *obj, const gchar *id, GError **error)
{
    return OGD_OBJECT_GET_CLASS (obj)->fill_by_id (obj, id, error);
}

static void ogd_object_class_init (OGDObjectClass *klass)
{
    GObjectClass *gobject_class;

    g_type_class_add_private (klass, sizeof (OGDObjectPrivate));

    klass->fill_by_xml = ogd_object_fill_by_xml;
    klass->fill_by_id = ogd_object_fill_by_id;

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
 * Return value:    the #OGDProvider from which the object has been took
 */
const OGDProvider* ogd_object_get_provider (OGDObject *obj)
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
void ogd_object_set_provider (OGDObject *obj, const OGDProvider *provider)
{
    obj->priv->provider = (OGDProvider*) provider;
}

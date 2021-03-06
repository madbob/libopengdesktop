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
#include "ogd-activity.h"
#include "ogd-private-utils.h"

#define OGD_ACTIVITY_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_ACTIVITY_TYPE, OGDActivityPrivate))

/**
 * SECTION: ogd-activity
 * @short_description:  description of what happens
 *
 * You can use #OGDActivity to see what is going on in friends network. For example who visited
 * you homepage, who has send you a message and who uploaded a new content to the website.
 *
 * You can also post a microblogging message which is visible on you profile page and in the
 * activities of your friends.
 */

struct _OGDActivityPrivate {
    gchar                   *authorid;
    GDate                   *date;
    OGD_ACTIVITY_CATEGORY   category;
    gchar                   *message;
    gchar                   *link;
};

G_DEFINE_TYPE (OGDActivity, ogd_activity, OGD_OBJECT_TYPE);

static void ogd_activity_finalize (GObject *obj)
{
    OGDActivity *activity;

    activity = OGD_ACTIVITY (obj);

    PTR_CHECK_FREE_NULLIFY (activity->priv->authorid);
    DATE_CHECK_FREE_NULLIFY (activity->priv->date);
    PTR_CHECK_FREE_NULLIFY (activity->priv->message);
    PTR_CHECK_FREE_NULLIFY (activity->priv->link);
}

static gboolean ogd_activity_fill_by_xml (OGDObject *obj, const xmlNode *xml, GError **error)
{
    xmlNode *cursor;
    OGDActivity *activity;

    activity = OGD_ACTIVITY (obj);

    if (MYSTRCMP (xml->name, "activity") != 0) {
        g_set_error (error, OGD_PARSING_ERROR_DOMAIN, OGD_XML_ERROR, "Invalid XML for OGDActivity");
        return FALSE;
    }

    ogd_activity_finalize (G_OBJECT (obj));

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        if (MYSTRCMP (cursor->name, "personid") == 0)
            activity->priv->authorid = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "timestamp") == 0)
            activity->priv->date = node_to_date (cursor);
        else if (MYSTRCMP (cursor->name, "type") == 0)
            activity->priv->category = (guint) node_to_num (cursor);
        else if (MYSTRCMP (cursor->name, "message") == 0)
            activity->priv->message = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "link") == 0)
            activity->priv->link = MYGETCONTENT (cursor);
    }

    return TRUE;
}

static void ogd_activity_class_init (OGDActivityClass *klass)
{
    GObjectClass *gobject_class;
    OGDObjectClass *ogd_object_class;

    g_type_class_add_private (klass, sizeof (OGDActivityPrivate));

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ogd_activity_finalize;

    ogd_object_class = OGD_OBJECT_CLASS (klass);
    ogd_object_class->fill_by_xml = ogd_activity_fill_by_xml;
}

static void ogd_activity_init (OGDActivity *item)
{
    item->priv = OGD_ACTIVITY_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDActivityPrivate));
}

/**
 * ogd_activity_get_authorid:
 * @activity:		an #OGDActivity to read
 *
 * To retrieve the author of the @activity
 *
 * Return value:	identifiers of the #OGDPerson who created the activity. You can use
                    ogd_object_fill_by_id() to obtain all informations
 */
const gchar* ogd_activity_get_authorid (OGDActivity *activity)
{
    return (const gchar*) activity->priv->authorid;
}

/**
 * ogd_activity_get_date:
 * @activity:		an #OGDActivity to read
 *
 * To retrieve the date of creation of the activity
 *
 * Return value:	a #GDate for the date of creation of the activity
 */
const GDate* ogd_activity_get_date (OGDActivity *activity)
{
    return (const GDate*) activity->priv->date;
}

/**
 * ogd_activity_get_category:
 * @activity:		an #OGDActivity to read
 *
 * To retrieve the category of the activity
 *
 * Return value:	identifier of the provided activity
 */
OGD_ACTIVITY_CATEGORY ogd_activity_get_category (OGDActivity *activity)
{
    return activity->priv->category;
}

/**
 * ogd_activity_get_message:
 * @activity:		an #OGDActivity to read
 *
 * To retrieve the message attached to the activity
 *
 * Return value:	a string containing a message, or NULL if no message has been assigned
 */
const gchar* ogd_activity_get_message (OGDActivity *activity)
{
    return (const gchar*) activity->priv->message;
}

/**
 * ogd_activity_get_link:
 * @activity:		an #OGDActivity to read
 *
 * To retrieve the web link attached to the activity
 *
 * Return value:	a string containing a web link, or NULL if no link has been assigned
 */
const gchar* ogd_activity_get_link (OGDActivity *activity)
{
    return (const gchar*) activity->priv->link;
}

/**
 * ogd_activity_set:
 * @myself:         return value of ogd_person_get_myself()
 * @status:         text to set as current status
 *
 * To update the status of the current user. Valid only if authentication is performed with
 * ogd_provider_auth_user_and_pwd()
 *
 * Return value:    %TRUE if status is correctly set, %FALSE otherwise
 */
gboolean ogd_activity_set (OGDPerson *myself, gchar *status)
{
    gboolean ret;
    GHashTable *params;
    OGDProvider *provider;

    provider = ogd_object_get_provider (OGD_OBJECT (myself));

    params = g_hash_table_new (g_str_hash, g_str_equal);
    g_hash_table_insert (params, "message", status);

    ret = ogd_provider_put (provider, "activity", params);

    g_hash_table_unref (params);
    return ret;
}

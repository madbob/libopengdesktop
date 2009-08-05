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
#include "ogd-message.h"
#include "ogd-private-utils.h"

#define OGD_MESSAGE_GET_PRIVATE(obj)        (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_MESSAGE_TYPE, OGDMessagePrivate))

/**
 * SECTION: ogd-message
 * @short_description:  a private message shared among users
 *
 * #OGDMessage is abstraction for a message shared between two users (where at least one of the
 * two is the current one). They are organized and made accessible throught #OGDFolder
 */

struct _OGDMessagePrivate {
    gchar               *id;
    gchar               *authorid;
    GDate               *date;
    OGD_MESSAGE_STATUS  status;
    gchar               *subject;
    gchar               *body;
};

G_DEFINE_TYPE (OGDMessage, ogd_message, OGD_OBJECT_TYPE);

static void ogd_message_finalize (GObject *obj)
{
    OGDMessage *msg;

    msg = OGD_MESSAGE (obj);

    PTR_CHECK_FREE_NULLIFY (msg->priv->id);
    PTR_CHECK_FREE_NULLIFY (msg->priv->authorid);
    DATE_CHECK_FREE_NULLIFY (msg->priv->date);
    PTR_CHECK_FREE_NULLIFY (msg->priv->subject);
    PTR_CHECK_FREE_NULLIFY (msg->priv->body);
}

static gboolean ogd_message_fill_by_xml (OGDObject *obj, const xmlNode *xml, GError **error)
{
    xmlNode *cursor;
    OGDMessage *msg;

    msg = OGD_MESSAGE (obj);

    if (MYSTRCMP (xml->name, "message") != 0)
        return FALSE;

    ogd_message_finalize (G_OBJECT (obj));

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        if (MYSTRCMP (cursor->name, "messageid") == 0)
            msg->priv->id = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "messagefrom") == 0)
            msg->priv->authorid = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "senddate") == 0)
            msg->priv->date = node_to_date (cursor);
        else if (MYSTRCMP (cursor->name, "status") == 0)
            msg->priv->status = (guint) node_to_num (cursor);
        else if (MYSTRCMP (cursor->name, "subject") == 0)
            msg->priv->subject = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "body") == 0)
            msg->priv->body = MYGETCONTENT (cursor);
    }

    return TRUE;
}

static void ogd_message_class_init (OGDMessageClass *klass)
{
    GObjectClass *gobject_class;
    OGDObjectClass *ogd_object_class;

    g_type_class_add_private (klass, sizeof (OGDMessagePrivate));

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ogd_message_finalize;

    ogd_object_class = OGD_OBJECT_CLASS (klass);
    ogd_object_class->fill_by_xml = ogd_message_fill_by_xml;
}

static void ogd_message_init (OGDMessage *item)
{
    item->priv = OGD_MESSAGE_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDMessagePrivate));
}

/**
 * ogd_message_get_authorid:
 * @msg:            an #OGDMessage to read
 *
 * To retrieve the author of the message
 * 
 * Return value:    identifiers of the #OGDPerson who sent the message. You can use
                    ogd_object_fill_by_id() to obtain all informations
 */
const gchar* ogd_message_get_authorid (OGDMessage *msg)
{
    return (const gchar*) msg->priv->authorid;
}

/**
 * ogd_message_get_date:
 * @msg:            an #OGDMessage to read
 *
 * To retrieve the date of message sending
 * 
 * Return value:    a #GDate for the date of message sending
 */
const GDate* ogd_message_get_date (OGDMessage *msg)
{
    return (const GDate*) msg->priv->date;
}

/**
 * ogd_message_get_status:
 * @msg:            an #OGDMessage to read
 *
 * To retrieve the current status for the message
 * 
 * Return value:    identifier of the status of the message
 */
OGD_MESSAGE_STATUS ogd_message_get_status (OGDMessage *msg)
{
    return msg->priv->status;
}

/**
 * ogd_message_get_subject:
 * @msg:            an #OGDMessage to read
 *
 * To retrieve the subject of the message
 * 
 * Return value:    a string containing the subject
 */
const gchar* ogd_message_get_subject (OGDMessage *msg)
{
    return (const gchar*) msg->priv->subject;
}

/**
 * ogd_message_get_body:
 * @msg:            an #OGDMessage to read
 *
 * To retrieve the body of the message
 * 
 * Return value:    a string containing the message
 */
const gchar* ogd_message_get_body (OGDMessage *msg)
{
    return (const gchar*) msg->priv->body;
}

static const gchar* retrieve_messages_folder (OGDProvider *provider, OGD_FOLDER_CATEGORY category)
{
    const gchar *ret;
    GList *folders;
    GList *iter;
    OGDFolder *fold;

    ret = NULL;
    folders = ogd_folder_fetch_all (provider);

    for (iter = g_list_first (folders); iter; iter = g_list_next (iter)) {
        fold = (OGDFolder*) iter->data;

        if (ogd_folder_get_category (fold) == category) {
            ret = ogd_folder_get_id (fold);
            break;
        }
    }

    g_list_free (folders);
    return ret;
}

/**
 * ogd_message_send:
 * @to:             recipent for the message
 * @subject:        text of the subject for the new message
 * @body:           content of the message
 *
 * Permits to send a new message
 */
void ogd_message_send (OGDPerson *to, const gchar *subject, const gchar *body)
{
    gchar *query;
    const gchar *id_of_send_folder;
    GHashTable *params;
    OGDProvider *provider;

    provider = (OGDProvider*) ogd_object_get_provider (OGD_OBJECT (to));

    id_of_send_folder = retrieve_messages_folder (provider, OGD_FOLDER_SEND);
    if (id_of_send_folder == NULL)
        return;

    query = g_strdup_printf ("message/%s", id_of_send_folder);

    params = g_hash_table_new (g_str_hash, g_str_equal);
    g_hash_table_insert (params, "to", (gpointer) ogd_person_get_id (to));
    g_hash_table_insert (params, "subject", (gpointer) subject);
    g_hash_table_insert (params, "message", (gpointer) body);

    ogd_provider_put (provider, query, params);

    g_hash_table_unref (params);
    g_free (query);
}

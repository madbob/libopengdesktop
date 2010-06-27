/*  libopengdesktop
 *  Copyright (C) 2010 Roberto -MadBob- Guido <madbob@users.barberaware.org>
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
#include "ogd-comment.h"
#include "ogd-private-utils.h"

#define OGD_COMMENT_GET_PRIVATE(obj)        (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_COMMENT_TYPE, OGDCommentPrivate))

/**
 * SECTION: ogd-comment
 * @short_description:  a comment assigned to a content
 *
 * #OGDComments are simple comments assigned to any kind of contents, of type
 * #OGDContents or #OGDEvent. Look at the documentation of specific classes
 * to know how to retrieve and assign comments to individual pieces of
 * informations
 */

struct _OGDCommentPrivate {
    gchar               *id;
    gchar               *authorid;
    GDate               *date;
    gchar               *subject;
    gchar               *message;
};

G_DEFINE_TYPE (OGDComment, ogd_comment, OGD_OBJECT_TYPE);

static void ogd_comment_finalize (GObject *obj)
{
    OGDComment *msg;

    msg = OGD_COMMENT (obj);

    PTR_CHECK_FREE_NULLIFY (msg->priv->id);
    PTR_CHECK_FREE_NULLIFY (msg->priv->authorid);
    DATE_CHECK_FREE_NULLIFY (msg->priv->date);
    PTR_CHECK_FREE_NULLIFY (msg->priv->subject);
    PTR_CHECK_FREE_NULLIFY (msg->priv->message);
}

static gboolean ogd_comment_fill_by_xml (OGDObject *obj, const xmlNode *xml, GError **error)
{
    xmlNode *cursor;
    OGDComment *msg;

    msg = OGD_COMMENT (obj);

    if (MYSTRCMP (xml->name, "comment") != 0) {
        g_set_error (error, OGD_PARSING_ERROR_DOMAIN, OGD_XML_ERROR, "Invalid XML for OGDComment");
        return FALSE;
    }

    ogd_comment_finalize (G_OBJECT (obj));

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        if (MYSTRCMP (cursor->name, "id") == 0)
            msg->priv->id = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "user") == 0)
            msg->priv->authorid = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "date") == 0)
            msg->priv->date = node_to_date (cursor);
        else if (MYSTRCMP (cursor->name, "subject") == 0)
            msg->priv->subject = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "text") == 0)
            msg->priv->message = MYGETCONTENT (cursor);
    }

    return TRUE;
}

static void ogd_comment_class_init (OGDCommentClass *klass)
{
    GObjectClass *gobject_class;
    OGDObjectClass *ogd_object_class;

    g_type_class_add_private (klass, sizeof (OGDCommentPrivate));

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ogd_comment_finalize;

    ogd_object_class = OGD_OBJECT_CLASS (klass);
    ogd_object_class->fill_by_xml = ogd_comment_fill_by_xml;
}

static void ogd_comment_init (OGDComment *item)
{
    item->priv = OGD_COMMENT_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDCommentPrivate));
}

/**
 * ogd_comment_get_authorid:
 * @msg:            an #OGDComment to read
 *
 * To retrieve the author of the message
 *
 * Return value:    identifiers of the #OGDPerson who sent the message. You can use
                    ogd_object_fill_by_id() to obtain all informations
 */
const gchar* ogd_comment_get_authorid (OGDComment *msg)
{
    return (const gchar*) msg->priv->authorid;
}

/**
 * ogd_comment_get_date:
 * @msg:            an #OGDComment to read
 *
 * To retrieve the date of message sending
 *
 * Return value:    a #GDate for the date of message sending
 */
const GDate* ogd_comment_get_date (OGDComment *msg)
{
    return (const GDate*) msg->priv->date;
}

/**
 * ogd_comment_get_subject:
 * @msg:            an #OGDComment to read
 *
 * To retrieve the subject of the message
 *
 * Return value:    a string containing the subject
 */
const gchar* ogd_comment_get_subject (OGDComment *msg)
{
    return (const gchar*) msg->priv->subject;
}

/**
 * ogd_comment_get_message:
 * @msg:            an #OGDComment to read
 *
 * To retrieve the message of the message
 *
 * Return value:    a string containing the message
 */
const gchar* ogd_comment_get_message (OGDComment *msg)
{
    return (const gchar*) msg->priv->message;
}

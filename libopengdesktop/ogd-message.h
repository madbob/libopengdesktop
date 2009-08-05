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

#ifndef OGD_MESSAGE_H
#define OGD_MESSAGE_H

G_BEGIN_DECLS

#define OGD_MESSAGE_TYPE                (ogd_message_get_type ())
#define OGD_MESSAGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                         OGD_MESSAGE_TYPE, OGDMessage))
#define OGD_MESSAGE_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                         OGD_MESSAGE_TYPE,                  \
                                         OGDMessageClass))
#define IS_OGD_MESSAGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                         OGD_MESSAGE_TYPE))
#define IS_OGD_MESSAGE_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                         OGD_MESSAGE_TYPE))
#define OGD_MESSAGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                         OGD_MESSAGE_TYPE,                  \
                                         OGDMessageClass))

typedef struct _OGDMessage        OGDMessage;
typedef struct _OGDMessageClass   OGDMessageClass;
typedef struct _OGDMessagePrivate OGDMessagePrivate;

struct _OGDMessage {
    OGDObject                parent;
    OGDMessagePrivate        *priv;
};

struct _OGDMessageClass {
    OGDObjectClass          parent_class;
};

/**
 * OGD_MESSAGE_STATUS:
 * @OGD_MESSAGE_UNREAD:                    the message was not read yet
 * @OGD_MESSAGE_READ:                      the message was read
 * @OGD_MESSAGE_ANSWERED:                  the message was read and answered
 *
 * Helps to identify status of the message
 */
typedef enum {
    OGD_MESSAGE_UNREAD,
    OGD_MESSAGE_READ,
    OGD_MESSAGE_ANSWERED
} OGD_MESSAGE_STATUS;

GType                   ogd_message_get_type                    ();

const OGDPerson*        ogd_message_get_author                  (OGDMessage *msg);
void                    ogd_message_get_author_async            (OGDMessage *message, OGDAsyncCallback callback, gpointer userdata);
const GDate*            ogd_message_get_date                    (OGDMessage *msg);
OGD_MESSAGE_STATUS      ogd_message_get_status                  (OGDMessage *msg);
const gchar*            ogd_message_get_subject                 (OGDMessage *msg);
const gchar*            ogd_message_get_body                    (OGDMessage *msg);

void                    ogd_message_send                        (OGDPerson *to, const gchar *subject, const gchar *body);

G_END_DECLS

#endif /* OGD_MESSAGE_H */

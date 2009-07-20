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

#ifndef OGD_ACTIVITY_H
#define OGD_ACTIVITY_H

G_BEGIN_DECLS

#define OGD_ACTIVITY_TYPE             (ogd_activity_get_type ())
#define OGD_ACTIVITY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                         OGD_ACTIVITY_TYPE, OGDActivity))
#define OGD_ACTIVITY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                         OGD_ACTIVITY_TYPE,               \
                                         OGDActivityClass))
#define IS_OGD_ACTIVITY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                         OGD_ACTIVITY_TYPE))
#define IS_OGD_ACTIVITY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                         OGD_ACTIVITY_TYPE))
#define OGD_ACTIVITY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                         OGD_ACTIVITY_TYPE,               \
                                         OGDActivityClass))

typedef struct _OGDActivity        OGDActivity;
typedef struct _OGDActivityClass   OGDActivityClass;
typedef struct _OGDActivityPrivate OGDActivityPrivate;

struct _OGDActivity {
    OGDObject                parent;
    OGDActivityPrivate        *priv;
};

struct _OGDActivityClass {
    OGDObjectClass          parent_class;
};

/**
 * OGD_ACTIVITY_CATEGORY:
 * @OGD_ACTIVITY_STATUS_MESSAGE:            the message status of a #OGDPerson has been updated
 * @OGD_ACTIVITY_NEW_FRIEND:                a new friend has been added to an existing friend
 * @OGD_ACTIVITY_NEW_CONTENT:               a new #OGDContent has been added by a friend
 * @OGD_ACTIVITY_PROFILE_UPDATE:            a friend updated his profile
 * @OGD_ACTIVITY_PROFILE_PHOTO_UPDATE:      a friend updated his avatar
 * @OGD_ACTIVITY_CONTENT_EDIT:              a #OGDContent has been modified by a friend
 * @OGD_ACTIVITY_NEW_MESSAGE:               someone sent a message to the current user
 * @OGD_ACTIVITY_HOMEPAGE_VISIT:            someone visit the profile page of the current user
 * @OGD_ACTIVITY_BECOME_FAN:                a friend become fan of a #OGDContent
 * @OGD_ACTIVITY_NO_LONGER_FAN:             a friend stops to be fan of a #OGDContent
 * @OGD_ACTIVITY_GROUP_CREATED:             a new group has been created
 * @OGD_ACTIVITY_GROUP_JOINED:              a friend joined a group
 * @OGD_ACTIVITY_GROUP_LEFT:                a friend left a group
 * @OGD_ACTIVITY_EVENT_CREATED:             a friend created an #OGDEvent
 * @OGD_ACTIVITY_ATTENDING_EVENT:           a friend is attending for an #OGDEvent
 * @OGD_ACTIVITY_NO_LONGER_ATTENDING_EVENT: a friend no longer attendees an #OGDEvent
 * @OGD_ACTIVITY_CREATED_JOB_OFFER:         a new job offer has been added
 * @OGD_ACTIVITY_EDITED_JOB_OFFER:          a job offer has been modified
 * @OGD_ACTIVITY_NEW_USER:                  a new user has registered to #OGDProvider
 *
 * Helps to identify type of activity happened on the provider
 */
typedef enum {
    OGD_ACTIVITY_STATUS_MESSAGE             = 1,
    OGD_ACTIVITY_NEW_FRIEND,
    OGD_ACTIVITY_NEW_CONTENT,
    OGD_ACTIVITY_PROFILE_UPDATE,
    OGD_ACTIVITY_PROFILE_PHOTO_UPDATE,
    OGD_ACTIVITY_CONTENT_EDIT,
    OGD_ACTIVITY_NEW_MESSAGE,
    OGD_ACTIVITY_HOMEPAGE_VISIT,
    OGD_ACTIVITY_BECOME_FAN,
    OGD_ACTIVITY_NO_LONGER_FAN,
    OGD_ACTIVITY_GROUP_CREATED,
    OGD_ACTIVITY_GROUP_JOINED,
    OGD_ACTIVITY_GROUP_LEFT,
    OGD_ACTIVITY_EVENT_CREATED,
    OGD_ACTIVITY_ATTENDING_EVENT,
    OGD_ACTIVITY_NO_LONGER_ATTENDING_EVENT,
    OGD_ACTIVITY_CREATED_JOB_OFFER,
    OGD_ACTIVITY_EDITED_JOB_OFFER,
    OGD_ACTIVITY_NEW_USER
} OGD_ACTIVITY_CATEGORY;

GType                   ogd_activity_get_type                   ();

const OGDPerson*        ogd_activity_get_author                 (OGDActivity *activity);
const GDate*            ogd_activity_get_date                   (OGDActivity *activity);
OGD_ACTIVITY_CATEGORY   ogd_activity_get_category               (OGDActivity *activity);
const gchar*            ogd_activity_get_message                (OGDActivity *activity);
const gchar*            ogd_activity_get_link                   (OGDActivity *activity);

gboolean                ogd_activity_set                        (OGDPerson *myself, const gchar *status);

G_END_DECLS

#endif /* OGD_ACTIVITY_H */

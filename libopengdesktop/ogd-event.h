/*  libopengdesktop
 *  Copyright (C) 2009/2010 Roberto -MadBob- Guido <madbob@users.barberaware.org>
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

#ifndef OGD_EVENT_H
#define OGD_EVENT_H

G_BEGIN_DECLS

#define OGD_EVENT_TYPE             (ogd_event_get_type ())
#define OGD_EVENT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                         OGD_EVENT_TYPE, OGDEvent))
#define OGD_EVENT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                         OGD_EVENT_TYPE,               \
                                         OGDEventClass))
#define IS_OGD_EVENT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                         OGD_EVENT_TYPE))
#define IS_OGD_EVENT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                         OGD_EVENT_TYPE))
#define OGD_EVENT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                         OGD_EVENT_TYPE,               \
                                         OGDEventClass))

typedef struct _OGDEvent        OGDEvent;
typedef struct _OGDEventClass   OGDEventClass;
typedef struct _OGDEventPrivate OGDEventPrivate;

struct _OGDEvent {
    OGDObject               parent;
    OGDEventPrivate         *priv;
};

struct _OGDEventClass {
    OGDObjectClass          parent_class;
};

/**
 * OGD_EVENT_CATEGORY:
 * @OGD_EVENT_UNDEFINED:            any type has been specified
 * @OGD_EVENT_PARTY:                #OGDEvent is a party
 * @OGD_EVENT_USER_GROUP:           #OGDEvent is related to a group of users
 * @OGD_EVENT_CONFERENCE:           #OGDEvent is a conference
 * @OGD_EVENT_DEV_MEETING:          #OGDEvent is a meeting among software developers
 * @OGD_EVENT_INSTALL_PARTY:        #OGDEvent is an Installation Party
 * @OGD_EVENT_OTHER_PARTY:          #OGDEvent is some kind of unspecified party
 *
 * Helps to identify type of an event registered on the provider
 */
typedef enum {
    OGD_EVENT_UNDEFINED,
    OGD_EVENT_PARTY,
    OGD_EVENT_USER_GROUP,
    OGD_EVENT_CONFERENCE,
    OGD_EVENT_DEV_MEETING,
    OGD_EVENT_INSTALL_PARTY,
    OGD_EVENT_OTHER_PARTY
} OGD_EVENT_CATEGORY;

GType                   ogd_event_get_type              ();

OGDEvent*               ogd_event_new                   (OGDProvider *provider);
GList*                  ogd_event_fetch_all             (OGDProvider *provider);

const gchar*            ogd_event_get_id                (OGDEvent *event);
const gchar*            ogd_event_get_authorid          (OGDEvent *event);
const gchar*            ogd_event_get_name              (OGDEvent *event);
void                    ogd_event_set_name              (OGDEvent *event, gchar *name);
const gchar*            ogd_event_get_description       (OGDEvent *event);
void                    ogd_event_set_description       (OGDEvent *event, gchar *description);
OGD_EVENT_CATEGORY      ogd_event_get_category          (OGDEvent *event);
void                    ogd_event_set_category          (OGDEvent *event, OGD_EVENT_CATEGORY category);
const GDate*            ogd_event_get_start_date        (OGDEvent *event);
void                    ogd_event_set_start_date        (OGDEvent *event, GDate *date);
const GDate*            ogd_event_get_end_date          (OGDEvent *event);
void                    ogd_event_set_end_date          (OGDEvent *event, GDate *date);
const gchar*            ogd_event_get_organizer         (OGDEvent *event);
void                    ogd_event_set_organizer         (OGDEvent *event, gchar *organizer);
const gchar*            ogd_event_get_location          (OGDEvent *event);
void                    ogd_event_set_location          (OGDEvent *event, gchar *location);
const gchar*            ogd_event_get_city              (OGDEvent *event);
void                    ogd_event_set_city              (OGDEvent *event, gchar *city);
const gchar*            ogd_event_get_country           (OGDEvent *event);
void                    ogd_event_set_country           (OGDEvent *event, gchar *country);
gdouble                 ogd_event_get_longitude         (OGDEvent *event);
void                    ogd_event_set_longitude         (OGDEvent *event, gdouble longitude);
gdouble                 ogd_event_get_latitude          (OGDEvent *event);
void                    ogd_event_set_latitude          (OGDEvent *event, gdouble latitude);
const gchar*            ogd_event_get_homepage          (OGDEvent *event);
void                    ogd_event_set_homepage          (OGDEvent *event, gchar *homepage);
const gchar*            ogd_event_get_telephone         (OGDEvent *event);
void                    ogd_event_set_telephone         (OGDEvent *event, gchar *telephone);
const gchar*            ogd_event_get_fax               (OGDEvent *event);
void                    ogd_event_set_fax               (OGDEvent *event, gchar *fax);
const gchar*            ogd_event_get_email             (OGDEvent *event);
void                    ogd_event_set_email             (OGDEvent *event, gchar *email);
const GDate*            ogd_event_get_changed           (OGDEvent *event);
gulong                  ogd_event_get_num_comments      (OGDEvent *event);
gulong                  ogd_event_get_num_partecipants  (OGDEvent *event);
const gchar*            ogd_event_get_image             (OGDEvent *event);

void                    ogd_event_save                  (OGDEvent *event);
void                    ogd_event_remove                (OGDEvent *event);

G_END_DECLS

#endif /* OGD_EVENT_H */

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
#include "ogd-event.h"
#include "ogd-private-utils.h"

#define OGD_EVENT_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_EVENT_TYPE, OGDEventPrivate))

struct _OGDEventPrivate {
    gchar               *id;
    gchar               *name;
    gchar               *description;
    OGD_EVENT_CATEGORY  category;
    GDate               *startdate;
    GDate               *enddate;
    gchar               *authorid;
    OGDPerson           *author;
    gchar               *organizer;
    gchar               *location;

    /*
        TODO    Integrate some kind of more advanced geolocation API (GeoClue?)
    */
    gchar               *city;
    gchar               *country;
    gdouble             latitude;
    gdouble             longitude;

    gchar               *homepage;
    gchar               *telephone;
    gchar               *fax;
    gchar               *mail;
    GDate               *changed;
    gulong              numcomments;
    gulong              numpartecipants;
    gchar               *image;
};

G_DEFINE_TYPE (OGDEvent, ogd_event, OGD_OBJECT_TYPE);

static void ogd_event_finalize (GObject *obj)
{
    OGDEvent *event;

    event = OGD_EVENT (obj);

    PTR_CHECK_FREE_NULLIFY (event->priv->id);
    PTR_CHECK_FREE_NULLIFY (event->priv->name);
    PTR_CHECK_FREE_NULLIFY (event->priv->description);
    g_date_free (event->priv->startdate);
    g_date_free (event->priv->enddate);
    PTR_CHECK_FREE_NULLIFY (event->priv->authorid);
    OBJ_CHECK_UNREF_NULLIFY (event->priv->author);
    PTR_CHECK_FREE_NULLIFY (event->priv->organizer);
    PTR_CHECK_FREE_NULLIFY (event->priv->location);
    PTR_CHECK_FREE_NULLIFY (event->priv->city);
    PTR_CHECK_FREE_NULLIFY (event->priv->country);
    PTR_CHECK_FREE_NULLIFY (event->priv->homepage);
    PTR_CHECK_FREE_NULLIFY (event->priv->telephone);
    PTR_CHECK_FREE_NULLIFY (event->priv->fax);
    PTR_CHECK_FREE_NULLIFY (event->priv->mail);
    g_date_free (event->priv->changed);
    PTR_CHECK_FREE_NULLIFY (event->priv->image);
}

static gboolean ogd_event_fill_by_xml (OGDObject *obj, const xmlNode *xml, GError **error)
{
    xmlNode *cursor;
    OGDEvent *event;
    xmlChar *tmp;

    event = OGD_EVENT (obj);

    if (strcmp (xml->name, "event") != 0)
        return FALSE;

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        /*
            TODO    Provide optimization for strings comparison
        */

        if (strcmp (cursor->name, "id") == 0)
            event->priv->id = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "name") == 0)
            event->priv->name = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "description") == 0)
            event->priv->description = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "category") == 0) {
            tmp = xmlNodeGetContent (cursor);
            if (strcmp (tmp, "Party") == 0)
                event->priv->category = OGD_EVENT_PARTY;
            else if (strcmp (tmp, "User Group") == 0)
                event->priv->category = OGD_EVENT_USER_GROUP;
            else if (strcmp (tmp, "Conference") == 0)
                event->priv->category = OGD_EVENT_CONFERENCE;
            else if (strcmp (tmp, "Developer Meeting") == 0)
                event->priv->category = OGD_EVENT_DEV_MEETING;
            else if (strcmp (tmp, "Install Party") == 0)
                event->priv->category = OGD_EVENT_INSTALL_PARTY;
            else if (strcmp (tmp, "otherParty") == 0)
                event->priv->category = OGD_EVENT_OTHER_PARTY;
            xmlFree (tmp);
        }
        else if (strcmp (cursor->name, "startdate") == 0)
            event->priv->startdate = node_to_date (cursor);
        else if (strcmp (cursor->name, "enddate") == 0)
            event->priv->enddate = node_to_date (cursor);
        else if (strcmp (cursor->name, "user") == 0)
            event->priv->authorid = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "organizer") == 0)
            event->priv->organizer = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "location") == 0)
            event->priv->location = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "city") == 0)
            event->priv->city = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "country") == 0)
            event->priv->country = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "latitude") == 0)
            event->priv->latitude = node_to_double (cursor);
        else if (strcmp (cursor->name, "longitude") == 0)
            event->priv->longitude = node_to_double (cursor);
        else if (strcmp (cursor->name, "homepage") == 0)
            event->priv->homepage = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "tel") == 0)
            event->priv->telephone = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "fax") == 0)
            event->priv->fax = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "email") == 0)
            event->priv->mail = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "changed") == 0)
            event->priv->changed = node_to_date (cursor);
        else if (strcmp (cursor->name, "comments") == 0)
            event->priv->numcomments = (guint) node_to_num (cursor);
        else if (strcmp (cursor->name, "partecipants") == 0)
            event->priv->numpartecipants = (guint) node_to_num (cursor);
        else if (strcmp (cursor->name, "image") == 0)
            event->priv->image = xmlNodeGetContent (cursor);
    }

    return TRUE;
}

static gboolean ogd_event_fill_by_id (OGDObject *obj, const gchar *id, GError **error)
{
    gchar *query;
    gboolean ret;
    xmlNode *data;

    query = g_strdup_printf ("event/data/%s", id);
    data = ogd_provider_get_raw (ogd_object_get_provider (obj), query);
    g_free (query);

    if (data != NULL) {
        ret = ogd_event_fill_by_xml (obj, data, error);
        xmlFreeDoc (data->doc);
    }
    else
        ret = FALSE;

    return ret;
}

static void ogd_event_class_init (OGDEventClass *klass)
{
    GObjectClass *gobject_class;
    OGDObjectClass *ogd_object_class;

    g_type_class_add_private (klass, sizeof (OGDEventPrivate));

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ogd_event_finalize;

    ogd_object_class = OGD_OBJECT_CLASS (klass);
    ogd_object_class->fill_by_xml = ogd_event_fill_by_xml;
    ogd_object_class->fill_by_id = ogd_event_fill_by_id;
}

static void ogd_event_init (OGDEvent *item)
{
    item->priv = OGD_EVENT_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDEventPrivate));
}

const gchar* ogd_event_get_id (OGDEvent *event)
{
    return (const gchar*) event->priv->id;
}

const gchar* ogd_event_get_name (OGDEvent *event)
{
    return (const gchar*) event->priv->name;
}

const gchar* ogd_event_get_description (OGDEvent *event)
{
    return (const gchar*) event->priv->description;
}

OGD_EVENT_CATEGORY ogd_event_get_category (OGDEvent *event)
{
    return event->priv->category;
}

const GDate* ogd_event_get_start_date (OGDEvent *event)
{
    return event->priv->startdate;
}

const GDate* ogd_event_get_end_date (OGDEvent *event)
{
    return event->priv->enddate;
}

/*
    TODO    Provide also an async version
*/

const OGDPerson* ogd_event_get_author (OGDEvent *event)
{
    if (event->priv->author == NULL) {
        if (event->priv->authorid != NULL && strlen (event->priv->authorid) != 0) {
            OGDPerson *author;

            author = g_object_new (OGD_PERSON_TYPE, NULL);
            ogd_object_set_provider (OGD_OBJECT (author), ogd_object_get_provider (OGD_OBJECT (event)));

            if (ogd_object_fill_by_id (OGD_OBJECT (author), event->priv->authorid, NULL))
                event->priv->author = author;
        }
    }

    return event->priv->author;
}

const gchar* ogd_event_get_organizer (OGDEvent *event)
{
    return (const gchar*) event->priv->organizer;
}

const gchar* ogd_event_get_location (OGDEvent *event)
{
    return (const gchar*) event->priv->location;
}

const gchar* ogd_event_get_city (OGDEvent *event)
{
    return (const gchar*) event->priv->city;
}

const gchar* ogd_event_get_country (OGDEvent *event)
{
    return (const gchar*) event->priv->country;
}

gdouble ogd_event_get_longitude (OGDEvent *event)
{
    return event->priv->longitude;
}

gdouble ogd_event_get_latitude (OGDEvent *event)
{
    return event->priv->latitude;
}

const gchar* ogd_event_get_homepage (OGDEvent *event)
{
    return (const gchar*) event->priv->homepage;
}

const gchar* ogd_event_get_telephone (OGDEvent *event)
{
    return (const gchar*) event->priv->telephone;
}

const gchar* ogd_event_get_fax (OGDEvent *event)
{
    return (const gchar*) event->priv->fax;
}

const gchar* ogd_event_get_email (OGDEvent *event)
{
    return (const gchar*) event->priv->mail;
}

const GDate* ogd_event_get_changed (OGDEvent *event)
{
    return event->priv->changed;
}

gulong ogd_event_get_num_comments (OGDEvent *event)
{
    return event->priv->numcomments;
}

gulong ogd_event_get_num_partecipants (OGDEvent *event)
{
    return event->priv->numpartecipants;
}

const gchar* ogd_event_get_image (OGDEvent *event)
{
    return (const gchar*) event->priv->image;
}

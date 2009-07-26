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

#include "ogd.h"
#include "ogd-event.h"
#include "ogd-private-utils.h"

#define OGD_EVENT_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_EVENT_TYPE, OGDEventPrivate))

/**
 * SECTION: ogd-event
 * @short_description:  a registered event
 *
 * An #OGDEvent is an happening which is shared through the social platform: each #OGDPerson may
 * create new events (at the moment, only using the web platform) or attendee a specific one
 */

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
    DATE_CHECK_FREE_NULLIFY (event->priv->startdate);
    DATE_CHECK_FREE_NULLIFY (event->priv->enddate);
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
    DATE_CHECK_FREE_NULLIFY (event->priv->changed);
    PTR_CHECK_FREE_NULLIFY (event->priv->image);
}

static gboolean ogd_event_fill_by_xml (OGDObject *obj, const xmlNode *xml, GError **error)
{
    xmlNode *cursor;
    OGDEvent *event;
    xmlChar *tmp;

    event = OGD_EVENT (obj);

    if (MYSTRCMP (xml->name, "event") != 0)
        return FALSE;

    ogd_event_finalize (G_OBJECT (obj));

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        if (MYSTRCMP (cursor->name, "id") == 0)
            event->priv->id = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "name") == 0)
            event->priv->name = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "description") == 0)
            event->priv->description = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "category") == 0) {
            tmp = xmlNodeGetContent (cursor);
            if (MYSTRCMP (tmp, "Party") == 0)
                event->priv->category = OGD_EVENT_PARTY;
            else if (MYSTRCMP (tmp, "User Group") == 0)
                event->priv->category = OGD_EVENT_USER_GROUP;
            else if (MYSTRCMP (tmp, "Conference") == 0)
                event->priv->category = OGD_EVENT_CONFERENCE;
            else if (MYSTRCMP (tmp, "Developer Meeting") == 0)
                event->priv->category = OGD_EVENT_DEV_MEETING;
            else if (MYSTRCMP (tmp, "Install Party") == 0)
                event->priv->category = OGD_EVENT_INSTALL_PARTY;
            else if (MYSTRCMP (tmp, "otherParty") == 0)
                event->priv->category = OGD_EVENT_OTHER_PARTY;
            xmlFree (tmp);
        }
        else if (MYSTRCMP (cursor->name, "startdate") == 0)
            event->priv->startdate = node_to_date (cursor);
        else if (MYSTRCMP (cursor->name, "enddate") == 0)
            event->priv->enddate = node_to_date (cursor);
        else if (MYSTRCMP (cursor->name, "user") == 0)
            event->priv->authorid = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "organizer") == 0)
            event->priv->organizer = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "location") == 0)
            event->priv->location = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "city") == 0)
            event->priv->city = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "country") == 0)
            event->priv->country = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "latitude") == 0)
            event->priv->latitude = node_to_double (cursor);
        else if (MYSTRCMP (cursor->name, "longitude") == 0)
            event->priv->longitude = node_to_double (cursor);
        else if (MYSTRCMP (cursor->name, "homepage") == 0)
            event->priv->homepage = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "tel") == 0)
            event->priv->telephone = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "fax") == 0)
            event->priv->fax = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "email") == 0)
            event->priv->mail = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "changed") == 0)
            event->priv->changed = node_to_date (cursor);
        else if (MYSTRCMP (cursor->name, "comments") == 0)
            event->priv->numcomments = (guint) node_to_num (cursor);
        else if (MYSTRCMP (cursor->name, "partecipants") == 0)
            event->priv->numpartecipants = (guint) node_to_num (cursor);
        else if (MYSTRCMP (cursor->name, "image") == 0)
            event->priv->image = MYGETCONTENT (cursor);
    }

    return TRUE;
}

static gboolean ogd_event_fill_by_id (OGDObject *obj, const gchar *id, GError **error)
{
    gchar *query;
    gboolean ret;
    xmlNode *data;

    query = g_strdup_printf ("event/data/%s", id);
    data = ogd_provider_get_raw ((OGDProvider*) ogd_object_get_provider (obj), query);
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

/**
 * ogd_event_get_id:
 * @event:          the #OGDEvent to query
 *
 * To obtain ID of an @event
 * 
 * Return value:    ID of the target @event
 */
const gchar* ogd_event_get_id (OGDEvent *event)
{
    return (const gchar*) event->priv->id;
}

/**
 * ogd_event_get_name:
 * @event:          the #OGDEvent to query
 *
 * To obtain name of an @event
 * 
 * Return value:    name of the target @event
 */
const gchar* ogd_event_get_name (OGDEvent *event)
{
    return (const gchar*) event->priv->name;
}

/**
 * ogd_event_get_description:
 * @event:          the #OGDEvent to query
 *
 * To obtain description of an @event
 * 
 * Return value:    description of the target @event
 */
const gchar* ogd_event_get_description (OGDEvent *event)
{
    return (const gchar*) event->priv->description;
}

/**
 * ogd_event_get_category:
 * @event:          the #OGDEvent to query
 *
 * To obtain category of an @event
 * 
 * Return value:    identifier of the category of the target @event
 */
OGD_EVENT_CATEGORY ogd_event_get_category (OGDEvent *event)
{
    return event->priv->category;
}

/**
 * ogd_event_get_start_date:
 * @event:          the #OGDEvent to query
 *
 * To obtain the date in which @event starts
 * 
 * Return value:    a #GDate for the starting date of the target @event
 */
const GDate* ogd_event_get_start_date (OGDEvent *event)
{
    return event->priv->startdate;
}

/**
 * ogd_event_get_end_date:
 * @event:          the #OGDEvent to query
 *
 * To obtain the date in which @event ends
 * 
 * Return value:    a #GDate for the ending date of the target @event
 */
const GDate* ogd_event_get_end_date (OGDEvent *event)
{
    return event->priv->enddate;
}

/**
 * ogd_event_get_author:
 * @event:          the #OGDEvent to query
 *
 * To obtain author of an @event
 * 
 * Return value:    #OGDPerson rappresenting the author of the target @event. Please note that
 *                  this call requires some syncronous communication with the server, so may
 *                  return after some time
 */

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

/**
 * ogd_event_get_organizer:
 * @event:          the #OGDEvent to query
 *
 * To obtain the organizer of @event
 * 
 * Return value:    a string summarizing the organizer of the target @event. Please note this
 *                  content is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_event_get_organizer (OGDEvent *event)
{
    return (const gchar*) event->priv->organizer;
}

/**
 * ogd_event_get_location:
 * @event:          the #OGDEvent to query
 *
 * To obtain the location of @event
 * 
 * Return value:    a string summarizing the location of the target @event. Please note this
 *                  content is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_event_get_location (OGDEvent *event)
{
    return (const gchar*) event->priv->location;
}

/**
 * ogd_event_get_city:
 * @event:          the #OGDEvent to query
 *
 * To obtain the city in which of @event happens
 * 
 * Return value:    a string summarizing the city of the target @event. Please note this content
 *                  is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_event_get_city (OGDEvent *event)
{
    return (const gchar*) event->priv->city;
}

/**
 * ogd_event_get_country:
 * @event:          the #OGDEvent to query
 *
 * To obtain the country in which of @event happens
 * 
 * Return value:    a string summarizing the country of the target @event. Please note this
 *                  content is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_event_get_country (OGDEvent *event)
{
    return (const gchar*) event->priv->country;
}

/**
 * ogd_event_get_longitude:
 * @event:          the #OGDEvent to query
 *
 * To obtain the longitude of @event location
 * 
 * Return value:    coordinates longitude of the target @event
 */
gdouble ogd_event_get_longitude (OGDEvent *event)
{
    return event->priv->longitude;
}

/**
 * ogd_event_get_latitude:
 * @event:          the #OGDEvent to query
 *
 * To obtain the latitude of @event location
 * 
 * Return value:    coordinates latitude of the target @event
 */
gdouble ogd_event_get_latitude (OGDEvent *event)
{
    return event->priv->latitude;
}

/**
 * ogd_event_get_homepage:
 * @event:          the #OGDEvent to query
 *
 * To obtain the homepage of @event
 * 
 * Return value:    URL of the homepage describing the target @event
 */
const gchar* ogd_event_get_homepage (OGDEvent *event)
{
    return (const gchar*) event->priv->homepage;
}

/**
 * ogd_event_get_telephone:
 * @event:          the #OGDEvent to query
 *
 * To obtain the telephone number dedicated to the @event
 * 
 * Return value:    a string containing the telephone number of the target @event. Please note
 *                  this content is not semantically validated, so cannot be programmatically
 *                  managed
 */
const gchar* ogd_event_get_telephone (OGDEvent *event)
{
    return (const gchar*) event->priv->telephone;
}

/**
 * ogd_event_get_fax:
 * @event:          the #OGDEvent to query
 *
 * To obtain the fax number dedicated to the @event
 * 
 * Return value:    a string containing the fax number of the target @event. Please note this
 *                  content is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_event_get_fax (OGDEvent *event)
{
    return (const gchar*) event->priv->fax;
}

/**
 * ogd_event_get_email:
 * @event:          the #OGDEvent to query
 *
 * To obtain the mail address dedicated to the @event
 * 
 * Return value:    a string containing the mail address of the target @event. Please note
 *                  this content is not semantically validated, so cannot be programmatically
 *                  managed
 */
const gchar* ogd_event_get_email (OGDEvent *event)
{
    return (const gchar*) event->priv->mail;
}

/**
 * ogd_event_get_changed:
 * @event:          the #OGDEvent to query
 *
 * To obtain the latest change date of @event
 * 
 * Return value:    a #GDate for the latest change date of the target @event
 */
const GDate* ogd_event_get_changed (OGDEvent *event)
{
    return event->priv->changed;
}

/**
 * ogd_event_get_num_comments:
 * @event:          the #OGDEvent to query
 *
 * To obtain the number of comments for @event. Comments are readable in the dedicated homepage
 * (see ogd_event_get_homepage())
 * 
 * Return value:    number of comments assigned to the target @event
 */
gulong ogd_event_get_num_comments (OGDEvent *event)
{
    return event->priv->numcomments;
}

/**
 * ogd_event_get_num_partecipants:
 * @event:          the #OGDEvent to query
 *
 * To obtain the number of partecipants to the @event
 * 
 * Return value:    number of partecipants to the target @event
 */
gulong ogd_event_get_num_partecipants (OGDEvent *event)
{
    return event->priv->numpartecipants;
}

/**
 * ogd_event_get_image:
 * @event:          the #OGDEvent to query
 *
 * To obtain the URL of the image attached to the @event
 * 
 * Return value:    URL of the image assigned to the target @event
 */
const gchar* ogd_event_get_image (OGDEvent *event)
{
    return (const gchar*) event->priv->image;
}

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

#include "ogd.h"
#include "ogd-event.h"
#include "ogd-provider-private.h"
#include "ogd-private-utils.h"

#define OGD_EVENT_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_EVENT_TYPE, OGDEventPrivate))

#define SET_STRING(__event, __field, __value) {             \
    if (check_ownership (__event) == FALSE) {               \
        g_warning ("No permissions to edit the event.");    \
        return;                                             \
    }                                                       \
                                                            \
    PTR_CHECK_FREE_NULLIFY (__event->priv->__field);        \
    __event->priv->__field = g_strdup (__value);            \
}

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

    if (MYSTRCMP (xml->name, "event") != 0) {
        g_set_error (error, OGD_PARSING_ERROR_DOMAIN, OGD_XML_ERROR, "Invalid XML for OGDEvent");
        return FALSE;
    }

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

static gchar* ogd_event_target_query (const gchar *id)
{
    return g_strdup_printf ("event/data/%s", id);
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
    ogd_object_class->target_query = ogd_event_target_query;
}

static void ogd_event_init (OGDEvent *item)
{
    item->priv = OGD_EVENT_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDEventPrivate));
}

static gboolean check_ownership (OGDEvent *event)
{
    const gchar *owner;
    static const gchar *myself = NULL;

    if (ogd_event_get_id (event) == NULL)
        return TRUE;

    owner = ogd_event_get_authorid (event);

    if (myself == NULL)
        myself = ogd_person_get_id (ogd_person_get_myself (ogd_object_get_provider (OGD_OBJECT (event))));

    return (strcmp (owner, myself) == 0);
}

/**
 * ogd_event_new:
 * @provider:       reference provider for the new event
 *
 * Creates an empty event suitable to be filled and saved with ogd_event_save()
 *
 * Return value:    ID of the target @event
 */
OGDEvent* ogd_event_new (OGDProvider *provider)
{
    OGDEvent *ret;

    ret = g_object_new (OGD_EVENT_TYPE, NULL);
    ogd_object_set_provider (OGD_OBJECT (ret), provider);
    return ret;
}

/**
 * ogd_event_fetch_all:
 *
 * Return value:
 */
GList* ogd_event_fetch_all (OGDProvider *provider)
{
    return ogd_provider_get (provider, "event/data");
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
 * ogd_event_get_authorid:
 * @event:          the #OGDEvent to query
 *
 * To obtain author of an @event
 *
 * Return value:    identifiers of the #OGDPerson who created the event. You can use
                    ogd_object_fill_by_id() to obtain all informations
 */
const gchar* ogd_event_get_authorid (OGDEvent *event)
{
    return (const gchar*) event->priv->authorid;
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
 * ogd_event_set_name:
 * @event:          the #OGDEvent to edit
 * @title:          new name for the event
 *
 * Sets a name to the @event. Can be used only if the current user has permission to edit the
 * element
 */
void ogd_event_set_name (OGDEvent *event, gchar *title)
{
    SET_STRING (event, name, title);
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
 * ogd_event_set_name:
 * @event:          the #OGDEvent to edit
 * @description:    new description for the event
 *
 * Sets a description to the @event. Can be used only if the current user has permission to edit
 * the element
 */
void ogd_event_set_description (OGDEvent *event, gchar *description)
{
    SET_STRING (event, description, description);
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
 * ogd_event_set_category:
 * @event:          the #OGDEvent to edit
 * @cat:            new category for the event
 *
 * Sets a category to the @event. Can be used only if the current user has permission to edit the
 * element
 */
void ogd_event_set_category (OGDEvent *event, OGD_EVENT_CATEGORY cat)
{
    if (check_ownership (event) == FALSE) {
        g_warning ("No permissions to edit the event.");
        return;
    }

    event->priv->category = cat;
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
 * ogd_event_set_start_date:
 * @event:          the #OGDEvent to edit
 * @cat:            new starting date for the event
 *
 * Sets a start date to the @event. Can be used only if the current user has permission to edit
 * the element
 */
void ogd_event_set_start_date (OGDEvent *event, GDate *date)
{
    if (check_ownership (event) == FALSE) {
        g_warning ("No permissions to edit the event.");
        return;
    }

    DATE_CHECK_FREE_NULLIFY (event->priv->startdate);
    event->priv->startdate = date;
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
 * ogd_event_set_end_date:
 * @event:          the #OGDEvent to edit
 * @cat:            new ending date for the event
 *
 * Sets a end date to the @event. Can be used only if the current user has permission to edit the
 * element
 */
void ogd_event_set_end_date (OGDEvent *event, GDate *date)
{
    if (check_ownership (event) == FALSE) {
        g_warning ("No permissions to edit the event.");
        return;
    }

    DATE_CHECK_FREE_NULLIFY (event->priv->enddate);
    event->priv->enddate = date;
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
 * ogd_event_set_organizer:
 * @event:          the #OGDEvent to edit
 * @organizer:      new organizer for the event
 *
 * Sets an organizer to the @event. Can be used only if the current user has permission to edit
 * the element
 */
void ogd_event_set_organizer (OGDEvent *event, gchar *organizer)
{
    SET_STRING (event, organizer, organizer);
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
 * ogd_event_set_location:
 * @event:          the #OGDEvent to edit
 * @location:       new location for the event
 *
 * Sets the location to the @event. Can be used only if the current user has permission to edit
 * the element
 */
void ogd_event_set_location (OGDEvent *event, gchar *location)
{
    SET_STRING (event, location, location);
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
 * ogd_event_set_city:
 * @event:          the #OGDEvent to edit
 * @city:           new city for the event
 *
 * Sets the city to the @event. Can be used only if the current user has permission to edit the
 * element
 */
void ogd_event_set_city (OGDEvent *event, gchar *city)
{
    SET_STRING (event, city, city);
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
 * ogd_event_set_country:
 * @event:          the #OGDEvent to edit
 * @country:        new country for the event
 *
 * Sets the country to the @event. Can be used only if the current user has permission to edit the
 * element
 */
void ogd_event_set_country (OGDEvent *event, gchar *country)
{
    SET_STRING (event, country, country);
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
 * ogd_event_set_longitude:
 * @event:          the #OGDEvent to edit
 * @longitude:      new longitude for the event
 *
 * Sets the longitude to the @event. Can be used only if the current user has permission to edit
 * the element
 */
void ogd_event_set_longitude (OGDEvent *event, gdouble longitude)
{
    if (check_ownership (event) == FALSE) {
        g_warning ("No permissions to edit the event.");
        return;
    }

    event->priv->longitude = longitude;
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
 * ogd_event_set_latitude:
 * @event:          the #OGDEvent to edit
 * @latitude:       new latitude for the event
 *
 * Sets the latitude to the @event. Can be used only if the current user has permission to edit
 * the element
 */
void ogd_event_set_latitude (OGDEvent *event, gdouble latitude)
{
    if (check_ownership (event) == FALSE) {
        g_warning ("No permissions to edit the event.");
        return;
    }

    event->priv->latitude = latitude;
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
 * ogd_event_set_homepage:
 * @event:          the #OGDEvent to edit
 * @homepage:       new homepage for the event
 *
 * Sets the homepage to the @event. Can be used only if the current user has permission to edit
 * the element
 */
void ogd_event_set_homepage (OGDEvent *event, gchar *homepage)
{
    SET_STRING (event, homepage, homepage);
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
 * ogd_event_set_telephone:
 * @event:          the #OGDEvent to edit
 * @telephone:      new telephone number for the event
 *
 * Sets the telephone number to the @event. Can be used only if the current user has permission
 * to edit the element
 */
void ogd_event_set_telephone (OGDEvent *event, gchar *telephone)
{
    SET_STRING (event, telephone, telephone);
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
 * ogd_event_set_fax:
 * @event:          the #OGDEvent to edit
 * @fax:            new fax number for the event
 *
 * Sets the fax number to the @event. Can be used only if the current user has permission to edit
 * the element
 */
void ogd_event_set_fax (OGDEvent *event, gchar *fax)
{
    SET_STRING (event, fax, fax);
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
 * ogd_event_set_mail:
 * @event:          the #OGDEvent to edit
 * @mail:           new mail for the event
 *
 * Sets the fax number to the @event. Can be used only if the current user has permission to edit
 * the element
 */
void ogd_event_set_mail (OGDEvent *event, gchar *mail)
{
    SET_STRING (event, mail, mail);
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

static int category_to_identifier (OGD_EVENT_CATEGORY category)
{
    switch (category) {
        case OGD_EVENT_PARTY:
            return 10;
        case OGD_EVENT_USER_GROUP:
            return 15;
        case OGD_EVENT_CONFERENCE:
            return 20;
        case OGD_EVENT_DEV_MEETING:
            return 25;
        case OGD_EVENT_INSTALL_PARTY:
            return 50;
        default:
            return 1000;
    }
}

/**
 * ogd_event_save:
 * @event:          a new #OGDEvent to be saved on the provider, or an existing event to edit
 *
 * To save a new or edited event. The function fails if trying to edit an event not owned by the
 * current user
 */
void ogd_event_save (OGDEvent *event)
{
    const gchar *id;
    gchar *query;
    gchar *tmp;
    const gchar *str;
    gdouble coord;
    struct tm tm;
    OGD_EVENT_CATEGORY cat;
    const GDate *date;
    GHashTable *data;
    xmlNode *response;
    xmlNode *inner_response;

    if (check_ownership (event) == FALSE) {
        g_warning ("No permissions to edit the event.");
        return;
    }

    str = ogd_event_get_name (event);
    if (str == NULL) {
        g_warning ("A name for the event is mandatory.");
        return;
    }

    data = g_hash_table_new (g_str_hash, g_str_equal);

    g_hash_table_insert (data, "name", (gchar*) str);

    str = ogd_event_get_description (event);
    if (str != NULL)
        g_hash_table_insert (data, "description", (gchar*) str);

    cat = ogd_event_get_category (event);
    tmp = (gchar*) alloca (100);
    snprintf (tmp, 100, "%d", category_to_identifier (cat));
    g_hash_table_insert (data, "category", tmp);

    date = ogd_event_get_start_date (event);
    if (date != NULL) {
        g_date_to_struct_tm (date, &tm);
        tmp = (gchar*) alloca (100);
        strftime (tmp, 100, "%Y-%m-%dT%H:%M:%S", &tm);
        g_hash_table_insert (data, "startdate", tmp);
    }

    date = ogd_event_get_end_date (event);
    if (date != NULL) {
        g_date_to_struct_tm (date, &tm);
        tmp = (gchar*) alloca (100);
        strftime (tmp, 100, "%Y-%m-%dT%H:%M:%S", &tm);
        g_hash_table_insert (data, "enddate", tmp);
    }

    str = ogd_event_get_organizer (event);
    if (str != NULL)
        g_hash_table_insert (data, "organizer", (gchar*) str);

    str = ogd_event_get_location (event);
    if (str != NULL)
        g_hash_table_insert (data, "location", (gchar*) str);

    str = ogd_event_get_city (event);
    if (str != NULL)
        g_hash_table_insert (data, "city", (gchar*) str);

    str = ogd_event_get_country (event);
    if (str != NULL)
        g_hash_table_insert (data, "country", (gchar*) str);

    coord = ogd_event_get_longitude (event);
    tmp = (gchar*) alloca (100);
    snprintf (tmp, 100, "%.03f", coord);
    g_hash_table_insert (data, "longitude", tmp);

    coord = ogd_event_get_latitude (event);
    tmp = (gchar*) alloca (100);
    snprintf (tmp, 100, "%.03f", coord);
    g_hash_table_insert (data, "latitude", tmp);

    str = ogd_event_get_homepage (event);
    if (str != NULL)
        g_hash_table_insert (data, "homepage", (gchar*) str);

    str = ogd_event_get_telephone (event);
    if (str != NULL)
        g_hash_table_insert (data, "tel", (gchar*) str);

    str = ogd_event_get_fax (event);
    if (str != NULL)
        g_hash_table_insert (data, "fax", (gchar*) str);

    str = ogd_event_get_email (event);
    if (str != NULL)
        g_hash_table_insert (data, "email", (gchar*) str);

    id = ogd_event_get_id (event);

    if (id == NULL) {
        response = ogd_provider_put_raw (ogd_object_get_provider (OGD_OBJECT (event)), "event/add", data);
        if (response != NULL) {
            if (MYSTRCMP (response->name, "data") == 0 && response->children != NULL &&
                    MYSTRCMP (response->children->name, "event") == 0 && response->children->children != NULL &&
                    MYSTRCMP (response->children->children->name, "id") == 0) {

                inner_response = response->children->children;
                tmp = (gchar*) MYGETCONTENT (inner_response);
                event->priv->id = g_strdup (tmp);
                xmlFree (tmp);
            }
            else {
                g_warning ("An error occurred while retriving ID for newly created event.");
            }

            xmlFreeDoc (response->doc);
        }
    }
    else {
        query = g_strdup_printf ("event/edit/%s", id);
        ogd_provider_put (ogd_object_get_provider (OGD_OBJECT (event)), query, data);
        g_free (query);
    }

    g_hash_table_unref (data);
}

/**
 * ogd_event_remove:
 * @event:          the #OGDEvent to remove
 *
 * Removes an existing event. The function fails if trying to edit an event not owned by the
 * current user
 */
void ogd_event_remove (OGDEvent *event)
{
    const gchar *id;
    gchar *query;

    if (check_ownership (event) == FALSE) {
        g_warning ("No permissions to edit the event.");
        return;
    }

    id = ogd_event_get_id (event);

    if (id != NULL) {
        query = g_strdup_printf ("event/delete/%s", id);
        ogd_provider_put (ogd_object_get_provider (OGD_OBJECT (event)), query, NULL);
        g_free (query);
    }
}

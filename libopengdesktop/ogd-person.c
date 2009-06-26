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
#include "ogd-person.h"
#include "ogd-private-utils.h"

#define OGD_PERSON_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_PERSON_TYPE, OGDPersonPrivate))

struct _OGDPersonPrivate {
    gchar               *id;
    guint               privacy;
    gchar               *firstname;
    gchar               *lastname;
    OGD_PERSON_GENDER   gender;
    OGD_PERSON_ROLE     role;
    gchar               *homepage;
    gchar               *company;
    gchar               *avatar;
    GDate               *birthday;
    OGD_PERSON_JOB      jobstatus;

    /*
        TODO    Integrate some kind of more advanced geolocation API (GeoClue?)
    */
    gchar               *city;
    gchar               *country;
    gdouble             latitude;
    gdouble             longitude;

    gchar               *likes;
    gchar               *dontlikes;
    gchar               *interests;
    gchar               *languages;
    gchar               *programminglangs;
    gchar               *favouritequote;
    gchar               *favouritemusic;
    gchar               *favouritetv;
    gchar               *favouritemovies;
    gchar               *favouritebooks;
    gchar               *favouritegames;
    gchar               *description;
    gchar               *profilepage;
};

G_DEFINE_TYPE (OGDPerson, ogd_person, OGD_OBJECT_TYPE);

static void ogd_person_finalize (GObject *obj)
{
    OGDPerson *person;

    person = OGD_PERSON (obj);

    PTR_CHECK_FREE_NULLIFY (person->priv->id);
    PTR_CHECK_FREE_NULLIFY (person->priv->firstname);
    PTR_CHECK_FREE_NULLIFY (person->priv->lastname);
    PTR_CHECK_FREE_NULLIFY (person->priv->homepage);
    PTR_CHECK_FREE_NULLIFY (person->priv->company);
    PTR_CHECK_FREE_NULLIFY (person->priv->avatar);
    g_date_free (person->priv->birthday);
    PTR_CHECK_FREE_NULLIFY (person->priv->city);
    PTR_CHECK_FREE_NULLIFY (person->priv->country);
    PTR_CHECK_FREE_NULLIFY (person->priv->likes);
    PTR_CHECK_FREE_NULLIFY (person->priv->dontlikes);
    PTR_CHECK_FREE_NULLIFY (person->priv->interests);
    PTR_CHECK_FREE_NULLIFY (person->priv->languages);
    PTR_CHECK_FREE_NULLIFY (person->priv->programminglangs);
    PTR_CHECK_FREE_NULLIFY (person->priv->favouritequote);
    PTR_CHECK_FREE_NULLIFY (person->priv->favouritemusic);
    PTR_CHECK_FREE_NULLIFY (person->priv->favouritetv);
    PTR_CHECK_FREE_NULLIFY (person->priv->favouritemovies);
    PTR_CHECK_FREE_NULLIFY (person->priv->favouritebooks);
    PTR_CHECK_FREE_NULLIFY (person->priv->favouritegames);
    PTR_CHECK_FREE_NULLIFY (person->priv->description);
    PTR_CHECK_FREE_NULLIFY (person->priv->profilepage);
}

static gboolean ogd_person_fill_by_xml (OGDObject *obj, const xmlNode *xml, GError **error)
{
    xmlNode *cursor;
    OGDPerson *person;
    xmlChar *tmp;

    person = OGD_PERSON (obj);

    if (strcmp (xml->name, "person") != 0)
        return FALSE;

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        /*
            TODO    Provide optimization for strings comparison
        */

        if (strcmp (cursor->name, "id") == 0)
            person->priv->id = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "privacy") == 0) {
            tmp = xmlNodeGetContent (cursor);
            person->priv->privacy = g_ascii_digit_value (tmp [0]);
            xmlFree (tmp);
        }
        else if (strcmp (cursor->name, "firstname") == 0)
            person->priv->firstname = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "lastname") == 0)
            person->priv->lastname = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "gender") == 0) {
            tmp = xmlNodeGetContent (cursor);
            if (strcmp (tmp, "man") == 0)
                person->priv->gender = OGD_PERSON_MAN;
            else if (strcmp (tmp, "woman") == 0)
                person->priv->gender = OGD_PERSON_WOMAN;
            xmlFree (tmp);
        }
        else if (strcmp (cursor->name, "communityrole") == 0) {
            tmp = xmlNodeGetContent (cursor);
            if (strcmp (tmp, "user") == 0)
                person->priv->role = OGD_PERSON_USER;
            else if (strcmp (tmp, "developer") == 0)
                person->priv->role = OGD_PERSON_DEVELOPER;
            else if (strcmp (tmp, "artist") == 0)
                person->priv->role = OGD_PERSON_ARTIST;
            else if (strcmp (tmp, "supporter") == 0)
                person->priv->role = OGD_PERSON_SUPPORTER;
            xmlFree (tmp);
        }
        else if (strcmp (cursor->name, "homepage") == 0)
            person->priv->homepage = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "company") == 0)
            person->priv->company = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "bigavatarpic") == 0)
            person->priv->avatar = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "birthday") == 0)
            person->priv->birthday = node_to_date (cursor);
        else if (strcmp (cursor->name, "jobstatus") == 0) {
            tmp = xmlNodeGetContent (cursor);
            if (strcmp (tmp, "working") == 0)
                person->priv->jobstatus = OGD_PERSON_JOB_WORKING;
            else if (strcmp (tmp, "student") == 0)
                person->priv->jobstatus = OGD_PERSON_JOB_STUDENT;
            else if (strcmp (tmp, "looking for a job") == 0)
                person->priv->jobstatus = OGD_PERSON_JOB_LOOKING;
            else if (strcmp (tmp, "retired") == 0)
                person->priv->jobstatus = OGD_PERSON_JOB_RETIRED;
            else if (strcmp (tmp, "free") == 0)
                person->priv->jobstatus = OGD_PERSON_JOB_FREE;
            xmlFree (tmp);
        }
        else if (strcmp (cursor->name, "city") == 0)
            person->priv->city = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "country") == 0)
            person->priv->country = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "latitude") == 0)
            person->priv->latitude = node_to_double (cursor);
        else if (strcmp (cursor->name, "longitude") == 0)
            person->priv->longitude = node_to_double (cursor);
        else if (strcmp (cursor->name, "likes") == 0)
            person->priv->likes = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "dontlikes") == 0)
            person->priv->dontlikes = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "interests") == 0)
            person->priv->interests = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "languages") == 0)
            person->priv->languages = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "programminglanguages") == 0)
            person->priv->programminglangs = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "favouritequote") == 0)
            person->priv->favouritequote = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "favouritemusic") == 0)
            person->priv->favouritemusic = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "favouritetvshows") == 0)
            person->priv->favouritetv = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "favouritemovies") == 0)
            person->priv->favouritemovies = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "favouritebooks") == 0)
            person->priv->favouritebooks = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "favouritegames") == 0)
            person->priv->favouritegames = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "description") == 0)
            person->priv->description = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "profilepage") == 0)
            person->priv->profilepage = xmlNodeGetContent (cursor);
    }

    return TRUE;
}

static gboolean ogd_person_fill_by_id (OGDObject *obj, const gchar *id, GError **error)
{
    gchar *query;
    gboolean ret;
    xmlNode *data;

    query = g_strdup_printf ("person/data/%s", id);
    data = ogd_provider_get_raw (ogd_object_get_provider (obj), query);
    g_free (query);

    if (data != NULL) {
        ret = ogd_person_fill_by_xml (obj, data, error);
        xmlFreeDoc (data->doc);
    }
    else
        ret = FALSE;

    return ret;
}

static void ogd_person_class_init (OGDPersonClass *klass)
{
    GObjectClass *gobject_class;
    OGDObjectClass *ogd_object_class;

    g_type_class_add_private (klass, sizeof (OGDPersonPrivate));

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ogd_person_finalize;

    ogd_object_class = OGD_OBJECT_CLASS (klass);
    ogd_object_class->fill_by_xml = ogd_person_fill_by_xml;
    ogd_object_class->fill_by_id = ogd_person_fill_by_id;
}

static void ogd_person_init (OGDPerson *item)
{
    item->priv = OGD_PERSON_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDPersonPrivate));
}

const gchar* ogd_person_get_id (OGDPerson *person)
{
    return (const gchar*) person->priv->id;
}

guint ogd_person_get_privacy (OGDPerson *person)
{
    return person->priv->privacy;
}

const gchar* ogd_person_get_firstname (OGDPerson *person)
{
    return (const gchar*) person->priv->firstname;
}

const gchar* ogd_person_get_lastname (OGDPerson *person)
{
    return (const gchar*) person->priv->lastname;
}

OGD_PERSON_GENDER ogd_person_get_gender (OGDPerson *person)
{
    return person->priv->gender;
}

OGD_PERSON_ROLE ogd_person_get_communityrole (OGDPerson *person)
{
    return person->priv->role;
}

const gchar* ogd_person_get_homepage (OGDPerson *person)
{
    return (const gchar*) person->priv->homepage;
}

const gchar* ogd_person_get_company (OGDPerson *person) {
    return (const gchar*) person->priv->company;
}

/*
    TODO    Provide also ogd_person_get_avatar_pixbuf()
*/

const gchar* ogd_person_get_avatar (OGDPerson *person)
{
    return (const gchar*) person->priv->avatar;
}

const GDate* ogd_person_get_birthday (OGDPerson *person)
{
    return (const GDate*) person->priv->birthday;
}

OGD_PERSON_JOB ogd_person_get_job_status (OGDPerson *person)
{
    return person->priv->jobstatus;
}

const gchar* ogd_person_get_city (OGDPerson *person)
{
    return (const gchar*) person->priv->city;
}

const gchar* ogd_person_get_country (OGDPerson *person)
{
    return (const gchar*) person->priv->country;
}

gdouble ogd_person_get_latitude (OGDPerson *person)
{
    return person->priv->latitude;
}

gdouble ogd_person_get_longitude (OGDPerson *person)
{
    return person->priv->longitude;
}

/*
    TODO    IRC is missing, some kind of advanced management is required
*/

const gchar* ogd_person_get_likes (OGDPerson *person)
{
    return (const gchar*) person->priv->likes;
}

const gchar* ogd_person_get_dont_likes (OGDPerson *person)
{
    return (const gchar*) person->priv->dontlikes;
}

const gchar* ogd_person_get_interests (OGDPerson *person)
{
    return (const gchar*) person->priv->interests;
}

const gchar* ogd_person_get_languages (OGDPerson *person)
{
    return (const gchar*) person->priv->languages;
}

const gchar* ogd_person_get_programming_langs (OGDPerson *person)
{
    return (const gchar*) person->priv->programminglangs;
}

const gchar* ogd_person_get_favourite_quote (OGDPerson *person)
{
    return (const gchar*) person->priv->favouritequote;
}

const gchar* ogd_person_get_favourite_music (OGDPerson *person)
{
    return (const gchar*) person->priv->favouritemusic;
}

const gchar* ogd_person_get_favourite_tv (OGDPerson *person)
{
    return (const gchar*) person->priv->favouritetv;
}

const gchar* ogd_person_get_favourite_movies (OGDPerson *person)
{
    return (const gchar*) person->priv->favouritemovies;
}

const gchar* ogd_person_get_favourite_books (OGDPerson *person)
{
    return (const gchar*) person->priv->favouritebooks;
}

const gchar* ogd_person_get_favourite_games (OGDPerson *person)
{
    return (const gchar*) person->priv->favouritegames;
}

const gchar* ogd_person_get_description (OGDPerson *person)
{
    return (const gchar*) person->priv->description;
}

const gchar* ogd_person_get_profile_page (OGDPerson *person)
{
    return (const gchar*) person->priv->profilepage;
}

/*
    TODO    Provide also an async version
*/

const OGDPersonBalance* ogd_person_get_balance (OGDPerson *person)
{
    /*
        TODO
    */

    return NULL;
}

/*
    TODO    Provide also an async version
*/

GList* ogd_person_get_friends (OGDPerson *person)
{
    /*
        TODO
    */

    return NULL;
}

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
#include "ogd-person.h"
#include "ogd-provider-private.h"
#include "ogd-private-utils.h"

#define OGD_PERSON_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_PERSON_TYPE, OGDPersonPrivate))

/**
 * SECTION: ogd-person
 * @short_description:  a user on the platform
 *
 * #OGDPerson describes an user of the target #OGDProvider, and may be a friend or the author of
 * a #OGDContent or a #OGDEvent. Utilities are provided to manage the #OGDPerson rappresenting
 * the user using the application (the one inited with ogd_provider_auth_user_and_pwd()), for
 * which some extra functionality is exposed.
 */

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
    DATE_CHECK_FREE_NULLIFY (person->priv->birthday);
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
    xml = xml->children;

    if (MYSTRCMP (xml->name, "person") != 0)
        return FALSE;

    ogd_person_finalize (G_OBJECT (obj));

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        if (MYSTRCMP (cursor->name, "personid") == 0)
            person->priv->id = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "privacy") == 0) {
            tmp = xmlNodeGetContent (cursor);
            person->priv->privacy = g_ascii_digit_value (tmp [0]);
            xmlFree (tmp);
        }
        else if (MYSTRCMP (cursor->name, "firstname") == 0)
            person->priv->firstname = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "lastname") == 0)
            person->priv->lastname = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "gender") == 0) {
            tmp = xmlNodeGetContent (cursor);
            if (MYSTRCMP (tmp, "man") == 0)
                person->priv->gender = OGD_PERSON_MAN;
            else if (MYSTRCMP (tmp, "woman") == 0)
                person->priv->gender = OGD_PERSON_WOMAN;
            xmlFree (tmp);
        }
        else if (MYSTRCMP (cursor->name, "communityrole") == 0) {
            tmp = xmlNodeGetContent (cursor);
            if (MYSTRCMP (tmp, "user") == 0)
                person->priv->role = OGD_PERSON_USER;
            else if (MYSTRCMP (tmp, "developer") == 0)
                person->priv->role = OGD_PERSON_DEVELOPER;
            else if (MYSTRCMP (tmp, "artist") == 0)
                person->priv->role = OGD_PERSON_ARTIST;
            else if (MYSTRCMP (tmp, "supporter") == 0)
                person->priv->role = OGD_PERSON_SUPPORTER;
            xmlFree (tmp);
        }
        else if (MYSTRCMP (cursor->name, "homepage") == 0)
            person->priv->homepage = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "company") == 0)
            person->priv->company = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "bigavatarpic") == 0)
            person->priv->avatar = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "birthday") == 0)
            person->priv->birthday = node_to_date (cursor);
        else if (MYSTRCMP (cursor->name, "jobstatus") == 0) {
            tmp = xmlNodeGetContent (cursor);
            if (MYSTRCMP (tmp, "working") == 0)
                person->priv->jobstatus = OGD_PERSON_JOB_WORKING;
            else if (MYSTRCMP (tmp, "student") == 0)
                person->priv->jobstatus = OGD_PERSON_JOB_STUDENT;
            else if (MYSTRCMP (tmp, "looking for a job") == 0)
                person->priv->jobstatus = OGD_PERSON_JOB_LOOKING;
            else if (MYSTRCMP (tmp, "retired") == 0)
                person->priv->jobstatus = OGD_PERSON_JOB_RETIRED;
            else if (MYSTRCMP (tmp, "free") == 0)
                person->priv->jobstatus = OGD_PERSON_JOB_FREE;
            xmlFree (tmp);
        }
        else if (MYSTRCMP (cursor->name, "city") == 0)
            person->priv->city = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "country") == 0)
            person->priv->country = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "latitude") == 0)
            person->priv->latitude = node_to_double (cursor);
        else if (MYSTRCMP (cursor->name, "longitude") == 0)
            person->priv->longitude = node_to_double (cursor);
        else if (MYSTRCMP (cursor->name, "likes") == 0)
            person->priv->likes = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "dontlikes") == 0)
            person->priv->dontlikes = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "interests") == 0)
            person->priv->interests = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "languages") == 0)
            person->priv->languages = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "programminglanguages") == 0)
            person->priv->programminglangs = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "favouritequote") == 0)
            person->priv->favouritequote = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "favouritemusic") == 0)
            person->priv->favouritemusic = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "favouritetvshows") == 0)
            person->priv->favouritetv = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "favouritemovies") == 0)
            person->priv->favouritemovies = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "favouritebooks") == 0)
            person->priv->favouritebooks = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "favouritegames") == 0)
            person->priv->favouritegames = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "description") == 0)
            person->priv->description = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "profilepage") == 0)
            person->priv->profilepage = MYGETCONTENT (cursor);
    }

    return TRUE;
}

static gchar* ogd_person_target_query (const gchar *id)
{
    return g_strdup_printf ("person/data/%s", id);
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
    ogd_object_class->target_query = ogd_person_target_query;
}

static void ogd_person_init (OGDPerson *item)
{
    item->priv = OGD_PERSON_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDPersonPrivate));
}

/**
 * ogd_person_get_id:
 * @person:         the #OGDPerson to query
 *
 * To obtain the ID of a @person
 * 
 * Return value:    the @OGDPerson ID
 */
const gchar* ogd_person_get_id (OGDPerson *person)
{
    return (const gchar*) person->priv->id;
}

/**
 * ogd_person_get_privacy:
 * @person:         the #OGDPerson to query
 *
 * Retrieve current privacy settings for a @person
 * 
 * Return value:    identifier of the current settings of privacy
 */
OGD_PERSON_PRIVACY ogd_person_get_privacy (OGDPerson *person)
{
    return person->priv->privacy;
}

/**
 * ogd_person_get_firstname:
 * @person:         the #OGDPerson to query
 *
 * Retrieve firstname of a @person
 * 
 * Return value:    a string containing the firstname for the target @person
 */
const gchar* ogd_person_get_firstname (OGDPerson *person)
{
    return (const gchar*) person->priv->firstname;
}

/**
 * ogd_person_get_lastname:
 * @person:         the #OGDPerson to query
 *
 * Retrieve lastname of a @person
 * 
 * Return value:    a string containing the lastname for the target @person
 */
const gchar* ogd_person_get_lastname (OGDPerson *person)
{
    return (const gchar*) person->priv->lastname;
}

/**
 * ogd_person_get_gender:
 * @person:         the #OGDPerson to query
 *
 * Retrieve gender of a @person
 * 
 * Return value:    identifier of the gender of the target @person
 */
OGD_PERSON_GENDER ogd_person_get_gender (OGDPerson *person)
{
    return person->priv->gender;
}

/**
 * ogd_person_get_communityrole:
 * @person:         the #OGDPerson to query
 *
 * Retrieve role of a @person into the community
 * 
 * Return value:    identifier of the role the target @person has
 */
OGD_PERSON_ROLE ogd_person_get_communityrole (OGDPerson *person)
{
    return person->priv->role;
}

/**
 * ogd_person_get_homepage:
 * @person:         the #OGDPerson to query
 *
 * Retrieve URL of the personal webpage of a @person
 * 
 * Return value:    a string containing URL of the webpage specified by the target @person
 */
const gchar* ogd_person_get_homepage (OGDPerson *person)
{
    return (const gchar*) person->priv->homepage;
}

/**
 * ogd_person_get_company:
 * @person:         the #OGDPerson to query
 *
 * Retrieve company for which a @person works
 * 
 * Return value:    a string containing the company name to which the target @person works.
 *                  Please note this content is not semantically validated, so cannot be
 *                  programmatically managed
 */
const gchar* ogd_person_get_company (OGDPerson *person) {
    return (const gchar*) person->priv->company;
}

/**
 * ogd_person_get_avatar:
 * @person:         the #OGDPerson to query
 *
 * Retrieve avatar picture URI of a @person
 * 
 * Return value:    a string containing the URI of the avatar picture for the target @person
 */

/*
    TODO    Provide also ogd_person_get_avatar_pixbuf()
*/

const gchar* ogd_person_get_avatar (OGDPerson *person)
{
    return (const gchar*) person->priv->avatar;
}

/**
 * ogd_person_get_birthday:
 * @person:         the #OGDPerson to query
 *
 * Retrieve birthday date of a @person
 * 
 * Return value:    a #GDate for the birthday of the user
 */
const GDate* ogd_person_get_birthday (OGDPerson *person)
{
    return (const GDate*) person->priv->birthday;
}

/**
 * ogd_person_get_job_status:
 * @person:         the #OGDPerson to query
 *
 * Retrieve current job status of a @person
 * 
 * Return value:    identified of the current job condition of the target @person
 */
OGD_PERSON_JOB ogd_person_get_job_status (OGDPerson *person)
{
    return person->priv->jobstatus;
}

/**
 * ogd_person_get_city:
 * @person:         the #OGDPerson to query
 *
 * Retrieve city from which the @person is
 * 
 * Return value:    a string containing the city from which target @person is. Please note
 *                  this content is not semantically validated, so cannot be programmatically
 *                  managed
 */
const gchar* ogd_person_get_city (OGDPerson *person)
{
    return (const gchar*) person->priv->city;
}

/**
 * ogd_person_get_country:
 * @person:         the #OGDPerson to query
 *
 * Retrieve country from which the @person is
 * 
 * Return value:    a string containing the country from which target @person is. Please note
 *                  this content is not semantically validated, so cannot be programmatically
 *                  managed
 */
const gchar* ogd_person_get_country (OGDPerson *person)
{
    return (const gchar*) person->priv->country;
}

/**
 * ogd_person_get_latitude:
 * @person:         the #OGDPerson to query
 *
 * Retrieve current coordinates latitude the @person is
 * 
 * Return value:    latitude of the current position of target @person
 */
gdouble ogd_person_get_latitude (OGDPerson *person)
{
    return person->priv->latitude;
}

/**
 * ogd_person_get_longitude:
 * @person:         the #OGDPerson to query
 *
 * Retrieve current coordinates longitude the @person is
 * 
 * Return value:    longitude of the current position of target @person
 */
gdouble ogd_person_get_longitude (OGDPerson *person)
{
    return person->priv->longitude;
}

/*
    TODO    IRC is missing, some kind of advanced management is required
*/

/**
 * ogd_person_get_likes:
 * @person:         the #OGDPerson to query
 *
 * Retrieve things a @person likes
 * 
 * Return value:    a string containing thing the target @person likes. Please note this content
 *                  is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_person_get_likes (OGDPerson *person)
{
    return (const gchar*) person->priv->likes;
}

/**
 * ogd_person_get_dont_likes:
 * @person:         the #OGDPerson to query
 *
 * Retrieve things a @person doesn't likes
 * 
 * Return value:    a string containing thing the target @person doesn't likes. Please note this
 *                  content is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_person_get_dont_likes (OGDPerson *person)
{
    return (const gchar*) person->priv->dontlikes;
}

/**
 * ogd_person_get_interests:
 * @person:         the #OGDPerson to query
 *
 * Retrieve interests by a @person
 * 
 * Return value:    a string containing interests of the target @person. Please note this content
 *                  is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_person_get_interests (OGDPerson *person)
{
    return (const gchar*) person->priv->interests;
}

/**
 * ogd_person_get_languages:
 * @person:         the #OGDPerson to query
 *
 * Retrieve languages spoke by a @person
 * 
 * Return value:    a string containing languages the target @person is able to spoke. Please
 *                  note this content is not semantically validated, so cannot be
 *                  programmatically managed
 */
const gchar* ogd_person_get_languages (OGDPerson *person)
{
    return (const gchar*) person->priv->languages;
}

/**
 * ogd_person_get_programming_langs:
 * @person:         the #OGDPerson to query
 *
 * Retrieve programming languages used by a @person
 * 
 * Return value:    a string containing programming languages the target @person is able to use.
 *                  Please note this content is not semantically validated, so cannot be
 *                  programmatically managed
 */
const gchar* ogd_person_get_programming_langs (OGDPerson *person)
{
    return (const gchar*) person->priv->programminglangs;
}

/**
 * ogd_person_get_favourite_quote:
 * @person:         the #OGDPerson to query
 *
 * Retrieve quote preferred by a @person
 * 
 * Return value:    a string containing favourite quote by the target @person. Please note this
 *                  content is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_person_get_favourite_quote (OGDPerson *person)
{
    return (const gchar*) person->priv->favouritequote;
}

/**
 * ogd_person_get_favourite_music:
 * @person:         the #OGDPerson to query
 *
 * Retrieve music preferred by a @person
 * 
 * Return value:    a string containing favourite music by the target @person. Please note this
 *                  content is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_person_get_favourite_music (OGDPerson *person)
{
    return (const gchar*) person->priv->favouritemusic;
}

/**
 * ogd_person_get_favourite_tv:
 * @person:         the #OGDPerson to query
 *
 * Retrieve TV shows preferred by a @person
 * 
 * Return value:    a string containing favourite TV shows by the target @person. Please note this
 *                  content is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_person_get_favourite_tv (OGDPerson *person)
{
    return (const gchar*) person->priv->favouritetv;
}

/**
 * ogd_person_get_favourite_movies:
 * @person:         the #OGDPerson to query
 *
 * Retrieve movies preferred by a @person
 * 
 * Return value:    a string containing favourite movies by the target @person. Please note this
 *                  content is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_person_get_favourite_movies (OGDPerson *person)
{
    return (const gchar*) person->priv->favouritemovies;
}

/**
 * ogd_person_get_favourite_books:
 * @person:         the #OGDPerson to query
 *
 * Retrieve books preferred by a @person
 * 
 * Return value:    a string containing favourite books by the target @person. Please note this
 *                  content is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_person_get_favourite_books (OGDPerson *person)
{
    return (const gchar*) person->priv->favouritebooks;
}

/**
 * ogd_person_get_favourite_games:
 * @person:         the #OGDPerson to query
 *
 * Retrieve games preferred by a @person
 * 
 * Return value:    a string containing favourite games by the target @person. Please note this
 *                  content is not semantically validated, so cannot be programmatically managed
 */
const gchar* ogd_person_get_favourite_games (OGDPerson *person)
{
    return (const gchar*) person->priv->favouritegames;
}

/**
 * ogd_person_get_description:
 * @person:         the #OGDPerson to query
 *
 * Retrieve description of a @person
 * 
 * Return value:    a string containing the description of the target @person
 */
const gchar* ogd_person_get_description (OGDPerson *person)
{
    return (const gchar*) person->priv->description;
}

/**
 * ogd_person_get_profile_page:
 * @person:         the #OGDPerson to query
 *
 * Retrieve link to the profile page of the target @person
 * 
 * Return value:    a string containing the URL of the profile webpage
 */
const gchar* ogd_person_get_profile_page (OGDPerson *person)
{
    return (const gchar*) person->priv->profilepage;
}

/**
 * ogd_person_get_friends:
 * @person:         the #OGDPerson to query
 *
 * Retrieve list of friends registered for the specified @person. Please note that this call
 * requires some syncronous communication with the server, so may return after some time
 * 
 * Return value:    list of #OGDPerson, one for each friend of the target @person
 */
const GList* ogd_person_get_friends (OGDPerson *person)
{
    gulong collected;
    gulong totalitems;
    gint page;
    gchar *query;
    GList *ret;
    xmlChar *friend_id;
    xmlNode *data;
    xmlNode *cursor;
    OGDPerson *obj;
    OGDProvider *provider;

    ret = NULL;
    collected = 0;
    totalitems = 0;
    page = 0;
    provider = (OGDProvider*) ogd_object_get_provider (OGD_OBJECT (person));

    do {
        query = g_strdup_printf ("friend/data/%s?pagesize=100&page=%d", ogd_person_get_id (person), page);
        data = ogd_provider_get_raw (provider, query);
        g_free (query);

        if (data != NULL) {
            totalitems = total_items_for_query (data);

            for (cursor = data->children; cursor; cursor = cursor->next) {
                friend_id = xmlNodeGetContent (cursor->children);

                if (friend_id != NULL) {
                    obj = g_object_new (OGD_PERSON_TYPE, NULL);

                    if (ogd_object_fill_by_id (OGD_OBJECT (obj), (char*) friend_id, NULL) == TRUE) {
                        ogd_object_set_provider (OGD_OBJECT (obj), provider);
                        ret = g_list_prepend (ret, obj);
                        collected++;
                    }

                    xmlFree (friend_id);
                }
            }

            xmlFreeDoc (data->doc);
            page++;
        }

    } while (collected < totalitems);

    if (ret)
        ret = g_list_reverse (ret);

    return ret;
}

static void pass_friend_up (OGDObject *obj, gpointer userdata)
{
    AsyncRequestDesc *req;

    req = (AsyncRequestDesc*) userdata;
    req->callback (obj, req->userdata);
}

static void retrieve_friends_slice (xmlNode *node, gpointer userdata)
{
    xmlChar *friend_id;
    OGDPerson *obj;
    AsyncRequestDesc *req;

    req = (AsyncRequestDesc*) userdata;

    if (node == NULL) {
        if (req->total <= req->counter) {
            return;
        }
        else {
            req->callback (NULL, req->userdata);
            g_free (req);
        }
    }
    else {
        friend_id = xmlNodeGetContent (node);

        if (friend_id != NULL) {
            obj = g_object_new (OGD_PERSON_TYPE, NULL);
            ogd_object_set_provider (OGD_OBJECT (obj), req->provider);
            ogd_object_fill_by_id_async (OGD_OBJECT (obj), (char*) friend_id, pass_friend_up, req);
            xmlFree (friend_id);
            req->counter += 1;
        }
    }
}

static void init_friends_async_rebuild (xmlNode *node, gpointer userdata)
{
    int tot;
    int page;
    gchar *query;
    AsyncRequestDesc *req;

    req = (AsyncRequestDesc*) userdata;

    if (node == NULL) {
        req->callback (NULL, req->userdata);
        g_free (req);
    }
    else {
        req->total = total_items_for_query (node);
        req->counter = 0;

        for (page = 0, tot = 0; tot < req->total; page++, tot += 100) {
            query = g_strdup_printf ("friend/data/%s?pagesize=%d&page=%d", ogd_person_get_id (OGD_PERSON (req->reference)), 100, page);
            ogd_provider_get_raw_async (req->provider, query, TRUE, retrieve_friends_slice, req);
            g_free (query);
        }
    }
}

/**
 * ogd_person_get_friends_async:
 * @person:         the #OGDPerson to query
 * @callback:       async callback to which incoming #OGDPerson are passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_person_get_friends()
 */
void ogd_person_get_friends_async (OGDPerson *person, OGDAsyncCallback callback, gpointer userdata)
{
    gchar *query;
    AsyncRequestDesc *req;
    OGDProvider *provider;

    provider = (OGDProvider*) ogd_object_get_provider (OGD_OBJECT (person));

    req = g_new0 (AsyncRequestDesc, 1);
    req->callback = callback;
    req->userdata = userdata;
    req->provider = provider;
    req->reference = OGD_OBJECT (person);

    query = g_strdup_printf ("friend/data/%s?pagesize=%d&page=%d", ogd_person_get_id (person), 1, 0);
    ogd_provider_get_raw_async (provider, query, FALSE, init_friends_async_rebuild, req);
    g_free (query);
}

/**
 * ogd_person_get_myself:
 * @provider:       #OGDProvider from which retrieve information
 *
 * Retrieve the #OGDPerson rappresenting the current user, in function of the @username provided
 * in ogd_provider_auth_user_and_pwd(), and is not usable if ogd_provider_auth_api_key() has been
 * engaged instead
 * 
 * Return value:    the #OGDPerson rappresenting the current user
 */
const OGDPerson* ogd_person_get_myself (OGDProvider *provider)
{
    GList *tmp_list;
    OGDPerson *ret;

    tmp_list = ogd_provider_get (provider, "person/self");
    if (tmp_list == NULL)
        return NULL;

    ret = (OGDPerson*) tmp_list->data;
    g_list_free (tmp_list);
    return ret;
}

/**
 * ogd_person_get_myself_async:
 * @provider:       #OGDProvider from which retrieve information
 * @callback:       async callback to which incoming #OGDPerson is passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_person_get_myself()
 */
void ogd_person_get_myself_async (OGDProvider *provider, OGDAsyncCallback callback, gpointer userdata)
{
    ogd_provider_get_single_async (provider, "person/self", callback, userdata);
}

static void effective_set_coordinates (OGDPerson *myself, gdouble latitude, gdouble longitude,
                                       gboolean async, OGDPutAsyncCallback callback, gpointer userdata)
{
    gchar lat_str [100];
    gchar lon_str [100];
    GHashTable *params;
    OGDProvider *provider;

    provider = (OGDProvider*) ogd_object_get_provider (OGD_OBJECT (myself));
    snprintf (lat_str, 100, "%.03f", latitude);
    snprintf (lon_str, 100, "%.03f", longitude);

    params = g_hash_table_new (g_str_hash, g_str_equal);
    g_hash_table_insert (params, "longitude", lon_str);
    g_hash_table_insert (params, "latitude", lat_str);

    ogd_provider_put (provider, "person/self", params);

    g_hash_table_unref (params);
}

/**
 * ogd_person_myself_set_coordinates:
 * @myself:         #OGDPerson returned by ogd_person_get_myself()
 * @latitude:       current latitude
 * @longitude:      current longitude
 *
 * To set current coordinates of the user which name has been expressed as @username parameter in
 * ogd_provider_auth_user_and_pwd(), and is not usable if ogd_provider_auth_api_key() has been
 * engaged instead
 */
void ogd_person_myself_set_coordinates (OGDPerson *myself, gdouble latitude, gdouble longitude)
{
    effective_set_coordinates (myself, latitude, longitude, FALSE, NULL, NULL);
}

/**
 * ogd_person_myself_set_coordinates_async:
 * @myself:         #OGDPerson returned by ogd_person_get_myself()
 * @latitude:       current latitude
 * @longitude:      current longitude
 * @callback:       async callback to which result of the operation is passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_person_myself_set_coordinates()
 */
void ogd_person_myself_set_coordinates_async (OGDPerson *myself, gdouble latitude, gdouble longitude, OGDPutAsyncCallback callback, gpointer userdata)
{
    effective_set_coordinates (myself, latitude, longitude, TRUE, callback, userdata);
}

static void effective_invite_friend (OGDPerson *person, gchar *message, gboolean async, OGDPutAsyncCallback callback, gpointer userdata)
{
    gchar *query;
    GHashTable *params;
    OGDProvider *provider;

    provider = (OGDProvider*) ogd_object_get_provider (OGD_OBJECT (person));

    query = g_strdup_printf ("friend/outbox/%s", ogd_person_get_id (person));
    params = g_hash_table_new (g_str_hash, g_str_equal);
    g_hash_table_insert (params, "message", message);

    if (async == FALSE)
        ogd_provider_put (provider, query, params);
    else
        ogd_provider_put_async (provider, query, params, callback, userdata);

    g_hash_table_unref (params);
    g_free (query);
}

/**
 * ogd_person_myself_invite_friend:
 * @person:         the #OGDPerson to which send friendship invite
 * @message:        message to attach to the friendship request
 *
 * Sends a friendship invite to the target @person. That will become friend of the user which
 * name has been expressed as @username parameter in ogd_provider_auth_user_and_pwd(), and is not
 * usable if ogd_provider_auth_api_key() has been engaged instead
 */
void ogd_person_myself_invite_friend (OGDPerson *person, gchar *message)
{
    effective_invite_friend (person, message, FALSE, NULL, NULL);
}

/**
 * ogd_person_myself_invite_friend_async:
 * @person:         the #OGDPerson to which send friendship invite
 * @message:        message to attach to the friendship request
 * @callback:       async callback to which result of the operation is passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_person_myself_invite_friend()
 */
void ogd_person_myself_invite_friend_async (OGDPerson *person, gchar *message, OGDPutAsyncCallback callback, gpointer userdata)
{
    effective_invite_friend (person, message, TRUE, callback, userdata);
}

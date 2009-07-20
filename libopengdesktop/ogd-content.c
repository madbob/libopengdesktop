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
#include "ogd-category.h"
#include "ogd-private-utils.h"

#define OGD_CONTENT_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_CONTENT_TYPE, OGDContentPrivate))

/**
 * SECTION: ogd-content
 * @short_description:  description of a specific content took from the provider
 *
 * A #OGDContent is the full description of a specific content took from the #OGDProvider, such
 * as a wallpaper or a theme. To access those material query the provider for existing categories
 * ( ogd_category_get_all() ) and iterate the preferred #OGDCategory.
 */

struct _OGDContentPrivate {
    gchar       *id;
    gchar       *name;
    gchar       *version;
    gchar       *language;
    gchar       *authorid;
    OGDPerson   *author;
    GDate       *creationdate;
    GDate       *changedate;
    gulong      numdownloads;
    guint       score;
    gchar       *description;
    gchar       *changelog;
    gchar       *homepage;
    gulong      numcomments;
    gulong      numfans;
    GList       *previews;
    GList       *downloads;
};

G_DEFINE_TYPE (OGDContent, ogd_content, OGD_OBJECT_TYPE);

static void ogd_content_finalize (GObject *obj)
{
    OGDContent *content;

    content = OGD_CONTENT (obj);

    PTR_CHECK_FREE_NULLIFY (content->priv->id);
    PTR_CHECK_FREE_NULLIFY (content->priv->name);
    PTR_CHECK_FREE_NULLIFY (content->priv->version);
    PTR_CHECK_FREE_NULLIFY (content->priv->language);
    PTR_CHECK_FREE_NULLIFY (content->priv->authorid);
    OBJ_CHECK_UNREF_NULLIFY (content->priv->author);
    DATE_CHECK_FREE_NULLIFY (content->priv->creationdate);
    DATE_CHECK_FREE_NULLIFY (content->priv->changedate);
    PTR_CHECK_FREE_NULLIFY (content->priv->description);
    PTR_CHECK_FREE_NULLIFY (content->priv->changelog);
    PTR_CHECK_FREE_NULLIFY (content->priv->homepage);
    STRLIST_CHECK_FREE_NULLIFY (content->priv->previews);
    STRLIST_CHECK_FREE_NULLIFY (content->priv->downloads);
}

static gboolean ogd_content_fill_by_xml (OGDObject *obj, const xmlNode *xml, GError **error)
{
    xmlNode *cursor;
    OGDContent *content;

    content = OGD_CONTENT (obj);

    if (strcmp (xml->name, "content") != 0)
        return FALSE;

    ogd_content_finalize (obj);

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        if (strcmp (cursor->name, "id") == 0)
            content->priv->id = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "name") == 0)
            content->priv->name = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "version") == 0)
            content->priv->version = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "language") == 0)
            content->priv->language = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "personid") == 0)
            content->priv->authorid = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "created") == 0)
            content->priv->creationdate = node_to_date (cursor);
        else if (strcmp (cursor->name, "changed") == 0)
            content->priv->changedate = node_to_date (cursor);
        else if (strcmp (cursor->name, "downloads") == 0)
            content->priv->numdownloads = (guint) node_to_num (cursor);
        else if (strcmp (cursor->name, "score") == 0)
            content->priv->score = (guint) node_to_num (cursor);
        else if (strcmp (cursor->name, "description") == 0)
            content->priv->description = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "changelog") == 0)
            content->priv->changelog = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "homepage") == 0)
            content->priv->homepage = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "comments") == 0)
            content->priv->numcomments = (guint) node_to_num (cursor);
        else if (strcmp (cursor->name, "fans") == 0)
            content->priv->numfans = (guint) node_to_num (cursor);
        else if (strncmp (cursor->name, "previewpic", strlen ("previewpic")) == 0)
            content->priv->previews = g_list_prepend (content->priv->previews, xmlNodeGetContent (cursor));
        else if (strncmp (cursor->name, "downloadlink", strlen ("downloadlink")) == 0)
            content->priv->downloads = g_list_prepend (content->priv->downloads, xmlNodeGetContent (cursor));
    }

    return TRUE;
}

static gboolean ogd_content_fill_by_id (OGDObject *obj, const gchar *id, GError **error)
{
    gchar *query;
    gboolean ret;
    xmlNode *data;

    query = g_strdup_printf ("content/data/%s", id);
    data = ogd_provider_get_raw (ogd_object_get_provider (obj), query);
    g_free (query);

    if (data != NULL) {
        ret = ogd_content_fill_by_xml (obj, data->children, error);
        xmlFreeDoc (data->doc);
    }
    else
        ret = FALSE;

    return ret;
}

static void ogd_content_class_init (OGDContentClass *klass)
{
    GObjectClass *gobject_class;
    OGDObjectClass *ogd_object_class;

    g_type_class_add_private (klass, sizeof (OGDContentPrivate));

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ogd_content_finalize;

    ogd_object_class = OGD_OBJECT_CLASS (klass);
    ogd_object_class->fill_by_xml = ogd_content_fill_by_xml;
    ogd_object_class->fill_by_id = ogd_content_fill_by_id;
}

static void ogd_content_init (OGDContent *item)
{
    item->priv = OGD_CONTENT_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDContentPrivate));
}

/**
 * ogd_content_get_id:
 * @content:        the #OGDContent to query
 *
 * To obtain the ID of the @content
 * 
 * Return value:    ID of the target @content
 */
const gchar* ogd_content_get_id (OGDContent *content)
{
    return (const gchar*) content->priv->id;
}

/**
 * ogd_content_get_name:
 * @content:        the #OGDContent to query
 *
 * To obtain the name of the @content
 * 
 * Return value:    name of the target @content
 */
const gchar* ogd_content_get_name (OGDContent *content)
{
    return (const gchar*) content->priv->name;
}

/**
 * ogd_content_get_version:
 * @content:        the #OGDContent to query
 *
 * To obtain the version of the @content
 * 
 * Return value:    version of the target @content. Please note this content is not semantically
 *                  validated, so cannot be programmatically managed
 */
const gchar* ogd_content_get_version (OGDContent *content)
{
    return (const gchar*) content->priv->version;
}

/**
 * ogd_content_get_language:
 * @content:        the #OGDContent to query
 *
 * To obtain the language of the @content
 * 
 * Return value:    language of the target @content. Please note this content is not semantically
 *                  validated, so cannot be programmatically managed
 */
const gchar* ogd_content_get_language (OGDContent *content)
{
    return (const gchar*) content->priv->language;
}

/**
 * ogd_content_get_author:
 * @content:        the #OGDContent to query
 *
 * To obtain the author of the @content. Please note that this call requires some syncronous
 * communication with the server, so may return after some time
 * 
 * Return value:    #OGDPerson who created the @content, or NULL if the object cannot be retrieved
 */

/*
    TODO    Provide also an async version
*/

const OGDPerson* ogd_content_get_author (OGDContent *content)
{
    if (content->priv->author == NULL) {
        if (content->priv->authorid != NULL && strlen (content->priv->authorid) != 0) {
            OGDPerson *author;

            author = g_object_new (OGD_PERSON_TYPE, NULL);
            ogd_object_set_provider (OGD_OBJECT (author), ogd_object_get_provider (OGD_OBJECT (content)));

            if (ogd_object_fill_by_id (OGD_OBJECT (author), content->priv->authorid, NULL))
                content->priv->author = author;
            else
                g_object_unref (author);
        }
    }

    return content->priv->author;
}

/**
 * ogd_content_get_creation_date:
 * @content:        the #OGDContent to query
 *
 * To obtain the creation date of the @content
 * 
 * Return value:    a #GDate for the creation date of the target @content
 */
const GDate* ogd_content_get_creation_date (OGDContent *content)
{
    return content->priv->creationdate;
}

/**
 * ogd_content_get_change_date:
 * @content:        the #OGDContent to query
 *
 * To obtain the date of latest change of the @content
 * 
 * Return value:    a #GDate for the latest change date of the target @content
 */
const GDate* ogd_content_get_change_date (OGDContent *content)
{
    return content->priv->changedate;
}

/**
 * ogd_content_get_num_downloads:
 * @content:        the #OGDContent to query
 *
 * To obtain the current downloads number of the @content
 * 
 * Return value:    current downloads number of the target @content
 */
gulong ogd_content_get_num_downloads (OGDContent *content)
{
    return content->priv->numdownloads;
}

/**
 * ogd_content_get_score:
 * @content:        the #OGDContent to query
 *
 * To obtain the current score of the @content
 * 
 * Return value:    current score of the target @content, in function of votes. To exprime your
 *                  vote, use ogd_content_vote()
 */
guint ogd_content_get_score (OGDContent *content)
{
    return content->priv->score;
}

/**
 * ogd_content_get_description:
 * @content:        the #OGDContent to query
 *
 * To obtain the description of the @content
 * 
 * Return value:    description of the target @content
 */
const gchar* ogd_content_get_description (OGDContent *content)
{
    return (const gchar*) content->priv->description;
}

/**
 * ogd_content_get_changelog:
 * @content:        the #OGDContent to query
 *
 * To obtain a changelog of @content
 * 
 * Return value:    changelog for the target @content
 */
const gchar* ogd_content_get_changelog (OGDContent *content)
{
    return (const gchar*) content->priv->changelog;
}

/**
 * ogd_content_get_homepage:
 * @content:        the #OGDContent to query
 *
 * To obtain homepage URL of @content
 * 
 * Return value:    URL of the homepage for the target @content
 */
const gchar* ogd_content_get_homepage (OGDContent *content)
{
    return (const gchar*) content->priv->homepage;
}

/**
 * ogd_content_get_num_comments:
 * @content:        the #OGDContent to query
 *
 * To obtain number of comments of @content
 * 
 * Return value:    number of comments for the target @content
 */
gulong ogd_content_get_num_comments (OGDContent *content)
{
    return content->priv->numcomments;
}

/**
 * ogd_content_get_num_fans:
 * @content:        the #OGDContent to query
 *
 * To obtain number of fans of @content
 * 
 * Return value:    number of fans for the target @content
 */
gulong ogd_content_get_num_fans (OGDContent *content)
{
    return content->priv->numfans;
}

/**
 * ogd_content_get_previews:
 * @content:        the #OGDContent to query
 *
 * To obtain a list of URL where to find previews of @content
 * 
 * Return value:    list of URL of previews for the target @content
 */
const GList* ogd_content_get_previews (OGDContent *content)
{
    return content->priv->previews;
}

/**
 * ogd_content_get_download_refs:
 * @content:        the #OGDContent to query
 *
 * To obtain a list of URL where @content may be downloaded
 * 
 * Return value:    list of URL from which download the target @content
 */
const GList* ogd_content_get_download_refs (OGDContent *content)
{
    return content->priv->downloads;
}

/**
 * ogd_content_vote:
 * @content:        the #OGDContent to vote
 * @vote:           rating for the @content
 *
 * To share a rating about the content. The vote is assigned to the current user, the one which
 * username is used in ogd_provider_auth_user_and_pwd(), and the function cannot be used if
 * ogd_provider_auth_api_key() has been used instead
 */

/*
    TODO    Provide also an async version
*/

void ogd_content_vote (OGDContent *content, OGD_CONTENT_VOTE vote)
{
    gchar *query;
    gchar *vote_str;
    OGDProvider *provider;

    provider = ogd_object_get_provider (OGD_OBJECT (content));

    switch (vote) {
        case OGD_CONTENT_GOOD:
            vote_str = "good";
            break;

        case OGD_CONTENT_BAD:
            vote_str = "bad";
            break;

        default:
            g_warning ("Invalid vote");
            return;
            break;
    }

    query = g_strdup_printf ("content/vote/%s?vote=%s", ogd_content_get_id (content), vote_str);
    ogd_provider_put (provider, query, NULL);
    g_free (query);
}

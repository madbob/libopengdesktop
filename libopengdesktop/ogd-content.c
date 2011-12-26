/*  libopengdesktop
 *  Copyright (C) 2009/2011 Roberto -MadBob- Guido <bob4job@gmail.com>
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
#include "ogd-provider-private.h"
#include "ogd-private-utils.h"

#define OGD_CONTENT_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_CONTENT_TYPE, OGDContentPrivate))

/**
 * SECTION: ogd-content
 * @short_description:  description of a specific content took from the provider
 *
 * A #OGDContent is the full description of a specific content took from the #OGDProvider, such
 * as a wallpaper or a theme. To access those material query the provider for existing categories
 * ( ogd_category_fetch_all() ) and iterate the preferred #OGDCategory.
 */

struct _OGDContentPrivate {
    gchar       *id;
    gchar       *name;
    gchar       *version;
    gchar       *language;
    gchar       *authorid;
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

    if (MYSTRCMP (xml->name, "data") == 0)
        xml = xml->children;

    if (MYSTRCMP (xml->name, "content") != 0) {
        g_set_error (error, OGD_PARSING_ERROR_DOMAIN, OGD_XML_ERROR, "Invalid XML for OGDContent");
        return FALSE;
    }

    ogd_content_finalize (G_OBJECT (obj));

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        if (MYSTRCMP (cursor->name, "id") == 0)
            content->priv->id = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "name") == 0)
            content->priv->name = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "version") == 0)
            content->priv->version = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "language") == 0)
            content->priv->language = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "personid") == 0)
            content->priv->authorid = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "created") == 0)
            content->priv->creationdate = node_to_date (cursor);
        else if (MYSTRCMP (cursor->name, "changed") == 0)
            content->priv->changedate = node_to_date (cursor);
        else if (MYSTRCMP (cursor->name, "downloads") == 0)
            content->priv->numdownloads = (guint) node_to_num (cursor);
        else if (MYSTRCMP (cursor->name, "score") == 0)
            content->priv->score = (guint) node_to_num (cursor);
        else if (MYSTRCMP (cursor->name, "description") == 0)
            content->priv->description = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "changelog") == 0)
            content->priv->changelog = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "detailpage") == 0)
            content->priv->homepage = MYGETCONTENT (cursor);
        else if (MYSTRCMP (cursor->name, "comments") == 0)
            content->priv->numcomments = (guint) node_to_num (cursor);
        else if (MYSTRCMP (cursor->name, "fans") == 0)
            content->priv->numfans = (guint) node_to_num (cursor);
        else if (strncmp ((char*) cursor->name, "previewpic", strlen ("previewpic")) == 0)
            content->priv->previews = g_list_prepend (content->priv->previews, MYGETCONTENT (cursor));
        else if (strncmp ((char*) cursor->name, "downloadlink", strlen ("downloadlink")) == 0)
            content->priv->downloads = g_list_prepend (content->priv->downloads, MYGETCONTENT (cursor));
    }

    return TRUE;
}

static gchar* ogd_content_target_query (const gchar *id)
{
    return g_strdup_printf ("content/data/%s", id);
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
    ogd_object_class->target_query = ogd_content_target_query;
}

static void ogd_content_init (OGDContent *item)
{
    item->priv = OGD_CONTENT_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDContentPrivate));
}

/**
 * ogd_content_new_by_id:
 * @provider        #OGDProvider from which retrieve the content
 * @id:             ID of the required content
 *
 * To obtain informations about an existing content published on the specified platform
 *
 * Return value:    a newly allocated and populated #OGDContent, or NULL if the ID is not valid
 */
OGDContent* ogd_content_new_by_id (OGDProvider *provider, const gchar *id)
{
    GError *err;
    OGDContent *ret;

    ret = g_object_new (ogd_content_get_type (), NULL);
    ogd_object_set_provider (OGD_OBJECT (ret), provider);

    err = NULL;

    if (ogd_object_fill_by_id (OGD_OBJECT (ret), id, &err) == TRUE) {
        return ret;
    }
    else {
        g_object_unref (ret);
        return NULL;
    }
}

/**
 * ogd_content_new_by_id_async:
 * @provider        #OGDProvider from which retrieve the content
 * @id:             ID of the required content
 * @callback:       async callback to which the filled #OGDObject is passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_content_new_by_id()
 */
void ogd_content_new_by_id_async (OGDProvider *provider, const gchar *id, OGDAsyncCallback callback, gpointer userdata)
{
    OGDContent *ret;

    ret = g_object_new (ogd_content_get_type (), NULL);
    ogd_object_set_provider (OGD_OBJECT (ret), provider);
    ogd_object_fill_by_id_async (OGD_OBJECT (ret), id, callback, userdata);
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
 * ogd_content_get_authorid:
 * @content:        the #OGDContent to query
 *
 * To obtain the author of the @content
 *
 * Return value:    identifiers of the #OGDPerson who created the content. You can use
                    ogd_object_fill_by_id() to obtain all informations
 */
const gchar* ogd_content_get_authorid (OGDContent *content)
{
    return (const gchar*) content->priv->authorid;
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
 * ogd_content_get_comments:
 * @content:        the #OGDContent to query
 *
 * Retrieves list of comments published for the given #OGDContent
 *
 * Return value:    a list of #OGDComment to be freed when no longer in use, or NULL
 */
GList* ogd_content_get_comments (OGDContent *content)
{
    gchar *query;
    GList *ret;

    query = g_strdup_printf ("comments/data/1/%s/0", ogd_content_get_id (content));
    ret = ogd_provider_get (ogd_object_get_provider (OGD_OBJECT (content)), query);
    g_free (query);
    return ret;
}

/**
 * ogd_content_get_comments_async:
 * @content:        the #OGDContent to query
 * @callback:       callback to call when comments collection is ended. If no comments exist, it
 *					is invoked with NULL
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_content_get_comments()
 */
void ogd_content_get_comments_async (OGDContent *content, OGDAsyncListCallback callback, gpointer userdata)
{
    gchar *query;

    query = g_strdup_printf ("comments/data/1/%s/0", ogd_content_get_id (content));
    ogd_provider_get_list_async (ogd_object_get_provider (content), query, callback, userdata);
    g_free (query);
}

/**
 * ogd_content_get_fans:
 * @content:        the #OGDContent to query
 *
 * Retrieves list of current fans for the given @content
 *
 * Return value:    a list of #OGDPerson to be freed when no longer in use, or NULL
 */
GList* ogd_content_get_fans (OGDContent *content)
{
    gchar *query;
    GList *ret;

    query = g_strdup_printf ("fan/data/%s", ogd_content_get_id (content));
    ret = list_of_people (OGD_OBJECT (content), query);
    g_free (query);
    return ret;
}

/**
 * ogd_content_get_fans_async:
 * @content:        the #OGDContent to query
 * @callback:       callback to call when fans collection is ended. If no fans exist, it is
 *                  invoked with NULL
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_content_get_fans()
 */
void ogd_content_get_fans_async (OGDContent *content, OGDAsyncListCallback callback, gpointer userdata)
{
    gchar *query;

    query = g_strdup_printf ("fan/data/%s", ogd_content_get_id (content));
    list_of_people_async (OGD_OBJECT (content), query, callback, userdata);
    g_free (query);
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

static void effective_vote (OGDContent *content, OGD_CONTENT_VOTE vote, gboolean async,
                            OGDPutAsyncCallback callback, gpointer userdata)
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

    if (async == FALSE)
        ogd_provider_put (provider, query, NULL);
    else
        ogd_provider_put_async (provider, query, NULL, callback, userdata);

    g_free (query);
}

#ifndef OGD_DISABLE_DEPRECATED

/**
 * ogd_content_get_num_comments:
 * @content:        the #OGDContent to query
 *
 * To obtain number of comments of @content
 *
 * Return value:    number of comments for the target @content
 *
 * Deprecated: 0.4: ogd_content_get_comments is suggested to be used instead
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
 *
 * Deprecated: 0.4: ogd_content_get_fans() is suggested to be used instead
 */
gulong ogd_content_get_num_fans (OGDContent *content)
{
    return content->priv->numfans;
}

#endif

/**
 * ogd_content_vote:
 * @content:        the #OGDContent to vote
 * @vote:           rating for the @content
 *
 * To share a rating about the content. The vote is assigned to the current user, the one which
 * username is used in ogd_provider_auth_user_and_pwd(), and the function cannot be used if
 * ogd_provider_auth_api_key() has been used instead
 */
void ogd_content_vote (OGDContent *content, OGD_CONTENT_VOTE vote)
{
    effective_vote (content, vote, FALSE, NULL, NULL);
}

/**
 * ogd_content_vote_async:
 * @content:        the #OGDContent to vote
 * @vote:           rating for the @content
 * @callback:       async callback to which result of the operation is passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_content_vote()
 */
void ogd_content_vote_async (OGDContent *content, OGD_CONTENT_VOTE vote, OGDPutAsyncCallback callback, gpointer userdata)
{
    effective_vote (content, vote, TRUE, callback, userdata);
}

static gchar* fan_query (OGDContent *content, gboolean fan)
{
    gchar *query;

    if (fan == TRUE)
        query = g_strdup_printf ("fan/add/%s", ogd_content_get_id (content));
    else
        query = g_strdup_printf ("fan/remove/%s", ogd_content_get_id (content));

    return query;
}

/**
 * ogd_content_set_fan:
 * @content:        the #OGDContent for which change the fan status
 * @fan:            %TRUE to become fan of the @content, %FALSE to be removed from the list of
 *                  fans
 *
 * Permits to change status of the current user as fan of the given @content
 */
void ogd_content_set_fan (OGDContent *content, gboolean fan)
{
    gchar *query;

    query = fan_query (content, fan);
    ogd_provider_put (ogd_object_get_provider (OGD_OBJECT (content)), query, NULL);
    g_free (query);
}

/**
 * ogd_content_set_fan_async:
 * @content:        the #OGDContent for which change the fan status
 * @fan:            %TRUE to become fan of the @content, %FALSE to be removed from the list of
 *                  fans
 * @callback:       async callback to which result of the operation is passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_content_set_fan()
 */
void ogd_content_set_fan_async (OGDContent *content, gboolean fan, OGDPutAsyncCallback callback, gpointer userdata)
{
    gchar *query;

    query = fan_query (content, fan);
    ogd_provider_put_async (ogd_object_get_provider (OGD_OBJECT (content)), query, NULL, callback, userdata);
    g_free (query);
}

static GHashTable* add_comment_params (OGDContent *content, gchar *subject, gchar *message)
{
    GHashTable *params;

    params = g_hash_table_new (g_str_hash, g_str_equal);
    g_hash_table_insert (params, "type", "1");
    g_hash_table_insert (params, "content", (gpointer) ogd_content_get_id (content));
    g_hash_table_insert (params, "subject", (gpointer) subject);
    g_hash_table_insert (params, "message", (gpointer) message);
    return params;
}

/**
 * ogd_content_add_comment:
 * @content:        the #OGDContent to comment
 * @subject:        subject of the new comment
 * @message:        text of the new comment
 *
 * Assigns a new comment to the given @content
 */
void ogd_content_add_comment (OGDContent *content, gchar *subject, gchar *message)
{
    GHashTable *params;

    params = add_comment_params (content, subject, message);
    ogd_provider_put (ogd_object_get_provider (OGD_OBJECT (content)), "comments/add", params);
    g_hash_table_unref (params);
}

/**
 * ogd_content_add_comment_async:
 * @content:        the #OGDContent to comment
 * @subject:        subject of the new comment
 * @message:        text of the new comment
 * @callback:       async callback to which result of the operation is passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_content_add_comment()
 */
void ogd_content_add_comment_async (OGDContent *content, gchar *subject, gchar *message, OGDPutAsyncCallback callback, gpointer userdata)
{
    GHashTable *params;

    params = add_comment_params (content, subject, message);
    ogd_provider_put_async (ogd_object_get_provider (OGD_OBJECT (content)), "comment/add", params, callback, userdata);
    g_hash_table_unref (params);
}

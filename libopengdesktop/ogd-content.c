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
#include "ogd-category.h"
#include "ogd-private-utils.h"

#define OGD_CONTENT_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_CONTENT_TYPE, OGDContentPrivate))

struct _OGDContentPrivate {
    gchar       *id;
    gchar       *name;
    gchar       *version;
    gchar       *subcategory;
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
    OGDContent *category;

    category = OGD_CONTENT (obj);

    PTR_CHECK_FREE_NULLIFY (category->priv->name);
}

static gboolean ogd_content_fill_by_xml (OGDObject *obj, const xmlNode *xml, GError **error)
{
    xmlNode *cursor;
    OGDContent *content;

    content = OGD_CONTENT (obj);

    if (strcmp (xml->name, "content") != 0)
        return FALSE;

    for (cursor = xml->children; cursor; cursor = cursor->next) {
        if (strcmp (cursor->name, "id") == 0)
            content->priv->id = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "name") == 0)
            content->priv->name = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "version") == 0)
            content->priv->version = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "typename") == 0)
            content->priv->subcategory = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "language") == 0)
            content->priv->language = xmlNodeGetContent (cursor);
        else if (strcmp (cursor->name, "author") == 0)
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
        ret = ogd_content_fill_by_xml (obj, data, error);
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

const gchar* ogd_content_get_id (OGDContent *content)
{
    return (const gchar*) content->priv->id;
}

const gchar* ogd_content_get_name (OGDContent *content)
{
    return (const gchar*) content->priv->name;
}

const gchar* ogd_content_get_version (OGDContent *content)
{
    return (const gchar*) content->priv->version;
}

const gchar* ogd_content_get_subcategory (OGDContent *content)
{
    return (const gchar*) content->priv->subcategory;
}

const gchar* ogd_content_get_language (OGDContent *content)
{
    return (const gchar*) content->priv->language;
}

/*
    TODO    Provide also an async version
*/

OGDPerson* ogd_content_get_author (OGDContent *content)
{
    if (content->priv->author == NULL) {
        if (content->priv->authorid != NULL && strlen (content->priv->authorid) != 0) {
            OGDPerson *author;

            author = g_object_new (OGD_PERSON_TYPE, NULL);
            ogd_object_set_provider (OGD_OBJECT (author), ogd_object_get_provider (OGD_OBJECT (content)));

            if (ogd_object_fill_by_id (OGD_OBJECT (author), content->priv->authorid, NULL))
                content->priv->author = author;
        }
    }

    return content->priv->author;
}

GDate* ogd_content_get_creation_date (OGDContent *content)
{
    return content->priv->creationdate;
}

GDate* ogd_content_get_change_date (OGDContent *content)
{
    return content->priv->changedate;
}

gulong ogd_content_get_num_downloads (OGDContent *content)
{
    return content->priv->numdownloads;
}

guint ogd_content_get_score (OGDContent *content)
{
    return content->priv->score;
}

const gchar* ogd_content_get_description (OGDContent *content)
{
    return (const gchar*) content->priv->description;
}

const gchar* ogd_content_get_changelog (OGDContent *content)
{
    return (const gchar*) content->priv->changelog;
}

const gchar* ogd_content_get_homepage (OGDContent *content)
{
    return (const gchar*) content->priv->homepage;
}

gulong ogd_content_get_num_comments (OGDContent *content)
{
    return content->priv->numcomments;
}

gulong ogd_content_get_num_fans (OGDContent *content)
{
    return content->priv->numfans;
}

GList* ogd_content_get_previews (OGDContent *content)
{
    return content->priv->previews;
}

GList* ogd_content_get_download_refs (OGDContent *content)
{
    return content->priv->downloads;
}

/*
    TODO    Provide also an async version
*/

gboolean ogd_content_vote (OGDContent *content, OGD_CONTENT_VOTE vote)
{
    /*
        TODO
    */

    return FALSE;
}

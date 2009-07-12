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
#include "ogd-provider.h"
#include "ogd-private-utils.h"

#define OPEN_COLLABORATION_API_VERSION      1

#define OGD_PROVIDER_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                             OGD_PROVIDER_TYPE, OGDProviderPrivate))

/**
 * SECTION: ogd-provider
 * @short_description:  a webservice implementing the Open Collaboration Services REST API
 *
 * The #OGDProvider is the center of all activities which may be performed with this library, and
 * all functions are just wrappers to different communications with this host. Has to be accessed
 * with a username/password pair, or an API key.
 */

struct _OGDProviderPrivate {
    gchar       *server_name;
    SoupSession *http_session;

    gchar       *access_url;
};

G_DEFINE_TYPE (OGDProvider, ogd_provider, G_TYPE_OBJECT);

static void ogd_provider_finalize (GObject *obj)
{
    OGDProvider *provider;

    provider = OGD_PROVIDER (obj);

    PTR_CHECK_FREE_NULLIFY (provider->priv->server_name);
    PTR_CHECK_FREE_NULLIFY (provider->priv->access_url);
}

static void ogd_provider_class_init (OGDProviderClass *klass)
{
    GObjectClass *gobject_class;

    g_type_class_add_private (klass, sizeof (OGDProviderPrivate));

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ogd_provider_finalize;
}

static void ogd_provider_init (OGDProvider *item)
{
    item->priv = OGD_PROVIDER_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDProviderPrivate));
    item->priv->http_session = soup_session_sync_new ();
}

/**
 * ogd_provider_new;
 * @url:            HTTP address to which retrieve the exposed REST API of the desired service.
 *                  Please take note the complete HTTP requests will be submited as
 *                  http://$authentication@$this_url/$version_of_api_supported/$provided_query
 *
 * Allocates a new #OGDProvider, which will be used to retrieve remote contents
 * 
 * Return value:    a newly allocated #OGDProvider
 */
OGDProvider* ogd_provider_new (gchar *url)
{
    OGDProvider *provider;

    provider = g_object_new (OGD_PROVIDER_TYPE, NULL);
    provider->priv->server_name = g_strdup (url);
    return provider;
}

/**
 * ogd_provider_auth_user_and_pwd:
 * @provider:       the #OGDProvider to which enable access with desired username and password
 * @username:       username used for authentication
 * @password:       password used for authentication
 * 
 * Set @username and @password to access the #OGDProvider. Those authentication parameters must
 * be obtained separately, perhaps registering to the target website
 */
void ogd_provider_auth_user_and_pwd (OGDProvider *provider, gchar *username, gchar *password)
{
    PTR_CHECK_FREE_NULLIFY (provider->priv->access_url);
    provider->priv->access_url = g_strdup_printf ("http://%s:%s@%s/v%d/", username, password,
                                                  provider->priv->server_name,
                                                  OPEN_COLLABORATION_API_VERSION);
}

/**
 * ogd_provider_auth_api_key:
 * @provider:       the #OGDProvider to which enable access with desired key
 * @key:            key used for authentication
 * 
 * Set @key to access the #OGDProvider. This authentication parameter must be obtained
 * separately, perhaps registering to the target website
 */
void ogd_provider_auth_api_key (OGDProvider *provider, gchar *key)
{
    PTR_CHECK_FREE_NULLIFY (provider->priv->access_url);
    provider->priv->access_url = g_strdup_printf ("http://%s@%s/v%d/", key,
                                                  provider->priv->server_name,
                                                  OPEN_COLLABORATION_API_VERSION);
}

static gboolean check_provider_response (xmlNode *root, xmlNode **data)
{
    xmlNode *status;
    xmlNode *subroot;
    xmlChar *st;
    xmlChar *me;

    if (root->type != XML_ELEMENT_NODE || strcmp (root->name, "ocs") != 0) {
        g_warning ("Unidentified root XML block\n");
        return FALSE;
    }

    status = root->children;
    subroot = status;

    if (strcmp (status->name, "meta") == 0)
        status = status->children;

    if (status == NULL || strcmp (status->name, "status") != 0) {
        g_warning ("Unidentified status XML block\n");
        return FALSE;
    }

    st = xmlNodeGetContent (status);
    if (st != NULL) {
        if (strcmp (st, "ok") != 0) {
            status = status->next;

            if (status != NULL && strcmp (status->name, "message") == 0) {
                me = xmlNodeGetContent (status);
                g_warning ("Failed to retrieve informations on server: %s\n", me);
                xmlFree (me);
            }
            else
                g_warning ("Failed to retrieve informations on server\n");

            return FALSE;
        }

        xmlFree (st);
    }
    else
        return FALSE;

    if (data != NULL) {
        if (strcmp (subroot->next->name, "data") == 0) {
            *data = subroot->next;
        }
        else {
            g_warning ("Some data were expected, but none is found\n");
            *data = NULL;
        }
    }

    return TRUE;
}

static xmlNode* parse_provider_response (SoupMessageBody *response)
{
    xmlDocPtr doc;
    xmlNode *root;
    xmlNode *data;

    data = NULL;

    doc = xmlReadMemory (response->data, response->length, NULL, NULL, XML_PARSE_NOBLANKS);
    if (doc == NULL) {
        g_warning ("Unable to parse response from server\n");
        return NULL;
    }

    root = xmlDocGetRootElement (doc);

    if (check_provider_response (root, &data) == FALSE || data == NULL)
        goto finish;

    return data;

finish:
    xmlFreeDoc (doc);
    return data;
}

static SoupMessage* send_msg_to_server (OGDProvider *provider, const gchar *complete_query)
{
    guint sendret;
    SoupMessage *msg;

    msg = soup_message_new ("GET", complete_query);
    if (msg == NULL) {
        g_warning ("Unable to build request to server\n");
        return NULL;
    }

    sendret = soup_session_send_message (provider->priv->http_session, msg);
    if (sendret != 200) {
        g_warning ("Unable to send request to server, error %u\n", sendret);
        g_object_unref (msg);
        return NULL;
    }

    if (msg->status_code != SOUP_STATUS_OK) {
        g_warning ("Unable to submit request to server: %s\n", msg->reason_phrase);
        g_object_unref (msg);
        return NULL;
    }

    return msg;
}

/**
 * ogd_provider_get_raw:
 * @provider:       the #OGDProvider to query
 * @query:          request to send to the server
 *
 * Used to access contents from the #OGDProvider with any high-level handling, in raw XML format.
 * For normal usage #ogd_provider_get() is suggested instead.
 * 
 * Return value:    reference to a raw XML node, is the &lt;data&gt; section of the upcoming
 *                  response from the server. If an error is found reading the head of the
 *                  message, NULL is returned. The returned structure must be freed with
 *                  xmlFreeDoc(return_prt->doc) when no longer in use
 */
xmlNode* ogd_provider_get_raw (OGDProvider *provider, const gchar *query)
{
    gchar *complete_query;
    SoupMessage *msg;
    xmlNode *ret;

    ret = NULL;

    complete_query = g_strdup_printf ("%s%s", provider->priv->access_url, query);
    msg = send_msg_to_server (provider, complete_query);
    g_free (complete_query);

    if (msg != NULL) {
        ret = parse_provider_response (msg->response_body);
        g_object_unref (msg);
    }

    return ret;
}

/**
 * ogd_provider_header_from_raw:
 * @response:       xmlNode returned by ogd_provider_get_raw()
 *
 * Given an xmlNode from ogd_provider_get_raw(), read informations from the header of the same
 * response. Here can be found informations about the status of the message and, in some
 * situation, the number of elements involved in the query which produced the response itself
 *
 * Return value:    a #GHashTable with informations from the header, with keys assigned to the
 *                  name of the XML fields and values assigned with their contents. Has to be
 *                  freed with g_hash_table_unref() when no longer in use
 */
GHashTable* ogd_provider_header_from_raw (xmlNode *response)
{
    GHashTable *header;
    xmlNode *node;

    node = xmlDocGetRootElement (response->doc);
    node = node->children;

    if (node == NULL || strcmp (node->name, "meta") != 0) {
        g_warning ("No header in this response\n");
        return NULL;
    }

    header = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) xmlFree, (GDestroyNotify) xmlFree);

    for (node = node->children; node; node = node->next)
        g_hash_table_insert (header, strdup (node->name), xmlNodeGetContent (node));

    return header;
}

static GList* parse_xml_node_to_list_of_objects (xmlNode *data, OGDProvider *provider, GType obj_type)
{
    xmlNode *cursor;
    GList *ret;
    OGDObject *obj;

    ret = NULL;

    for (cursor = data->children; cursor; cursor = cursor->next) {
        obj = g_object_new (obj_type, NULL);

        if (ogd_object_fill_by_xml (obj, cursor, NULL) == TRUE) {
            ogd_object_set_provider (obj, provider);
            ret = g_list_prepend (ret, obj);
        }
    }

    if (ret)
        ret = g_list_reverse (ret);

    xmlFreeDoc (data->doc);
    return ret;
}

/**
 * ogd_provider_get:
 * @provider:       the #OGDProvider from which retrieve data
 * @query:          query to ask contents
 * @obj_type:       GType of the expected objects
 * 
 * To retrieve some content from the server
 * 
 * Return value:    a list of GObject of type @obj_type, or NULL if an error occours
 */

/*
    TODO    Provide also an async version
*/

GList* ogd_provider_get (OGDProvider *provider, const gchar *query, GType obj_type)
{   
    GList *ret;
    xmlNode *data;

    ret = NULL;

    data = ogd_provider_get_raw (provider, query);
    if (data != NULL)
        ret = parse_xml_node_to_list_of_objects (data, provider, obj_type);

    return ret;
}

/**
 * ogd_provider_put:
 * @provider:       the #OGDProvider to which save information
 * @query:          query to use to send contents
 * @data:           addictional data to ship with the command in "POST" section
 * 
 * To save some content on the server
 * 
 * Return value:    %TRUE if data are saved correctly, %FALSE otherwise
 */

/*
    TODO    Provide also an async version
*/

gboolean ogd_provider_put (OGDProvider *provider, const gchar *query, GHashTable *data)
{
    guint sendret;
    gboolean ret;
    gchar *complete_query;
    SoupMessage *msg;

    complete_query = g_strdup_printf ("%s%s", provider->priv->access_url, query);
    msg = soup_form_request_new_from_hash ("POST", complete_query, data);
    sendret = soup_session_send_message (provider->priv->http_session, msg);
    ret = (sendret == 200 && msg->status_code == SOUP_STATUS_OK);
    g_free (complete_query);
    g_object_unref (msg);
    return ret;
}

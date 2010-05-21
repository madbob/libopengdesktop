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
#include "ogd-provider.h"
#include "ogd-provider-private.h"
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
    SoupSession *async_http_session;

    gchar       *access_url;
};

G_DEFINE_TYPE (OGDProvider, ogd_provider, G_TYPE_OBJECT);

/*
    TODO    This is not good: an explicit counter of references is maintained, it would be far
            better to use pure GObject. But assigning a class_finalize callback to
            OGDProviderClass seems doesn't works: another solution must be found
*/
static int  InstancesCounter        = 0;

static void ogd_provider_finalize (GObject *obj)
{
    OGDProvider *provider;

    provider = OGD_PROVIDER (obj);

    PTR_CHECK_FREE_NULLIFY (provider->priv->server_name);
    PTR_CHECK_FREE_NULLIFY (provider->priv->access_url);
    OBJ_CHECK_UNREF_NULLIFY (provider->priv->http_session);
    OBJ_CHECK_UNREF_NULLIFY (provider->priv->async_http_session);

    InstancesCounter--;
    if (InstancesCounter == 0)
        finalize_types_management ();
}

static void ogd_provider_class_init (OGDProviderClass *klass)
{
    GObjectClass *gobject_class;

    /*
        When the first OGDProvider is allocated, the internal map for GTypes is inited. This is
        finalized when the last OGDProvider is freed, in ogd_provider_finalize()
    */
    init_types_management ();

    g_type_class_add_private (klass, sizeof (OGDProviderPrivate));

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = ogd_provider_finalize;
}

static void ogd_provider_init (OGDProvider *item)
{
    InstancesCounter++;
    item->priv = OGD_PROVIDER_GET_PRIVATE (item);
    memset (item->priv, 0, sizeof (OGDProviderPrivate));
    item->priv->http_session = soup_session_sync_new ();
    item->priv->async_http_session = soup_session_async_new ();
}

/**
 * ogd_provider_new;
 * @url:            HTTP address to which retrieve the exposed REST API of the desired service.
 *                  Please take note the complete HTTP requests will be submited as
 *                  http://$authentication@$this_url/$version_of_api_supported/$provided_query
 *
 * Allocates a new #OGDProvider, which will be used to retrieve remote contents. Pay attention to
 * the fact an #OGDProvider is required for all function provided by this library, and this type
 * of object is the first to be inited due internal disposition
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

    if (root->type != XML_ELEMENT_NODE || MYSTRCMP (root->name, "ocs") != 0) {
        g_warning ("Unidentified root XML block\n");
        return FALSE;
    }

    status = root->children;
    subroot = status;

    if (MYSTRCMP (status->name, "meta") == 0)
        status = status->children;

    if (status == NULL || MYSTRCMP (status->name, "status") != 0) {
        g_warning ("Unidentified status XML block\n");
        return FALSE;
    }

    st = xmlNodeGetContent (status);
    if (st != NULL) {
        if (MYSTRCMP (st, "ok") != 0) {
            status = status->next;

            if (status != NULL && MYSTRCMP (status->name, "message") == 0) {
                me = xmlNodeGetContent (status);
                g_warning ("Failed to retrieve informations on server: %s\n", me);
                xmlFree (me);
            }
            else {
                g_warning ("Failed to retrieve informations on server\n");
            }

            return FALSE;
        }

        xmlFree (st);
    }
    else
        return FALSE;

    if (data != NULL) {
        if (MYSTRCMP (subroot->next->name, "data") == 0) {
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

    if (check_provider_response (root, &data) == FALSE || data == NULL) {
        xmlFreeDoc (doc);
        return NULL;
    }
    else
        return data;
}

static GList* parse_xml_node_to_list_of_objects (xmlNode *data, OGDProvider *provider)
{
    xmlNode *cursor;
    GList *ret;
    GType obj_type;
    GError *error;
    OGDObject *obj;

    ret = NULL;

    for (cursor = data->children; cursor; cursor = cursor->next) {
        obj_type = retrieve_type ((const gchar*) cursor->name);
        obj = g_object_new (obj_type, NULL);
        error = NULL;

        if (ogd_object_fill_by_xml (obj, cursor, &error) == TRUE) {
            ogd_object_set_provider (obj, provider);
            ret = g_list_prepend (ret, obj);
        }
        else {
            g_warning ("%s", error->message);
            g_error_free (error);
        }
    }

    if (ret)
        ret = g_list_reverse (ret);

    xmlFreeDoc (data->doc);
    return ret;
}

static gboolean check_msg (SoupMessage *msg)
{
    if (msg->status_code != SOUP_STATUS_OK) {
        g_warning ("Unable to submit request to server: %s\n", msg->reason_phrase);
        g_object_unref (msg);
        return FALSE;
    }
    else
        return TRUE;
}

static void handle_async_get_response (SoupSession *session, SoupMessage *msg, gpointer userdata)
{
    xmlNode *ret;
    GList *list;
    GList *iter;
    xmlNode *cursor;
    AsyncRequestDesc *async;

    if (check_msg (msg) == FALSE)
        return;

    async = (AsyncRequestDesc*) userdata;
    ret = parse_provider_response (msg->response_body);

    if (async->objectize) {
        list = parse_xml_node_to_list_of_objects (ret, async->provider);

        for (iter = g_list_first (list); iter; iter = g_list_next (iter)) {
            async->callback ((OGDObject*) iter->data, async->userdata);
            g_object_unref (iter->data);
        }

        if (async->one_shot == FALSE)
            async->callback (NULL, async->userdata);

        g_list_free (list);
    }
    else {
        for (cursor = ret->children; cursor; cursor = cursor->next)
            async->rcallback ((xmlNode*) cursor, async->userdata);

        if (async->one_shot == FALSE)
            async->rcallback (NULL, async->userdata);
    }
}

static void send_async_msg_to_server (const gchar *complete_query, AsyncRequestDesc *async)
{
    SoupMessage *msg;

    msg = soup_message_new ("GET", complete_query);
    if (msg == NULL) {
        g_warning ("Unable to build request to server\n");
        return;
    }

    soup_session_queue_message (async->provider->priv->async_http_session, msg, handle_async_get_response, async);
}

/*
    Params:
        provider:   OGDProvider from which fetch contents
        query:      the query to execute
        single:     TRUE if a single object is expected, callback is no invoked with NULL at the end
        objects:    TRUE to build OGDObjects from incoming XML, FALSE to use raw data
        callback:   if objects == TRUE, callback to which pass built objects
        rcallback:  if objects == FALSE, callback to which pass raw XML
        userdata:   the user data for callback or rcallback
*/
static void get_async (OGDProvider *provider, gchar *query, gboolean single, gboolean objects,
                       OGDAsyncCallback callback, OGDProviderRawAsyncCallback rcallback, gpointer userdata)
{
    gchar *complete_query;
    AsyncRequestDesc *async;

    complete_query = g_strdup_printf ("%s%s", provider->priv->access_url, query);

    async = g_new0 (AsyncRequestDesc, 1);
    async->one_shot = single;
    async->userdata = userdata;
    async->callback = callback;
    async->rcallback = rcallback;
    async->provider = provider;
    async->objectize = objects;

    send_async_msg_to_server (complete_query, async);
    g_free (complete_query);
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

    if (check_msg (msg) == FALSE)
        return NULL;
    else
        return msg;
}

xmlNode* ogd_provider_get_raw (OGDProvider *provider, gchar *query)
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

void ogd_provider_get_raw_async (OGDProvider *provider, gchar *query, gboolean many, OGDProviderRawAsyncCallback callback, gpointer userdata)
{
    get_async (provider, query, many == FALSE, FALSE, NULL, callback, userdata);
}

GHashTable* ogd_provider_header_from_raw (xmlNode *response)
{
    GHashTable *header;
    xmlNode *node;

    node = xmlDocGetRootElement (response->doc);
    node = node->children;

    if (node == NULL || MYSTRCMP (node->name, "meta") != 0) {
        g_warning ("No header in this response\n");
        return NULL;
    }

    header = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) xmlFree, (GDestroyNotify) xmlFree);

    for (node = node->children; node; node = node->next)
        g_hash_table_insert (header, strdup ((char*) node->name), xmlNodeGetContent (node));

    return header;
}

/**
 * ogd_provider_get:
 * @provider:       the #OGDProvider from which retrieve data
 * @query:          query to ask contents
 *
 * To retrieve some content from the server
 *
 * Return value:    a list of GObject, or NULL if an error occours
 */
GList* ogd_provider_get (OGDProvider *provider, gchar *query)
{
    GList *ret;
    xmlNode *data;

    ret = NULL;

    data = ogd_provider_get_raw (provider, query);
    if (data != NULL)
        ret = parse_xml_node_to_list_of_objects (data, provider);

    return ret;
}

/**
 * ogd_provider_get_async:
 * @provider:       the #OGDProvider from which retrieve data
 * @query:          query to ask contents
 * @callback:       async callback to which incoming #OGDObject are passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_provider_get()
 */
void ogd_provider_get_async (OGDProvider *provider, gchar *query, OGDAsyncCallback callback, gpointer userdata)
{
    get_async (provider, query, FALSE, TRUE, callback, NULL, userdata);
}

void ogd_provider_get_single_async (OGDProvider *provider, gchar *query, OGDAsyncCallback callback, gpointer userdata)
{
    get_async (provider, query, TRUE, TRUE, callback, NULL, userdata);
}

static SoupMessage* prepare_message_to_put (OGDProvider *provider, gchar *query, GHashTable *data)
{
    gchar *complete_query;
    SoupMessage *msg;

    complete_query = g_strdup_printf ("%s%s", provider->priv->access_url, query);

    if (data != NULL)
        msg = soup_form_request_new_from_hash ("POST", complete_query, data);
    else
        msg = soup_form_request_new ("POST", complete_query, NULL, NULL);

    g_free (complete_query);
    return msg;
}

xmlNode* ogd_provider_put_raw (OGDProvider *provider, gchar *query, GHashTable *data)
{
    guint sendret;
    SoupMessage *msg;
    xmlNode *ret;

    ret = NULL;

    msg = prepare_message_to_put (provider, query, data);
    sendret = soup_session_send_message (provider->priv->http_session, msg);

    if (sendret == 200 && msg->status_code == SOUP_STATUS_OK) {
        ret = parse_provider_response (msg->response_body);
        g_object_unref (msg);
    }

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
gboolean ogd_provider_put (OGDProvider *provider, gchar *query, GHashTable *data)
{
    guint sendret;
    gboolean ret;
    SoupMessage *msg;

    msg = prepare_message_to_put (provider, query, data);
    sendret = soup_session_send_message (provider->priv->http_session, msg);
    ret = (sendret == 200 && msg->status_code == SOUP_STATUS_OK);
    g_object_unref (msg);
    return ret;
}

static void handle_async_put_response (SoupSession *session, SoupMessage *msg, gpointer userdata)
{
    gboolean result;
    AsyncRequestDesc *async;

    async = (AsyncRequestDesc*) userdata;

    if (async->pcallback != NULL) {
        result = ( msg->status_code == SOUP_STATUS_OK );
        async->pcallback (result, async->userdata);
    }

    g_free (async);
}

/**
 * ogd_provider_put_async:
 * @provider:       the #OGDProvider to which save information
 * @query:          query to use to send contents
 * @data:           addictional data to ship with the command in "POST" section
 * @callback:       async callback to which result of the operation is passed
 * @userdata:       the user data for the callback
 *
 * Async version of ogd_provider_put()
 */
void ogd_provider_put_async (OGDProvider *provider, gchar *query, GHashTable *data, OGDPutAsyncCallback callback, gpointer userdata)
{
    SoupMessage *msg;
    AsyncRequestDesc *async;

    msg = prepare_message_to_put (provider, query, data);

    async = g_new0 (AsyncRequestDesc, 1);
    async->pcallback = callback;
    async->userdata = userdata;

    soup_session_queue_message (provider->priv->async_http_session, msg, handle_async_put_response, async);

    g_object_unref (msg);
}

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

#ifndef OGD_PROVIDER_H
#define OGD_PROVIDER_H

G_BEGIN_DECLS

#include "ogd.h"

#define OGD_PROVIDER_TYPE             (ogd_provider_get_type ())
#define OGD_PROVIDER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                         OGD_PROVIDER_TYPE, OGDProvider))
#define OGD_PROVIDER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                         OGD_PROVIDER_TYPE,               \
                                         OGDProviderClass))
#define IS_OGD_PROVIDER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                         OGD_PROVIDER_TYPE))
#define IS_OGD_PROVIDER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                         OGD_PROVIDER_TYPE))
#define OGD_PROVIDER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                         OGD_PROVIDER_TYPE,               \
                                         OGDProviderClass))

typedef struct _OGDProvider        OGDProvider;
typedef struct _OGDProviderClass   OGDProviderClass;
typedef struct _OGDProviderPrivate OGDProviderPrivate;

struct _OGDProvider {
    GObject                 parent;
    OGDProviderPrivate      *priv;
};

struct _OGDProviderClass {
    GObjectClass    parent_class;
};

#include "ogd-object.h"

GType           ogd_provider_get_type               ();

OGDProvider*    ogd_provider_new                    (gchar *url);
void            ogd_provider_auth_user_and_pwd      (OGDProvider *provider, gchar *username, gchar *password);
void            ogd_provider_auth_api_key           (OGDProvider *provider, gchar *key);

const gchar*    ogd_provider_get_url                (OGDProvider *provider);

GList*          ogd_provider_get                    (OGDProvider *provider, gchar *query);
void            ogd_provider_get_async              (OGDProvider *provider, gchar *query, OGDAsyncCallback callback, gpointer userdata);
void            ogd_provider_get_list_async         (OGDProvider *provider, gchar *query, OGDAsyncListCallback callback, gpointer userdata);
gboolean        ogd_provider_put                    (OGDProvider *provider, gchar *query, GHashTable *data);
void            ogd_provider_put_async              (OGDProvider *provider, gchar *query, GHashTable *data, OGDPutAsyncCallback callback, gpointer userdata);

G_END_DECLS

#endif /* OGD_PROVIDER_H */

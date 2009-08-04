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

#ifndef OGD_PROVIDER_PRIVATE_H
#define OGD_PROVIDER_PRIVATE_H

#include "ogd-provider.h"

typedef void (*OGDProviderRawAsyncCallback) (xmlNode *node, gpointer userdata);

xmlNode*        ogd_provider_get_raw                (OGDProvider *provider, gchar *query);
void            ogd_provider_get_raw_async          (OGDProvider *provider, gchar *query, OGDProviderRawAsyncCallback rcallback, gpointer userdata);
GHashTable*     ogd_provider_header_from_raw        (xmlNode *response);
void            ogd_provider_get_single_async       (OGDProvider *provider, gchar *query, OGDAsyncCallback callback, gpointer userdata);

#endif /* OGD_PROVIDER_PRIVATE_H */

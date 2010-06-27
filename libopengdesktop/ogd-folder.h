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

#ifndef OGD_FOLDER_H
#define OGD_FOLDER_H

G_BEGIN_DECLS

#include "ogd-provider.h"

#define OGD_FOLDER_TYPE                 (ogd_folder_get_type ())
#define OGD_FOLDER(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                         OGD_FOLDER_TYPE, OGDFolder))
#define OGD_FOLDER_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                         OGD_FOLDER_TYPE,               \
                                         OGDFolderClass))
#define IS_OGD_FOLDER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                         OGD_FOLDER_TYPE))
#define IS_OGD_FOLDER_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                         OGD_FOLDER_TYPE))
#define OGD_FOLDER_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                         OGD_FOLDER_TYPE,               \
                                         OGDFolderClass))

typedef struct _OGDFolder        OGDFolder;
typedef struct _OGDFolderClass   OGDFolderClass;
typedef struct _OGDFolderPrivate OGDFolderPrivate;

struct _OGDFolder {
    OGDObject                parent;
    OGDFolderPrivate        *priv;
};

struct _OGDFolderClass {
    OGDObjectClass          parent_class;
};

/**
 * OGD_FOLDER_SORTING:
 * @OGD_FOLDER_SORT_NEWEST:           newer content come before
 * @OGD_FOLDER_SORT_ALPHABETICAL:     contents ordered alphabetically, by name
 * @OGD_FOLDER_SORT_RATING:           contents with higher scores come before
 * @OGD_FOLDER_SORT_DOWNLOADS:        contents with most downloads come before
 *
 * Types of sorting for listing #OGDContents into a #OGDFolder
 */
typedef enum {
    OGD_FOLDER_UNDEFINED,
    OGD_FOLDER_INBOX,
    OGD_FOLDER_SEND,
    OGD_FOLDER_TRASH
} OGD_FOLDER_CATEGORY;

GType                   ogd_folder_get_type             ();

GList*                  ogd_folder_fetch_all            (OGDProvider *provider);
void                    ogd_folder_fetch_all_async      (OGDProvider *provider, OGDAsyncCallback callback, gpointer userdata);

const gchar*            ogd_folder_get_id               (OGDFolder *folder);
const gchar*            ogd_folder_get_name             (OGDFolder *folder);
OGD_FOLDER_CATEGORY     ogd_folder_get_category         (OGDFolder *folder);
guint                   ogd_folder_get_count            (OGDFolder *folder);
OGDIterator*            ogd_folder_get_contents         (OGDFolder *folder);

G_END_DECLS

#endif /* OGD_FOLDER_H */

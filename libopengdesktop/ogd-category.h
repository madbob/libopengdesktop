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

#ifndef OGD_CATEGORY_H
#define OGD_CATEGORY_H

G_BEGIN_DECLS

#include "ogd-provider.h"

#define OGD_CATEGORY_TYPE             (ogd_category_get_type ())
#define OGD_CATEGORY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                         OGD_CATEGORY_TYPE, OGDCategory))
#define OGD_CATEGORY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                         OGD_CATEGORY_TYPE,               \
                                         OGDCategoryClass))
#define IS_OGD_CATEGORY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                         OGD_CATEGORY_TYPE))
#define IS_OGD_CATEGORY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                         OGD_CATEGORY_TYPE))
#define OGD_CATEGORY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                         OGD_CATEGORY_TYPE,               \
                                         OGDCategoryClass))

typedef struct _OGDCategory        OGDCategory;
typedef struct _OGDCategoryClass   OGDCategoryClass;
typedef struct _OGDCategoryPrivate OGDCategoryPrivate;

struct _OGDCategory {
    OGDObject                parent;
    OGDCategoryPrivate        *priv;
};

struct _OGDCategoryClass {
    OGDObjectClass          parent_class;
};

/**
 * OGD_CATEGORY_SORTING:
 * @OGD_CATEGORY_SORT_NEWEST:           newer content come before
 * @OGD_CATEGORY_SORT_ALPHABETICAL:     contents ordered alphabetically, by name
 * @OGD_CATEGORY_SORT_RATING:           contents with higher scores come before
 * @OGD_CATEGORY_SORT_DOWNLOADS:        contents with most downloads come before
 * 
 * Types of sorting for listing #OGDContents into a #OGDCategory
 */
typedef enum {
    OGD_CATEGORY_SORT_NEWEST,
    OGD_CATEGORY_SORT_ALPHABETICAL,
    OGD_CATEGORY_SORT_RATING,
    OGD_CATEGORY_SORT_DOWNLOADS
} OGD_CATEGORY_SORTING;

GType                   ogd_category_get_type               ();

GList*                  ogd_category_fetch_all              (OGDProvider *provider);
void                    ogd_category_fetch_all_async        (OGDProvider *provider, OGDAsyncCallback callback, gpointer userdata);

const gchar*            ogd_category_get_id                 (OGDCategory *category);
const gchar*            ogd_category_get_name               (OGDCategory *category);
OGDIterator*            ogd_category_get_contents           (OGDCategory *category, OGD_CATEGORY_SORTING sorting);

G_END_DECLS

#endif /* OGD_CATEGORY_H */

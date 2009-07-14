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

#ifndef OGD_PRIVATE_UTILS_H
#define OGD_PRIVATE_UTILS_H

#define PTR_CHECK_FREE_NULLIFY(__ptr) {     \
    if (__ptr != NULL) {                    \
        g_free (__ptr);                     \
        __ptr = NULL;                       \
    }                                       \
}

#define OBJ_CHECK_UNREF_NULLIFY(__ptr) {    \
    if (__ptr != NULL) {                    \
        g_object_unref (__ptr);             \
        __ptr = NULL;                       \
    }                                       \
}

#define STRLIST_CHECK_FREE_NULLIFY(__ptr) {                                         \
    if (__ptr != NULL) {                                                            \
        GList *__iter;                                                              \
        for (__iter = g_list_first (__ptr); __iter; __iter = g_list_next (__iter))  \
            g_free (__iter->data);                                                  \
        g_list_free (__ptr);                                                        \
        __ptr = NULL;                                                               \
    }                                                                               \
}

GDate*      node_to_date                (xmlNode *node);
guint64     node_to_num                 (xmlNode *node);
gdouble     node_to_double              (xmlNode *node);

gulong      total_items_for_query       (xmlNode *package);

#endif /* OGD_PRIVATE_UTILS_H */

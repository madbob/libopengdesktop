/*  libopengdesktop
 *  Copyright (C) 2009/2012 Roberto -MadBob- Guido <bob4job@gmail.com>
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

#include "ogd-provider-private.h"

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

#define DATE_CHECK_FREE_NULLIFY(__date) {   \
    if (__date != NULL) {                   \
        g_date_free (__date);               \
        __date = NULL;                      \
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

#define FREE_LIST_OF_OBJECTS(__list) {                                  \
    GList *__iter;                                                      \
                                                                        \
    if (__list != NULL) {                                               \
        for (__iter = __list; __iter; __iter = g_list_next (__iter))    \
            g_object_unref (__iter->data);                              \
        g_list_free (__list);                                           \
    }                                                                   \
}

#define MEMDUP(__a) ({                      \
    void *__b;                              \
    int __size;                             \
                                            \
    __size = sizeof (typeof(__a));          \
    __b = g_new0 (typeof(__a), 1);          \
    memcpy (__b, &__a, __size);             \
    __b;                                    \
})

#define SET_STRING(__obj, __field, __value) {             \
    PTR_CHECK_FREE_NULLIFY (__obj->priv->__field);        \
    __obj->priv->__field = g_strdup (__value);            \
}

#define SET_OBJECT(__obj, __field, __value) {             \
    OBJ_CHECK_UNREF_NULLIFY (__obj->priv->__field);       \
    __obj->priv->__field = g_object_ref (__value);        \
}

#define MYSTRCMP(__a,__b)       strcmp ((char*) __a, (char*) __b)
#define MYGETCONTENT(__a)       (char*) xmlNodeGetContent(__a)

typedef struct {
    OGDProvider                 *provider;
    OGDObject                   *reference;
    GList*                      list;
    gboolean                    one_shot;
    gboolean                    objectize;

    gpointer                    userdata;
    OGDAsyncCallback            callback;
    OGDProviderRawAsyncCallback rcallback;
    OGDPutAsyncCallback         pcallback;
    OGDAsyncListCallback        lcallback;

    gulong                      total;
    gulong                      counter;
} AsyncRequestDesc;

GDate*      node_to_date                (xmlNode *node);
guint64     node_to_num                 (xmlNode *node);
gdouble     node_to_double              (xmlNode *node);

gulong      total_items_for_query       (xmlNode *package);
GList*      list_of_people              (OGDObject *reference, gchar *query);
void        list_of_people_async        (OGDObject *reference, gchar *query, OGDAsyncListCallback callback, gpointer userdata);

void        init_types_management       ();
GType       retrieve_type               (const gchar *xml_name);
void        finalize_types_management   ();

#endif /* OGD_PRIVATE_UTILS_H */

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

#ifndef OGD_ITERATOR_H
#define OGD_ITERATOR_H

G_BEGIN_DECLS

#include "ogd-provider.h"

#define OGD_ITERATOR_TYPE             (ogd_iterator_get_type ())
#define OGD_ITERATOR(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                         OGD_ITERATOR_TYPE, OGDIterator))
#define OGD_ITERATOR_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                         OGD_ITERATOR_TYPE,               \
                                         OGDIteratorClass))
#define IS_OGD_ITERATOR(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                         OGD_ITERATOR_TYPE))
#define IS_OGD_ITERATOR_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                         OGD_ITERATOR_TYPE))
#define OGD_ITERATOR_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                         OGD_ITERATOR_TYPE,               \
                                         OGDIteratorClass))

typedef struct _OGDIterator        OGDIterator;
typedef struct _OGDIteratorClass   OGDIteratorClass;
typedef struct _OGDIteratorPrivate OGDIteratorPrivate;

struct _OGDIterator {
    GObject                 parent;
    OGDIteratorPrivate      *priv;
};

struct _OGDIteratorClass {
    GObjectClass    parent_class;
};

GType           ogd_iterator_get_type               ();

OGDIterator*    ogd_iterator_new                    (const OGDProvider *provider, const gchar *base_query);

GList*          ogd_iterator_get_slice              (OGDIterator *iter, gulong start, gulong quantity);
GList*          ogd_iterator_next_slice             (OGDIterator *iter);
void            ogd_iterator_set_step               (OGDIterator *iter, gulong step);

G_END_DECLS

#endif /* OGD_ITERATOR_H */

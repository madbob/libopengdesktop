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

#ifndef OGD_OBJECT_H
#define OGD_OBJECT_H

G_BEGIN_DECLS

#define OGD_OBJECT_TYPE             (ogd_object_get_type ())
#define OGD_OBJECT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                         OGD_OBJECT_TYPE, OGDObject))
#define OGD_OBJECT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                         OGD_OBJECT_TYPE,               \
                                         OGDObjectClass))
#define IS_OGD_OBJECT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                         OGD_OBJECT_TYPE))
#define IS_OGD_OBJECT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                         OGD_OBJECT_TYPE))
#define OGD_OBJECT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                         OGD_OBJECT_TYPE,               \
                                         OGDObjectClass))

typedef struct _OGDObject        OGDObject;
typedef struct _OGDObjectClass   OGDObjectClass;
typedef struct _OGDObjectPrivate OGDObjectPrivate;

struct _OGDObject {
    GObject                 parent;
    OGDObjectPrivate        *priv;
};

struct _OGDObjectClass {
    GObjectClass        parent_class;

    gboolean (*fill_by_xml)     (OGDObject *obj, const xmlNode *xml, GError **error);
    gboolean (*fill_by_id)      (OGDObject *obj, const gchar *id, GError **error);
};

GType           ogd_object_get_type                 ();

OGDProvider*    ogd_object_get_provider             (OGDObject *obj);
void            ogd_object_set_provider             (OGDObject *obj, OGDProvider *provider);

gboolean        ogd_object_fill_by_xml              (OGDObject *obj, const xmlNode *xml, GError **error);
gboolean        ogd_object_fill_by_id               (OGDObject *obj, const gchar *id, GError **error);

G_END_DECLS

#endif /* OGD_OBJECT_H */

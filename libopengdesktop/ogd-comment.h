/*  libopengdesktop
 *  Copyright (C) 2010 Roberto -MadBob- Guido <madbob@users.barberaware.org>
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

#ifndef OGD_COMMENT_H
#define OGD_COMMENT_H

G_BEGIN_DECLS

#define OGD_COMMENT_TYPE            (ogd_comment_get_type ())
#define OGD_COMMENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                     OGD_COMMENT_TYPE, OGDComment))
#define OGD_COMMENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                     OGD_COMMENT_TYPE,                  \
                                     OGDCommentClass))
#define IS_OGD_COMMENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                     OGD_COMMENT_TYPE))
#define IS_OGD_COMMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                     OGD_COMMENT_TYPE))
#define OGD_COMMENT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                     OGD_COMMENT_TYPE,                  \
                                     OGDCommentClass))

typedef struct _OGDComment          OGDComment;
typedef struct _OGDCommentClass     OGDCommentClass;
typedef struct _OGDCommentPrivate   OGDCommentPrivate;

struct _OGDComment {
    OGDObject                parent;
    OGDCommentPrivate        *priv;
};

struct _OGDCommentClass {
    OGDObjectClass          parent_class;
};

GType                   ogd_comment_get_type                   ();

const gchar*            ogd_comment_get_authorid               (OGDComment *comment);
const GDate*            ogd_comment_get_date                   (OGDComment *comment);
const gchar*            ogd_comment_get_subject                (OGDComment *comment);
const gchar*            ogd_comment_get_message                (OGDComment *comment);
const GList*            ogd_comment_get_children               (OGDComment *comment);

G_END_DECLS

#endif /* OGD_COMMENT_H */

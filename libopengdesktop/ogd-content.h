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

#ifndef OGD_CONTENT_H
#define OGD_CONTENT_H

G_BEGIN_DECLS

#include "ogd-person.h"

#define OGD_CONTENT_TYPE             (ogd_content_get_type ())
#define OGD_CONTENT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                         OGD_CONTENT_TYPE, OGDContent))
#define OGD_CONTENT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                         OGD_CONTENT_TYPE,               \
                                         OGDContentClass))
#define IS_OGD_CONTENT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                         OGD_CONTENT_TYPE))
#define IS_OGD_CONTENT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                         OGD_CONTENT_TYPE))
#define OGD_CONTENT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                         OGD_CONTENT_TYPE,               \
                                         OGDContentClass))

typedef struct _OGDContent        OGDContent;
typedef struct _OGDContentClass   OGDContentClass;
typedef struct _OGDContentPrivate OGDContentPrivate;

struct _OGDContent {
    OGDObject                parent;
    OGDContentPrivate        *priv;
};

struct _OGDContentClass {
    OGDObjectClass          parent_class;
};

/**
 * OGD_CONTENT_VOTE:
 * @OGD_CONTENT_BAD:            content not has been appreciated
 * @OGD_CONTENT_GOOD:           content has been appreciated
 * 
 * Type of rating which may be assigned by the current user to a specific content with
 * ogd_content_vote()
 */
typedef enum {
    OGD_CONTENT_BAD,
    OGD_CONTENT_GOOD
} OGD_CONTENT_VOTE;

GType                   ogd_content_get_type                ();

const gchar*            ogd_content_get_id                  (OGDContent *content);
const gchar*            ogd_content_get_name                (OGDContent *content);
const gchar*            ogd_content_get_version             (OGDContent *content);
const gchar*            ogd_content_get_language            (OGDContent *content);
const OGDPerson*        ogd_content_get_author              (OGDContent *content);
const GDate*            ogd_content_get_creation_date       (OGDContent *content);
const GDate*            ogd_content_get_change_date         (OGDContent *content);
gulong                  ogd_content_get_num_downloads       (OGDContent *content);
guint                   ogd_content_get_score               (OGDContent *content);
const gchar*            ogd_content_get_description         (OGDContent *content);
const gchar*            ogd_content_get_changelog           (OGDContent *content);
const gchar*            ogd_content_get_homepage            (OGDContent *content);
gulong                  ogd_content_get_num_comments        (OGDContent *content);
gulong                  ogd_content_get_num_fans            (OGDContent *content);
const GList*            ogd_content_get_previews            (OGDContent *content);
const GList*            ogd_content_get_download_refs       (OGDContent *content);

void                    ogd_content_vote                    (OGDContent *content, OGD_CONTENT_VOTE vote);

G_END_DECLS

#endif /* OGD_CONTENT_H */

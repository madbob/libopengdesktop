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

OGDContent*             ogd_content_new                     (OGDProvider *provider);
OGDContent*             ogd_content_new_by_id               (OGDProvider *provider, const gchar *id);
void                    ogd_content_new_by_id_async         (OGDProvider *provider, const gchar *id, OGDAsyncCallback callback, gpointer userdata);
const gchar*            ogd_content_get_id                  (OGDContent *content);
const OGDCategory*      ogd_content_get_category            (OGDContent *content);
void                    ogd_content_set_category            (OGDContent *content, OGDCategory *category);
const gchar*            ogd_content_get_name                (OGDContent *content);
void                    ogd_content_set_name                (OGDContent *content, gchar *name);
const gchar*            ogd_content_get_version             (OGDContent *content);
void                    ogd_content_set_version             (OGDContent *content, gchar *version);
const gchar*            ogd_content_get_language            (OGDContent *content);
void                    ogd_content_set_language            (OGDContent *content, gchar *language);
const gchar*            ogd_content_get_authorid            (OGDContent *content);
const GDate*            ogd_content_get_creation_date       (OGDContent *content);
const GDate*            ogd_content_get_change_date         (OGDContent *content);
gulong                  ogd_content_get_num_downloads       (OGDContent *content);
guint                   ogd_content_get_score               (OGDContent *content);
const gchar*            ogd_content_get_description         (OGDContent *content);
void                    ogd_content_set_description         (OGDContent *content, gchar *description);
const gchar*            ogd_content_get_changelog           (OGDContent *content);
void                    ogd_content_set_changelog           (OGDContent *content, gchar *changelog);
const gchar*            ogd_content_get_homepage            (OGDContent *content);
void                    ogd_content_set_homepage            (OGDContent *content, gchar *homepage);
GList*                  ogd_content_get_comments            (OGDContent *content);
void                    ogd_content_get_comments_async      (OGDContent *content, OGDAsyncListCallback callback, gpointer userdata);
GList*                  ogd_content_get_fans                (OGDContent *content);
void                    ogd_content_get_fans_async          (OGDContent *content, OGDAsyncListCallback callback, gpointer userdata);
const GList*            ogd_content_get_previews            (OGDContent *content);
const GList*            ogd_content_get_download_refs       (OGDContent *content);
void                    ogd_content_set_download_refs       (OGDContent *content, const GList *links);

#ifndef OGD_DISABLE_DEPRECATED
gulong                  ogd_content_get_num_comments        (OGDContent *content);
gulong                  ogd_content_get_num_fans            (OGDContent *content);
#endif

void                    ogd_content_vote                    (OGDContent *content, OGD_CONTENT_VOTE vote);
void                    ogd_content_vote_async              (OGDContent *content, OGD_CONTENT_VOTE vote, OGDPutAsyncCallback callback, gpointer userdata);
void                    ogd_content_set_fan                 (OGDContent *content, gboolean fan);
void                    ogd_content_set_fan_async           (OGDContent *content, gboolean fan, OGDPutAsyncCallback callback, gpointer userdata);
void                    ogd_content_add_comment             (OGDContent *content, gchar *subject, gchar *message);
void                    ogd_content_add_comment_async       (OGDContent *content, gchar *subject, gchar *message, OGDPutAsyncCallback callback, gpointer userdata);

void                    ogd_content_save                    (OGDContent *content);
void                    ogd_content_remove                  (OGDContent *content);

G_END_DECLS

#endif /* OGD_CONTENT_H */

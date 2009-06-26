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

#ifndef OGD_PERSON_H
#define OGD_PERSON_H

G_BEGIN_DECLS

#include "ogd-object.h"

#define OGD_PERSON_TYPE             (ogd_person_get_type ())
#define OGD_PERSON(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                         OGD_PERSON_TYPE, OGDPerson))
#define OGD_PERSON_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                         OGD_PERSON_TYPE,               \
                                         OGDPersonClass))
#define IS_OGD_PERSON(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                         OGD_PERSON_TYPE))
#define IS_OGD_PERSON_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                         OGD_PERSON_TYPE))
#define OGD_PERSON_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                         OGD_PERSON_TYPE,               \
                                         OGDPersonClass))

typedef struct _OGDPerson        OGDPerson;
typedef struct _OGDPersonClass   OGDPersonClass;
typedef struct _OGDPersonPrivate OGDPersonPrivate;

struct _OGDPerson {
    OGDObject               parent;
    OGDPersonPrivate        *priv;
};

struct _OGDPersonClass {
    OGDObjectClass          parent_class;
};

typedef enum {
    OGD_PERSON_GENDER_UNDEFINED,
    OGD_PERSON_MAN,
    OGD_PERSON_WOMAN
} OGD_PERSON_GENDER;

typedef enum {
    OGD_PERSON_ROLE_UNDEFINED,
    OGD_PERSON_USER,
    OGD_PERSON_DEVELOPER,
    OGD_PERSON_ARTIST,
    OGD_PERSON_SUPPORTER
} OGD_PERSON_ROLE;

typedef enum {
    OGD_PERSON_JOB_UNDEFINED,
    OGD_PERSON_JOB_WORKING,
    OGD_PERSON_JOB_STUDENT,
    OGD_PERSON_JOB_LOOKING,
    OGD_PERSON_JOB_RETIRED,
    OGD_PERSON_JOB_FREE
} OGD_PERSON_JOB;

typedef struct {
    gchar       *currency;
    gdouble     balance;
} OGDPersonBalance;

GType                   ogd_person_get_type                 ();

const gchar*            ogd_person_get_id                   (OGDPerson *person);
guint                   ogd_person_get_privacy              (OGDPerson *person);
const gchar*            ogd_person_get_firstname            (OGDPerson *person);
const gchar*            ogd_person_get_lastname             (OGDPerson *person);
OGD_PERSON_GENDER       ogd_person_get_gender               (OGDPerson *person);
OGD_PERSON_ROLE         ogd_person_get_communityrole        (OGDPerson *person);
const gchar*            ogd_person_get_homepage             (OGDPerson *person);
const gchar*            ogd_person_get_company              (OGDPerson *person);
const gchar*            ogd_person_get_avatar               (OGDPerson *person);
const GDate*            ogd_person_get_birthday             (OGDPerson *person);
OGD_PERSON_JOB          ogd_person_get_job_status           (OGDPerson *person);
const gchar*            ogd_person_get_city                 (OGDPerson *person);
const gchar*            ogd_person_get_country              (OGDPerson *person);
gdouble                 ogd_person_get_latitude             (OGDPerson *person);
gdouble                 ogd_person_get_longitude            (OGDPerson *person);
const gchar*            ogd_person_get_likes                (OGDPerson *person);
const gchar*            ogd_person_get_dont_likes           (OGDPerson *person);
const gchar*            ogd_person_get_interests            (OGDPerson *person);
const gchar*            ogd_person_get_languages            (OGDPerson *person);
const gchar*            ogd_person_get_programming_langs    (OGDPerson *person);
const gchar*            ogd_person_get_favourite_quote      (OGDPerson *person);
const gchar*            ogd_person_get_favourite_music      (OGDPerson *person);
const gchar*            ogd_person_get_favourite_tv         (OGDPerson *person);
const gchar*            ogd_person_get_favourite_movies     (OGDPerson *person);
const gchar*            ogd_person_get_favourite_books      (OGDPerson *person);
const gchar*            ogd_person_get_favourite_games      (OGDPerson *person);
const gchar*            ogd_person_get_description          (OGDPerson *person);
const gchar*            ogd_person_get_profile_page         (OGDPerson *person);
const OGDPersonBalance* ogd_person_get_balance              (OGDPerson *person);
GList*                  ogd_person_get_friends              (OGDPerson *person);

const OGDPerson*        ogd_person_get_myself               ();
void                    ogd_person_myself_set_coordinates   (gdouble latitude, gdouble longitude);
void                    ogd_person_myself_invite_friend     (OGDPerson *person);

G_END_DECLS

#endif /* OGD_PERSON_H */

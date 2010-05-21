/*  libopengdesktop 0.3
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

/**
 * OGD_PERSON_PRIVACY:
 * @OGD_PERSON_PROFILE_PUBLIC:              everyone may see information about the #OGDPerson
 * @OGD_PERSON_PROFILE_ONLY_REGISTERED:     only authenticated user may see informations
 * @OGD_PERSON_PROFILE_ONLY_FRIENDS:        only friends of the #OGDPerson may see informations
 * @OGD_PERSON_PROFILE_PRIVATE:             nobody can see informations
 *
 * Type of privacy settings for a #OGDPerson, in function of this some informations may be
 * accessed or not
 */
typedef enum {
    OGD_PERSON_PROFILE_PUBLIC               = 0,
    OGD_PERSON_PROFILE_ONLY_REGISTERED,
    OGD_PERSON_PROFILE_ONLY_FRIENDS,
    OGD_PERSON_PROFILE_PRIVATE
} OGD_PERSON_PRIVACY;

/**
 * OGD_PERSON_GENDER:
 * @OGD_PERSON_GENDER_UNDEFINED:            the person has not specified his own gender
 * @OGD_PERSON_MAN:                         #OGDPerson is a man
 * @OGD_PERSON_WOMAN:                       #OGDPerson is a woman
 *
 * Describe gender of a #OGDPerson
 */
typedef enum {
    OGD_PERSON_GENDER_UNDEFINED,
    OGD_PERSON_MAN,
    OGD_PERSON_WOMAN
} OGD_PERSON_GENDER;

/**
 * OGD_PERSON_ROLE:
 * @OGD_PERSON_ROLE_UNDEFINED:              the role of #OGDPerson is not specified
 * @OGD_PERSON_USER:                        the #OGDPerson is a user
 * @OGD_PERSON_DEVELOPER:                   the #OGDPerson is a developer
 * @OGD_PERSON_ARTIST:                      the #OGDPerson is an artist
 * @OGD_PERSON_SUPPORTER:                   the #OGDPerson is a supporter
 *
 * Possible roles in community a #OGDPerson may have
 */
typedef enum {
    OGD_PERSON_ROLE_UNDEFINED,
    OGD_PERSON_USER,
    OGD_PERSON_DEVELOPER,
    OGD_PERSON_ARTIST,
    OGD_PERSON_SUPPORTER
} OGD_PERSON_ROLE;

/**
 * OGD_PERSON_JOB:
 * @OGD_PERSON_JOB_UNDEFINED:               the job status of #OGDPerson is undefined
 * @OGD_PERSON_JOB_WORKING:                 the #OGDPerson has a job
 * @OGD_PERSON_JOB_STUDENT:                 the #OGDPerson is a student
 * @OGD_PERSON_JOB_LOOKING:                 the #OGDPerson is looking for a job
 * @OGD_PERSON_JOB_RETIRED:                 the #OGDPerson is retired
 * @OGD_PERSON_JOB_FREE:                    the #OGDPerson has not a job
 *
 * Possible working status of a #OGDPerson
 */
typedef enum {
    OGD_PERSON_JOB_UNDEFINED,
    OGD_PERSON_JOB_WORKING,
    OGD_PERSON_JOB_STUDENT,
    OGD_PERSON_JOB_LOOKING,
    OGD_PERSON_JOB_RETIRED,
    OGD_PERSON_JOB_FREE
} OGD_PERSON_JOB;

GType                   ogd_person_get_type                 ();

const gchar*            ogd_person_get_id                   (OGDPerson *person);
OGD_PERSON_PRIVACY      ogd_person_get_privacy              (OGDPerson *person);
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
const GList*            ogd_person_get_friends              (OGDPerson *person);
void                    ogd_person_get_friends_async        (OGDPerson *person, OGDAsyncCallback callback, gpointer userdata);

OGDPerson*              ogd_person_get_myself                   (OGDProvider *provider);
void                    ogd_person_get_myself_async             (OGDProvider *provider, OGDAsyncCallback callback, gpointer userdata);
void                    ogd_person_myself_set_coordinates       (OGDPerson *myself, gdouble latitude, gdouble longitude);
void                    ogd_person_myself_set_coordinates_async (OGDPerson *myself, gdouble latitude, gdouble longitude,
                                                                 OGDPutAsyncCallback callback, gpointer userdata);
void                    ogd_person_myself_invite_friend         (OGDPerson *person, gchar *message);
void                    ogd_person_myself_invite_friend_async   (OGDPerson *person, gchar *message, OGDPutAsyncCallback callback, gpointer userdata);
GList*                  ogd_person_myself_pending_friends       (OGDPerson *person);
void                    ogd_person_myself_action_on_friend      (OGDPerson *person, gboolean accept, OGDPerson *friend);

G_END_DECLS

#endif /* OGD_PERSON_H */

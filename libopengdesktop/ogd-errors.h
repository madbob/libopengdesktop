/*  libopengdesktop
 *  Copyright (C) 2009/2011 Roberto -MadBob- Guido <bob4job@gmail.com>
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

#ifndef OGD_ERRORS_H
#define OGD_ERRORS_H

#define OGD_PARSING_ERROR_DOMAIN    g_quark_from_string("ParsingError")
#define OGD_HIERARCHY_ERROR_DOMAIN  g_quark_from_string("HierarchyError")
#define OGD_TYPE_ERROR_DOMAIN       g_quark_from_string("TypeError")
#define OGD_NETWORK_ERROR_DOMAIN    g_quark_from_string("NetworkError")

typedef enum {
    OGD_XML_ERROR,
    OGD_HIERARCHY_ERROR,
    OGD_TYPE_ERROR,
    OGD_NETWORK_ERROR,
    OGD_END_ERRORS
} OGD_ERRORS;

#endif /* OGD_ERRORS_H */

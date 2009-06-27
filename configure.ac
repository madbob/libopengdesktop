m4_define([libogd_major_version], [0])
m4_define([libogd_minor_version], [1])
m4_define([libogd_micro_version], [0])

m4_define([libogd_version],
          [libogd_major_version.libogd_minor_version.libogd_micro_version])

m4_define([libogd_interface_age], [0])
m4_define([libogd_binary_age],
          [m4_eval(100 * libogd_minor_version + libogd_micro_version)])

m4_define([lt_current],
          [m4_eval(100 * libogd_minor_version + libogd_micro_version - libogd_interface_age)])
m4_define([lt_revision], [libogd_interface_age])
m4_define([lt_age],
          [m4_eval(libogd_binary_age - libogd_interface_age)])

m4_define([glib_req_version], [2.16])
m4_define([soup_req_version], [2.4.1])
m4_define([xml_req_version], [2.7.3])

AC_PREREQ([2.59])
AC_INIT([libogd], [libogd_version], [], [libogd])

AM_INIT_AUTOMAKE([1.10])
AM_CONFIG_HEADER([config.h])

GNOME_COMMON_INIT
GNOME_COMPILE_WARNINGS
GNOME_DEBUG_CHECK

AM_DISABLE_STATIC
AM_PATH_GLIB_2_0
AM_PROG_CC_C_O
AM_PROG_LIBTOOL

AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])
AC_C_CONST
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_PATH_PROG([GLIB_GENMARSHAL], [glib-genmarshal])
AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])
AC_CHECK_FUNCS([strptime localtime_r])

LIBOGD_MAJOR_VERSION=libogd_major_version
LIBOGD_MINOR_VERSION=libogd_minor_version
LIBOGD_MICRO_VERSION=libogd_micro_version
LIBOGD_VERSION=libogd_version
AC_SUBST(LIBOGD_MAJOR_VERSION)
AC_SUBST(LIBOGD_MICRO_VERSION)
AC_SUBST(LIBOGD_MINOR_VERSION)
AC_SUBST(LIBOGD_VERSION)

dnl libogd checks
PKG_CHECK_MODULES(LIBOGD,
                  gobject-2.0 >= glib_req_version dnl
                  libxml-2.0 >= xml_req_version dnl
                  libsoup-2.4 >= soup_req_version)
AC_SUBST(LIBOGD_CFLAGS)
AC_SUBST(LIBOGD_LIBS)

dnl = Enable strict compiler flags =========================================

# use strict compiler flags only on development releases
#m4_define([maintainer_flags_default],
#          m4_if(m4_eval(libogd_minor_version % 2), [1], [yes], [no]))
m4_define([maintainer_flags_default], [no])
AC_ARG_ENABLE([maintainer-flags],
              AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                             [Use strict compiler flags @<:@default=maintainer_flags_default@:>@]),,
              enable_maintainer_flags=maintainer_flags_default)

if test "x$enable_maintainer_flags" = "xyes"; then
  CPPFLAGS="$CPPFLAGS -g -Wall -Wshadow -Wcast-align -Wno-uninitialized -Werror"
else
  CPPFLAGS="$CPPFLAGS -g -Wall"
fi

dnl Internationalization
GETTEXT_PACKAGE=libogd
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], "$GETTEXT_PACKAGE", [Define the gettext package to use])
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT

dnl gnome-doc-utils stuff
GNOME_DOC_INIT

GTK_DOC_CHECK([1.10])

AC_CONFIG_FILES([
        Makefile
        libopengdesktop/Makefile
        doc/Makefile
        doc/reference/Makefile
        doc/reference/version.xml
        libopengdesktop.pc
])

AC_OUTPUT

echo ""
echo "        libopengdesktop: $VERSION"
echo ""
echo "                 Prefix: ${prefix}"
echo "         Compiler flags: ${CPPFLAGS}"
echo ""

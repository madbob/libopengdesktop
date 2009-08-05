NULL =

INCLUDES = \
	-I$(top_srcdir)/libopengdesktop	\
	-DPREFIX=\""$(prefix)"\"	\
	-DLIBDIR=\""$(libdir)"\"	\
	-DG_DISABLE_DEPRECATED		\
	$(LIBOGD_CFLAGS)		\
	$(NULL)

LDADD = $(LIBOGD_LT_LDFLAGS) -export-dynamic -rpath $(libdir)

sources_private_h = \
   ogd-private-utils.h  \
   $(NULL)

sources_public_h = \
    ogd-activity.h    \
    ogd-category.h    \
    ogd-content.h     \
    ogd-event.h       \
    ogd-errors.h      \
    ogd-folder.h      \
    ogd.h             \
    ogd-iterator.h    \
    ogd-message.h     \
    ogd-object.h      \
    ogd-person.h      \
    ogd-provider.h    \
    $(NULL)

sources_c = \
    ogd-activity.c      \
    ogd-category.c      \
    ogd-content.c       \
    ogd-event.c         \
    ogd-folder.c        \
    ogd-iterator.c      \
    ogd-message.c       \
    ogd-object.c        \
    ogd-person.c        \
    ogd-private-utils.c \
    ogd-provider.c      \
    $(NULL)

lib_LTLIBRARIES = libopengdesktop-1.0.la

libopengdesktop_1_0_la_LIBADD = $(LIBOGD_LIBS)
libopengdesktop_1_0_la_SOURCES = \
	$(sources_public_h) \
	$(sources_private_h) \
	$(sources_c) \
	$(NULL)
libopengdesktop_1_0_la_LDFLAGS = $(LDADD)

libopengdesktopincludedir = $(includedir)/libopengdesktop
libopengdesktopinclude_DATA = $(sources_public_h)

CLEANFILES = $(pcfile_DATA)

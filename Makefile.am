ACLOCAL_AMFLAGS = -I m4
SUBDIRS = libopengdesktop doc
EXTRA_DIST = autogen.sh

pcfiledir = $(libdir)/pkgconfig
pcfile_DATA = libopengdesktop-0.pc

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

libopengdesktop-0.pc: libopengdesktop.pc
	@cp -f $< $@

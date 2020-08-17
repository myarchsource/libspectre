EXTRA_DIST=spectre-version.h.in

lib_LTLIBRARIES = libspectre.la

libspectreincludedir = $(includedir)/libspectre
libspectreinclude_HEADERS =		\
	spectre.h			\
	spectre-macros.h		\
	spectre-status.h		\
	spectre-document.h		\
	spectre-render-context.h	\
	spectre-page.h			\
	spectre-exporter.h		\
	spectre-version.h

gv_sources = 				\
	ps.h				\
	ps.c

libspectre_la_SOURCES = 		\
	spectre.h			\
	spectre-private.h		\
	spectre-macros.h		\
	spectre-status.h		\
	spectre-status.c		\
	spectre-document.h		\
	spectre-document.c		\
	spectre-gs.h			\
	spectre-gs.c			\
	spectre-render-context.h	\
	spectre-render-context.c	\
	spectre-device.h		\
	spectre-device.c		\
	spectre-page.h			\
	spectre-page.c			\
	spectre-exporter.h		\
	spectre-exporter.c		\
	spectre-exporter-pdf.c		\
	spectre-exporter-ps.c		\
	spectre-utils.h			\
	spectre-utils.c			\
	$(gv_sources)

libspectre_la_CPPFLAGS = 		\
	$(SPECTRE_CFLAGS)

libspectre_la_CFLAGS = 			\
	$(VISIBILITY_CFLAGS)

libspectre_la_LIBADD = $(LIB_GS)
libspectre_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

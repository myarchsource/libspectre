noinst_PROGRAMS = spectre-test parser-test fuzz-test

spectre_test_SOURCES = \
	spectre-test.c \
	$(top_srcdir)/libspectre/spectre-utils.c \
	$(top_srcdir)/libspectre/spectre-utils.h

spectre_test_CPPFLAGS =			\
	-I$(top_srcdir)/libspectre	\
	$(SPECTRE_CFLAGS)		\
	$(CAIRO_CFLAGS)

spectre_test_LDADD =                         		\
	$(top_builddir)/libspectre/libspectre.la	\
	$(CAIRO_LIBS)

parser_test_SOURCES = \
	parser-test.c \
	$(top_srcdir)/libspectre/ps.c \
	$(top_srcdir)/libspectre/ps.h \
	$(top_srcdir)/libspectre/spectre-utils.c \
	$(top_srcdir)/libspectre/spectre-utils.h

parser_test_CPPFLAGS = 			\
	-I$(top_srcdir)/libspectre	\
	$(SPECTRE_CFLAGS)

parser_test_LDADD = $(top_builddir)/libspectre/libspectre.la

fuzz_test_SOURCES = \
	fuzz_main.c \
	spectre_read_fuzzer.c \
	$(top_srcdir)/libspectre/ps.c \
	$(top_srcdir)/libspectre/ps.h \
	$(top_srcdir)/libspectre/spectre-utils.c \
	$(top_srcdir)/libspectre/spectre-utils.h
	
fuzz_test_CPPFLAGS = \
	-I$(top_srcdir)/libspectre	\
	$(SPECTRE_CFLAGS)
	
fuzz_test_LDADD = $(top_builddir)/libspectre/libspectre.la

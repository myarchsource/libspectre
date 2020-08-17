AUTOMAKE_OPTIONS = foreign

SUBDIRS = libspectre

if ENABLE_TEST
SUBDIRS += test
endif

EXTRA_DIST = 			\
	AUTHORS			\
	Doxyfile.in		\
	INSTALL			\
	README			\
	libspectre.pc.in	

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libspectre.pc

DISTCLEANFILES = 		\
	$(pkgconfig_DATA)	\
	Doxyfile		\
	ChangeLog

dist-hook: changelog

changelog:
	@if test -d "$(srcdir)/.git"; then \
	  git log --stat > ChangeLog; \
	  cp ChangeLog $(distdir)/; \
	fi

doc: Doxyfile
	doxygen Doxyfile

# Releasing stuff based on Cairo
# Some custom targets to make it easier to release things.
# To make real stable releases use:
#             make release-publish

RELEASE_UPLOAD_HOST =   libspectre.freedesktop.org
RELEASE_UPLOAD_BASE =   /srv/$(RELEASE_UPLOAD_HOST)/www
RELEASE_UPLOAD_DIR =    $(RELEASE_UPLOAD_BASE)/releases
RELEASE_URL_BASE =      http://$(RELEASE_UPLOAD_HOST)/releases

MANUAL_DATED =          libspectre-manual-`date +%Y%m%d`
MANUAL_TAR_FILE =       $(MANUAL_DATED).tar.gz
MANUAL_UPLOAD_DIR =     $(RELEASE_UPLOAD_BASE)

tar_file = $(PACKAGE)-$(VERSION).tar.gz
sha1_file = $(tar_file).sha1
gpg_file = $(sha1_file).asc

$(sha1_file): $(tar_file)
	sha1sum $^ > $@

$(gpg_file): $(sha1_file)
	@echo "Please enter your GPG password to sign the checksum."
	gpg --armor --sign $^

release-verify-soname-major:
	@echo -n "Checking that the libspectre soname major number is 1..."
	@test "$(LT_CURRENT_MINUS_AGE)" = "1" \
		|| (echo "Ouch." && echo "The soname major number is $(LT_CURRENT_MINUS_AGE) instead of 1." \
		&& echo "The libtool shared library version numbers in configure.ac must be incremented properly." \
		&& false)
	@echo "Good."

release-verify-newer:
	@echo -n "Checking that no $(VERSION) release already exists..."
	@ssh $(RELEASE_UPLOAD_HOST) test ! -e $(RELEASE_UPLOAD_DIR)/$(tar_file) \
		|| (echo "Ouch." && echo "Found: $(RELEASE_UPLOAD_HOST):$(RELEASE_UPLOAD_DIR)/$(tar_file)" \
		&& echo "Are you sure you have an updated checkout?" \
		&& echo "This should never happen." \
		&& false)
	@echo "Good."

release-remove-old:
	$(RM) $(tar_file) $(sha1_file) $(gpg_file)

release-cleanup-group-sticky:
	find . -type f | xargs chmod g-s

release-check: release-verify-soname-major release-verify-newer release-remove-old release-cleanup-group-sticky distcheck

release-upload: release-check $(tar_file) $(sha1_file) $(gpg_file)
	mkdir -p releases
	scp $(tar_file) $(sha1_file) $(gpg_file) $(RELEASE_UPLOAD_HOST):$(RELEASE_UPLOAD_DIR)
	mv $(tar_file) $(sha1_file) $(gpg_file) releases
	ssh $(RELEASE_UPLOAD_HOST) "rm -f $(RELEASE_UPLOAD_DIR)/LATEST-$(PACKAGE)-[0-9]* && ln -s $(tar_file) $(RELEASE_UPLOAD_DIR)/LATEST-$(PACKAGE)-$(VERSION)"
	 git tag -s -m "libspectre $(SPECTRE_MAJOR_VERSION).$(SPECTRE_MINOR_VERSION).$(SPECTRE_MICRO_VERSION) release" $(SPECTRE_MAJOR_VERSION).$(SPECTRE_MINOR_VERSION).$(SPECTRE_MICRO_VERSION)

release-publish-message: releases/$(sha1_file)
	@echo "Please follow the instructions in RELEASING to push stuff out and"
	@echo "send out the announcement mails.  Here is the excerpt you need:"
	@echo ""
	@echo "Subject: $(PACKAGE) release $(VERSION) now available"
	@echo ""
	@echo "============================== CUT HERE =============================="
	@echo "A new $(PACKAGE) release $(VERSION) is now available from:"
	@echo ""
	@echo " $(RELEASE_URL_BASE)/$(tar_file)"
	@echo ""
	@echo "    which can be verified with:"
	@echo ""
	@echo " $(RELEASE_URL_BASE)/$(sha1_file)"
	@echo -n "      "
	@cat releases/$(sha1_file)
	@echo ""
	@echo " $(RELEASE_URL_BASE)/$(gpg_file)"
	@echo " (signed by `getent passwd "$$USER" | cut -d: -f 5 | cut -d, -f 1`)"
	@echo ""
	@echo "  Additionally, a git clone of the source tree:"
	@echo ""
	@echo " git clone git://anongit.freedesktop.org/git/libspectre"
	@echo ""
	@echo "    will include a signed $(VERSION) tag which points to a commit named:"
	@echo " `git cat-file tag $(VERSION) | grep ^object | sed -e 's,object ,,'`"
	@echo ""
	@echo "    which can be verified with:"
	@echo " git verify-tag $(VERSION)"
	@echo ""
	@echo "    and can be checked out with a command such as:"
	@echo " git checkout -b build $(VERSION)"
	@echo ""
	@echo "============================== CUT HERE =============================="

release-publish: release-upload release-publish-message

doc-publish: doc
	cp -a doc/html $(MANUAL_DATED)
	tar czf $(MANUAL_TAR_FILE) $(MANUAL_DATED)
	scp $(MANUAL_TAR_FILE) $(RELEASE_UPLOAD_HOST):$(MANUAL_UPLOAD_DIR)
	ssh $(RELEASE_UPLOAD_HOST) "cd $(MANUAL_UPLOAD_DIR) && tar xzf $(MANUAL_TAR_FILE) && rm -f manual && ln -s $(MANUAL_DATED) manual && ln -sf $(MANUAL_TAR_FILE) libspectre-manual.tar.gz"

.PHONY: release-verify-newer release-remove-old release-cleanup-group-sticky release-check release-upload release-publish doc-publish changelog

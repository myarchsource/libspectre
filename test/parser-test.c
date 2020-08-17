#include <stdio.h>
#include <stdlib.h>

#include "../libspectre/spectre-utils.h"
#include "../libspectre/ps.h"

static char *outputdir;

#define BUFFER_SIZE 32768

static void
write_section (FILE *fd, const char *section, long begin, long end)
{
	FILE *dst;
	char *filename;
	static char buf[BUFFER_SIZE];
	unsigned int read;
	size_t left = end - begin;

	filename = _spectre_strdup_printf ("%s/%s.txt", outputdir, section);
	dst = fopen (filename, "w");
	if (!dst) {
		printf ("Skipping section %s, could not open dest file %s\n",
			section, filename);
		free (filename);
		return;
	}
	free (filename);

	fseek (fd, begin, SEEK_SET);

	while (left > 0) {
		size_t to_read = BUFFER_SIZE;

		if (left < to_read)
			to_read = left;

		read = fread (buf, sizeof (char), to_read, fd);
		fwrite (buf, sizeof (char), read, dst);

		left -= read;
	}

	fclose (dst);
}

int main (int argc, char **argv)
{
	FILE            *fd;
	struct document *doc;
	unsigned int     i;

	outputdir = argv[2];
	
	fd = fopen (argv[1], "r");
	if (!fd) {
		printf ("Error opening file %s\n", argv[1]);
		return 1;
	}
	
	doc = psscan (fd, argv[1], SCANSTYLE_NORMAL);
	if (!doc) {
		printf ("Error parsing document\n");
		fclose (fd);

		return 1;
	}

	write_section (fd, "prolog", doc->beginprolog, doc->endprolog);
	write_section (fd, "setup", doc->beginsetup, doc->endsetup);
	for (i = 0; i < doc->numpages; i++) {
		char *pagename;

		pagename = _spectre_strdup_printf ("page-%d", i);
		write_section (fd, pagename, doc->pages[i].begin, doc->pages[i].end);
		free (pagename);
	}
	write_section (fd, "trailer", doc->begintrailer, doc->endtrailer);

	fclose (fd);
	psdocdestroy (doc);
	
	return 0;
}

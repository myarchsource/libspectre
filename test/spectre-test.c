#include <stdio.h>
#include <stdlib.h>

#include <cairo.h>

#include <libspectre/spectre.h>

#include "../libspectre/spectre-utils.h"

static const char *
orientation_to_string (SpectreOrientation orientation)
{
	switch (orientation) {
	case SPECTRE_ORIENTATION_PORTRAIT:
		return "Portrait";
	case SPECTRE_ORIENTATION_LANDSCAPE:
		return "Landscape";
	case SPECTRE_ORIENTATION_REVERSE_LANDSCAPE:
		return "Reverse Landscape";
	case SPECTRE_ORIENTATION_REVERSE_PORTRAIT:
		return "Reverse Portrait";
	}

	return "Unknown Orientation";
}

static void
test_metadata (SpectreDocument *document)
{
	const char *format;

	format = spectre_document_get_format (document);
	printf ("Document format: %s\n",
		format ? format : "Unknown");
	printf ("Postscript language level: %d\n",
		spectre_document_get_language_level (document));
	printf ("Encapsulated PostScript: %s\n",
		spectre_document_is_eps (document) ? "Yes" : "No");
	printf ("Number of pages: %d\n",
		spectre_document_get_n_pages (document));
	printf ("Title: %s\n",
		spectre_document_get_title (document));
	printf ("Creator: %s\n",
		spectre_document_get_creator (document));
	printf ("For: %s\n",
		spectre_document_get_for (document));
	printf ("Creation date: %s\n",
		spectre_document_get_creation_date (document));
	printf ("Document Orientation: %s\n",
		orientation_to_string (spectre_document_get_orientation (document)));
}

static void
test_rotation (SpectreDocument *document,
	       const char      *output)
{
	SpectrePage          *page;
	SpectreRenderContext *rc;
	int                   i;
	int                   width_points;
	int                   height_points;

	page = spectre_document_get_page (document, 0);

	spectre_page_get_size (page, &width_points, &height_points);

	rc = spectre_render_context_new ();
	
	for (i = 0; i < 4; i++) {
		cairo_surface_t *surface;
		char            *filename;
		int              rotation;
		int              width, height;
		unsigned char   *data = NULL;
		int              row_length;

		rotation = 90 * i;

		if (rotation == 90 || rotation == 270) {
			width = height_points;
			height = width_points;
		} else {
			width = width_points;
			height = height_points;
		}

		spectre_render_context_set_rotation (rc, rotation);
		spectre_page_render (page, rc, &data, &row_length);
		if (spectre_page_status (page)) {
			printf ("Error rendering page %d at rotation %d: %s\n", i, rotation, 
				spectre_status_to_string (spectre_page_status (page)));
			free (data);
			continue;
		}

		surface = cairo_image_surface_create_for_data (data,
							       CAIRO_FORMAT_RGB24,
							       width, height,
							       row_length);

		filename = _spectre_strdup_printf ("%s/page-rotated-%d.png", output, rotation);
		cairo_surface_write_to_png (surface, filename);
		free (filename);

		cairo_surface_destroy (surface);
		free (data);
	}

	spectre_render_context_free (rc);
	spectre_page_free (page);
}

static void
test_render_slice (SpectreDocument *document,
		   const char      *output)
{
	SpectrePage          *page;
	SpectreRenderContext *rc;
	int                   x, y;
	int                   width, height;
	int                   width_points;
	int                   height_points;
	unsigned char        *data = NULL;
	int                   row_length;

	page = spectre_document_get_page (document, 0);
	
	spectre_page_get_size (page, &width_points, &height_points);

	/* Render the central slice */
	width = width_points / 3;
	height = height_points / 3;
	x = width;
	y = height;

	printf ("Rendering page 0 slice %d, %d, %d, %d\n",
		x, y, width, height);
	
	rc = spectre_render_context_new ();

	spectre_page_render_slice (page, rc, x, y, width, height,
				   &data, &row_length);
	if (!spectre_page_status (page)) {
		cairo_surface_t *surface;
		char            *filename;

		surface = cairo_image_surface_create_for_data (data,
							       CAIRO_FORMAT_RGB24,
							       width, height,
							       row_length);

		filename = _spectre_strdup_printf ("%s/page-slice.png", output);
		cairo_surface_write_to_png (surface, filename);
		free (filename);

		cairo_surface_destroy (surface);
	} else {
		printf ("Error rendering page slice %d, %d, %d, %d: %s\n",
			x, y, width, height,
			spectre_status_to_string (spectre_page_status (page)));
	}

	free (data);
	spectre_render_context_free (rc);
	spectre_page_free (page);
}

static void
test_page_size (SpectreDocument *document,
		const char      *output)
{
	SpectrePage          *page;
	SpectreRenderContext *rc;
	unsigned char        *data = NULL;
	int                   row_length;

	page = spectre_document_get_page (document, 0);
	
	printf ("Rendering page 0 in a4 page\n");
	
	rc = spectre_render_context_new ();

	spectre_render_context_set_page_size (rc, 595, 842);
	spectre_page_render (page, rc, &data, &row_length);
	if (!spectre_page_status (page)) {
		cairo_surface_t *surface;
		char            *filename;

		surface = cairo_image_surface_create_for_data (data,
							       CAIRO_FORMAT_RGB24,
							       595, 842,
							       row_length);

		filename = _spectre_strdup_printf ("%s/page-a4.png", output);
		cairo_surface_write_to_png (surface, filename);
		free (filename);

		cairo_surface_destroy (surface);
	} else {
		printf ("Error rendering page in a4 page: %s\n",
			spectre_status_to_string (spectre_page_status (page)));
	}

	free (data);
	spectre_render_context_free (rc);
	spectre_page_free (page);
}

static void
test_document_render (SpectreDocument *document,
		      const char      *output)
{
	unsigned char *data = NULL;
	int            row_length;

	printf ("Rendering document\n");

	spectre_document_render (document, &data, &row_length);
	if (!spectre_document_status (document)) {
		cairo_surface_t *surface;
		char            *filename;
		int              width, height;

		spectre_document_get_page_size (document, &width, &height);

		surface = cairo_image_surface_create_for_data (data,
							       CAIRO_FORMAT_RGB24,
							       width, height,
							       row_length);

		filename = _spectre_strdup_printf ("%s/document.png", output);
		cairo_surface_write_to_png (surface, filename);
		free (filename);

		cairo_surface_destroy (surface);
	} else {
		printf ("Error rendering document: %s\n",
			spectre_status_to_string (spectre_document_status (document)));
	}

	free (data);
}

static void
test_document_render_full (SpectreDocument *document,
			   const char      *output)
{
	unsigned char        *data = NULL;
	int                   row_length;
	SpectreRenderContext *rc;

	printf ("Rendering document at 2x\n");

	rc = spectre_render_context_new ();
	
	spectre_render_context_set_scale (rc, 2.0, 2.0);
	spectre_document_render_full (document, rc, &data, &row_length);
	if (!spectre_document_status (document)) {
		cairo_surface_t *surface;
		char            *filename;
		int              width, height;

		spectre_document_get_page_size (document, &width, &height);

		width = (int) ((width * 2.0) + 0.5);
		height = (int) ((height * 2.0) + 0.5);

		surface = cairo_image_surface_create_for_data (data,
							       CAIRO_FORMAT_RGB24,
							       width, height,
							       row_length);

		filename = _spectre_strdup_printf ("%s/document-2x.png", output);
		cairo_surface_write_to_png (surface, filename);
		free (filename);

		cairo_surface_destroy (surface);
	} else {
		printf ("Error rendering document at 2x: %s\n",
			spectre_status_to_string (spectre_document_status (document)));
	}

	spectre_render_context_free (rc);
	free (data);
}

static void
test_export (SpectreDocument      *document,
	     SpectreExporterFormat format,
	     const char           *output_dir)
{
	SpectreExporter *exporter;
	SpectreStatus status;
	char *filename;
	const char *format_str = format == SPECTRE_EXPORTER_FORMAT_PS ? "ps" : "pdf";
	int i;

	exporter = spectre_exporter_new (document, format);

	filename = _spectre_strdup_printf ("%s/output.%s", output_dir, format_str);
	status = spectre_exporter_begin (exporter, filename);
	free (filename);

	if (status) {
		printf ("Error exporting document as %s: %s\n", format_str,
			spectre_status_to_string (status));
		spectre_exporter_free (exporter);
		
		return;
	}

	/* Export even pages in reverse order */
	for (i = spectre_document_get_n_pages (document) - 1; i >= 0; i--) {
		if (i % 2 != 0)
			continue;
		
		status = spectre_exporter_do_page (exporter, i);
		if (status) {
			printf ("Error exporting page %d as %s: %s\n", i, format_str,
				spectre_status_to_string (status));
			break;
		}
	}
			
	spectre_exporter_end (exporter);
	spectre_exporter_free (exporter);
}

static void
test_save (SpectreDocument *document,
	   const char      *output_dir)
{
	char *filename;

	filename = _spectre_strdup_printf ("%s/document-copy.ps", output_dir);
	spectre_document_save (document, filename);
	if (spectre_document_status (document)) {
		printf ("Error saving document as %s: %s\n", filename,
			spectre_status_to_string (spectre_document_status (document)));
	}
	free (filename);
}

static void
test_save_to_pdf (SpectreDocument *document,
		  const char      *output_dir)
{
	char *filename;

	filename = _spectre_strdup_printf ("%s/document-copy.pdf", output_dir);
	spectre_document_save_to_pdf (document, filename);
	if (spectre_document_status (document)) {
		printf ("Error saving document as %s: %s\n", filename,
			spectre_status_to_string (spectre_document_status (document)));
	}
	free (filename);
}

int main (int argc, char **argv)
{
	SpectreDocument      *document;
	SpectreRenderContext *rc;
	unsigned int          i;

	/* TODO: check argv */

	printf ("Testing libspectre version: %s\n", SPECTRE_VERSION_STRING);

	spectre_document_load (NULL, argv[1]);

	document = spectre_document_new ();
	spectre_document_load (document, argv[1]);
	if (spectre_document_status (document)) {
		printf ("Error loading document %s: %s\n", argv[1],
			spectre_status_to_string (spectre_document_status (document)));
		spectre_document_free (document);

		return 1;
	}

	test_document_render (document, argv[2]);
	test_document_render_full (document, argv[2]);
	test_export (document, SPECTRE_EXPORTER_FORMAT_PDF, argv[2]);
	test_export (document, SPECTRE_EXPORTER_FORMAT_PS, argv[2]);
	test_save (document, argv[2]);
	test_save_to_pdf (document, argv[2]);
	test_metadata (document);

	rc = spectre_render_context_new ();

	for (i = 0; i < spectre_document_get_n_pages (document); i++) {
		SpectrePage     *page, *page2;
		cairo_surface_t *surface;
		char            *filename;
		int              width, height;
		unsigned char   *data = NULL;
		int              row_length;

		page = spectre_document_get_page (document, i);
		if (spectre_document_status (document)) {
			printf ("Error getting page %d: %s\n", i, 
				spectre_status_to_string (spectre_document_status (document)));
			continue;
		}

		spectre_page_get_size (page, &width, &height);
		printf ("Page %d\n", i);
		printf ("\tPage label: %s\n", spectre_page_get_label (page));
		printf ("\tPage size: %d x %d\n", width, height);
		printf ("\tPage orientation: %s\n", 
			orientation_to_string (spectre_page_get_orientation (page)));

		page2 = spectre_document_get_page_by_label (document, spectre_page_get_label (page));
		if (!page2 || spectre_document_status (document)) {
			printf ("Error getting page %d by its label %s: %s\n", i,
				spectre_page_get_label (page),
				spectre_status_to_string (spectre_document_status (document)));
		}
		spectre_page_free (page2);
		

		spectre_render_context_set_page_size (rc, width, height);
		spectre_page_render (page, rc, &data, &row_length);
		if (spectre_page_status (page)) {
			printf ("Error rendering page %d: %s\n", i,
				spectre_status_to_string (spectre_page_status (page)));
			free (data);
			spectre_page_free (page);
			continue;
		}

		surface = cairo_image_surface_create_for_data (data,
							       CAIRO_FORMAT_RGB24,
							       width, height,
							       row_length);

		filename = _spectre_strdup_printf ("%s/page-%d.png", argv[2], i);
		cairo_surface_write_to_png (surface, filename);
		free (filename);

		cairo_surface_destroy (surface);
		free (data);
		spectre_page_free (page);
	}

	spectre_render_context_free (rc);

	test_render_slice (document, argv[2]);
	test_page_size (document, argv[2]);
	test_rotation (document, argv[2]);
	
	spectre_document_free (document);
	
	return 0;
}

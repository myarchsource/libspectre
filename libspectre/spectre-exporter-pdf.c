/* This file is part of Libspectre.
 *
 * Copyright (C) 2007 Albert Astals Cid <aacid@kde.org>
 * Copyright (C) 2007 Carlos Garcia Campos <carlosgc@gnome.org>
 *
 * Libspectre is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * Libspectre is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdlib.h>

#include "spectre-private.h"
#include "spectre-utils.h"

static SpectreStatus
spectre_exporter_pdf_begin (SpectreExporter *exporter,
			    const char      *filename)
{
	char *args[10];
	int arg = 0;
	char *output_file;
	struct document *doc = exporter->doc;

	exporter->gs = spectre_gs_new ();
	if (!spectre_gs_create_instance (exporter->gs, NULL)) {
		spectre_gs_cleanup (exporter->gs, CLEANUP_DELETE_INSTANCE);
		spectre_gs_free (exporter->gs);
		exporter->gs = NULL;

		return SPECTRE_STATUS_EXPORTER_ERROR;
	}

	args[arg++] = "libspectre"; /* This value doesn't really matter */
	args[arg++] = "-dMaxBitmap=10000000";
	args[arg++] = "-dBATCH";
	args[arg++] = "-dNOPAUSE";
	args[arg++] = "-dSAFER";
	args[arg++] = "-P-";
	args[arg++] = "-sDEVICE=pdfwrite";
	args[arg++] = output_file = _spectre_strdup_printf ("-sOutputFile=%s",
							    filename);
	args[arg++] = "-c";
	args[arg++] = ".setpdfwrite";

	if (!spectre_gs_run (exporter->gs, 10, args)) {
		free (output_file);
		spectre_gs_free (exporter->gs);
		exporter->gs = NULL;

		return SPECTRE_STATUS_EXPORTER_ERROR;
	}

	free (output_file);

	if (!spectre_gs_process (exporter->gs,
				 doc->filename,
				 0, 0,
				 doc->beginprolog,
				 doc->endprolog)) {
		spectre_gs_free (exporter->gs);
		exporter->gs = NULL;

		return SPECTRE_STATUS_EXPORTER_ERROR;
	}

	if (!spectre_gs_process (exporter->gs,
				 doc->filename,
				 0, 0,
				 doc->beginsetup,
				 doc->endsetup)) {
		spectre_gs_free (exporter->gs);
		exporter->gs = NULL;

		return SPECTRE_STATUS_EXPORTER_ERROR;
	}

	return SPECTRE_STATUS_SUCCESS;
}

static SpectreStatus
spectre_exporter_pdf_do_page (SpectreExporter *exporter,
			      unsigned int     page_index)
{
	struct document *doc = exporter->doc;

	if (!exporter->gs)
		return SPECTRE_STATUS_EXPORTER_ERROR;

	if (!spectre_gs_process (exporter->gs,
				 doc->filename,
				 0, 0,
				 doc->pages[page_index].begin,
				 doc->pages[page_index].end)) {
		spectre_gs_free (exporter->gs);
		exporter->gs = NULL;

		return SPECTRE_STATUS_EXPORTER_ERROR;
	}
	
	return SPECTRE_STATUS_SUCCESS;
}

static SpectreStatus
spectre_exporter_pdf_end (SpectreExporter *exporter)
{
	int ret;
	struct document *doc = exporter->doc;

	if (!exporter->gs)
		return SPECTRE_STATUS_EXPORTER_ERROR;

	ret = spectre_gs_process (exporter->gs,
				  doc->filename,
				  0, 0,
				  doc->begintrailer,
				  doc->endtrailer);
	spectre_gs_free (exporter->gs);
	exporter->gs = NULL;

	return ret ? SPECTRE_STATUS_SUCCESS : SPECTRE_STATUS_EXPORTER_ERROR;
}

SpectreExporter *
_spectre_exporter_pdf_new (struct document *doc)
{
	SpectreExporter *exporter;

	exporter = calloc (1, sizeof (SpectreExporter));
	if (!exporter)
		return NULL;

	exporter->doc = psdocreference (doc);

	exporter->begin = spectre_exporter_pdf_begin;
	exporter->do_page = spectre_exporter_pdf_do_page;
	exporter->end = spectre_exporter_pdf_end;

	return exporter;
}

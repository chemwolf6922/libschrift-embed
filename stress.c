/* A stress testing program. Useful for profiling hot spots in libschrift. */
/* See LICENSE file for copyright and license details. */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schrift.h"

#define ABORT(msg) do { fprintf(stderr, "%s\n", msg); exit(1); } while (0)

int
main(int argc, char *argv[])
{
	const char *font_file = "resources/FiraGO-Regular_extended_with_NotoSansEgyptianHieroglyphs-Regular.ttf";
	double size = 20.0;

	int opt = 0;
	while((opt = getopt(argc, argv, "f:s:")) != -1) {
		switch(opt) {
			case 'f':
				font_file = optarg;
				break;
			case 's':
				size = atof(optarg);
				break;
			default:
				fprintf(stderr, "usage: %s [-f font file] [-s size in px]\n", argv[0]);
				exit(1);
		}
	}

	SFT_Font *font;
	if (!(font = sft_loadfile(font_file)))
		ABORT("Can't load font file.");
	
	SFT sft;
	memset(&sft, 0, sizeof sft);
	sft.font = font;
	sft.xScale = size;
	sft.yScale = size;
	sft.flags = SFT_DOWNWARD_Y;

	for (int i = 0; i < 5000; ++i) {
		for (SFT_UChar cp = 32; cp < 128; ++cp) {
			SFT_Glyph gid;
			if (sft_lookup(&sft, cp, &gid) < 0)
				continue;
			SFT_GMetrics mtx;
			if (sft_gmetrics(&sft, gid, &mtx) < 0)
				continue;
			SFT_Image    image;
			image.width  = mtx.minWidth;
			image.height = mtx.minHeight;
			image.pixels = malloc((size_t) image.width * (size_t) image.height);
			sft_render(&sft, gid, image);
			free(image.pixels);
		}
	}
	sft_freefont(font);
	return 0;
}


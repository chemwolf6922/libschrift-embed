/*
 * A simple command line application that shows how to use libschrift.
 * See LICENSE file for copyright and license details.
 * Contributed by Andor Badi.
 */

/**
 * This demo still requires a posix system to run.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/mman.h>

#include "../schrift.h"
#include "util.h"

static int utf8_to_utf32(const uint8_t *utf8, uint32_t *utf32, int max)
{
	unsigned int c;
	int i = 0;
	--max;
	while (*utf8) {
		if (i >= max)
			return 0;
		if (!(*utf8 & 0x80U)) {
			utf32[i++] = *utf8++;
		} else if ((*utf8 & 0xe0U) == 0xc0U) {
			c = (*utf8++ & 0x1fU) << 6;
			if ((*utf8 & 0xc0U) != 0x80U) return 0;
			utf32[i++] = c + (*utf8++ & 0x3fU);
		} else if ((*utf8 & 0xf0U) == 0xe0U) {
			c = (*utf8++ & 0x0fU) << 12;
			if ((*utf8 & 0xc0U) != 0x80U) return 0;
			c += (*utf8++ & 0x3fU) << 6;
			if ((*utf8 & 0xc0U) != 0x80U) return 0;
			utf32[i++] = c + (*utf8++ & 0x3fU);
		} else if ((*utf8 & 0xf8U) == 0xf0U) {
			c = (*utf8++ & 0x07U) << 18;
			if ((*utf8 & 0xc0U) != 0x80U) return 0;
			c += (*utf8++ & 0x3fU) << 12;
			if ((*utf8 & 0xc0U) != 0x80U) return 0;
			c += (*utf8++ & 0x3fU) << 6;
			if ((*utf8 & 0xc0U) != 0x80U) return 0;
			c += (*utf8++ & 0x3fU);
			if ((c & 0xFFFFF800U) == 0xD800U) return 0;
            utf32[i++] = c;
		} else return 0;
	}
	utf32[i] = 0;
	return i;
}

/**
 * @brief Copy glyph to frame buffer. Blank pixels are ignored.
 * 
 * @param fb 
 * @param img 
 * @param x 
 * @param y 
 * @return int 
 */
static void overlay_glyph_to_frame_buffer(SFT_Image* fb, const SFT_Image* glyph, int offset_x, int offset_y)
{
	/** Overlay all pixels inside fb to fb. Ignore out of bound pixels */
	for (int i = 0; i < glyph->height; i ++)
	{
		if (offset_y + i < 0 || offset_y + i >= fb->height)
			continue;
		for (int j = 0; j < glyph->width; j ++)
		{
			if (offset_x + j < 0 || offset_x + j >= fb->width)
				continue;
			fb->pixels[(offset_y + i) * fb->width + (offset_x + j)] = glyph->pixels[i * glyph->width + j];
		}
	}
}

/**
 * @brief 
 * 
 * @param fb 
 * @param sft 
 * @param cp 
 * @param px 
 * @param y 
 * @return int 
 */
static int add_glyph(SFT_Image* fb, const SFT *sft, SFT_UChar cp, int* px, int y, bool do_kerning)
{
#define ABORT(cp, m) do { fprintf(stderr, "codepoint 0x%04"PRIx32" %s\n", cp, m); return -1; } while (0)

	/** For demo only. DO NOT do this in production. */
	static SFT_Glyph prev_gid = 0;
	static bool prev_gid_valid = false;

	/** Render glyph */
	SFT_Glyph gid;
	if (sft_lookup(sft, cp, &gid) < 0)
		ABORT(cp, "missing");

	SFT_GMetrics mtx;
	if (sft_gmetrics(sft, gid, &mtx) < 0)
		ABORT(cp, "bad glyph metrics");

	SFT_Image img = {
		.width  = (mtx.minWidth + 3) & ~3,
		.height = mtx.minHeight,
	};
	uint8_t pixels[img.width * img.height];
	img.pixels = pixels;
	if (sft_render(sft, gid, img) < 0)
		ABORT(cp, "not rendered");

	/** Do kerning if asked and able to */
	SFT_Kerning kerning = {
		.xShift = 0,
		.yShift = 0,
	};
	if (do_kerning && prev_gid_valid)
	{
		if (sft_kerning(sft, prev_gid, gid, &kerning) < 0)
			ABORT(cp, "kerning failed");	
	}

	/** Overlay to frame buffer. */
	overlay_glyph_to_frame_buffer(fb, &img, *px + (int)(mtx.leftSideBearing + kerning.xShift), y + (int)(mtx.yOffset + kerning.yShift));

	/** Update *px. y should be managed by the caller */
	*px += (int)(mtx.advanceWidth + kerning.xShift);

	/** Update previous gid */
	prev_gid = gid;
	prev_gid_valid = true;

	return 0;

#undef ABORT
}

#define FB_WIDTH 1400
#define FB_HEIGHT 700

int main()
{
#define END(m) do { fprintf(stderr, "%s\n", m); return 1; } while (0)

	uint8_t fb_pixels[FB_WIDTH * FB_HEIGHT];
	SFT_Image fb = {
		.width  = FB_WIDTH,
		.height = FB_HEIGHT,
		.pixels = fb_pixels
	};
	memset(fb.pixels, 0, FB_HEIGHT * FB_WIDTH);

	size_t font_data_size = 0;
	void* font_data = map_file("./FiraGO-Regular_extended_with_NotoSansEgyptianHieroglyphs-Regular.ttf", &font_data_size);
	if (font_data == NULL)
		END("Cannot map font file");

	SFT sft = {
		.xScale = 32,
		.yScale = 32,
		.flags  = SFT_DOWNWARD_Y,
	};
	sft.font = sft_loadmem(font_data, font_data_size);
	if (sft.font == NULL)
		END("TTF load failed");

	FILE *text_file = fopen("./test_text.txt", "r");
	if (text_file == NULL)
		END("Cannot open input text");

	SFT_LMetrics lmtx;
	sft_lmetrics(&sft, &lmtx);

	int y = 20 + (int)(lmtx.ascender + lmtx.lineGap);
	char line[256] = {0};
	while (fgets(line, sizeof(line), text_file)) {
		if(line[strlen(line) - 1] == '\n')
		{
			line[strlen(line) - 1] = '\0';
		}

		SFT_UChar codepoints[sizeof(line)] = {0};
		int n = utf8_to_utf32((unsigned char *) line, codepoints, sizeof(codepoints)/sizeof(codepoints[0]));

		int x = 20;
		for (int i = 0; i < n; i++) {
			add_glyph(&fb, &sft, codepoints[i], &x, y, i!=0);
		}

		y += 2 * (int)(lmtx.ascender + lmtx.descender + lmtx.lineGap);
	}

	fclose(text_file);

	sft_freefont(sft.font);
	unmap_file(font_data, font_data_size);

	/** Inverse fb */
	for (int i = 0; i < fb.height * fb.width; i++) {
		fb.pixels[i] = 255 - fb.pixels[i];
	}

	/** Print fb as text ppm to stdout */
	FILE* output = fopen("demo.ppm", "w");
	fprintf(output, "P3\n%d %d\n255\n", fb.width, fb.height);
	for (int i = 0; i < fb.height; i++) {
		for (int j = 0; j < fb.width; j++) {
			fprintf(output, "%d %d %d ", fb.pixels[i * fb.width + j], fb.pixels[i * fb.width + j], fb.pixels[i * fb.width + j]);
		}
		fprintf(output, "\n");
	}

	return 0;

#undef END
}


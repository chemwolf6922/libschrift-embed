#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../schrift.h"
#include "util.h"

int main(int argc, char* const *argv)
{
    /** Parse options */
    char* utf8_str = "/";
    int size = 500;
    int opt = 0;
    while ((opt = getopt(argc, argv, "c:s:")) != -1)
    {
        switch (opt)
        {
        case 'c':
            utf8_str = optarg;
            break;
        case 's':
            size = atoi(optarg);
            break;
        default:
            fprintf(stderr, "usage: %s [-c char] [-s font size]\n", argv[0]);
            return 1;
        }
    }

    /** Load font and library */
    size_t font_data_size = 0;
    void *font_data = map_file("./FiraGO-Regular_extended_with_NotoSansEgyptianHieroglyphs-Regular.ttf", &font_data_size);
    assert(font_data != NULL);
    SFT_Font *font = sft_loadmem(font_data, font_data_size);
    assert(font != NULL);
    SFT sft = {
        .xScale = sft_from_int(size),
        .yScale = sft_from_int(size),
        .flags = SFT_DOWNWARD_Y,
        .font = font,
    };

    /** Render char */
    SFT_UChar c[2];
    assert(utf8_to_utf32((unsigned char *)utf8_str, c, 2) > 0);
    SFT_Glyph gid;
    assert(sft_lookup(&sft, c[0], &gid) == 0);
    SFT_GMetrics mtx;
    assert(sft_gmetrics(&sft, gid, &mtx) == 0);
    SFT_Image img = {
        .width = mtx.minWidth,
        .height = mtx.minHeight,
    };
    uint8_t pixels[img.width * img.height];
    img.pixels = pixels;
    assert(sft_render(&sft, gid, img) == 0);

    /** Inverse pixels */
    for (int i = 0; i < img.width * img.height; i++)
    {
        img.pixels[i] = 255 - img.pixels[i];
    }

    /** Print image to ppm */
    FILE* output = fopen("single_char.ppm", "w");
    fprintf(output, "P3\n%d %d\n255\n", img.width, img.height);
    for (int i = 0; i < img.height; i++)
    {
        for (int j = 0; j < img.width; j++)
        {
            fprintf(output, "%d %d %d ", img.pixels[i * img.width + j], img.pixels[i * img.width + j], img.pixels[i * img.width + j]);
        }
        fprintf(output, "\n");
    }

    /** Free resources */
    sft_freefont(font);
    unmap_file(font_data, font_data_size);
    return 0;
}

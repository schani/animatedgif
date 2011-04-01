#ifndef __SCHANI_WRITEGIF_H__
#define __SCHANI_WRITEGIF_H__

#include <stdbool.h>

bool write_gif (const char *filename, int width, int height,
		color_t *palette, int num_colors, unsigned char *pixels);

#endif

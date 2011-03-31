#include <assert.h>
#include <stdlib.h>

#include "dither.h"

static int
nearest_color (unsigned char red, unsigned char green, unsigned char blue,
	       color_t *palette, int num_colors)
{
	int best = -1, best_dist = -1;
	int i;
	for (i = 0; i < num_colors; ++i) {
		int dist = abs ((int)palette [i].red - (int)red) +
			abs ((int)palette [i].green - (int)green) +
			abs ((int)palette [i].blue - (int)blue);
		if (i == 0 || dist < best_dist) {
			best = i;
			best_dist = dist;
		}
	}
	return best;
}

void
quantize (unsigned char *pixels,
	  int num_rows, int row_stride,
	  int num_cols, int col_stride,
	  color_t *palette, int num_colors,
	  unsigned char *output)
{
	int row, col;
	unsigned char *o = output;

	assert (num_colors <= 256);

	for (row = 0; row < num_rows; ++row) {
		unsigned char *p = pixels + row * row_stride;

		for (col = 0; col < num_cols; ++col) {
			int idx = nearest_color (p [0], p [1], p [2], palette, num_colors);
			*o++ = idx;
			p [0] = palette [idx].red;
			p [1] = palette [idx].green;
			p [2] = palette [idx].blue;
			p += col_stride;
		}
	}
}

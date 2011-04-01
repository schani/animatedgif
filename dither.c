#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "palette.h"
#include "dither.h"

#define CLAMP(x)	((x)<0 ? 0 : ((x) > 255 ? 255 : (x)))

void
sierra_lite (unsigned char *pixels,
	     int num_rows, int row_stride,
	     int num_cols, int col_stride,
	     int red_bits, int green_bits, int blue_bits,
	     unsigned char *output)
{
	int num_colors;
	color_t *palette = make_cube_palette (red_bits, green_bits, blue_bits, &num_colors);
	int row, col;
	int errors1 [(num_cols + 2) * 3], errors2 [(num_cols + 2) * 3];
	int *current_errors = errors1 + 3;
	int *next_errors = errors2 + 3;
	unsigned char *o = output;

	memset (errors1, 0, sizeof (errors1));
	memset (errors2, 0, sizeof (errors2));

	for (row = 0; row < num_rows; ++row) {
		unsigned char *p = pixels + row * row_stride;

		for (col = 0; col < num_cols; ++col) {
			int r = p [0] + current_errors [col * 3 + 0];
			int g = p [1] + current_errors [col * 3 + 1];
			int b = p [2] + current_errors [col * 3 + 2];
			unsigned char cr = CLAMP (r);
			unsigned char cg = CLAMP (g);
			unsigned char cb = CLAMP (b);
			int index = (cr >> (8 - red_bits)) |
				((cg >> (8 - green_bits)) << red_bits) |
				((cb >> (8 - blue_bits)) << (red_bits + green_bits));

			assert (index >= 0 && index < num_colors);

			int er = r - palette [index].red;
			int eg = g - palette [index].green;
			int eb = g - palette [index].blue;

			*o++ = index;

			p += col_stride;

			current_errors [col * 3 + 3] += er >> 1;
			current_errors [col * 3 + 4] += eg >> 1;
			current_errors [col * 3 + 5] += eb >> 1;

			next_errors [col * 3 - 3] += er >> 2;
			next_errors [col * 3 - 2] += eg >> 2;
			next_errors [col * 3 - 1] += eb >> 2;

			next_errors [col * 3 + 0] += er >> 2;
			next_errors [col * 3 + 1] += eg >> 2;
			next_errors [col * 3 + 2] += eb >> 2;
		}

		/* errors for last column */
		next_errors [0] += current_errors [num_cols * 3 + 0];
		next_errors [1] += current_errors [num_cols * 3 + 1];
		next_errors [2] += current_errors [num_cols * 3 + 2];

		if (current_errors == errors1 + 3) {
			current_errors = errors2 + 3;
			next_errors = errors1 + 3;
		} else {
			current_errors = errors1 + 3;
			next_errors = errors2 + 3;
		}

		/* errors for first column */
		current_errors [0] += current_errors [-3];
		current_errors [1] += current_errors [-2];
		current_errors [2] += current_errors [-1];

		memset (next_errors - 3, 0, sizeof (errors1));
	}

	free (palette);
}

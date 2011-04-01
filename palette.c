#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "rwimg/readimage.h"
#include "rwimg/writeimage.h"

#include "palette.h"
#include "dither.h"

color_t*
make_cube_palette (int red_bits, int green_bits, int blue_bits, int *num_colors)
{
	int num, i;
	color_t *colors;

	assert (red_bits + green_bits + blue_bits <= 8);

	*num_colors = num = 1 << (red_bits + green_bits + blue_bits);
	colors = malloc (sizeof (color_t) * num);

	for (i = 0; i < num; ++i) {
		int r = i & ((1 << red_bits) - 1);
		int g = (i >> red_bits) & ((1 << green_bits) - 1);
		int b = (i >> (red_bits + green_bits)) & ((1 << blue_bits) - 1);

		colors [i].red = 255 * r / ((1 << red_bits) - 1);
		colors [i].green = 255 * g / ((1 << green_bits) - 1);
		colors [i].blue = 255 * b / ((1 << blue_bits) - 1);
	}

	return colors;
}

int
main (int argc, char *argv [])
{
	int width, height;
	unsigned char *data = read_image (argv [1], &width, &height);
	unsigned char *dithered = malloc (width * height);
	sierra_lite (data, height, width * 3, width, 3, 3, 3, 2, dithered);
	write_image ("out.png", width, height, data, 3, width * 3, IMAGE_FORMAT_AUTO);
	return 0;
}

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "rwimg/readimage.h"
#include "rwimg/writeimage.h"

#include "palette.h"
#include "dither.h"

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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <gif_lib.h>

#include "palette.h"

#include "writegif.h"

bool
write_gif (const char *filename, int width, int height,
	   color_t *palette, int num_colors, unsigned char *pixels)
{
	GifColorType *gif_colors;
	ColorMapObject *color_map;
	GifFileType *file;
	bool success = true;
	int i, result;
	static char *loop_extension_name = "NETSCAPE2.0";

	assert (num_colors <= 256);

	gif_colors = malloc (sizeof (GifColorType) * num_colors);
	for (i = 0; i < num_colors; ++i) {
		gif_colors [i].Red = palette [i].red;
		gif_colors [i].Green = palette [i].green;
		gif_colors [i].Blue = palette [i].blue;
	}

	color_map = MakeMapObject (num_colors, gif_colors);
	assert (color_map != NULL);

	free (gif_colors);

	file = EGifOpenFileName (filename, false);
	if (file == NULL) {
		success = false;
		goto done;
	}

	result = EGifPutScreenDesc (file, width, height, num_colors, 0, color_map);
	assert (result == GIF_OK);

	result = EGifPutExtensionFirst (file, APPLICATION_EXT_FUNC_CODE, strlen (loop_extension_name), loop_extension_name);
	assert (result == GIF_OK);

	result = EGifPutExtensionLast (file, APPLICATION_EXT_FUNC_CODE, 3, "\x01\xff\xff");
	assert (result == GIF_OK);

	result = EGifPutImageDesc (file, 0, 0, width, height, false, color_map);
	assert (result == GIF_OK);

	for (i = 0; i < height; ++i) {
		result = EGifPutLine (file, pixels + width * i, width);
		assert (result == GIF_OK);
	}

	result = EGifPutImageDesc (file, 0, 0, width, height, false, color_map);
	assert (result == GIF_OK);

	for (i = height - 1; i >= 0; --i) {
		result = EGifPutLine (file, pixels + width * i, width);
		assert (result == GIF_OK);
	}

	result = EGifCloseFile (file);
	assert (result == GIF_OK);

 done:
	FreeMapObject (color_map);

	return success;
}

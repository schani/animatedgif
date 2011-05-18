#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <gif_lib.h>

#include "palette.h"
#include "dither.h"

#include "writegif.h"

struct gif_write_state_t {
	GifFileType *file;

	int width;
	int height;

	int red_bits;
	int green_bits;
	int blue_bits;
};

gif_write_state_t*
gif_write_start (const char *filename, int width, int height,
		 int red_bits, int green_bits, int blue_bits)
{
	int num_colors;
	color_t *palette = make_cube_palette (red_bits, green_bits, blue_bits, &num_colors);
	ColorMapObject *color_map;
	GifFileType *file = NULL;
	GifColorType *gif_colors;
	gif_write_state_t *state = NULL;
	int result, i;

	assert (num_colors <= 256);

	gif_colors = malloc (sizeof (GifColorType) * num_colors);
	for (i = 0; i < num_colors; ++i) {
		gif_colors [i].Red = palette [i].red;
		gif_colors [i].Green = palette [i].green;
		gif_colors [i].Blue = palette [i].blue;
	}

	free (palette);

	color_map = MakeMapObject (num_colors, gif_colors);
	assert (color_map != NULL);

	free (gif_colors);

	file = EGifOpenFileName (filename, false);
	if (file == NULL)
		goto done;

	result = EGifPutScreenDesc (file, width, height, num_colors, 0, color_map);
	if (result != GIF_OK)
		goto done;

	result = EGifPutExtensionFirst (file, APPLICATION_EXT_FUNC_CODE, 11, "NETSCAPE2.0");
	if (result != GIF_OK)
		goto done;

	result = EGifPutExtensionLast (file, APPLICATION_EXT_FUNC_CODE, 3, "\x01\xff\xff");
	if (result != GIF_OK)
		goto done;

	state = malloc (sizeof (gif_write_state_t));

	state->file = file;

	state->width = width;
	state->height = height;

	state->red_bits = red_bits;
	state->green_bits = green_bits;
	state->blue_bits = blue_bits;

 done:
	FreeMapObject (color_map);
	if (state == NULL && file != NULL)
		EGifCloseFile (file);

	return state;
}

bool
gif_write_frame (gif_write_state_t *state, unsigned char *rgb_data, int row_stride, int pixel_stride)
{
	unsigned char *dithered = malloc (state->width * state->height);
	int result, i;

	sierra_lite (rgb_data, state->height, row_stride, state->width, pixel_stride,
		     state->red_bits, state->green_bits, state->blue_bits, dithered);

	result = EGifPutExtension (state->file, GRAPHICS_EXT_FUNC_CODE, 4, "\x04\x05\x00\x00");
	if (result != GIF_OK)
		return false;

	result = EGifPutImageDesc (state->file, 0, 0, state->width, state->height, false, NULL);
	if (result != GIF_OK)
		return false;

	for (i = 0; i < state->height; ++i) {
		result = EGifPutLine (state->file, dithered + state->width * i, state->width);
		if (result != GIF_OK)
			return false;
	}

	free (dithered);

	return true;
}

bool
gif_write_close (gif_write_state_t *state)
{
	int result = EGifCloseFile (state->file);

	free (state);

	return result == GIF_OK;
}

/*
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

	result = EGifPutImageDesc (file, 0, 0, width, height, false, NULL);
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
*/

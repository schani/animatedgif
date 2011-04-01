#ifndef __SCHANI_WRITEGIF_H__
#define __SCHANI_WRITEGIF_H__

#include <stdbool.h>

typedef struct gif_write_state_t gif_write_state_t;

gif_write_state_t* gif_write_start (const char *filename, int width, int height,
				    int red_bits, int green_bits, int blue_bits);

bool gif_write_frame (gif_write_state_t *state, unsigned char *rgb_data, int row_stride, int pixel_stride);

bool gif_write_close (gif_write_state_t *state);

#endif

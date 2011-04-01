#ifndef __SCHANI_PALETTE_H__
#define __SCHANI_PALETTE_H__

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} color_t;

color_t* make_cube_palette (int red_bits, int green_bits, int blue_bits, int *num_colors);

#endif

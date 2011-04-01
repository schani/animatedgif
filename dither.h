#ifndef __SCHANI_DITHER_H__
#define __SCHANI_DITHER_H__

void sierra_lite (unsigned char *pixels,
		  int num_rows, int row_stride,
		  int num_cols, int col_stride,
		  int red_bits, int green_bits, int blue_bits,
		  unsigned char *output);

#endif

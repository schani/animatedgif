#ifndef __SCHANI_DITHER_H__
#define __SCHANI_DITHER_H__

#include "palette.h"

void quantize (unsigned char *pixels,
	       int num_rows, int row_stride,
	       int num_cols, int col_stride,
	       octree_t *tree,
	       unsigned char *output);

void sierra_lite (unsigned char *pixels,
		  int num_rows, int row_stride,
		  int num_cols, int col_stride,
		  octree_t *tree,
		  unsigned char *output);

#endif

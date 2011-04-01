#include <assert.h>
#include <stdlib.h>

#include "dither.h"

static octree_t*
nearest_color (unsigned char red, unsigned char green, unsigned char blue, octree_t *tree)
{
	unsigned char mask = 0x80;
	do {
		int index = OCTREE_CHILD_INDEX (red, green, blue, mask);
		octree_t *child = tree->children [index];
		if (child == NULL)
			break;
		tree = child;
		mask >>= 1;
	} while (mask != 0);
	assert (tree->index >= 0);
	return tree;
}

void
quantize (unsigned char *pixels,
	  int num_rows, int row_stride,
	  int num_cols, int col_stride,
	  octree_t *tree,
	  unsigned char *output)
{
	int row, col;
	unsigned char *o = output;

	for (row = 0; row < num_rows; ++row) {
		unsigned char *p = pixels + row * row_stride;

		for (col = 0; col < num_cols; ++col) {
			octree_t *node = nearest_color (p [0], p [1], p [2], tree);
			*o++ = node->index;
			p [0] = node->red;
			p [1] = node->green;
			p [2] = node->blue;
			p += col_stride;
		}
	}
}

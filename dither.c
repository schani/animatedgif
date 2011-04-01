#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

#define CLAMP(x)	((x)<0 ? 0 : ((x) > 255 ? 255 : (x)))

void
sierra_lite (unsigned char *pixels,
	     int num_rows, int row_stride,
	     int num_cols, int col_stride,
	     octree_t *tree,
	     unsigned char *output)
{
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
			octree_t *node = nearest_color (CLAMP (r), CLAMP(g), CLAMP (b), tree);
			int er = r - node->red;
			int eg = g - node->green;
			int eb = g - node->blue;

			*o++ = node->index;
			p [0] = node->red;
			p [1] = node->green;
			p [2] = node->blue;

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
}

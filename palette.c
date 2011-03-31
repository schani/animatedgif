#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "rwimg/readimage.h"

typedef struct octree octree_t;

struct octree {
	unsigned int count;
	unsigned int red;
	unsigned int green;
	unsigned int blue;
	octree_t *parent;
	octree_t *children [8];
};

static octree_t*
octree_alloc_node (octree_t *parent)
{
	octree_t *node = (octree_t*)malloc (sizeof (octree_t));
	memset (node, 0, sizeof (octree_t));
	node->parent = parent;
	return node;
}

static octree_t*
octree_insert (octree_t *tree, unsigned char r, unsigned char g, unsigned char b, int num_levels)
{
	unsigned char mask = 0x80;
	octree_t *n;
	int i;

	if (tree == NULL)
		tree = octree_alloc_node (NULL);

	n = tree;
	for (i = 0; i < num_levels; ++i) {
		int i = ((r & mask) ? 1 : 0) | ((g & mask) ? 2 : 0) | ((b & mask) ? 4 : 0);
		assert (i >= 0 && i < 8);
		if (n->children [i] == NULL)
			n->children [i] = octree_alloc_node (n);
		n = n->children [i];

		mask >>= 1;
		assert (mask != 0);
	}

	n->red += r;
	n->green += g;
	n->blue += b;

	do {
		++n->count;
		n = n->parent;
	} while (n != NULL);

	return tree;
}

static octree_t*
build_octree (unsigned char *pixels,
	      int num_rows, int row_stride,
	      int num_cols, int col_stride,
	      int num_levels)
{
	octree_t *tree = NULL;
	int row, col;

	for (row = 0; row < num_rows; ++row) {
		unsigned char *p = pixels + row * row_stride;

		for (col = 0; col < num_cols; ++col) {
			tree = octree_insert (tree, p [0], p [1], p [2], num_levels);
			p += col_stride;
		}
	}

	return tree;
}

static bool
gather_collapse_candidates (octree_t *tree, int *num, octree_t **arr)
{
	int i;
	bool is_leaf = true;
	bool have_nonleaf_children = false;

	assert (tree != NULL);

	for (i = 0; i < 8; ++i) {
		octree_t *c = tree->children [i];
		if (c != NULL) {
			is_leaf = false;
			if (!gather_collapse_candidates (c, num, arr))
				have_nonleaf_children = true;
		}
	}

	if (!is_leaf && !have_nonleaf_children) {
		if (arr != NULL)
			arr [*num] = tree;
		++*num;
	}

	return is_leaf;
}

int
main (void)
{
	int width, height;
	unsigned char *data = read_image ("in.jpg", &width, &height);
	octree_t *tree = build_octree (data, height, width * 3, width, 3, 6);
	int num_candidates = 0;
	octree_t **candidates;
	gather_collapse_candidates (tree, &num_candidates, NULL);
	candidates = malloc (sizeof (octree_t*) * num_candidates);
	num_candidates = 0;
	gather_collapse_candidates (tree, &num_candidates, candidates);
	printf ("%u - %d\n", tree->count, num_candidates);
	return 0;
}

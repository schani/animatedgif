#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "rwimg/readimage.h"

#include "palette.h"

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

static int
compare_candidates (const void *p1, const void *p2)
{
	octree_t *t1 = *(octree_t**)p1;
	octree_t *t2 = *(octree_t**)p2;
	if (t1->count < t2->count)
		return -1;
	if (t1->count == t2->count)
		return 0;
	return 1;
}

static bool
is_leaf (octree_t *node)
{
	int i;
	assert (node != NULL);
	for (i = 0; i < 8; ++i) {
		if (node->children [i] != NULL)
			return false;
	}
	return true;
}

static octree_t*
collapse_node (octree_t *node)
{
	int i;
	assert (!is_leaf (node));
	for (i = 0; i < 8; ++i) {
		octree_t *c = node->children [i];
		if (c == NULL)
			continue;
		assert (is_leaf (c));
		node->red += c->red;
		node->green += c->green;
		node->blue += c->blue;
		free (c);
		node->children [i] = NULL;
	}
	node = node->parent;
	if (node == NULL)
		return NULL;
	for (i = 0; i < 8; ++i) {
		octree_t *c = node->children [i];
		if (c != NULL && !is_leaf (c))
			return NULL;
	}
	return node;
}

static int
reduce_octree (octree_t *tree, int max, color_t *colors)
{
	int num_candidates = 0;
	octree_t **candidates;
	int first = 0;
	int i;

	gather_collapse_candidates (tree, &num_candidates, NULL);

	candidates = malloc (sizeof (octree_t*) * num_candidates);
	printf ("%u - %d\n", tree->count, num_candidates);
	num_candidates = 0;
	gather_collapse_candidates (tree, &num_candidates, candidates);

	qsort (candidates, num_candidates, sizeof (octree_t*), compare_candidates);

	while (num_candidates - first > max) {
		octree_t *new_candidate = collapse_node (candidates [first]);
		++first;
		if (new_candidate != NULL) {
			int i = --first;
			while (i < num_candidates - 1 && new_candidate->count > candidates [i + 1]->count) {
				candidates [i] = candidates [i + 1];
				++i;
			}
			candidates [i] = new_candidate;
		}
	}

	for (i = 0; first + i < num_candidates; ++i) {
		octree_t *node = candidates [first + i];
		collapse_node (node);
		colors [i].red = node->red / node->count;
		colors [i].green = node->green / node->count;
		colors [i].blue = node->blue / node->count;
		printf ("%3d  %3d  %3d   %d\n", colors [i].red, colors [i].green, colors [i].blue, node->count);
	}

	return i;
}

#define NUM_COLORS	16

int
main (void)
{
	int width, height;
	unsigned char *data = read_image ("in.jpg", &width, &height);
	octree_t *tree = build_octree (data, height, width * 3, width, 3, 6);
	color_t colors [NUM_COLORS];
	int num = reduce_octree (tree, 16, colors);
	return 0;
}

#ifndef __SCHANI_PALETTE_H__
#define __SCHANI_PALETTE_H__

typedef struct octree octree_t;

struct octree {
	unsigned int count;
	unsigned int red;
	unsigned int green;
	unsigned int blue;
	int index;
	octree_t *parent;
	octree_t *children [8];
};

#define OCTREE_CHILD_INDEX(r,g,b,mask)	((((r) & (mask)) ? 1 : 0) | (((g) & (mask)) ? 2 : 0) | (((b) & (mask)) ? 4 : 0))

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} color_t;

#endif

#ifndef _MAP_H_
#define _MAP_H_

#include "img.h"
 
struct TILE
{
	unsigned char img;
	unsigned char attr;
};

struct MAP
{
	struct TILE tile[9][8];
};

void initMap();
void removeMap();
void drawMap();
void drawTile(int x, int y);
void moveMap(int mx, int my);

extern DIB_BUFFER *offMap;
extern int vy, vy_offset;
extern struct MAP map;

enum {
	TILE_OPEN = 0,
	TILE_LOCK,
	TILE_DESTROY
};

#endif

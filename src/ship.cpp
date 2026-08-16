#include "res.h"
#include "obj.h"
#include "map.h"
#include "global.h"
#include "util.h"

int moveShip(OBJp o)
{
	o->timer++; 
	if (o->timer % o->speed != 0)
		return 0;

	int dest_x, dest_lx, dest_rx, dest_dy, dest_y;

	dest_x = o->x / 32;
	dest_lx = (o->x + 32) / 32;
	dest_rx = (o->x + 32) / 32;	
	dest_dy = (vy * 32 - (o->y - 32)) / 32;	
	dest_y = (vy * 32 - o->y) / 32;

	switch (o->dir)
	{
		case DIR_DOWN:
			if (o->y < vy * 32 - 288)
			{				
				subObject(o);
			}
			else if (map.tile[dest_dy][dest_x].attr != TILE_OPEN)
				o->dir = DIR_LEFT;
			else
				o->y -= o->y_dist;
			break;		
		case DIR_LEFT:
			if (dest_lx == 0)
			{				
				o->dir = DIR_RIGHT;
			}
			else if (map.tile[dest_y][dest_x].attr != TILE_OPEN)
				o->dir = DIR_RIGHT;
			else
				o->x -= o->x_dist;
			break;
		case DIR_RIGHT:
			if (dest_lx == 7)
				o->dir = DIR_DOWN;
			else if (map.tile[dest_y][dest_rx].attr != TILE_OPEN)
				o->dir = DIR_DOWN;
			else
				o->x += o->x_dist;
			break;
	}

	o->cur_frame++;
	o->cur_frame %= o->tot_frame;

	return 0;
}

int drawShip(OBJp o)
{
	if (o->y <= vy * 32)
	{			
		int sx = o->x;
		int sy = (vy - 1) * 32 + vy_offset - o->y - 1;
		draw_sprite_c(offscreen, o->spr[o->cur_frame], sx, sy);		
	}

	return 0;
}

OBJp initShip(int x, int y)
{
	OBJp o = (OBJp)_MEM_ALLOC(sizeof(OBJ));

	o->x = x;
	o->y = y;
	o->x_dist = 1;
	o->y_dist = 1;
	o->tot_frame = 3;
	o->cur_frame = 0;
	o->dir = DIR_DOWN;
	o->speed = 8;
	o->timer = 0;
	o->state = 0;
	o->count = 0;
	o->type = OBJ_SHIP;
	o->spr = (SPRITE **)_MEM_ALLOC(sizeof(SPRITE *) * 3);
	o->spr[0] = tile[7];
	o->spr[1] = tile[8];
	o->spr[2] = tile[9];
	o->move = moveShip;
	o->draw = drawShip;
	o->data = NULL;

	return o;
}

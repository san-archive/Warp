#include "res.h"
#include "obj.h"
#include "map.h"
#include "global.h"
#include "deto.h"
#include "util.h"
 
int moveDeto(OBJp o)
{	
	o->timer++;
	if (o->timer % o->speed == 0)
	{
		switch (o->state)
		{
			case DETO_STEP1:
				if (o->count == 0)
				{
					o->count = 32;
					o->state = DETO_STEP2;
					o->cur_frame = 1;
				}
				break;
			case DETO_STEP2:				
				if (o->count == 0)
				{
					o->count = 16;
					o->state = DETO_STEP3;
					o->cur_frame = 2;
				}
				break;
			case DETO_STEP3:
				if (o->count == 0)
				{
					subObject(o);
				}
				break;
		}		
		o->count--;
		o->x += o->x_dist;
		o->y += o->y_dist;
	}		

	return 0;
}

int drawDeto(OBJp o)
{
	if (o->y <= vy * 32 && o->y >= vy * 32 - 256)
	{			
		int sx = o->x;
		int sy = (vy - 1) * 32 + vy_offset - o->y - 1;
		draw_sprite_c(offscreen, o->spr[o->cur_frame], sx, sy);		
	}
	return 0;
}

OBJp initDeto(int x, int y, int type)
{
	OBJp o = (OBJp)_MEM_ALLOC(sizeof(OBJ));

	o->x = x;
	o->y = y;

	int dir;
	dir = rand() % 7;
	switch (dir)
	{
		case 0:
			o->x_dist = -3;
			break;
		case 1:
			o->x_dist = -2;
			break;
		case 2:
			o->x_dist = -1;
			break;
		case 3:
			o->x_dist = 0;
			break;
		case 4:
			o->x_dist = 1;
			break;
		case 5:
			o->x_dist = 2;
			break;
		case 6:
			o->x_dist = 3;
			break;
	}
	dir = rand() % 5;
	switch (dir)
	{
		case 0:
			o->y_dist = -3;
			break;
		case 1:
			o->y_dist = -2;
			break;
		case 2:
			o->y_dist = -1;
			break;
		case 3:
			o->y_dist = 0;
			break;
		case 4:
			o->y_dist = 1;
			break;
		case 5:
			o->y_dist = 2;
			break;
		case 6:
			o->y_dist = 3;
			break;
	}	
	o->tot_frame = 3;
	o->cur_frame = 0;
	o->dir = 0;	
	o->speed = 4;
	o->timer = 0;
	o->state = DETO_STEP1;
	o->count = 32;
	o->type = type;
	o->spr = (SPRITE **)_MEM_ALLOC(sizeof(SPRITE *) * 3);
	o->spr[0] = tile[19];
	o->spr[1] = tile[20];
	o->spr[2] = tile[21];
	o->move = moveDeto;
	o->draw = drawDeto;

	return o;
}

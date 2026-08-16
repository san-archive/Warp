#include "res.h"
#include "obj.h"
#include "map.h"
#include "my.h"
#include "bullet.h"
#include "global.h"
#include "util.h"

#define MY_MOVE_DELAY	8
#define MY_SHOOT_DELAY	125
 
int checkMyCrash(OBJp o)
{
	int x1, y1, x2, y2, _x1, _y1, _x2, _y2;

	for (OBJp _o = obj_bottom; _o != NULL; _o = _o->next) 
	{
		if (_o->dead)
			continue;

		if (_o->type != OBJ_MY &&
			_o->type != OBJ_MY_BULLET &&
			_o->type != OBJ_ENEMY_DETO)
		{
			if (_o->y >= vy)
			{
				//my
				x1 = o->x;				
				y1 = vy * 32 + vy_offset - o->y - 1;
				x2 = x1 + o->spr[o->cur_frame]->width;
				y2 = y1 + o->spr[o->cur_frame]->height;

				//enemy
				_x1 = _o->x;
				_y1 = vy * 32 + vy_offset - _o->y - 1;
				_x2 = _x1 + _o->spr[_o->cur_frame]->width;
				_y2 = _y1 + _o->spr[_o->cur_frame]->height;

				if ((x1 < _x2) &&	(_x1 < x2) && (y1 < _y2) && (_y1 < y1))
				{
					addDeto(o->x, o->y, OBJ_MY_DETO);
					addDeto(o->x, o->y, OBJ_MY_DETO);
					addDeto(o->x, o->y, OBJ_MY_DETO);
					addDeto(o->x, o->y, OBJ_MY_DETO);
					addDeto(o->x, o->y, OBJ_MY_DETO);
					addDeto(o->x, o->y, OBJ_MY_DETO);
					addDeto(o->x, o->y, OBJ_MY_DETO);
					addDeto(o->x, o->y, OBJ_MY_DETO);		
					subObject(o);
					break;
				}
			}
		}
	}

	return 0;
}

int moveMy(OBJp o)
{
	o->timer++;	
	if (o->timer % o->speed == 0)
	{
		int lx, rx;

		lx = o->x;
		rx = o->x + 40;

		if (o->timer % MY_MOVE_DELAY == 0)
		{
			if (key & KEY_LEFT)
			{
				if (o->x > 0)
				{
					o->x -= o->x_dist;
					o->cur_frame = 2;
					o->count = 128;
				}
			}
			else if (key & KEY_RIGHT)
			{
				if (o->x + 33 < 256)
				{
					o->x += o->x_dist;
					o->cur_frame = 1;
					o->count = 128;
				}
			}
		}

		if ((key & KEY_SHOOT) && ((MY_DATA *)o->data)->shoot_timer == 0)
		{
			int dest_x, dest_y;
			//포신이 가리키는 맵좌표를 구한다
			dest_x = (o->x + 7) / 32;
			dest_y = vy - ((o->y + 16) / 32);
			//포신이 가리키는 맵의 속성이 벽이 아니면 쏜다
			if (map.tile[dest_y][dest_x].attr == TILE_OPEN)
			{
				dest_x = o->x + 7;
				dest_y = o->y + 16;
				addBullet(dest_x, dest_y, OBJ_MY_BULLET);
			}
			((MY_DATA *)o->data)->shoot_timer = MY_SHOOT_DELAY;
		}

		if (((MY_DATA *)o->data)->shoot_timer > 0)
			((MY_DATA *)o->data)->shoot_timer--;
		if (o->count > 0) o->count--;
		else o->cur_frame = 0;		
	}

	int dest_lx, dest_rx, dest_y;

	dest_lx = o->x / 32;
	dest_rx = (o->x + 31) / 32;	
	//dest_y = vy - (o->y / 32);
	dest_y = (vy * 32 - o->y) / 32;

	unsigned char lx_attr, rx_attr;

	lx_attr = map.tile[dest_y][dest_lx].attr;
	rx_attr = map.tile[dest_y][dest_rx].attr;

	if (lx_attr == TILE_LOCK || lx_attr == TILE_DESTROY ||
		rx_attr == TILE_LOCK || rx_attr == TILE_DESTROY)
	{
		/*
		addDeto(o->x, o->y, OBJ_MY_DETO);
		addDeto(o->x, o->y, OBJ_MY_DETO);
		addDeto(o->x, o->y, OBJ_MY_DETO);
		addDeto(o->x, o->y, OBJ_MY_DETO);
		addDeto(o->x, o->y, OBJ_MY_DETO);
		addDeto(o->x, o->y, OBJ_MY_DETO);
		addDeto(o->x, o->y, OBJ_MY_DETO);
		addDeto(o->x, o->y, OBJ_MY_DETO);		
		subObject(o);		
		*/
	}

	if (o->timer % 24 == 0)
	{
		moveMap(0, 1);
		o->y += o->y_dist;
	}

	checkMyCrash(o);

	return 0;
}

int drawMy(OBJp o)
{
	int sx = o->x;
	int sy = (vy - 1) * 32 + vy_offset - o->y - 1;

	draw_sprite(offscreen, o->spr[o->cur_frame], sx, sy);

	return 0;
}

OBJp initMy(int x, int y)
{
	OBJp o = (OBJp)_MEM_ALLOC(sizeof(OBJ));

	o->x = x;
	o->y = y;	
	o->x_dist = 2;
	o->y_dist = 1;
	o->tot_frame = 3;
	o->cur_frame = 0;
	o->dir = DIR_LEFT;
	o->speed = 1;
	o->timer = 0;
	o->state = 0;
	o->count = 0;
	o->type = OBJ_MY;
	o->spr = (SPRITE **)_MEM_ALLOC(sizeof(SPRITE *) * 3);
	o->spr[0] = tile[13];
	o->spr[1] = tile[14];
	o->spr[2] = tile[15];
	o->move = moveMy;
	o->draw = drawMy;
	o->data = _MEM_ALLOC(sizeof(MY_DATA));

	return o;
}

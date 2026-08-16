#include "res.h"
#include "obj.h"
#include "map.h"
#include "global.h"
#include "tank.h"
#include "bullet.h"
#include "util.h"

void tankMove(OBJp o)
{
	int dest_x, dest_y;
	
	dest_x = o->x / 32;
	dest_y = (vy * 32 - o->y) / 32;

	switch(o->dir)
	{
		case DIR_LEFT:			 
			if (dest_x == 0)
			{
				o->dir = DIR_RIGHT;
			}
			else 
			{
				dest_x = (o->x - o->x_dist) / 32;
				if (map.tile[dest_y][dest_x].attr == 1)
				{
					o->dir = DIR_RIGHT;				
				}
				else
				{
					o->x -= o->x_dist;
				}

			}
			break;
		case DIR_RIGHT:
			if (dest_x == 7)
			{
				o->dir = DIR_LEFT;
			}
			else 
			{
				dest_x = (o->x + o->x_dist + 32) / 32;
				if (map.tile[dest_y][dest_x].attr == 1)
				{
					o->dir = DIR_LEFT;
				}
				else
				{
					o->x += o->x_dist;
				}
			}
			break;
	}
	
	o->cur_frame++;
	o->cur_frame %= o->tot_frame;

	int r = rand() % 50;
	if (r < 1)
	{		
		int dest_x;
		dest_x = (o->x + 15) / 32;
		dest_y++;
		//탱크의 포신이 가리키는 위치가 벽이 아니면 쏜다
		if (dest_y < 9 && map.tile[dest_y][dest_x].attr == TILE_OPEN)
		{			
			o->state = TANK_SHOOT;			
			addBullet(o->x + 9, o->y - 32, OBJ_ENEMY_BULLET);
		}
	}
}

void tankShoot(OBJp o)
{
	if (o->count > 31)
	{
		o->state = TANK_MOVE;		
		o->count = 0;		
	}
	else
	{
		o->count++;
	}	
}

int moveTank(OBJp o)
{	
	o->timer++;
	if (o->timer % o->speed != 0)
		return 0;

	if (o->y < vy * 32 - 256)
	{
		subObject(o);
		return 0;
	}

	switch(o->state)
	{
		case TANK_MOVE:
			tankMove(o);
			break;
		case TANK_SHOOT:
			tankShoot(o);
			break;
	}	

	return 0;
}

int drawTank(OBJp o)
{
	if (o->y <= vy * 32 && o->y >= vy * 32 - 256)
	{			
		int sx = o->x;
		int sy = (vy - 1) * 32 + vy_offset - o->y - 1;

		draw_sprite_c(offscreen, o->spr[o->cur_frame], sx, sy);		
	}
	return 0;
}

OBJp initTank(int x, int y)
{
	OBJp o = (OBJp)_MEM_ALLOC(sizeof(OBJ));

	o->x = x;
	o->y = y;
	o->x_dist = 1;
	o->y_dist = 1;
	o->tot_frame = 3;
	o->cur_frame = 0;
	o->dir = DIR_RIGHT;
	o->speed = 16;
	o->timer = 0;
	o->state = TANK_MOVE;
	o->count = 0;
	o->type = OBJ_TANK;
	o->spr = (SPRITE **)_MEM_ALLOC(sizeof(SPRITE *) * 3);
	o->spr[0] = tile[10];
	o->spr[1] = tile[11];
	o->spr[2] = tile[12];
	o->move = moveTank;
	o->draw = drawTank;
	o->data = NULL;

	return o;
}

#include "res.h"
#include "obj.h"
#include "map.h"
#include "global.h"
#include "bullet.h"
#include "util.h"
 
void bulletEnergy(OBJp o)
{
	if (o->count > 31)
	{
		o->state = BULLET_BULLET;
		o->count = 0;
		o->cur_frame = 1;
	}
	else
	{
		o->count++;
	}
}

void bulletBullet(OBJp o)
{	
	o->y -= o->y_dist;

	//벽에 부딪히면 없앤다
	int dest_x, dest_y;

	dest_x = o->x / 32;	
	dest_y = vy - ((o->y - 16) / 32);

	if (dest_y >= vy + 8 && map.tile[dest_y][dest_x].attr != TILE_OPEN)
	{
		subObject(o);
	}
}

int checkMyBulletCrash(OBJp o)
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
					for (int i = 0; i < 16; i++)
					{
						addDeto(o->x, o->y, OBJ_ENEMY_DETO);
					}
					subObject(_o);
					subObject(o);
					break;
				}
			}
		}
	}

	return 0;
}

void bulletBullet_my(OBJp o)
{
	o->y += o->y_dist;

	//벽에 부딪히면 없앤다
	int dest_x, dest_lx, dest_rx, dest_y;

	dest_lx = o->x / 32;
	dest_rx = (o->x + 16) / 32;	
	dest_y = (vy * 32 - o->y) / 32;

	if (o->y <= vy * 32)
	{
		unsigned char lx_attr, rx_attr;

		lx_attr = map.tile[dest_y][dest_lx].attr;
		rx_attr = map.tile[dest_y][dest_rx].attr;

		if (lx_attr == TILE_OPEN && rx_attr == TILE_OPEN)
		{
			checkMyBulletCrash(o);
		}
		else
		{
			if (lx_attr == TILE_DESTROY || rx_attr ==  TILE_DESTROY)
			{
				if (lx_attr == TILE_DESTROY)
				{
					dest_x = dest_lx;
				}
				else if (rx_attr == TILE_DESTROY)
				{
					dest_x = dest_rx;
				}
				map.tile[dest_y][dest_x].img = 2;
				map.tile[dest_y][dest_x].attr = 0;
				drawTile(dest_x, dest_y);
				if (dest_x < 7)
					drawTile(dest_x + 1, dest_y);
			}			
			subObject(o);
		}
	}
	else
	{
		subObject(o);
	}
}

int moveBullet(OBJp o)
{	
	o->timer++;
	if (o->timer % o->speed != 0)
		return 0;

	if (o->type == OBJ_ENEMY_BULLET)
	{
		switch(o->state)
		{
			case BULLET_ENERGY:
				bulletEnergy(o);
				break;
			case BULLET_BULLET:
				bulletBullet(o);
				break;
		}	
	}
	else
	{
		bulletBullet_my(o);
	}

	return 0;
}

int drawBullet(OBJp o)
{	
	if (o->y <= vy * 32 && o->y >= vy * 32 - 256)
	{			
		int sx = o->x;
		int sy = (vy - 1) * 32 + vy_offset - o->y - 1;
		draw_sprite_c(offscreen, o->spr[o->cur_frame], sx, sy);		
	}

	return 0;
}

OBJp initBullet(int x, int y, int type)
{
	OBJp o = (OBJp)_MEM_ALLOC(sizeof(OBJ));

	o->x = x;
	o->y = y;
	o->x_dist = 0;
	o->y_dist = 4;
	o->tot_frame = 3;
	if (o->type == OBJ_ENEMY_BULLET)
		o->cur_frame = 1;
	else
		o->cur_frame = 0;
	o->dir = 0;
	o->speed = 8;
	o->timer = 0;
	o->state = BULLET_ENERGY;
	o->count = 0;
	o->type = type;
	if (o->type == OBJ_ENEMY_BULLET)
	{
		o->spr = (SPRITE **)_MEM_ALLOC(sizeof(SPRITE *) * 2);
		o->spr[0] = tile[18];
		o->spr[1] = tile[17];	
	}
	else
	{
		o->spr = (SPRITE **)_MEM_ALLOC(sizeof(SPRITE *) * 1);
		o->spr[0] = tile[16];		
	}
	o->move = moveBullet;
	o->draw = drawBullet;

	return o;
}

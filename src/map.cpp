#include "map.h"
#include "img.h"
#include "res.h"
#include "obj.h"
#include "global.h"
 
struct MAP map;
DIB_BUFFER *offMap;
int vy = 8, vy_offset;

void createMapRow(int y)
{
	int r, x;

	//1.벽 5%
	for (x = 0; x < 8; x++)
	{
		map.tile[y][x].img = 0;
		map.tile[y][x].attr = 0;

		r = rand() % 100;
		if (r < 5)
		{
			r = rand() % 2;
			if (r == 0)
			{
				map.tile[y][x].img = 3;
				map.tile[y][x].attr = 1;
			}
			else
			{
				map.tile[y][x].img = 6;
				map.tile[y][x].attr = 1;
			}
		}
	}
	//2.일반 10%
	for (x = 0; x < 8; x++)
	{
		if (map.tile[y][x].attr == 0)
		{
			r = rand() % 100;
			if (r < 10)
			{
				map.tile[y][x].img = 5;
				map.tile[y][x].attr = 0;
			}
		}
	}
	//3.오브젝트
}

void initMap()
{
	offMap = create_dib_buffer(32 * 8, 32 * 9);
	int i, j, y, tileNum;

	for (y = 0; y < 9; y++)	{
		createMapRow(y);
	}

	map.tile[6][0].img = 0;
	map.tile[6][0].attr = 0;
	map.tile[7][0].img = 0;
	map.tile[7][0].attr = 0;
	map.tile[8][0].img = 0;
	map.tile[8][0].attr = 0;	
/*
	addObject(OBJ_SHIP, 3 * 32, 8 * 32);
	map.tile[5][3].img = 3;
	map.tile[5][3].attr = 1;	
*/
	for (i = 0; i < 9; i++)	{
		for (j = 7; j >= 0; j--) {
			tileNum = map.tile[i][j].img;
			draw_sprite_c(offMap, tile[0], j * 32, i * 32);
			draw_sprite_c(offMap, tile[tileNum], j * 32, i * 32);
		}
	}
}

void removeMap()
{
	destroy_dib_buffer(offMap);
}

void drawTile(int x, int y)
{	
	int tileNum = map.tile[y][x].img;
	draw_sprite_c(offMap, tile[0], x * 32, (y - vy) * 32);
	draw_sprite_c(offMap, tile[tileNum], x * 32, (y - vy) * 32);
}

void drawMap()
{
	copy_dib_buffer(offscreen, 0, 0, 256, 256,
		offMap, 0, 32 - vy_offset);	
}

void moveMap(int mx, int my)
{
	int r, obj;

	vy_offset += my;
	if (vy_offset > 31)
	{
		vy++;
		vy_offset = 0;

		int i, j, tileNum;

		//랜덤 맵 생성
		for (i = 7; i >= 0; i--)
		{
			memcpy(map.tile[i + 1], map.tile[i], sizeof(struct TILE) * 8);
		}
		createMapRow(0);

		for (i = 0; i < 8; i++)
		{
			if (map.tile[0][i].attr == TILE_OPEN)
			{				
				r = rand() % 100;
				if (r < 20)
				{					
					obj = rand() % 2;				
					switch (obj)
					{
						case 0:
							addObject(OBJ_SHIP, i * 32, vy * 32);
							break;
						case 1:
							addObject(OBJ_TANK, i * 32, vy * 32);
							break;
					}										
				}
			}
		}

		for (i = 0; i < 9; i++)	{
			for (j = 7; j >= 0; j--) {
				tileNum = map.tile[i][j].img;
				draw_sprite_c(offMap, tile[0], j * 32, i * 32);
				draw_sprite_c(offMap, tile[tileNum], j * 32, i * 32);
			}
		}	
	}
}

#include <stdlib.h>
#include <string.h>
#include "img.h"
#include "obj.h"

SPRITE *tile[32];

void loadResource()
{
	SPRITE *tileSet;
	DIB_BUFFER *cropOffscreen;	
	struct CROP {
		int x, y, width, height;
	} crop[32] = {
		{0,0,32,32},//0
		{32,0,40,32},
		{72,0,32,32},
		{104,0,40,32},
		{144,0,32,32},
		{176,0,32,32},
		{208,0,40,32},
		{248,0,40,32},
		{288,0,40,32},//8
		{328,0,40,32},
 
		{368,0,40,32},//tank
		{408,0,40,32},//tank
		{448,0,40,32},//tank
		{240,128,40,32},//my 
		{280,128,40,32},//my left
		{240,160,40,32},//my right
		{280,160,16,16},//my bullet 16
		{296,160,16,16},//bullet
		{280,176,16,16},//energy
		{96,96,32,32},//my deto step1
		{128,96,32,32},//my deto step2
		{160,96,32,32},//my deto step3
		{192,96,32,32},
		{224,96,32,32},
		{256,96,32,32},//24
		{288,96,32,32},
	};	
	
	int i;

	cropOffscreen = create_crop_dib_buffer(640, 480, 31);
	tileSet = load_sprite(".\\res\\warp1.spr");
	draw_sprite(cropOffscreen, tileSet, 0, 0);
	for (i = 0; i < 13; i++) {
		tile[i] = crop_dib_buffer_sprite(cropOffscreen, 
			crop[i].x, 
			crop[i].y, 
			crop[i].width, 
			crop[i].height, 
			31);
	}
	destroy_sprite(tileSet);

	tileSet = load_sprite(".\\res\\warp0.spr");
	draw_sprite(cropOffscreen, tileSet, 0, 0);	
	for (i = 13; i < 26; i++) {
		tile[i] = crop_dib_buffer_sprite(cropOffscreen, 
			crop[i].x, 
			crop[i].y, 
			crop[i].width, 
			crop[i].height, 
			31);
	}
	destroy_sprite(tileSet);
	destroy_crop_dib_buffer(cropOffscreen);

	addObject(OBJ_MY, 0, 0);
}

void removeResource()
{
	for (int i = 0; i < 32; i++)
	{
		if (tile[i])
			destroy_sprite(tile[i]);
	}
}
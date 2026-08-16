#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "img.h"
 
struct OBJECT;
typedef struct OBJECT OBJ, *OBJp;
typedef int RUN (OBJp o);
typedef RUN *RUNp;
struct OBJECT
{	
	OBJp next, prev;
	int x, y;
	int x_dist, y_dist;
	int tot_frame, cur_frame;
	int dir, speed, timer;
	int state, count;
	int type;
	int dead;
	SPRITE **spr;
	RUNp move, draw;
	void *data;
};

enum {
	OBJ_TANK = 0,
	OBJ_SHIP,
	OBJ_SHOOT,
	OBJ_MY,
	OBJ_MY_BULLET,
	OBJ_ENEMY_BULLET,
	OBJ_MY_DETO,
	OBJ_ENEMY_DETO
};

enum {
	DIR_UP = 0,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_UP_RIGHT,
	DIR_DOWN_RIGHT,
	DIR_DOWN_LEFT,
	DIR_UP_LEFT
};

OBJp addBullet(int x, int y, int type);
OBJp addDeto(int x, int y, int type);
OBJp addObject(int type, int x, int y);
void subObject(OBJp o);	
void moveAllObject();
void drawAllObject();
void destroyAllObject();

extern OBJp obj_top, obj_bottom;

#endif

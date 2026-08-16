#ifndef _BULLET_H_
#define _BULLET_H_

#include "obj.h"
 
int moveBullet(OBJp o);
int drawBullet(OBJp o);
OBJp initBullet(int x, int y, int type);

enum {
	BULLET_ENERGY = 0,
	BULLET_BULLET,
};

enum {
	BULLET_UP = 0,
	BULLET_DOWN
};

#endif

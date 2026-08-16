#ifndef _TANK_H_
#define _TANK_H_

#include "obj.h" 

int moveTank(OBJp o);
int drawTank(OBJp o);
OBJp initTank(int x, int y);

enum {
	TANK_MOVE = 0,
	TANK_SHOOT,
};

#endif

#ifndef _MY_H_
#define _MY_H_

#include "obj.h"
 
int moveMy(OBJp o);
int drawMy(OBJp o);
OBJp initMy(int x, int y);

enum {
	MY_MOVE = 0,
	MY_SHOOT,	
};

struct MY_DATA
{
	int shoot_timer;
};

#endif

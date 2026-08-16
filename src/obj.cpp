#include "obj.h"
#include "tank.h"
#include "ship.h"
#include "bullet.h"
#include "my.h"
#include "deto.h"
#include "map.h"
#include "util.h"
 
OBJp obj_top, obj_bottom;

OBJp addList(OBJp o)
{
	if (!obj_bottom)
	{	
		obj_bottom = o;
		o->prev = NULL;
	}
	else
	{
		o->prev = obj_top;
		o->prev->next = o;
	}
	obj_top = o;
	o->next = NULL;

	return o;
}

OBJp addObject(int type, int x, int y)
{
	OBJp o;

	switch (type)
	{
		case OBJ_MY:
			o = initMy(x, y);
			break;
		case OBJ_TANK:
			o = initTank(x, y);
			break;
		case OBJ_SHIP:
			o = initShip(x, y);
			break;
	}

	return addList(o);
}

OBJp addBullet(int x, int y, int type)
{
	OBJp o;

	o = initBullet(x, y, type);
	return addList(o);	
}

OBJp addDeto(int x, int y, int type)
{
	OBJp o;

	o = initDeto(x, y, type);
	return addList(o);	
}

static void freeObject(OBJp o)
{
	if (o->data)
		_MEM_FREE(o->data);
	_MEM_FREE(o->spr);
	_MEM_FREE(o);
}

static void unlinkObject(OBJp o)
{
	if (o->prev)
		o->prev->next = o->next;
	else
		obj_bottom = o->next;

	if (o->next)
		o->next->prev = o->prev;
	else
		obj_top = o->prev;
}

void subObject(OBJp o)
{
	o->dead = 1;
}

static void sweepObject()
{
	OBJp next;

	for (OBJp o = obj_bottom; o != NULL; o = next) {
		next = o->next;
		if (o->dead) {
			unlinkObject(o);
			freeObject(o);
		}
	}
}

void moveAllObject()
{
	for (OBJp o = obj_bottom; o != NULL; o = o->next) {
		if (o->dead)
			continue;
		//현재 뷰상에 디스플레이되는 오브젝트만 실행한다
		if (o->y <= vy * 32)
			o->move(o);
	}

	sweepObject();
}

void drawAllObject()
{
	for (OBJp o = obj_bottom; o != NULL; o = o->next) {
		if (o->dead)
			continue;
		if (o->y <= vy * 32)
			o->draw(o);
	}
}

void destroyAllObject()
{
	OBJp next;

	for (OBJp o = obj_bottom; o != NULL; o = next) {
		next = o->next;
		freeObject(o);
	}
	obj_bottom = NULL;
	obj_top = NULL;
}

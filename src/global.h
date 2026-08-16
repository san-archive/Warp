#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "img.h"
 
extern DIB_BUFFER *offscreen;
extern int key;

enum {
	KEY_LEFT	= 0x01,
	KEY_RIGHT	= 0x02,
	KEY_SHOOT	= 0x04
};

#endif
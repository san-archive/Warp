#ifndef _DETO_H_
#define _DETO_H_
 
enum {
	DETO_STEP1 = 0,
	DETO_STEP2,
	DETO_STEP3
};

int moveDeto(OBJp o);
int drawDeto(OBJp o);
OBJp initDeto(int x, int y, int type);

#endif
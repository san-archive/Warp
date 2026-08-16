#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
/* 
util.c
*/
/*---------------------------------------------------------------------------*/
HANDLE __hMem__;
/*---------------------------------------------------------------------------*/
void _FTRACE(char *fn, char *f, ...)
{
	char buf[256];
	FILE *fp;

	va_list arg;
	va_start(arg, f);
	vsprintf(buf, f, arg);
	va_end(arg);

	fp = fopen(fn, "a+");
	fprintf(fp, buf);
	fclose(fp);
}
/*---------------------------------------------------------------------------*/
void _CTRACE(char* f, ...)
{
	static int h = 0, cnt = 0;
	char buf1[256] = "";
	char buf2[256] = "";
	HDC hdc;

	va_list arg;
	va_start(arg, f);
	vsprintf(buf1, f, arg);
	va_end(arg);	
	
	hdc = GetDC(0);
	SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
	TextOut(hdc, 0, h, buf2, 40);
	sprintf(buf2, "%d: %s", cnt++, buf1);
	TextOut(hdc, 0, h, buf2, strlen(buf2));
	ReleaseDC(0, hdc);

	h += 16;
	if (h > 640) h = 0;
}

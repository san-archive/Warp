#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "img.h"
#include "map.h"
#include "res.h"
#include "obj.h"
#include "util.h"
#include "global.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void gameProc();
void initGame();
void exitGame();
void drawGame();
void restartGame();

#define PROC_FREQ_DEFAULT		500
#define DRAW_FREQ		60
#define PROC_FREQ_MIN	30
#define PROC_FREQ_MAX	5000
#define MAX_DELTA_MS	100

#define GM_OVER_SEC	2

HWND hAppWnd;
BOOL bActive;
BOOL bAlive;
unsigned char pal[768];
enum
{
	GM_INIT = 0,
	GM_DEMO,
	GM_PLAY,
	GM_OVER,
	GM_EXIT
};
int mode;
int stage;
int key;
int proc_freq;
int over_timer;
int vy_start;
DIB_BUFFER *offscreen;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg = {0,};
	WNDCLASS wc = {0,};
	RECT rc = {0, 0, 256, 256};
	LARGE_INTEGER freq, now, prev;
	LONGLONG tick_len, frame_len, max_delta, delta;
	LONGLONG proc_acc = 0, draw_acc = 0, stat_acc = 0;
	int stat_ticks = 0, stat_frames = 0;

	proc_freq = atoi(lpCmdLine);
	if (proc_freq < PROC_FREQ_MIN || proc_freq > PROC_FREQ_MAX)
		proc_freq = PROC_FREQ_DEFAULT;

	wc.lpszClassName= "warp";
	wc.hInstance = hInstance;
	wc.lpfnWndProc	= WndProc;
	wc.hIcon	= LoadIcon (NULL,IDI_APPLICATION);
	wc.hCursor = LoadCursor (NULL,IDC_ARROW);
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wc))
		return 0;

	bActive = FALSE;
	bAlive = TRUE;

   AdjustWindowRectEx(&rc, WS_CAPTION, NULL, NULL);
	hAppWnd = CreateWindow("warp", "warp",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL,
		NULL,
		hInstance, NULL);

	if (!hAppWnd)
		return 0;

   ShowWindow(hAppWnd, nCmdShow);
   UpdateWindow(hAppWnd);

	initGame();

	timeBeginPeriod(1);

	QueryPerformanceFrequency(&freq);
	tick_len = freq.QuadPart / proc_freq;
	frame_len = freq.QuadPart / DRAW_FREQ;
	max_delta = freq.QuadPart * MAX_DELTA_MS / 1000;
	QueryPerformanceCounter(&prev);

	while (bAlive) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				bAlive = FALSE;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (!bAlive)
			break;

		QueryPerformanceCounter(&now);
		delta = now.QuadPart - prev.QuadPart;
		prev.QuadPart = now.QuadPart;

		if (!bActive) {
			proc_acc = 0;
			draw_acc = 0;
			Sleep(10);
			continue;
		}

		if (delta > max_delta)
			delta = max_delta;
		proc_acc += delta;
		draw_acc += delta;
		stat_acc += delta;

		while (proc_acc >= tick_len) {
			gameProc();
			proc_acc -= tick_len;
			stat_ticks++;
		}

		if (draw_acc >= frame_len) {
			draw_acc %= frame_len;
			drawGame();
			stat_frames++;
		}

		delta = tick_len - proc_acc;
		if (frame_len - draw_acc < delta)
			delta = frame_len - draw_acc;
		if (delta * 1000 >= freq.QuadPart)
			Sleep(1);
	}

	timeEndPeriod(1);

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;

	switch (message) {
		case WM_ACTIVATEAPP:
			bActive = (BOOL)wParam;
			break;
		case WM_PAINT:
			hDC = BeginPaint(hWnd, &ps);
			if (offscreen)
				draw_dib_buffer(offscreen, hDC);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			exitGame();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }

   return 0;
}

void initGame()
{
	srand((unsigned int)time(NULL));
	vy_start = vy;
	offscreen = create_dib_buffer(256, 256);
	load_palette(".\\res\\warp.pal", pal);
	set_palette(offscreen, pal);
	loadResource();
	initMap();

	mode = GM_PLAY;
}

void exitGame()
{
	destroy_dib_buffer(offscreen);
	offscreen = NULL;
	removeResource();
	removeMap();
}

void demoGame()
{
}

static int isMyAlive()
{
	OBJp o;

	for (o = obj_bottom; o != NULL; o = o->next) {
		if (o->type == OBJ_MY)
			return 1;
	}

	return 0;
}

void playGame()
{
	moveAllObject();

	if (!isMyAlive()) {
		mode = GM_OVER;
		over_timer = proc_freq * GM_OVER_SEC;
	}
}

void overGame()
{
	moveAllObject();

	if (--over_timer <= 0)
		restartGame();
}

void restartGame()
{
	destroyAllObject();
	removeMap();

	vy = vy_start;
	vy_offset = 0;

	initMap();
	addObject(OBJ_MY, 0, 0);

	mode = GM_PLAY;
}

void drawGame()
{
	if (!offscreen)
		return;

	drawMap();
	drawAllObject();
	HDC hDC = GetDC(hAppWnd);
	blit_dib_buffer(hDC,
		0, 0, 256, 256,
		offscreen, 0, 0);
	ReleaseDC(hAppWnd, hDC);
}

void gameProc()
{
	key = 0;
	if (GET_KEY_STATE(VK_LEFT))
		key |= KEY_LEFT;
	if (GET_KEY_STATE(VK_RIGHT))
		key |= KEY_RIGHT;
	if (GET_KEY_STATE(VK_SPACE))
		key |= KEY_SHOOT;

	switch (mode)
	{
		case GM_INIT:
			initGame();
			break;
		case GM_DEMO:
			demoGame();
			break;
		case GM_PLAY:
			playGame();
			break;
		case GM_OVER:
			overGame();
			break;
		case GM_EXIT:
			exitGame();
			break;
	}
}

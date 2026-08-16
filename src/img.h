#ifndef _IMG_H_
#define _IMG_H_

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
 
#ifdef __cplusplus
extern "C"
{
#endif
/*---------------------------------------------------------------------------*/
typedef struct {
	int width, height;
	HDC hMemDC;
	HBITMAP hbm, hbmOld;
	void *bits;
	int cx1, cy1, cx2, cy2;
} DIB_BUFFER;

DIB_BUFFER *create_dib_buffer(int width, int height);
void destroy_dib_buffer(DIB_BUFFER *_dib_ptr);
void draw_dib_buffer(DIB_BUFFER *_dib_ptr, HDC hDC);
void blit_dib_buffer(HDC hDestDC, 
	int dest_x, int dest_y, int dest_width, int dest_height,
	DIB_BUFFER *_src_ptr, int src_x, int src_y);
void copy_dib_buffer(DIB_BUFFER *_dest_ptr, 
	int dest_x, int dest_y, int dest_width, int dest_height,
	DIB_BUFFER *_src_ptr, int src_x, int src_y);
void redraw_dib_buffer(HWND hWnd);
void clear_dib_buffer(DIB_BUFFER *_dib_ptr, unsigned char c);
DIB_BUFFER* create_crop_dib_buffer(int width, int height, 
	unsigned char t_color);
void destroy_crop_dib_buffer(DIB_BUFFER *_dib_ptr);
void set_palette(DIB_BUFFER *_dib_ptr, unsigned char *pal);
unsigned char *load_palette(char *fn, unsigned char *pal);
/*---------------------------------------------------------------------------*/
typedef struct {
	int width, height;
	LPBYTE bits;
} SPRITE;

SPRITE *load_sprite(char *fn);
void draw_sprite(DIB_BUFFER *_dib_ptr, SPRITE *spr, int x, int y);
void draw_sprite_c(DIB_BUFFER *_dib_ptr, SPRITE *spr, int x, int y);
SPRITE *crop_dib_buffer_sprite(DIB_BUFFER *_dib_ptr, 
	int x, int y, int width, int height, unsigned char t_color);
void destroy_sprite(SPRITE *spr);

typedef struct {
	int width, height;
	LPBYTE bits;
} IMAGE;

IMAGE *load_image(char *fn);
void draw_image(DIB_BUFFER *_dib_ptr, IMAGE *img, int x, int y);
void draw_image_c(DIB_BUFFER *_dib_ptr, IMAGE *img, int x, int y);
IMAGE *crop_dib_buffer_image(DIB_BUFFER *_dib_ptr, 
	int x, int y, int width, int height);
void destroy_image(IMAGE *img);
/*---------------------------------------------------------------------------*/
#define PUT_PIXEL(_DIB_PTR, X, Y, C) *((unsigned char *)_DIB_PTR->bits + X + \
(Y * _DIB_PTR->width)) = C
void draw_pixel(DIB_BUFFER *_dib_ptr, int x, int y, unsigned char c);
void draw_hline(DIB_BUFFER *_dib_ptr, int x1, int x2, int y, unsigned char c);
void draw_vline(DIB_BUFFER *_dib_ptr, int x1, int x2, int y, unsigned char c);
void draw_line(DIB_BUFFER *_dib_ptr, int x1, int y1, int x2, int y2, 
	unsigned char c);
void draw_text(HDC hdc, int x, int y, char *str, ...);
/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif

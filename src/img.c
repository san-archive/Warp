#include "img.h"
#include "util.h" 
/*---------------------------------------------------------------------------*/
DIB_BUFFER *create_dib_buffer(int width, int height)
{	
	int biSize;
	BITMAPINFOHEADER bih;
	DIB_BUFFER *_dib_ptr;

	_dib_ptr = (DIB_BUFFER *)_MEM_ALLOC(sizeof(DIB_BUFFER));
	_dib_ptr->width = width;
	_dib_ptr->height = height;
	
	biSize = sizeof(BITMAPINFOHEADER);	
	memset(&bih, 0, biSize);

	bih.biSize = biSize;
	bih.biWidth = width;
	bih.biHeight = -height;
	bih.biPlanes = 1;
	bih.biBitCount = 8;
	bih.biCompression = BI_RGB;
	_dib_ptr->hMemDC = CreateCompatibleDC(NULL);
	_dib_ptr->hbm = CreateDIBSection(_dib_ptr->hMemDC,
		(LPBITMAPINFO)&bih,
		DIB_PAL_COLORS,
		&_dib_ptr->bits,
		NULL,
		0);

	_dib_ptr->hbmOld = 
		(HBITMAP)SelectObject(_dib_ptr->hMemDC, _dib_ptr->hbm);
	_dib_ptr->cx1 = 0;
	_dib_ptr->cy1 = 0;
	_dib_ptr->cx2 = width;
	_dib_ptr->cy2 = height;

	return _dib_ptr;
}

/*---------------------------------------------------------------------------*/
void destroy_dib_buffer(DIB_BUFFER *_dib_ptr)
{
	SelectObject(_dib_ptr->hMemDC, _dib_ptr->hbmOld);
	DeleteObject(_dib_ptr->hbm);
	DeleteDC(_dib_ptr->hMemDC);
	_MEM_FREE(_dib_ptr);
}

/*---------------------------------------------------------------------------*/
void draw_dib_buffer(DIB_BUFFER *_dib_ptr, HDC hDC)
{
	BitBlt(hDC, 0, 0,
		_dib_ptr->width, _dib_ptr->height,
		_dib_ptr->hMemDC,
		0, 0, SRCCOPY);
}

/*---------------------------------------------------------------------------*/
void blit_dib_buffer(HDC hDestDC, 
	int dest_x, int dest_y, int dest_width, int dest_height,
	DIB_BUFFER *_src_ptr, int src_x, int src_y)
{
	BitBlt(hDestDC, dest_x, dest_y, dest_width, dest_height,
		_src_ptr->hMemDC,	src_x, src_y, SRCCOPY);
}

/*---------------------------------------------------------------------------*/
void copy_dib_buffer(DIB_BUFFER *_dest_ptr, 
	int dest_x, int dest_y, int dest_width, int dest_height,
	DIB_BUFFER *_src_ptr, int src_x, int src_y)
{
/*
	BitBlt(_dest_ptr->hMemDC, dest_x, dest_y, dest_width, dest_height,
		_src_ptr->hMemDC,	src_x, src_y, SRCCOPY);
*/
	unsigned char *dest_bits, *src_bits;
	int y1;	

	for (y1 = src_y; y1 < src_y + dest_height; y1++)
	{	
		dest_bits = (unsigned char *)_dest_ptr->bits + 
			dest_x + ((dest_y + y1 - src_y) * _dest_ptr->width);
		src_bits = (unsigned char *)_src_ptr->bits + 
			src_x + y1 * _src_ptr->width;
		memcpy(dest_bits, src_bits, dest_width);
	}
}

/*---------------------------------------------------------------------------*/
void redraw_dib_buffer(HWND hWnd)
{
	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}

/*---------------------------------------------------------------------------*/
void clear_dib_buffer(DIB_BUFFER *_dib_ptr, unsigned char c)
{
	memset((unsigned char *)_dib_ptr->bits, c, 
		_dib_ptr->width * _dib_ptr->height);
}

/*---------------------------------------------------------------------------*/
/*비트맵 버퍼의 팔레트를 변경한다*/
/*---------------------------------------------------------------------------*/
void set_palette(DIB_BUFFER *_dib_ptr, unsigned char *pal)
{
	RGBQUAD *rgb;
	int i;
	
	rgb = (RGBQUAD *)_MEM_ALLOC(sizeof(RGBQUAD) * 256);
	for (i = 0; i < 256; i++, pal += 3) {
		rgb[i].rgbRed = pal[0];
		rgb[i].rgbGreen = pal[1];
		rgb[i].rgbBlue = pal[2];
		rgb[i].rgbReserved = 0;
	}
	SetDIBColorTable(_dib_ptr->hMemDC, 0, 256, rgb);

	_MEM_FREE(rgb);
}
/*---------------------------------------------------------------------------*/
/*crop을 위한 비트맵 버퍼를 만든다*/
/*---------------------------------------------------------------------------*/
DIB_BUFFER *create_crop_dib_buffer(int width, int height, 
	unsigned char t_color)
{
	DIB_BUFFER *_dib_ptr;	
	
	_dib_ptr = create_dib_buffer(width, height);
	clear_dib_buffer(_dib_ptr, t_color);
	return _dib_ptr;
}

/*---------------------------------------------------------------------------*/
void destroy_crop_dib_buffer(DIB_BUFFER *_dib_ptr)
{
	destroy_dib_buffer(_dib_ptr);
}

/*---------------------------------------------------------------------------*/
/*비트맵 버퍼로부터 일정영역을 이미지로 오려온다*/
/*---------------------------------------------------------------------------*/
IMAGE *crop_dib_buffer_image(DIB_BUFFER *_dib_ptr, 
	int x, int y, int width, int height)
{
	int y1;
	IMAGE *img_ptr;
	unsigned char *img_bits;
	
	img_ptr = _MEM_ALLOC(sizeof(IMAGE));
	img_ptr->width = width;
	img_ptr->height = height;
	img_ptr->bits = (unsigned char *)_MEM_ALLOC(width * height);
	img_bits = img_ptr->bits;
	for (y1 = 0; y1 < height; y1++)
	{		
		memcpy(img_bits, 
			(unsigned char *)_dib_ptr->bits + x + ((y + y1) * _dib_ptr->width), 
			width);
		img_bits += width;
	}
	
	return img_ptr;
}

/*---------------------------------------------------------------------------*/
/*비트맵 버퍼로부터 일정영역을 스프라이트로 오려온다*/
/*---------------------------------------------------------------------------*/
SPRITE *crop_dib_buffer_sprite(DIB_BUFFER *_dib_ptr, 
	int x, int y, int width, int height, unsigned char t_color)
{
	int x1, y1, i;
	struct PAK {
		unsigned short skip;
		unsigned short put;
	};
	struct PAK pak[256][256] = {{0,},};	
	int pak_num[256] = {0,};
	unsigned short skip, put;
	DWORD dwMemBytes = 0;
	unsigned char *dib_bits, *spr_bits, t;	
	SPRITE* spr_ptr;
	
	for (y1 = 0; y1 < height; y1++) {
		dib_bits = (unsigned char *)_dib_ptr->bits + x + ((y + y1) * _dib_ptr->width);
		pak_num[y1] = 0;
		x1 = 0;
		while (x1 < width) {
			skip = 0;
			put = 0;
			while ((t = dib_bits[x1]) == t_color && x1 < width) {
				skip++;
				x1++;
			}			
			while ((t = dib_bits[x1]) != t_color && x1 < width) {
				put++;
				x1++;
			}
			pak[y1][pak_num[y1]].skip = skip;
			pak[y1][pak_num[y1]].put = put;
			pak_num[y1]++;
		}
		dwMemBytes += sizeof(unsigned short);
		i = 0;
		while (i < pak_num[y1]) {
			dwMemBytes += sizeof(struct PAK);
			if (pak[y1][i].put) {
				dwMemBytes += pak[y1][i].put;				
			}			
			i++;
		}
   }
   
	spr_ptr = (SPRITE *)_MEM_ALLOC(sizeof(SPRITE));
   spr_ptr->width = width;
   spr_ptr->height = height;
   spr_ptr->bits = (unsigned char *)_MEM_ALLOC(dwMemBytes);
   spr_bits = spr_ptr->bits;
   for (y1 = 0; y1 < height; y1++) {
   	dib_bits = (unsigned char *)_dib_ptr->bits + x + ((y + y1) * _dib_ptr->width);
   	memcpy(spr_bits, &pak_num[y1], sizeof(unsigned short));
		spr_bits += sizeof(unsigned short);
		i = 0;
		while (i < pak_num[y1]) {
			memcpy(spr_bits, &pak[y1][i], sizeof(struct PAK));
			spr_bits += sizeof(struct PAK);
			if (pak[y1][i].put) {
				memcpy(spr_bits, dib_bits + pak[y1][i].skip, pak[y1][i].put);
				spr_bits += pak[y1][i].put;
			}
			dib_bits += pak[y1][i].skip + pak[y1][i].put;
			i++;
		}
   }

	return spr_ptr;
}

/*---------------------------------------------------------------------------*/
unsigned char *load_palette(char *fn, unsigned char *pal)
{
	HANDLE hFile;
	DWORD dwSize;	
	
	if ((hFile = _FILE_OPEN(fn, GENERIC_READ)) == INVALID_HANDLE_VALUE)
		return 0;
	if (!(_FILE_READ(hFile, pal, 768, &dwSize)) || dwSize != 768)	{
		_FILE_CLOSE(hFile);
		return 0;
	}
	_FILE_CLOSE(hFile);	

	return pal;
}

/*---------------------------------------------------------------------------*/
/*스프라이트를 읽어온다*/
/*로딩에 성공하면 spr->bits에 메모리를 할당한다*/
/*---------------------------------------------------------------------------*/
SPRITE *load_sprite(char *fn)
{
	HANDLE hFile;
	DWORD dwFileSize;
	struct SPRHDR {
		short width, height;
	} sprhdr;
	DWORD dwHdrLen;
	DWORD dwReadSize;
	DWORD dwPakSize;
	SPRITE *spr;

	if ((hFile = _FILE_OPEN(fn, GENERIC_READ)) == INVALID_HANDLE_VALUE)
		return 0;

	dwFileSize = GetFileSize(hFile, NULL);	
	dwHdrLen = sizeof(sprhdr);
	if (!(_FILE_READ(hFile, &sprhdr, dwHdrLen, &dwReadSize))
		|| dwReadSize != dwHdrLen) {
		_FILE_CLOSE(hFile);
		return 0;
	}

	spr = _MEM_ALLOC(sizeof(SPRITE));
	spr->width = sprhdr.width;
	spr->height = sprhdr.height;

	dwPakSize = dwFileSize - dwHdrLen;
	if (!(spr->bits = (LPBYTE)_MEM_ALLOC(dwPakSize))) {
		_FILE_CLOSE(hFile);
		return 0;
	}
	if (!(_FILE_READ(hFile, spr->bits, dwPakSize, &dwReadSize))
		|| dwReadSize != dwPakSize) {
		_MEM_FREE(spr->bits);
		_FILE_CLOSE(hFile);
		return 0;
	}
	_FILE_CLOSE(hFile);

	return spr;
}

/*---------------------------------------------------------------------------*/
/*스프라이트를 비트맵버퍼에 출력한다*/
/*---------------------------------------------------------------------------*/
void draw_sprite(DIB_BUFFER *_dib_ptr, SPRITE *spr, int x, int y)
{
	LPBYTE vbuf;
	LPWORD bits;
	unsigned short ys, y1, skip, pak_len, put_len;

	/*스프라이트를 그리기 시작할 비트맵버퍼의 위치를 구한다*/
	vbuf = (LPBYTE)(_dib_ptr->bits) + x + y * _dib_ptr->width;
	bits = (LPWORD)(spr->bits);	

	ys = spr->height;
	/*스프라이트의 높이만큼 반복*/	
	for (y1 = 0; y1 < ys; y1++, vbuf += _dib_ptr->width)	{
		/*PAK 갯수를 구한다*/
		pak_len = *bits++;
		skip = 0;
		/*PAK 갯수만큼 반복*/
		while (pak_len--) {
			/*투명색 길이를 얻는다*/
			skip += *bits++;
			/*불투명색 길이를 얻는다*/
			put_len = *bits++;
			/*
			비트맵버퍼의 시작으로부터 투명색 길이만큼 건너뛴다음
			출력색을 복사한다
			*/			
			memcpy(vbuf + skip, (LPBYTE)bits, put_len);
			bits = (LPWORD)((LPBYTE)bits + put_len);
			skip += put_len;
		}
	}
}

/*---------------------------------------------------------------------------*/
/*클리핑 처리해 스프라이트를 그린다*/
/*---------------------------------------------------------------------------*/
void draw_sprite_c(DIB_BUFFER *_dib_ptr, SPRITE *spr, int x, int y)
{	
	int y2;
	int cx1, cy1, cx2, cy2, y1;
	int x1, cx, t;
	unsigned short pak_len, put_len, c_put_len;
	LPBYTE vbuf;
	LPWORD bits, c_bits;

	y2 = y + spr->height;

	/*
	클리핑 경계를 벗어나지않으면 normal 드로잉한다
	*/
	if (x >= _dib_ptr->cx1 && y >= _dib_ptr->cy1 &&
		x + spr->width < _dib_ptr->cx2 && y2 < _dib_ptr->cy2) {
		draw_sprite(_dib_ptr, spr, x, y);
		return;
	}	

	cx1 = _dib_ptr->cx1;
	cy1 = _dib_ptr->cy1;
	cx2 = _dib_ptr->cx2;
	cy2 = _dib_ptr->cy2;

	/*클리핑 경계를 완전히 벗어나면 리턴*/
	if (x > cx2 || y > cy2 ||
		x + spr->width < cx1 || y + spr->height < cy1)
		return;
	if (y2 >= cy2)
		y2 = cy2;
		
	bits = (LPWORD)(spr->bits);

	/*y 높이만큼 반복*/
	for (y1 = y; y1 < y2; y1++) {
		/*클리핑 경계를 벗어나는 y좌표는 건너뛴다*/
		if (y1 < cy1) {
			pak_len = *bits++;
			while (pak_len--) {
				bits++;
				put_len = *bits++;
				bits = (LPWORD)((LPBYTE)bits + put_len);
			}
			continue;
		}

		vbuf = (LPBYTE)(_dib_ptr->bits) + y1 * _dib_ptr->width;
		pak_len = *bits++;
		x1 = x;
			
		while (pak_len--) {
			/*투명길이*/
			x1 += *bits++;
			/*불투명길이*/
			put_len = *bits++;
			/*불투명색상*/
			c_bits = bits;
			c_put_len = put_len;
			cx = x1;
			/*시작좌표가 클리핑 x경계를 벗어나지않으면*/
			if (cx < cx2 && cx + c_put_len > cx1)	{
				/*클리핑 x좌표보다 작은 경우*/
				if (cx < cx1) {
					t = cx1 - cx;
					c_bits = (LPWORD)((LPBYTE)c_bits + t);
					c_put_len -= t;
					cx = cx1;
				}				
				/*클리핑 x좌표보다 큰 경우*/
				if (cx + c_put_len >= cx2)
					c_put_len = cx2 - cx;
				memcpy(vbuf + cx, (LPBYTE)c_bits, c_put_len);
			}
			bits = (LPWORD)((LPBYTE)bits + put_len);
			x1 += put_len;
		}
	}
}

/*---------------------------------------------------------------------------*/
void destroy_sprite(SPRITE *spr)
{
	_MEM_FREE(spr->bits);
	_MEM_FREE(spr);
}

/*---------------------------------------------------------------------------*/
/*이미지를 읽어온다*/
/*로딩에 성공하면 img->bits에 메모리를 할당한다*/
/*---------------------------------------------------------------------------*/
IMAGE *load_image(char *fn)
{
	HANDLE hFile;
	DWORD dwFileSize;
	struct IMGHDR {
		short width, height;
	} imghdr;
	DWORD dwHdrLen;
	DWORD dwReadSize;
	DWORD dwPakSize;
	IMAGE *img; 

	if ((hFile = _FILE_OPEN(fn, GENERIC_READ)) == INVALID_HANDLE_VALUE)
		return 0;

	dwFileSize = GetFileSize(hFile, NULL);	
	dwHdrLen = sizeof(imghdr);	
	if (!(_FILE_READ(hFile, &imghdr, dwHdrLen, &dwReadSize))
		|| dwReadSize != dwHdrLen) {
		_FILE_CLOSE(hFile);
		return 0;
	}

	img = (IMAGE *)_MEM_ALLOC(sizeof(IMAGE));
	img->width = imghdr.width;
	img->height = imghdr.height;

	dwPakSize = dwFileSize - dwHdrLen;
	if (!(img->bits = (LPBYTE)_MEM_ALLOC(dwPakSize))) {
		_FILE_CLOSE(hFile);
		return 0;
	}
	if (!(_FILE_READ(hFile, img->bits, dwPakSize, &dwReadSize))
		|| dwReadSize != dwPakSize) {
		_MEM_FREE(img->bits);
		_FILE_CLOSE(hFile);
		return 0;
	}

	_FILE_CLOSE(hFile);

	return img;
}

/*---------------------------------------------------------------------------*/
/*이미지를 그린다*/
/*---------------------------------------------------------------------------*/
void draw_image(DIB_BUFFER *_dib_ptr, IMAGE *img, int x, int y)
{
	LPBYTE vbuf;
	LPWORD bits;
	unsigned short ys, y1, width;

	vbuf = (LPBYTE)(_dib_ptr->bits) + x + y * _dib_ptr->width;
	bits = (LPWORD)(img->bits);	

	width = img->width;
	ys = img->height;
	for (y1 = 0; y1 < ys; y1++, vbuf += _dib_ptr->width) {
		memcpy(vbuf, (LPBYTE)bits, width);
		bits = (LPWORD)((LPBYTE)bits + width);
	}
}

/*---------------------------------------------------------------------------*/
void draw_image_c(DIB_BUFFER *_dib_ptr, IMAGE *img, int x, int y)
{
}

/*---------------------------------------------------------------------------*/
void destroy_image(IMAGE *img)
{
	_MEM_FREE(img->bits);
	_MEM_FREE(img);
}

/*---------------------------------------------------------------------------*/
void draw_pixel(DIB_BUFFER *_dib_ptr, int x, int y, unsigned char c)
{
	memset((unsigned char *)_dib_ptr->bits + x + (y * _dib_ptr->width), c, 1);
}

/*---------------------------------------------------------------------------*/
void draw_hline(DIB_BUFFER *_dib_ptr, int x1, int x2, int y, unsigned char c)
{
	int x;
	for (x = x1; x <= x2; x++) PUT_PIXEL(_dib_ptr, x, y, c);
}

/*---------------------------------------------------------------------------*/
void draw_vline(DIB_BUFFER *_dib_ptr, int y1, int y2, int x, unsigned char c)
{
	int y;
	for (y = y1; y <= y2; y++) PUT_PIXEL(_dib_ptr, x, y, c);
}

/*---------------------------------------------------------------------------*/
void draw_line(DIB_BUFFER *_dib_ptr, int x1, int y1, int x2, int y2,
	unsigned char c)
{
	int x_delta = x2 - x1, y_delta = y2 - y1;
	int distance;
	int x_step, y_step;
	int x = 0, y = 0;
	int sx = x1, sy = y1;
	int i;

	if (x_delta > 0)
		x_step = 1;
	else
		x_step = -1;
	if (y_delta > 0)
		y_step = 1;
	else
		y_step=-1;

	x_delta = abs(x_delta);
	y_delta = abs(y_delta);

	if (x_delta > y_delta)
		distance = x_delta;
	else
		distance = y_delta;

	for (i = 0; i < distance; i++) {   
		PUT_PIXEL(_dib_ptr, sx, sy, c);
		x += x_delta;
		y += y_delta;
		if (x > distance)	{
			x -= distance;
			sx += x_step;
		}
		if (y > distance)	{
			y -= distance;
			sy += y_step;
		}
	}
}
/*---------------------------------------------------------------------------*/
void draw_text(HDC hdc, int x, int y, char *str, ...)
{
	char buf[256] = "";	

	va_list arg;
	va_start(arg, str);
	vsprintf(buf, str, arg);
	va_end(arg);
	
	SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
	TextOut(hdc, x, y, buf, strlen(buf));
}

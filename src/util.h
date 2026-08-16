#ifndef _UTIL_H_
#define _UTIL_H_

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
 
#ifdef __cplusplus
extern "C"
{
#endif

extern HGLOBAL __hMem__;

#define _MEM_ALLOC(SIZE) ((__hMem__ = GlobalAlloc(GHND, (DWORD)(SIZE))) ? GlobalLock(__hMem__) : NULL)
#define _MEM_REALLOC(HMEM, SIZE) ((__hMem__ = GlobalReAlloc(HMEM, (DWORD)(SIZE), GMEM_MOVEABLE)) ? GlobalLock(__hMem__) : NULL)
#define _MEM_FREE(PTR) (GlobalUnlock(GlobalHandle(PTR)),GlobalFree(GlobalHandle(PTR)))
#define _MEM_SIZE(PTR) (GlobalSize(GlobalHandle(PTR)))
#define _FILE_OPEN(FN, AM) (CreateFile(FN, AM, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
#define _FILE_READ(FH, DPTR, SIZE, SPTR) (ReadFile(FH, DPTR, SIZE, SPTR, NULL))
#define _FILE_CLOSE(FH) (CloseHandle(FH));

#define GET_KEY_STATE(VKEY) HIBYTE(GetAsyncKeyState(VKEY))
#define GET_KEY_PUSH(VKEY) LOBYTE(GetAsyncKeyState(VKEY))

void _CTRACE(char* f, ...);
void _FTRACE(char *fn, char *f, ...);

#ifdef __cplusplus
}
#endif

#endif

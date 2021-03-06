﻿#pragma once

#ifndef _COMP_LZS_H_
#define _COMP_LZS_H_

extern int	InitEncode( TCHAR *fname, char **dst, char **src );
extern int	LZS_EncodeMemory( char *dst, char *src, int file_size );
extern int	LZS_EncodeMemoryBmpTbl( char *dst, char *src, DWORD *tbl, int file_size, int key );
extern int  LZS_Encode(TCHAR *oldName, TCHAR *newName );
extern int  LZS_DecodeMemory( char *lpDst, char *lpSrc, int filesize, int dst_size);
extern char *LZS_Decode( TCHAR *filename );
extern char *LZS_Decode2( char *lpSrc, int filesize );

#endif


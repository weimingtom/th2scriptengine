#pragma once

#ifndef _PACMAIN_H_
#define _PACMAIN_H_

typedef struct {
	DWORD	type;		
	TCHAR	fname[24];	
	DWORD	offset;		
	DWORD	size;		
}FILE_INFO;

typedef struct {
	FILE_INFO	fi;
	int			pos;
}OPEN_FILE_INFO;

typedef struct {
	DWORD	head;		
	DWORD	famount;	
}PAC_FILE_INFO;

extern DWORD PAC_CheckFile( TCHAR *pname, TCHAR *fname );
extern DWORD PAC_LoadFile( TCHAR *pname, TCHAR *fname, char **buf );
extern DWORD PAC_AllFilecheck( TCHAR *pname );
extern int PAC_CheckPackFile( TCHAR *pname, TCHAR *fname );
extern int PAC_OpenPackFile( TCHAR *pname, TCHAR *fname, char **buf );
extern OPEN_FILE_INFO PAC_OpenFileHandle( TCHAR *pname, TCHAR *fname, HANDLE *fh );
extern void PAC_OpenFileHandle( HANDLE *fh );
extern DWORD PAC_SeekFileHandle( HANDLE *fh, OPEN_FILE_INFO *fi, int pos );
extern DWORD PAC_ReedFileHandle( HANDLE *fh, OPEN_FILE_INFO *fi, BYTE *buf, int size );

#endif


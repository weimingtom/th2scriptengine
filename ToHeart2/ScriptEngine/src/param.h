#pragma once

typedef struct{
	TCHAR str[1024];	
	long num;		
	long reg;		
}ESC_PARAM;

extern ESC_PARAM		EscParam[15];
extern int				EscCnt;

extern int GetScriptParamByte( int pc, int *start );
extern int GetScriptParamWord( int pc, int *start );
extern int GetScriptParam_str( int pc, int *start, TCHAR *buf );
extern int GetScriptParam_str2( int pc, int *start, TCHAR *buf );
extern int GetScriptParam( int pc, int *start,int size );
extern int GetScriptParamControl( int opr );
extern void SetScriptRegisterControl( int opr );

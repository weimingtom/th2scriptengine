#pragma once

#include "escript.h"

enum {
	CALL_SMALL = 0,
	CALL_ESMALL,
	CALL_LARGE,
	CALL_ELARGE,
	CALL_EQUAL,
	CALL_NEQUAL,
	CALL_ADD,
	CALL_SUB,
	CALL_MUL,
	CALL_DIV,
	CALL_MOD,
};

enum {
	OPRS_START = 0,
	OPRS_END,
	OPRS_MOVR,
	OPRS_MOVV,
	OPRS_SWAP,
	OPRS_RAND,
	OPRS_IFR,
	OPRS_IFV,
	OPRS_IFELSER,
	OPRS_IFELSEV,
	OPRS_LOOP,
	OPRS_GOTO,
	OPRS_INC,
	OPRS_DEC,
	OPRS_NOT,
	OPRS_NEG,
	OPRS_ADDR,
	OPRS_ADDV,
	OPRS_SUBR,
	OPRS_SUBV,
	OPRS_MULR,
	OPRS_MULV,
	OPRS_DIVR,
	OPRS_DIVV,
	OPRS_MODR,
	OPRS_MODV,
	OPRS_ANDR,
	OPRS_ANDV,
	OPRS_ORR,
	OPRS_ORV,
	OPRS_XORR,
	OPRS_XORV,
	OPRS_CALC,
	OPRS_PUSHA,
	OPRS_POPA,
	OPRS_CALL,
	OPRS_RET,
	OPRS_WAIT,
	OPRS_TWAIT,
	OPRS_RUN,
	OPRS_SLOAD,
	OPRS_TSTART,
	OPRS_OPREND,
};

typedef struct {		
	DWORD		BlockAddres[SCRIPT_BLOCK_MAX];		
	long		sp_buf[STACK_MAX];					
	long		reg[REGISTER_MAX];					
	DWORD		pc;									
	DWORD		sp;									
	DWORD		WaitCounter;						
	DWORD		WaitEnd;							
	DWORD		TWaitEnd;							
	char		*LangBuf;							
	char		BusyFlg;							
} EXEC_DATA;

extern BOOL		EXEC_ReadLang( char *filename, EXEC_DATA *scr );
extern BOOL		EXEC_StartLang( EXEC_DATA *scr, int scr_no );
extern void		EXEC_ReleaseLang( EXEC_DATA *scr );
extern void		EXEC_EndLang( EXEC_DATA *scr, int init );

extern short	EXEC_GetShortValue( char *buf );
extern long		EXEC_GetLongValue( char *buf );
extern int		EXEC_GetValue( char *buf, int mode );
extern int		EXEC_ControlLang( EXEC_DATA *scr );
extern int		EXEC_SLoad( TCHAR *str );
extern void		EXEC_OprCallFunc( int call_no, int next_pc, long *num );

extern	char		*EXEC_LangBuf;
extern	EXEC_DATA	*EXEC_LangInfo;

#define	EXEC_GetParam( N )		( EXEC_LangInfo->reg[ ( N ) ]         )
#define	EXEC_SetParam( N, V )	( EXEC_LangInfo->reg[ ( N ) ] = ( V ) )
#define	EXEC_AddPC( N )			( EXEC_LangInfo->pc += ( N )          )
#define	EXEC_GetPC				( EXEC_LangInfo->pc                   )
#define	EXEC_GetBusyFlag		( EXEC_LangInfo->BusyFlg              )

extern BOOL EXEC_OprControl(int opr);

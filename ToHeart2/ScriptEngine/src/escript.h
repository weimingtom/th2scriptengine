#pragma once 
#ifndef _ESCRIPT_H__
#define _ESCRIPT_H__

#define	ESC_OBJSADDR			0x80028000							
#define	ESC_OBJDSIZEMAX			0x100000							
#define ESC_FLAG_MAX			1024

#define	_MONTH			0	
#define	_DAY			1	
#define	_TIME			2	
#define	_EVENT_NEXT		3	
#define	_EVENT_END		4	
#define	_DEFAULT_NAME	5	
#define	_CALENDER_SKIP	6	
#define	_CLOCK_TIME		7
#define	_DEBUG_CALL		9	

#define ESC_SetFlag( N, D )		ESC_FlagBuf[ ( N ) ] = ( D )		
#define ESC_GetFlag( N )		ESC_FlagBuf[ ( N ) ]				

extern int  ESC_OprControl( int opr );
extern void ESC_InitFlag( void );
extern void ESC_InitEOprFlag( void );
extern int	ESC_FlagBuf[ ESC_FLAG_MAX ];
extern int	ESC_GameFlagBuf[ ESC_FLAG_MAX ];				
extern void SetVoiceDialog( int flag );
extern void AddVoiceDialogMes( char *mes, ...);
extern void ESC_SetDrawFlag( void );
extern void ESC_CodeCheck(void);

#define	SCRIPT_BLOCK_MAX		256

typedef struct {
	short			h1, h2;
	long			Fsize;
	unsigned long	BlockAddres[ SCRIPT_BLOCK_MAX ];
} SCRIPT_HEADER;

extern char ScrCodeList[64][32];
extern void EXEC_SetPackDir( char *pac_dir );

#define	MAIN_SCRIPT				0
#define SCCODE_NOOPR			0					
#define SCCODE_RUN				1					
#define SCCODE_WAIT_WAIT		2					
#define SCCODE_WAIT_TWAIT		3					
#define SCCODE_WAIT_SLOAD		4					
#define SCCODE_WAIT_DEBUG		0xFF				
#define REGISTER_MAX			50					
#define	STACK_MAX				512					
#define	TASK_MAX				8					

enum{												
	SCRMODE_EVENT,
	SCRMODE_MAX
};

enum {
	FACT_VAL = 0,
	FACT_REG,
	FACT_CLC,
	FACT_END,
};

extern	char		NowLangFileName[64];
extern	int			RunLangBlockNo;
extern	int			PrevRunLangBlockNo;


extern int GetScriptParamControlEx( int pc, int opr, TCHAR *str );
extern char	*PackScriptDir;

#endif 

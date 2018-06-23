#include <tchar.h>
#include <windows.h>
#include "exec.h"
#include "mm_std.h"
#include "escript.h"
#include "comp_pac.h"


//-------------------------------------------------------------

EXEC_DATA 	*EXEC_LangInfo;
char		*EXEC_LangBuf;

//-------------------------------------------------------------
static void EXEC_LangWait( void )
{
	if( EXEC_LangInfo->WaitCounter < EXEC_LangInfo->WaitEnd )	
		EXEC_LangInfo->WaitCounter++;
	else														
		EXEC_LangInfo->BusyFlg = SCCODE_RUN;
}

static void EXEC_LangTWait( void )
{	
	if( timeGetTime() >= EXEC_LangInfo->TWaitEnd )	
		EXEC_LangInfo->BusyFlg = SCCODE_RUN;
}

static BOOL EXEC_CallOprControl( int mode )
{
	int		opr;
	BOOL	ret=0;
	if(EXEC_LangBuf)
	{
		opr = *(WORD*)&EXEC_LangBuf[ EXEC_LangInfo->pc ];
		if( opr < OPRS_OPREND )
		{	
			ret = EXEC_OprControl( opr );	
		}			
		else
		{																
			switch( mode )
			{													
			case SCRMODE_EVENT:	
				ret = ESC_OprControl( opr );	
				break;	
			}
		}
	}
	return( ret );
}

static int EXEC_ControlTask( int mode )
{
	if( EXEC_LangInfo->BusyFlg == SCCODE_WAIT_TWAIT )		
		EXEC_LangTWait();
	if( EXEC_LangInfo->BusyFlg == SCCODE_WAIT_WAIT )		
		EXEC_LangWait();
	if( EXEC_LangInfo->BusyFlg == SCCODE_RUN )
	{			
		while( EXEC_CallOprControl( mode ) );
	}
	return( EXEC_LangInfo->BusyFlg );
}

//--------------------------------------------------------------

static void EXEC_SpPush( int num )
{
	EXEC_LangInfo->sp_buf[ EXEC_LangInfo->sp ] = num;
	EXEC_LangInfo->sp++;
}

static int EXEC_SpPop( void )
{	
	EXEC_LangInfo->sp--;
	return( EXEC_LangInfo->sp_buf[ EXEC_LangInfo->sp ] );
}

static void EXEC_Pusha( void )
{
	int i;	
	for(i = 0; i < REGISTER_MAX; i++) 
	{
		EXEC_SpPush(EXEC_LangInfo->reg[i]); 
	}
}

static void EXEC_Popa( void )
{
	int	i;
	for(i = REGISTER_MAX - 1; i >= 0; i--) 
	{ 
		EXEC_LangInfo->reg[i] = EXEC_SpPop(); 
	}
}

//---------------------------------------------------------------
// 2, 3
static void EXEC_OprMov( char mode )
{
	int param1, param2;
	param1 = EXEC_LangBuf[EXEC_LangInfo->pc + 2];							
	param2 = EXEC_GetValue(&EXEC_LangBuf[EXEC_LangInfo->pc + 3], mode);
	EXEC_LangInfo->reg[param1] = param2;
	if(mode == 0)	
		EXEC_AddPC(4);	
	else			
		EXEC_AddPC(7);	
}

// 4
static void EXEC_OprSwap( void )
{
	int	param1, param2, work;
	param1 = EXEC_LangBuf[EXEC_LangInfo->pc + 2];		
	param2 = EXEC_LangBuf[EXEC_LangInfo->pc + 3];
	work = EXEC_LangInfo->reg[param1];
	EXEC_LangInfo->reg[param1] = EXEC_LangInfo->reg[param2];
	EXEC_LangInfo->reg[param2] = work;
	EXEC_AddPC(4);
}

// 5
static void EXEC_OprRand( void )
{
	int param1;
	param1 = EXEC_LangBuf[EXEC_LangInfo->pc + 2];
	EXEC_LangInfo->reg[param1] = (rand() % 65535);	
	EXEC_AddPC(3);
}

// 6, 7
static void EXEC_OprIf( char mode )
{
	int		param1, param2, param3, p, flg;
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];							
	param1 = EXEC_GetParam( param1 );											
	param2 = EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ];							
	param3 = EXEC_GetValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 4 ], mode );	
	switch( param2 ) 
	{
	case CALL_SMALL:	
		flg = ( param1 <  param3 ) ? 1 : 0;		
		break;		
	
	case CALL_ESMALL:	
		flg = ( param1 <= param3 ) ? 1 : 0;		
		break;		
	
	case CALL_LARGE:	
		flg = ( param1 >  param3 ) ? 1 : 0;		
		break;		
	
	case CALL_ELARGE:	
		flg = ( param1 >= param3 ) ? 1 : 0;		
		break;		
	
	case CALL_EQUAL:	
		flg = ( param1 == param3 ) ? 1 : 0;		
		break;		
	
	case CALL_NEQUAL:	
		flg = ( param1 != param3 ) ? 1 : 0;		
		break;		
	}
	if( flg )
	{	
		p = ( mode == 0 ) ? 5 : 8;
		EXEC_LangInfo->pc = EXEC_GetLongValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + p ] );
	}
	else
	{		
		EXEC_AddPC( ( mode == 0 ) ? 9 : 12 );
	}
}

// 8, 9
static void EXEC_OprIfElse( char mode )
{
	int		param1, param2, param3, p;
	char	flg;
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];							
	param1 = EXEC_GetParam( param1 );											
	param2 = EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ];							
	param3 = EXEC_GetValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 4 ], mode );	
	switch( param2 ) 
	{
	case CALL_SMALL:	
		flg = ( param1 <  param3 ) ? 1 : 0;		
		break;		
	
	case CALL_ESMALL:	
		flg = ( param1 <= param3 ) ? 1 : 0;		
		break;		
	
	case CALL_LARGE:	
		flg = ( param1 >  param3 ) ? 1 : 0;		
		break;		
	
	case CALL_ELARGE:	
		flg = ( param1 >= param3 ) ? 1 : 0;		
		break;		
	
	case CALL_EQUAL:	
		flg = ( param1 == param3 ) ? 1 : 0;		
		break;		
	
	case CALL_NEQUAL:	
		flg = ( param1 != param3 ) ? 1 : 0;		
		break;		
	}
	if( flg )	
		p = ( mode == 0 ) ? 5 : 8;
	else		
		p = (( mode == 0 ) ? 5 : 8) + 4;
	EXEC_LangInfo->pc = EXEC_GetLongValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + p ] );
}

// 10
static void EXEC_OprLoop( void )
{
	int		param1;
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];
	if( EXEC_LangInfo->reg[ param1 ] > 0 ) 
	{			
		EXEC_LangInfo->reg[ param1 ]--;
		EXEC_LangInfo->pc = EXEC_GetLongValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ] );
	}
	else
	{
		EXEC_AddPC( 7 );
	}
}

// 11
static void EXEC_OprGoto( void  )
{	
	EXEC_LangInfo->pc = EXEC_GetLongValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ] );
}

// 12 - 15
static void EXEC_OprArithmetic1( int opr )
{
	int		param1;
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];
	switch( opr ) 
	{
	case OPRS_INC:	
		EXEC_LangInfo->reg[ param1 ] ++;								
		break;
	
	case OPRS_DEC:	
		EXEC_LangInfo->reg[ param1 ] --;								
		break;
	
	case OPRS_NOT:	
		EXEC_LangInfo->reg[ param1 ] = ~EXEC_LangInfo->reg[ param1 ];	
		break;

	case OPRS_NEG:	
		EXEC_LangInfo->reg[ param1 ] *= -1;							
		break;
	}	
	EXEC_AddPC( 3 );
}

// 16 - 31
static void EXEC_OprArithmetic2( int opr )
{
	int		param1, val1, val2 = 0, ans = 0, mode;
	param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];
	val1 = EXEC_GetValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ], 0 );
	switch( opr ) 
	{
	case OPRS_ADDR:	case OPRS_SUBR:	case OPRS_MULR:	case OPRS_DIVR:
	case OPRS_MODR:	case OPRS_ANDR:	case OPRS_ORR:	case OPRS_XORR:
		val2 = EXEC_GetValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ], 0 );
		mode = 0;
		break;

	case OPRS_ADDV:	case OPRS_SUBV:	case OPRS_MULV:	case OPRS_DIVV:
	case OPRS_MODV:	case OPRS_ANDV:	case OPRS_ORV:	case OPRS_XORV:
		val2 = EXEC_GetValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ], 1 );
		mode = 1;
		break;
	}
	switch( opr ) 
	{
	case OPRS_ADDR:	case OPRS_ADDV:	
		ans = val1 + val2;							
		break;
	
	case OPRS_SUBR:	case OPRS_SUBV:	
		ans = val1 - val2;							
		break;
	
	case OPRS_MULR:	case OPRS_MULV:	
		ans = val1 * val2;							
		break;
	
	case OPRS_DIVR:	case OPRS_DIVV:	
		ans = ( val2 != 0 ) ? ( val1 / val2 ) : 0;	
		break;
	
	case OPRS_MODR:	case OPRS_MODV:	
		ans = ( val2 != 0 ) ? ( val1 % val2 ) : 0;	
		break;
	
	case OPRS_ANDR:	case OPRS_ANDV:	
		ans = val1 & val2;							
		break;
	
	case OPRS_ORR:	case OPRS_ORV:	
		ans = val1 | val2;							
		break;
	
	case OPRS_XORR:	case OPRS_XORV:	
		ans = val1 ^ val2;							
		break;
	}
	EXEC_LangInfo->reg[ param1 ] = ans;	
	EXEC_AddPC( ( mode == 0 ) ? 4 : 7 );
}

// 32
static void EXEC_OprCalc( void )
{
	short	size;
	int		reg, bp, sbp, s1, s2, ans, cnt;
	struct CALC{
		char	opr;
		long	data;
	} Calc[ 250 ];
	reg  = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];
	size = EXEC_GetShortValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 3 ] );
	bp = sbp = 0;
	while( 1 ) 
	{
		if( sbp >= size ) 
		{ 
			break; 
		}
		if( EXEC_LangBuf[ EXEC_LangInfo->pc + 5 + sbp ] == FACT_VAL ) 
		{
			Calc[ bp ].opr = EXEC_LangBuf[ EXEC_LangInfo->pc + 5 + sbp ];
			Calc[ bp ].data = EXEC_GetLongValue( &EXEC_LangBuf[ EXEC_LangInfo->pc+3+sbp+1 ] );
			bp++;	sbp += 5;
		}
		else {
			Calc[ bp ].opr = EXEC_LangBuf[ EXEC_LangInfo->pc + 5 + sbp ];
			Calc[ bp ].data = EXEC_LangBuf[ EXEC_LangInfo->pc + 5 + sbp + 1 ];
			bp++;	sbp += 2;
		}
	}
	Calc[ bp ].opr = FACT_END;
	bp = 0;
	while( Calc[ bp ].opr != FACT_END ) 
	{
		if( Calc[ bp ].opr != FACT_CLC ) 
		{ 
			bp++; 
			continue; 
		}
		if( Calc[ bp - 2 ].opr == FACT_VAL ) 
		{ 
			s1 = Calc[ bp - 2 ].data;                        
		}
		else                                  
		{ 
			s1 = EXEC_LangInfo->reg[ Calc[ bp - 2 ].data ]; 
		}
		if( Calc[ bp - 1 ].opr == FACT_VAL ) 
		{ 
			s2 = Calc[ bp - 1 ].data;                        
		}
		else                                  
		{ 
			s2 = EXEC_LangInfo->reg[ Calc[ bp - 2 ].data ]; 
		}
		switch( Calc[ bp ].data ) 
		{
		case CALL_ADD:	
			ans = s1 + s2;							
			break;
		
		case CALL_SUB:	
			ans = s1 - s2;							
			break;
		
		case CALL_MUL:	
			ans = s1 * s2;							
			break;
		
		case CALL_DIV:	
			ans = ( s2 != 0 ) ? ( s1 / s2 ) : 0;	
			break;
		
		case CALL_MOD:	
			ans = ( s2 != 0 ) ? ( s1 % s2 ) : 0;	
			break;
		}
		Calc[ bp ].opr = FACT_VAL;
		Calc[ bp ].data = ans;
		for( cnt=0; Calc[ bp + cnt ].opr != FACT_END; cnt++ ) 
		{
			Calc[ bp + cnt - 2 ].opr = Calc[ bp + cnt ].opr;
			Calc[ bp + cnt - 2 ].data = Calc[ bp + cnt ].data;
		}
		Calc[ bp + cnt - 2 ].opr = FACT_END;
		bp -= 2;
	}
	EXEC_SetParam( reg, Calc[ 0 ].data );
	EXEC_AddPC( 5 + size );
}

// 33
static void EXEC_OprPusha( void )
{
	EXEC_Pusha();	
	EXEC_AddPC(2);
}

// 34
static void EXEC_OprPopa( void )
{
	EXEC_Popa();	
	EXEC_AddPC(2);
}

// 35
static void EXEC_OprCall( void )
{
	int			callno;
	DWORD		addres;
	EXEC_SpPush( EXEC_LangInfo->pc + 3 );
	callno = (char)EXEC_LangBuf[ EXEC_LangInfo->pc+2 ];
	if( EXEC_LangInfo->BlockAddres[ callno ] == 0 )
	{
		DebugBox( NULL, "Lang Call Error : LangBlock(%d) Code None", callno );
		return;
	}
	addres = EXEC_LangInfo->BlockAddres[ callno ] - 1;
	PrevRunLangBlockNo = RunLangBlockNo;
	RunLangBlockNo = callno;
	EXEC_LangInfo->pc = addres;
}

// 36
static void EXEC_OprRet( void )
{	
	EXEC_LangInfo->pc = EXEC_SpPop();
	RunLangBlockNo = PrevRunLangBlockNo;
}

// 37
static void EXEC_OprWait( void )
{
	int		param1;
	param1 = EXEC_GetShortValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ] );
	EXEC_LangInfo->WaitCounter = 0;					
	EXEC_LangInfo->WaitEnd     = param1;				
	EXEC_LangInfo->BusyFlg     = SCCODE_WAIT_WAIT;	
	EXEC_AddPC( 4 );
}

// 38
static void EXEC_OprTWait( void )
{
	int		param1;
	param1 = EXEC_GetShortValue( &EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ] );
	EXEC_LangInfo->TWaitEnd   = timeGetTime()+(DWORD)param1;	
 	EXEC_LangInfo->BusyFlg    = SCCODE_WAIT_TWAIT;				
	EXEC_AddPC( 4 );
}

// 39
static void EXEC_OprRun( void )
{
	ESC_SetDrawFlag();
	EXEC_AddPC( 2 );
}

// 40
static void EXEC_OprSLoad( void )
{
	int		i, len;
	char	str[256];
	len = EXEC_LangBuf[ EXEC_LangInfo->pc + 2 ];						
	for( i=0; i<len; i++ ) 
	{ 
		str[ i ] = EXEC_LangBuf[ EXEC_LangInfo->pc + 3 + i ]; 
	}
	str[i] = '\0';	
	str[i+1] = '\0';
	EXEC_SLoad( str );
}

//----------------------------------------------------------------

// 1
void EXEC_EndLang( EXEC_DATA *scr, int init )
{
	scr->BusyFlg   = SCCODE_NOOPR;
	scr->pc        = 0;
	if(!init)
	{
		if( scr->sp != 0 )
			DebugBox( NULL, "Lang Error : Stack Pointer Abnormal" );
	}
	RunLangBlockNo = -1;
}

BOOL EXEC_OprControl( int opr )
{
	BOOL	ret;	
	switch( opr ) 
	{
		case OPRS_END:		
			EXEC_EndLang( EXEC_LangInfo, OFF);	
			ret = FALSE;	
			break;	
		
		case OPRS_MOVR:		
			EXEC_OprMov( 0 );					
			ret = TRUE;		
			break;	
		
		case OPRS_MOVV:		
			EXEC_OprMov( 1 );					
			ret = TRUE;		
			break;	
		
		case OPRS_SWAP:		
			EXEC_OprSwap();						
			ret = TRUE;		
			break;	
		
		case OPRS_RAND:		
			EXEC_OprRand();						
			ret = TRUE;		
			break;	
		
		case OPRS_IFR:		
			EXEC_OprIf( 0 );						
			ret = TRUE;		
			break;	
		
		case OPRS_IFV:		
			EXEC_OprIf( 1 );						
			ret = TRUE;		
			break;	
		
		case OPRS_IFELSER:	
			EXEC_OprIfElse( 0 );					
			ret = TRUE;		
			break;	
		
		case OPRS_IFELSEV:	
			EXEC_OprIfElse( 1 );					
			ret = TRUE;		
			break;	
		
		case OPRS_LOOP:		
			EXEC_OprLoop();						
			ret = TRUE;		
			break;	
		
		case OPRS_GOTO:		
			EXEC_OprGoto();						
			ret = TRUE;		
			break;	

		case OPRS_INC:     																
		case OPRS_DEC:     																
		case OPRS_NOT:     																
		case OPRS_NEG:		
			EXEC_OprArithmetic1( opr );			
			ret = TRUE;		
			break;	

		case OPRS_ADDR:	case OPRS_ADDV:													
		case OPRS_SUBR:	case OPRS_SUBV:													
		case OPRS_MULR:	case OPRS_MULV:													
		case OPRS_DIVR:	case OPRS_DIVV:													
		case OPRS_MODR:	case OPRS_MODV:													
		case OPRS_ANDR:	case OPRS_ANDV:													
		case OPRS_ORR:	case OPRS_ORV:													
		case OPRS_XORR:	case OPRS_XORV:													
			EXEC_OprArithmetic2( opr );			
			ret = TRUE;		
			break;	
		
		case OPRS_CALC:		
			EXEC_OprCalc();						
			ret = TRUE;		
			break;	
		
		case OPRS_PUSHA:	
			EXEC_OprPusha();						
			ret = TRUE;		
			break;	
		
		case OPRS_POPA:		
			EXEC_OprPopa();						
			ret = TRUE;		
			break;	
		
		case OPRS_CALL:		
			EXEC_OprCall();						
			ret = TRUE;		
			break;	
		
		case OPRS_RET:		
			EXEC_OprRet();						
			ret = TRUE;		
			break;	
		
		case OPRS_WAIT:		
			EXEC_OprWait();						
			ret = FALSE;	
			break;	
		
		case OPRS_TWAIT:	
			EXEC_OprTWait();						
			ret = FALSE;	
			break;	
		
		case OPRS_RUN:		
			EXEC_OprRun();						
			ret = FALSE;	
			break;	
		
		case OPRS_SLOAD:	
			EXEC_OprSLoad();						
			ret = FALSE;	
			break;	
	}
	return(	ret );
}

//-------------------------------------------------------------

BOOL EXEC_ReadLang( char *filename, EXEC_DATA *scr )
{
	SCRIPT_HEADER	*header;
	int				i, size;
	char			*buf=NULL;
	size = PAC_LoadFile( PackScriptDir, filename, &buf );
	if(size==0)
	{
		return FALSE;
	}
	header = (SCRIPT_HEADER*)buf;
	if( ( header->h1 != 'L' ) || ( header->h2 != 'F' ) ) 
	{
		DebugBox( NULL, "スクリプトファイルではありません。" );
		GFree( buf );
		return FALSE;
	}
	size = header->Fsize - sizeof( SCRIPT_HEADER );
	GFree(scr->LangBuf);
	scr->LangBuf = (char *)GAlloc( size );
	CopyMemory( scr->LangBuf, buf + sizeof(SCRIPT_HEADER), size );
	for( i=0; i<SCRIPT_BLOCK_MAX; i++ ) 
	{
		scr->BlockAddres[i] = header->BlockAddres[i];
	}
	scr->BusyFlg  = SCCODE_NOOPR;								
	wsprintf( NowLangFileName, "%s", filename );
	GFree( buf );
	return( TRUE );
}

BOOL EXEC_StartLang( EXEC_DATA *scr, int scr_no )
{
	int		i;
	if( scr->BlockAddres[ scr_no ] == 0 ) 
	{
		return( FALSE );
	}
	if( (scr->BusyFlg != SCCODE_NOOPR) && (scr->BusyFlg != SCCODE_WAIT_SLOAD) ) 
	{
		return( FALSE );
	}
	scr->BusyFlg = SCCODE_RUN;									
	scr->sp      = 0;											
	scr->pc      = scr->BlockAddres[ scr_no ] - 1;				
	for( i=0; i<REGISTER_MAX; i++ ) 
	{
		scr->reg[ i ] = 0;
	}
	RunLangBlockNo = scr_no;
	return( TRUE );
}

void EXEC_ReleaseLang( EXEC_DATA *scr )
{
	int	i;
	GFree( scr->LangBuf );
	scr->BusyFlg   = SCCODE_NOOPR;
	scr->pc        = 0;
	scr->LangBuf = NULL;
	scr->sp      = 0;										
	for( i=0; i<STACK_MAX; i++ ) 
	{
		scr->sp_buf[ i ] = 0;
	}	
	for( i=0; i<REGISTER_MAX; i++ ) 
	{
		scr->reg[ i ] = 0; 
	}		
	RunLangBlockNo = -1;
}

int EXEC_ControlLang( EXEC_DATA *scr )
{
	int		ret;
	EXEC_LangBuf  = (char *)scr->LangBuf;
	EXEC_LangInfo = scr;
	ret = EXEC_ControlTask( SCRMODE_EVENT );
	return( ret );
}

//-------------------------------------------------------------

short EXEC_GetShortValue( char *buf )
{
	int		i;
	union {
		char	byte[ 2 ];
		WORD	word;
	}val;
	for( i=0; i<2; i++ ) 
	{ 
		val.byte[ i ] = buf[ i ]; 
	}	
	return(	val.word );
}

long EXEC_GetLongValue( char *buf )
{
	long	i;
	union {
		char byte[ 4 ];
		long dword;
	}val;
	for( i=0; i<4; i++ ) 
	{ 
		val.byte[ i ] = buf[ i ]; 
	}
	return(	val.dword );
}

int EXEC_GetValue( char *buf, int mode )
{
	int 	value;
	if( mode == 0 )	
		value = EXEC_LangInfo->reg[ *buf ];	
	else			
		value = EXEC_GetLongValue( buf );		
	return( value );
}

//-------------------------------------------------------------

void EXEC_OprCallFunc( int call_no, int next_pc, long *num )
{
	DWORD	addres;
	int		i;
	EXEC_SpPush( EXEC_LangInfo->pc + next_pc );
	for( i=0; i<14 ; i++ )
	{
		EXEC_LangInfo->reg[i] = num[i];
	}
	if( EXEC_LangInfo->BlockAddres[ call_no ] == 0 )
	{
		DebugBox( NULL, "Lang Call Error : LangBlock(%d) Code None", call_no );
		return;
	}
	addres = EXEC_LangInfo->BlockAddres[ call_no ] - 1;
	PrevRunLangBlockNo = RunLangBlockNo;
	RunLangBlockNo = call_no;	
	EXEC_LangInfo->pc = addres;
}

int EXEC_SLoad( TCHAR *str )
{
	char	buf[256];
	char	src[MAX_PATH],dst[MAX_PATH];
	wsprintf( dst, "sdt\\buf\\%s", str );
	wsprintf( src, "sdt\\%s", str );
	CopyFile( src, dst, TRUE);
	wsprintf( buf, "%s\\%s", PackScriptDir, str );
	if( PAC_CheckFile( PackScriptDir, str ) )
	{
		EXEC_ReleaseLang( EXEC_LangInfo );
		EXEC_ReadLang( str, EXEC_LangInfo );
		EXEC_LangInfo->BusyFlg = SCCODE_WAIT_SLOAD;
		EXEC_StartLang( EXEC_LangInfo, 0 );
		return 1;
	}
	else
	{
		DebugBox( NULL, "スクリプトファイルがありません[%s\\%s]", PackScriptDir, str );		
		EXEC_EndLang( EXEC_LangInfo, 1 );
		return 0;
	}
}

//-------------------------------------------------------------

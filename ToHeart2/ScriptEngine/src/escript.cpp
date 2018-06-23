﻿#ifdef _MSC_VER
#pragma warning(disable:4996)
#pragma warning(disable:4101)
#endif

#include <tchar.h>
#include "mm_std.h"
#include "main.h"
#include "escript.h"
#include "escr.h" //#include "..\\mes\\escr.h"
#include "draw.h"
#include "disp.h"
#include "gm_avg.h"
#include "gm_save.h"
#include "param.h"
#include "exec.h"
#include <math.h>

void ESC_SetDrawFlag(void)
{
	MainWindow.draw_flag=ON;
}

int	ESC_FlagBuf[ ESC_FLAG_MAX ];				
int	ESC_GameFlagBuf[ ESC_FLAG_MAX ];

int GetScriptParamControlEx( int pc, int opr, TCHAR *str )
{
	int		i;
	int		cnt = 2,flag=0;
	TCHAR	buf[32];
	if(opr < 64)	
		DebugPrintf(_T("イベントスクリプトの命令コードとして不適です[%d]"), opr);
	opr -= 64;
	for(i=0;i<15;i++)
	{
		flag = 1;
		switch( EScroptOpr[opr].type[i] )
		{
			default:	
				DebugPrintf(_T("引数タイプが不適です[%d]"), EScroptOpr[opr].type[i]);

			case ESC_NOT:	
				return cnt;												
			
			case ESC_REG:	
				EscParam[i].num = GetScriptParamByte( pc, &cnt );	
				wsprintf( buf, _T("REG%d, "), EscParam[i].num); 	
				break;	
			
			case ESC_ADD:	
				EscParam[i].num = GetScriptParamByte( pc, &cnt );	
				wsprintf( buf, _T("ADD(%d), "), EscParam[i].num); 	
				break;	

			case ESC_ASC:	
				EscParam[i].num = GetScriptParamByte( pc, &cnt );	
				wsprintf( buf, _T("%c, "), EscParam[i].num);	
				break;
			
			case ESC_CNT:	
				EscParam[i].num = GetScriptParamWord( pc, &cnt );	
				wsprintf( buf, _T("CNT(%d), "), EscParam[i].num);	
				break;	

			case ESC_VCNT:	
				EscParam[i].num = GetScriptParamWord( pc, &cnt );	
				wsprintf( buf, _T("VCNT(%d), "), EscParam[i].num);		
				break;	

			case ESC_NUM:	
				EscParam[i].num = GetScriptParam( pc, &cnt, 4 );	
				wsprintf( buf, _T("%d, "), EscParam[i].num); 	
				break;	
			
			case ESC_STR:	
				GetScriptParam_str( pc, &cnt, EscParam[i].str );	
				strcat(str, EscParam[i].str ); 
				strcat(str, _T(", ") ); 
				flag = 0;
				break;

			case ESC_STR2:	
				GetScriptParam_str2( pc, &cnt, EscParam[i].str );	
				strcat(str, EscParam[i].str ); 
				strcat(str, _T(", ") ); 
				flag = 0;
				break;	
			
			case ESC_CMP:
				{
					int	param1, param2, param3;
					param1 = EXEC_LangBuf[ pc + cnt++ ];	
					param1 = EXEC_GetParam( param1 );						
					param2 = EXEC_LangBuf[ pc + cnt++ ];	
					param3 = GetScriptParam( pc, &cnt, 4 );
					switch( param2 ) 
					{
					case CALL_SMALL:	
						EscParam[i].num = (param1 < param3) ? 1 : 0;	
						wsprintf( buf, _T("REG%d < %d, "), param1, param3 );	
						break;
					
					case CALL_ESMALL:	
						EscParam[i].num = (param1 <= param3) ? 1 : 0;	
						wsprintf( buf, _T("REG%d <= %d, "), param1, param3 );	
						break;
					
					case CALL_LARGE:	
						EscParam[i].num = ( param1 >  param3 ) ? 1 : 0;	
						wsprintf( buf, _T("REG%d > %d, "), param1, param3 );	
						break;
					
					case CALL_ELARGE:	
						EscParam[i].num = ( param1 >= param3 ) ? 1 : 0;	
						wsprintf(buf, _T("REG%d >= %d, "), param1, param3 );	
						break;
					
					case CALL_EQUAL:	
						EscParam[i].num = ( param1 == param3 ) ? 1 : 0;	
						wsprintf( buf, _T("REG%d == %d, "), param1, param3 );	
						break;		
					
					case CALL_NEQUAL:
						EscParam[i].num = ( param1 != param3 ) ? 1 : 0;	
						wsprintf( buf, _T("REG%d != %d, "), param1, param3 );	
						break;		
					}
				}
				break;	
		}
		if(flag)
		{
			strcat( str, buf );
		}
	}
	return cnt;
}

void ESC_InitFlag( void )
{
	int	i;
	for(i = 0; i < ESC_FLAG_MAX; i++)
	{
		ESC_FlagBuf[i] = 0;
	}
}

void ESC_InitGameFlag( void )
{
	int	i;
	for(i = 0; i < ESC_FLAG_MAX; i++)
	{
		ESC_GameFlagBuf[i] = 0;
	}
}

static char	EOprFlag[ESC_OPR_MAX];
void ESC_InitEOprFlag( void )
{
	int	i;
	for(i = 0; i < ESC_OPR_MAX; i++)
	{
		EOprFlag[i] = 0;
	}
}

int LoadScript( TCHAR *fno )
{
	TCHAR str[256];
	int	ret;
	wsprintf(str, _T("%s.sdt"), fno);
	int	i;
	for(i = 0;i < MAX_SCRIPT_OBJ; i++)
	{
		if(SpriteBmp[i].flag)
		{
			DebugBox( NULL, _T("SetBmpの解除し忘れ(%d)%s"), i, SpriteBmp[i].fname );
			AVG_ReleaseBmp(i);
		}
	}
	ret = EXEC_SLoad(str);
	if(ret)
	{
		AVG_SetScenarioNo(GetScenarioNo(str));
	}
	return ret;
}

static void LoadScriptNum(int fno)
{
	TCHAR str[256];
	wsprintf(str, _T("%05d.sdt"), fno);	
	EXEC_SLoad(str);
	AVG_SetScenarioNo(fno);
}

static void ESC_EOprWaitBgmFade(void)
{	
	if( AVG_WaitBGM() )
	{		
		EXEC_AddPC( EscCnt );
	}
}

// 64
static void ESC_EOprB( void )
{
	int	bak_no=-1;	
	if(EscParam[1].num!=-1) 
		bak_no = EscParam[1].num*10;					
	if(EscParam[2].num!=-1) 
		bak_no += EscParam[2].num;	
	if(EscParam[3].num==-1) 
		EscParam[3].num = -2;	
	if(EscParam[4].num==-1) 
		EscParam[4].num = 0;	
	if(EscParam[5].num==-1) 
		EscParam[5].num = 0;	
	if(EscParam[6].num==-1) 
		EscParam[6].num = 128;	
	if(EOprFlag[ESC_B]==0)
	{
		EOprFlag[ESC_B] = 1;
		AVG_ResetBackHalfTone( bak_no, EscParam[0].num );
	}
	else if(EOprFlag[ESC_B]==1)
	{
		EOprFlag[ESC_B] = 2;
		AVG_SetBack( bak_no, EscParam[4].num, EscParam[5].num, EscParam[0].num, OFF, 0, EscParam[3].num, EscParam[6].num );
	}
	if( !AVG_WaitBack() && EOprFlag[ESC_B]==2 )
	{
		EOprFlag[ESC_B] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 65
static void ESC_EOprBT( void )
{
	int	bak_no;	
	if(EOprFlag[ESC_BT]==0)
	{
		EOprFlag[ESC_BT] = 1;	
		bak_no = EscParam[1].num*10;					
		if(EscParam[2].num!=-1) 
			bak_no += EscParam[2].num;	
		if(EscParam[3].num==-1) 
			EscParam[3].num = -2;	
		if(EscParam[4].num==-1) 
			EscParam[4].num = 0;	
		if(EscParam[5].num==-1) 
			EscParam[5].num = 0;	
		if(EscParam[6].num==-1) 
			EscParam[6].num = 128;	
		AVG_ResetBackHalfTone( bak_no, EscParam[0].num );
		AVG_SetBack( bak_no, EscParam[4].num, EscParam[5].num, EscParam[0].num, OFF, 0, EscParam[3].num, EscParam[6].num );
	}
	if( !AVG_WaitBack() )
	{
		EOprFlag[ESC_BT] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 66
static void ESC_EOprBC( void )
{
	int	bak_no;
	bak_no = EscParam[1].num*10;
	if(EscParam[2].num!=-1) 
		bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) 
		EscParam[3].num = -2;
	if(EscParam[4].num==-1) 
		EscParam[4].num = 0;	
	if(EscParam[5].num==-1) 
		EscParam[5].num = 0;	
	if(EscParam[6].num==-1) 
		EscParam[6].num = 128;	
	if(EOprFlag[ESC_BC]==0)
	{
		EOprFlag[ESC_BC] = 1;
		AVG_ResetBackHalfTone( bak_no, EscParam[0].num );
	}
	else if(EOprFlag[ESC_BC]==1)
	{
		EOprFlag[ESC_BC] = 2;
		AVG_SetBack( bak_no, EscParam[4].num, EscParam[5].num, EscParam[0].num, ON, 0, EscParam[3].num, EscParam[6].num );
	}
	if( !AVG_WaitBack() && EOprFlag[ESC_BC]==2 )
	{
		EOprFlag[ESC_BC] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 67
static void ESC_EOprBCT( void )
{
	int	bak_no;
	if(EOprFlag[ESC_BCT]==0)
	{
		EOprFlag[ESC_BCT] = 1;
		bak_no = EscParam[1].num*10;
		if(EscParam[2].num!=-1) 
			bak_no += EscParam[2].num;
		if(EscParam[3].num==-1) 
			EscParam[3].num = -2;
		if(EscParam[4].num==-1) 
			EscParam[4].num = 0;	
		if(EscParam[5].num==-1) 
			EscParam[5].num = 0;	
		if(EscParam[6].num==-1) 
			EscParam[6].num = 128;	
		AVG_ResetBackHalfTone( bak_no, EscParam[0].num );
		AVG_SetBack( bak_no, EscParam[4].num, EscParam[5].num, EscParam[0].num, ON, 0, EscParam[3].num, EscParam[6].num );
	}
	if( !AVG_WaitBack() )
	{
		EOprFlag[ESC_BCT] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 68
static void ESC_EOprBD( void )
{
	AVG_SetBack_Delete();
	EXEC_AddPC( EscCnt );
}

// 69
static void ESC_EOprBR( void )
{
	int	bak_no;
	bak_no = EscParam[1].num*10;
	if(EscParam[2].num!=-1) 
		bak_no += EscParam[2].num;
	if(EOprFlag[ESC_BR]==0)
	{
		EOprFlag[ESC_BR] = 1;
		AVG_ResetBackHalfTone( bak_no, EscParam[0].num );
	}
	else if(EOprFlag[ESC_BR]==1)
	{
		EOprFlag[ESC_BR] = 2;	
		AVG_SetBack( bak_no, 0, 0, EscParam[0].num, OFF, 0, -2 );
	}
	if( !AVG_WaitBack() && EOprFlag[ESC_BR]==2 )
	{
		EOprFlag[ESC_BR] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 70
static void ESC_EOprBF( void )
{
	int	bak_no;
	bak_no = EscParam[1].num*10;
	if(EscParam[2].num!=-1) 
		bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) 
		EscParam[3].num = -2;
	if(EOprFlag[ESC_BF]==0)
	{
		EOprFlag[ESC_BF] = 1;
		AVG_ResetBackHalfTone( bak_no, EscParam[0].num );
	}
	else if(EOprFlag[ESC_BF]==1)
	{
		EOprFlag[ESC_BF] = 2;
		AVG_SetBack( bak_no, 0, 0, EscParam[0].num, ON, 0, EscParam[3].num );
	}
	if( !AVG_WaitBack() && EOprFlag[ESC_BF]==2 )
	{
		EOprFlag[ESC_BF] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 71
static void ESC_EOprV( void )
{
	int	bak_no;
	bak_no = EscParam[1].num*10;
	if(EscParam[2].num!=-1) 
		bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) 
		EscParam[3].num = -2;
	if(EscParam[4].num==-1) 
		EscParam[4].num = OFF;
	if(EscParam[5].num==-1) 
		EscParam[5].num = 0;
	if(EscParam[6].num==-1) 
		EscParam[6].num = 0;
	if(EscParam[7].num==-1) 
		EscParam[7].num = 128;	
	if(EOprFlag[ESC_V]==0)
	{
		EOprFlag[ESC_V] = 1;
		AVG_ResetBackHalfTone( bak_no, EscParam[0].num );
	}
	else if(EOprFlag[ESC_V]==1)
	{
		EOprFlag[ESC_V] = 2;	
		AVG_SetBack( bak_no, EscParam[5].num, EscParam[6].num, EscParam[0].num, EscParam[4].num, 1, EscParam[3].num, EscParam[7].num );
	}
	if( !AVG_WaitBack() && EOprFlag[ESC_V]==2 )
	{
		EOprFlag[ESC_V] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 72
static void ESC_EOprH( void )
{
	int	bak_no;
	bak_no = EscParam[1].num*10;
	if(EscParam[2].num!=-1) 
		bak_no += EscParam[2].num;
	if(EscParam[3].num==-1) 
		EscParam[3].num = -2;
	if(EscParam[4].num==-1) 
		EscParam[4].num = OFF;
	if(EscParam[5].num==-1) 
		EscParam[5].num = 0;
	if(EscParam[6].num==-1) 
		EscParam[6].num = 0;
	if(EscParam[7].num==-1) 
		EscParam[7].num = 128;	
	if(EOprFlag[ESC_H]==0)
	{
		EOprFlag[ESC_H] = 1;
		AVG_ResetBackHalfTone( bak_no, EscParam[0].num );
	}
	else if(EOprFlag[ESC_H]==1)
	{
		EOprFlag[ESC_H] = 2;
		AVG_SetBack( bak_no, EscParam[5].num, EscParam[6].num, EscParam[0].num, EscParam[4].num, 2, EscParam[3].num, EscParam[7].num );
	}
	if( !AVG_WaitBack() && EOprFlag[ESC_H]==2 )
	{
		EOprFlag[ESC_H] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 73
static void ESC_EOprS( void )
{
	if(!EOprFlag[ESC_S])
	{
		EOprFlag[ESC_S] = 1;
		AVG_SetBackScroll( EscParam[0].num, EscParam[1].num, DISP_X, DISP_Y, EscParam[2].num, EscParam[3].num );
	}	
	if( !AVG_WaitBackScroll() )
	{
		EOprFlag[ESC_S] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 74
static void ESC_EOprZ( void )
{
	if(!EOprFlag[ESC_Z])
	{
		EOprFlag[ESC_Z] = 1;
		AVG_SetBackScroll( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num+3 );
	}
	if( !AVG_WaitBackScroll() )
	{
		EOprFlag[ESC_Z] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 75
static void ESC_EOprFI( void )
{
	if(!EOprFlag[ESC_FI])
	{
		EOprFlag[ESC_FI] = 1;
		AVG_SetBackFade( 128, 128, 128, -1 );
	}
	if( !AVG_WaitBackFade() )
	{	
		EOprFlag[ESC_FI] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 76
static void ESC_EOprFIF( void )
{
	if(!EOprFlag[ESC_FIF])
	{
		EOprFlag[ESC_FIF] = 1;
		AVG_SetBackFade( 128, 128, 128, EscParam[0].num );
	}
	if( !AVG_WaitBackFade() )
	{	
		EOprFlag[ESC_FIF] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 77
static void ESC_EOprFO( void )
{
	if(!EOprFlag[ESC_FO])
	{
		EOprFlag[ESC_FO] = 1;
		AVG_SetBackFade( 0, 0, 0, -1 );
	}
	if( !AVG_WaitBackFade() )
	{	
		EOprFlag[ESC_FO] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 78
static void ESC_EOprFOF( void )
{
	if(!EOprFlag[ESC_FOF])
	{
		EOprFlag[ESC_FOF] = 1;
		AVG_SetBackFade( 0, 0, 0, EscParam[0].num );
	}
	if( !AVG_WaitBackFade() )
	{	
		EOprFlag[ESC_FOF] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 79
static void ESC_EOprFB( void )
{
	if(!EOprFlag[ESC_FB])
	{
		EOprFlag[ESC_FB] = 1;
		AVG_SetBackFade( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );
	}
	if( !AVG_WaitBackFade() )
	{	
		EOprFlag[ESC_FB] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 80
static void ESC_EOprPFI( void )
{
	if(!EOprFlag[ESC_PFI])
	{
		EOprFlag[ESC_PFI] = 1;
		if( EscParam[2].num==-1 ) 
			EscParam[2].num = 0;
		if( EscParam[3].num==-1 ) 
			EscParam[3].num = 0;
		if( EscParam[4].num==-1 ) 
			EscParam[4].num = 400;
		if( EscParam[5].num==-1 ) 
			EscParam[5].num = 300;
		AVG_SetBackFadePatern( ON, 0, EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num );
	}
	if( !AVG_WaitBackFade() )
	{	
		EOprFlag[ESC_PFI] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 81
static void ESC_EOprPFO( void )
{
	if(!EOprFlag[ESC_PFO])
	{
		EOprFlag[ESC_PFO] = 1;
		if( EscParam[2].num==-1 ) 
			EscParam[2].num = 0;
		if( EscParam[3].num==-1 ) 
			EscParam[3].num = 0;
		if( EscParam[4].num==-1 ) 
			EscParam[4].num = 400;
		if( EscParam[5].num==-1 ) 
			EscParam[5].num = 300;
		AVG_SetBackFadePatern( OFF, 0, EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num );
	}
	if( !AVG_WaitBackFade() )
	{	
		EOprFlag[ESC_PFO] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 82
static void ESC_EOprPWI( void )
{
	if(!EOprFlag[ESC_PWI])
	{
		EOprFlag[ESC_PWI] = 1;
		if( EscParam[2].num==-1 ) 
			EscParam[2].num = 0;
		if( EscParam[3].num==-1 ) 
			EscParam[3].num = 0;
		if( EscParam[4].num==-1 ) 
			EscParam[4].num = 400;
		if( EscParam[5].num==-1 ) 
			EscParam[5].num = 300;
		AVG_SetBackFadePatern( ON, 1, EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num );
	}
	if( !AVG_WaitBackFade() )
	{	
		EOprFlag[ESC_PWI] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 83
static void ESC_EOprPWO( void )
{
	if(!EOprFlag[ESC_PWO])
	{
		EOprFlag[ESC_PWO] = 1;
		if( EscParam[2].num==-1 ) 
			EscParam[2].num = 0;
		if( EscParam[3].num==-1 ) 
			EscParam[3].num = 0;
		if( EscParam[4].num==-1 ) 
			EscParam[4].num = 400;
		if( EscParam[5].num==-1 ) 
			EscParam[5].num = 300;
		AVG_SetBackFadePatern( OFF, 1, EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num );
	}
	if( !AVG_WaitBackFade() )
	{	
		EOprFlag[ESC_PWO] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 84
static void ESC_EOprQ( void )
{
	if(!EOprFlag[ESC_Q])
	{
		EOprFlag[ESC_Q] = 1;
		AVG_SetShake( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );
	}
	if( !AVG_WaitShake() )
	{	
		EOprFlag[ESC_Q] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 85
static void ESC_EOprF( void )
{
	if(!EOprFlag[ESC_F])
	{
		EOprFlag[ESC_F] = 1;
		AVG_SetFlash( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	}
	if( !AVG_WaitFade() )
	{	
		EOprFlag[ESC_F] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 86
static void ESC_EOprC( void )
{
	if(EscParam[2].num==-1)
	{
		int	pos = AVG_CheckCharLocate( EscParam[0].num );
		EscParam[2].num = (pos==-1) ? 1 : pos ;
	}
	if(EscParam[3].num==-1) 
		EscParam[3].num = 0;
	if(EscParam[3].num==-2) 
		EscParam[3].num = -1;
	if(EscParam[4].num==-1) 
		EscParam[4].num = 0;
	if(EscParam[5].num==-1) 
		EscParam[5].num = 128;
	if(EscParam[6].num==-1) 
		EscParam[6].num = 256;
	if(EscParam[7].num==-1) 
		EscParam[7].num = -1;
	if(!EOprFlag[ESC_C])
	{
		EOprFlag[ESC_C] = 1;
		AVG_SetChar( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[4].num, EscParam[3].num, EscParam[5].num, EscParam[6].num, EscParam[7].num );	
	}
	if( !AVG_WaitChar( EscParam[0].num ) )
	{	
		EOprFlag[ESC_C] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 87
static void ESC_EOprCR( void )
{
	if(EscParam[1].num==-1) 
		EscParam[1].num = 0;
	if(EscParam[2].num==-1) 
		EscParam[2].num = -1;
	if(!EOprFlag[ESC_CR])
	{
		EOprFlag[ESC_CR] = 1;
		AVG_ResetChar( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	}
	if( !AVG_WaitChar( EscParam[0].num ) )
	{	
		EOprFlag[ESC_CR] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 88
static void ESC_EOprCP( void )
{
	if(!EOprFlag[ESC_CP])
	{
		EOprFlag[ESC_CP] = 1;
		if(EscParam[2].num==-1) 
			EscParam[2].num = 0;
		AVG_SetCharPose( EscParam[0].num, EscParam[1].num, EscParam[2].num, -1 );
	}
	if( !AVG_WaitChar( EscParam[0].num ) )
	{	
		EOprFlag[ESC_CP] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 89
static void ESC_EOprCL( void )
{
	if(!EOprFlag[ESC_CL])
	{
		EOprFlag[ESC_CL] = 1;
		if( EscParam[2].num==-1 ) EscParam[2].num = -1;
		AVG_SetCharLocate( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	}
	if( !AVG_WaitChar( EscParam[0].num ) )
	{	
		EOprFlag[ESC_CL] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 90
static void ESC_EOprCY( void )
{
	if(!EOprFlag[ESC_CY])
	{
		EOprFlag[ESC_CY] = 1;
		AVG_SetCharLayer( EscParam[0].num, EscParam[1].num );
	}
	if( !AVG_WaitChar( EscParam[0].num ) )
	{	
		EOprFlag[ESC_CY] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 91
static void ESC_EOprCB( void )
{
	if(!EOprFlag[ESC_CB])
	{
		EOprFlag[ESC_CB] = 1;
		AVG_SetCharBright( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	}
	if( !AVG_WaitChar( EscParam[0].num ) )
	{	
		EOprFlag[ESC_CB] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 92
static void ESC_EOprCA( void )
{
	if(!EOprFlag[ESC_CA])
	{
		EOprFlag[ESC_CA] = 1;
		AVG_SetCharAlph( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	}
	if( !AVG_WaitChar( EscParam[0].num ) )
	{	
		EOprFlag[ESC_CA] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 93
static void ESC_EOprCW( void )
{
	if(EscParam[2].num==-1)
	{
		int	pos = AVG_CheckCharLocate( EscParam[0].num );
		EscParam[2].num = (pos==-1) ? 1 : pos ;
	}
	if(EscParam[3].num==-1) 
		EscParam[3].num = 0;
	if(EscParam[4].num==-1) 
		EscParam[4].num = 128;
	if(EscParam[5].num==-1) 
		EscParam[5].num = 256;
	AVG_SetChar( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, 3, EscParam[4].num, EscParam[5].num, -1 );
	EXEC_AddPC( EscCnt );
}

// 94
static void ESC_EOprCRW( void )
{
	AVG_ResetChar( EscParam[0].num, 3, -1 );
	EXEC_AddPC( EscCnt );
}

// 95
static void ESC_EOprW( void )
{
	EXEC_AddPC( EscCnt );
}

// 96
static void ESC_EOprWR( void )
{
	EXEC_AddPC( EscCnt );
}

// 97
static void ESC_EOprKW( void )
{
	AVG_SetMessageWait( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 98
static void ESC_EOprK( void )
{
	if( AVG_WaitKey() )
	{
		EXEC_AddPC( EscCnt );
	}
}

// 99
static void ESC_EOprM( void )
{
	if(EscParam[1].num==-1) 
		EscParam[1].num = 0;
	if(EscParam[2].num==-1) 
		EscParam[2].num = ON;
	if(EscParam[3].num==-1) 
		EscParam[3].num = 0xff;
	AVG_PlayBGM( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, 0 );
	EXEC_AddPC( EscCnt );
}

// 100
static void ESC_EOprMS( void )
{
	int		fade = EscParam[0].num;
	if(EscParam[0].num==-1) 
		fade = 0;
	AVG_StopBGM( fade );
	EXEC_AddPC( EscCnt );
}

// 101
static void ESC_EOprMP( void )
{
	int		flag = EscParam[0].num;
	AVG_PauseBGM( flag );
	EXEC_AddPC( EscCnt );
}

// 102
static void ESC_EOprMV( void )
{
	AVG_SetVolumeBGM( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 103
static void ESC_EOprMW(void)
{
	if( AVG_WaitBGM() )
	{	
		EXEC_AddPC( EscCnt );
	}
}

// 104
static void ESC_EOprSE( void )
{
	if( EscParam[1].num==-1) 
		EscParam[1].num = 0xff;
	AVG_PlaySE3( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 105
static void ESC_EOprSEP( void )
{
	if( EscParam[2].num==-1) 
		EscParam[2].num = 0;
	if( EscParam[3].num==-1) 
		EscParam[3].num = TRUE;
	if( EscParam[4].num==-1) 
		EscParam[4].num = 0xff;
	AVG_PlaySE2( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	EXEC_AddPC( EscCnt );
}

// 106
static void ESC_EOprSES( void )
{
	AVG_StopSE2( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 108
static void ESC_EOprSEV( void )
{
	AVG_SetVolumeSE( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 107
static void ESC_EOprSEW( void )
{
	if( !AVG_WaitSe(EscParam[0].num) )
	{		
		EXEC_AddPC( EscCnt );
	}
}

// 109
static void ESC_EOprSEVW( void )
{
	if( !AVG_WaitFadeSe(EscParam[0].num) )
	{		
		EXEC_AddPC( EscCnt );
	}
}

// 110
static void ESC_EOprSetVolumeVoiceChar( void )
{
	AVG_SetVolumeVoiceChar( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 111
static void ESC_EOprSetVolumeVoiceScript( void )
{
	AVG_SetVolumeVoiceScript( EscParam[0].num );
	EXEC_AddPC( EscCnt );
}

// 112
static void ESC_EOprSV( void )
{
	if(EscParam[1].num==-1) 
		EscParam[1].num = -1;		
	if(EscParam[2].num==-1) 
		EscParam[2].num = FALSE;	
	if(EscParam[4].num==-1) 
		EscParam[4].num = 0;		
	AVG_SizukuPlayVoice( EscParam[4].num, EscParam[3].num, -1, EscParam[0].num, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 113
static void ESC_EOprSVEX( void )
{
	if(EscParam[1].num==-1) 
		EscParam[1].num = -1;		
	if(EscParam[2].num==-1) 
		EscParam[2].num = -1;		
	if(EscParam[3].num==-1) 
		EscParam[3].num = -1;		
	if(EscParam[4].num==-1) 
		EscParam[4].num = FALSE;	
	if(EscParam[5].num==-1) 
		EscParam[5].num = 0;	
	AVG_SizukuPlayVoice( EscParam[5].num, EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	EXEC_AddPC( EscCnt );
}

// 114
static void ESC_EOprVV( void )
{
	if(EscParam[1].num==-1) 
		EscParam[1].num = 256;		
	if(EscParam[2].num==-1) 
		EscParam[2].num = FALSE;	
	if(EscParam[4].num==-1) 
		EscParam[4].num = 0;		
	AVG_PlayVoice( EscParam[4].num, EscParam[0].num, -1, EscParam[3].num, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 115
static void ESC_EOprVA( void )
{
	if(EscParam[1].num==-1) 
		EscParam[1].num = 255;		
	if(EscParam[2].num==-1) 
		EscParam[2].num = FALSE;	
	if(EscParam[4].num==-1) 
		EscParam[4].num = 0;		
	AVG_PlayVoice( EscParam[4].num, EscParam[0].num, -1, EscParam[3].num, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 116
static void ESC_EOprVB( void )
{
	if(EscParam[1].num==-1) 
		EscParam[1].num = 255;		
	if(EscParam[2].num==-1) 
		EscParam[2].num = FALSE;	
	if(EscParam[4].num==-1) 
		EscParam[4].num = 0;		
	AVG_PlayVoice( EscParam[4].num, EscParam[0].num, -1, EscParam[3].num, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 117
static void ESC_EOprVC( void )
{
	if(EscParam[1].num==-1) 
		EscParam[1].num = 255;		
	if(EscParam[2].num==-1) 
		EscParam[2].num = FALSE;	
	if(EscParam[4].num==-1) EscParam[4].num = 0;		
	AVG_PlayVoice( EscParam[4].num, EscParam[0].num, -1, EscParam[3].num, EscParam[1].num, EscParam[2].num, 1 );
	EXEC_AddPC( EscCnt );
}

// 118
static void ESC_EOprVX( void )
{
	if(EscParam[3].num==-1) 
		EscParam[3].num = 255;		
	if(EscParam[4].num==-1) 
		EscParam[4].num = FALSE;	
	if(EscParam[5].num==-1) 
		EscParam[5].num = 0;		
	AVG_PlayVoice( EscParam[5].num, EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );	
	EXEC_AddPC( EscCnt );
}

// 119
static void ESC_EOprVW( void )
{
	if(EscParam[0].num==-1) 
		EscParam[0].num = 0;		
	if( AVG_WaitVoice(EscParam[0].num) )
	{
		EXEC_AddPC( EscCnt );
	}
}

// 120
static void ESC_EOprVS( void )
{
	if(EscParam[0].num==-1) 
		EscParam[0].num = 0;		
	if(EscParam[1].num==-1) 
		EscParam[1].num = 0;		
	if(EscParam[0].num==0)
	{
		AVG_StopVoice(EscParam[1].num);
	}
	else
	{
		AVG_FadeOutVoice( EscParam[1].num, EscParam[0].num );
	}	
	EXEC_AddPC( EscCnt );
}

// 121
static void ESC_EOprVI( void )
{
	AddVoiceDialogMes( "カウンター更新[%04d]", EscParam[0].num );
	if(EscParam[2].num!=-1)
	{
		AddVoiceDialogMes( "カウンター全更新[%04d]", EscParam[2].num );
	}
	else
	{
		if(EscParam[1].num!=-1)	
			AddVoiceDialogMes( "カウンター下２桁更新[%04d]", EscParam[1].num );
	}
	AVG_SetVI_EventVoiceNo( EscParam[1].num, EscParam[2].num  );	
	EXEC_AddPC( EscCnt );
}

// 122
static void ESC_EOprR( void )
{
	AVG_SetWeather( 0, EscParam[0].num, EscParam[1].num, EscParam[2].num );	
	EXEC_AddPC( EscCnt );
}

// 123
static void ESC_EOprRC( void )
{
	AVG_SetWeatherWind( EscParam[0].num );
	AVG_SetWeatherSpeed( EscParam[1].num );
	AVG_SetWeatherAmount( EscParam[2].num );	
	EXEC_AddPC( EscCnt );
}

// 124
static void ESC_EOprRR( void )
{
	if(!EOprFlag[ESC_RR] )
	{
		EOprFlag[ESC_RR] = 1;
		AVG_ResetWeather( );
	}
	if( AVG_WaitWeather() )
	{
		EOprFlag[ESC_RR] = 0;		
		EXEC_AddPC( EscCnt );
	}
}

// 125
static void ESC_EOprLF( void )
{
	AVG_SetLensFrea( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 126
static void ESC_EOprWE( void )
{
	if(!EOprFlag[ESC_WE])
	{
		EOprFlag[ESC_WE] = 1;
		if( EscParam[2].num==-1) 
			EscParam[2].num = 0;
		AVG_SetWavEffect( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	}
	if( AVG_WaitWavEffect() )
	{
		EOprFlag[ESC_WE] = 0;		
		EXEC_AddPC( EscCnt );
	}
}

// 127
static void ESC_EOprWER( void )
{
	AVG_ResetWavEffect( );
	EXEC_AddPC( EscCnt );
}

// 128
static void ESC_EOprSetFlag( void )
{	
	if(50 <= EscParam[0].num && EscParam[0].num < 100)
	{
		AVG_SetGameFlag( EscParam[0].num, EscParam[1].num );
		int	i,cnt;
		cnt = 0;
		if( AVG_GetGameFlag(81) ) 
			cnt++;
		if( AVG_GetGameFlag(83) ) 
			cnt++;
		if( AVG_GetGameFlag(84) ) 
			cnt++;
		if( AVG_GetGameFlag(85) ) 
			cnt++;
		if( AVG_GetGameFlag(86) ) 
			cnt++;
		if( AVG_GetGameFlag(88) ) 
			cnt++;
		if( AVG_GetGameFlag(89) ) 
			cnt++;
		if( AVG_GetGameFlag(90) ) 
			cnt++;
		if( AVG_GetGameFlag(93) ) 
			cnt++;
		AVG_SetGameFlag( 80, cnt );
	}
	else
	{
		ESC_SetFlag( EscParam[0].num, EscParam[1].num  );
	}
	EXEC_AddPC( EscCnt );
}

// 129
static void ESC_EOprGetFlag( void )
{	
	if(50 <= EscParam[0].num && EscParam[0].num < 100)
	{
		EscParam[1].reg = AVG_GetGameFlag( EscParam[0].num );
	}
	else
	{
		EscParam[1].reg = ESC_GetFlag( EscParam[0].num );
	}
	EXEC_AddPC( EscCnt );
}

// 130
static void ESC_EOprSetGameFlag( void )
{	
	AVG_SetGameFlag( EscParam[0].num, EscParam[1].num );	
	EXEC_AddPC( EscCnt );
}

// 131
static void ESC_EOprGetGameFlag( void )
{	
	EscParam[1].reg = AVG_GetGameFlag( EscParam[0].num );	
	EXEC_AddPC( EscCnt );
}

// 132
static void ESC_EOprLoadScript(void)
{
	if(!LoadScript(EscParam[0].str))
	{
		EXEC_AddPC( EscCnt );
	}
}

// 133
static void ESC_EOprGameEnd( void )
{
	if(EscParam[0].num==-1)
	{
		EscParam[0].num = 0;
	}
	if(!EOprFlag[ESC_GAMEEND])
	{
		EOprFlag[ESC_GAMEEND] = 1;
		AVG_SetGotoTitle();
	}
}

// 134
static void ESC_EOprCallFunc(void)
{
	long num[14], i;
	for(i = 0; i < 14; i++)
	{
		num[i] = EscParam[i+1].num;
	}
	EXEC_OprCallFunc( EscParam[0].num, EscCnt, num );
}

// 135
static void ESC_EOprViewClock(void)
{
	if(EscParam[1].num == -1)
	{
		EscParam[1].num = 0;
	}
	if(EscParam[2].num == -1) 
	{
		EscParam[2].num = 0;
	}
	if(AVG_ViewClock( EscParam[0].num))
	{
		EXEC_AddPC( EscCnt );
	}
}

// 136
static void ESC_EOprSetWeatherMode( void )
{
	if(EscParam[0].num==-1) 
		EscParam[0].num = 0;
	AVG_SetWeatherMode( EscParam[0].num );
	EXEC_AddPC( EscCnt );
}

// 137
static void ESC_EOprSetTimeMode( void )
{
	if(EscParam[0].num==-1) 
		EscParam[0].num = 0;
	if(EscParam[1].num==-1) 
		EscParam[1].num = 0;
	AVG_SetTimeMode( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 138
static void ESC_EOprSetTimeBack( void )
{
	if(EscParam[0].num==-1) 
		EscParam[0].num = 0;
	if(EscParam[1].num==-1) 
		EscParam[1].num = 0;
	AVG_SetTimeBack( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 139
static void ESC_EOprSetTimeChar( void )
{
	if(EscParam[0].num==-1) 
		EscParam[0].num = 0;
	if(EscParam[1].num==-1) 
		EscParam[1].num = 0;
	AVG_SetTimeChar( EscParam[0].num, EscParam[1].num );	
	EXEC_AddPC( EscCnt );
}

// 140
static void ESC_EOprSetMatrix( void )
{
	MainWindow.draw_matrix = (short)EscParam[0].num;
	EXEC_AddPC( EscCnt );
}

// 141
static void ESC_EOprShowChipBack( void )
{
	if(!EOprFlag[ESC_SHOWCHIPBACK])
	{
		EOprFlag[ESC_SHOWCHIPBACK] = 1;		
		if(EscParam[0].num==-1) 
			EscParam[0].num = 0;	
		if(EscParam[1].num==-1) 
			EscParam[1].num = -2;
		AVG_ShowChipBack( EscParam[0].num, EscParam[1].num );	
	}
	if( !AVG_WaitBack() )
	{
		EOprFlag[ESC_SHOWCHIPBACK] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 142
static void ESC_EOprPoseChipBack( void )
{	
	AVG_PoseChipBack();
	EXEC_AddPC( EscCnt );
}

// 143
static void ESC_EOprPlayChipBack( void )
{	
	AVG_PlayChipBack();
	EXEC_AddPC( EscCnt );
}

// 144
static void ESC_EOprSetChipBack( void )
{	
	AVG_SetChipBack( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	EXEC_AddPC( EscCnt );
}

// 145
static void ESC_EOprSetChipParts( void )
{
	if(EscParam[11].num==-1) 
		EscParam[11].num = DRW_NML;
	if(EscParam[12].num==-1) 
		EscParam[12].num = 0;
	AVG_SetChipBackParts(	EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num,
							EscParam[5].num, EscParam[6].num, EscParam[7].num, EscParam[8].num, EscParam[9].num, EscParam[10].num,
							DRW_PRM(EscParam[11].num,EscParam[12].num) );
	EXEC_AddPC( EscCnt );
}

// 146
static void ESC_EOprSetChipScroll( void )
{
	AVG_SetChipBackScrool( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );
	EXEC_AddPC( EscCnt );
}

// 147
static void ESC_EOprSetChipScroll2( void )
{
	AVG_SetChipBackScrool2( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );	
	EXEC_AddPC( EscCnt );
}

// 148
static void ESC_EOprSetChipScrollSpeed( void )
{
	AVG_SetChipBackScroolSpeed( EscParam[0].num, EscParam[1].num, EscParam[2].num );	
	EXEC_AddPC( EscCnt );
}

// 149
static void ESC_EOprWaitChipScroll( void )
{	
	if( !AVG_WaitChipBackScrool() )
	{	
		EXEC_AddPC( EscCnt );
	}
}

// 150
static void ESC_EOprWaitChipScrollSpeed( void )
{
	if( !AVG_WaitChipBackScroolSpeed() )
	{		
		EXEC_AddPC( EscCnt );
	}
}

// 151
static void ESC_EOprSetChipCharCash( void )
{
	AVG_LoadChipCharCash( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 152
static void ESC_EOprResetChipCharCash( void )
{	
	AVG_ResetChipCharCash();	
	EXEC_AddPC( EscCnt );
}

// 153
static void ESC_EOprSetChipChar( void )
{
	AVG_SetChipChar( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, ON, EscParam[4].num, EscParam[5].num );	
	EXEC_AddPC( EscCnt );
}

// 154
static void ESC_EOprSetChipCharAni( void )
{
	AVG_LoadChipChar( EscParam[0].num, EscParam[1].num, ON, EscParam[2].num );	
	EXEC_AddPC( EscCnt );
}

// 155
static void ESC_EOprResetChipChar( void )
{	
	AVG_ResetChipChar( EscParam[0].num );	
	EXEC_AddPC( EscCnt );
}

// 156
static void ESC_EOprSetChipCharParam( void )
{
	AVG_SetChipCharParam( EscParam[0].num, DRW_PRM(EscParam[1].num, EscParam[2].num) );
	EXEC_AddPC( EscCnt );
}

// 157
static void ESC_EOprSetChipCharBright( void )
{	
	AVG_SetChipCharBright( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );	
	EXEC_AddPC( EscCnt );
}

// 158
static void ESC_EOprSetChipCharMove( void )
{
	AVG_SetChipCharMove( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );	
	EXEC_AddPC( EscCnt );
}

// 159
static void ESC_EOprSetChipCharMove2( void )
{	
	AVG_SetChipCharMove2( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );	
	EXEC_AddPC( EscCnt );
}

// 160
static void ESC_EOprSetChipCharMoveSpeed( void )
{	
	AVG_SetChipCharMoveSpeed( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );	
	EXEC_AddPC( EscCnt );
}

// 161
static void ESC_EOprGetChipCharMove( void )
{	
	AVG_GetChipCharMove( EscParam[0].num, &EscParam[1].reg, &EscParam[2].reg );	
	EXEC_AddPC( EscCnt );
}

// 162
static void ESC_EOprGetChipCharMoveSpeed( void )
{	
	AVG_GetChipCharMoveSpeed( EscParam[0].num, &EscParam[1].reg, &EscParam[2].reg );	
	EXEC_AddPC( EscCnt );
}

// 163
static void ESC_EOprCopyChipCharPos( void )
{	
	AVG_CopyChipCharPos( EscParam[0].num, EscParam[1].num );	
	EXEC_AddPC( EscCnt );
}

// 164
static void ESC_EOprSetChipCharRev( void )
{
	AVG_SetChipCharRev( EscParam[0].num, EscParam[1].num );	
	EXEC_AddPC( EscCnt );
}

// 165
static void ESC_EOprThroughChipCharAni( void )
{
	AVG_ThroughChipCharAni( EscParam[0].num );	
	EXEC_AddPC( EscCnt );
}

// 166
static void ESC_EOprWaitChipCharRepeat( void )
{
	BOOL	through=FALSE;
	if(!EOprFlag[ESC_WAITCHIPCHARREPEAT])
	{
		EOprFlag[ESC_WAITCHIPCHARREPEAT] = 1;	
		if(EscParam[1].num!=-1) 
			through = EscParam[1].num;
		if(through) 
			AVG_ThroughChipCharAni( EscParam[0].num );
		AVG_SetSpriteRepeatFind( EscParam[0].num );
	}
	if( !AVG_WaitChipCharRepeat( EscParam[0].num ) )
	{
		EOprFlag[ESC_WAITCHIPCHARREPEAT] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 167
static void ESC_EOprWaitChipCharAni( void )
{
	BOOL	through=FALSE;
	if(!EOprFlag[ESC_WAITCHIPCHARANI])
	{
		EOprFlag[ESC_WAITCHIPCHARANI] = 1;		
		through = EscParam[1].num;
		if(EscParam[1].num==-1) 
			through = ON;
		if(through) 
			AVG_ThroughChipCharAni( EscParam[0].num );
	}
	if( !AVG_WaitChipCharAni( EscParam[0].num ) )
	{
		EOprFlag[ESC_WAITCHIPCHARANI] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 168
static void ESC_EOprWaitChipCharAniLoop( void )
{
	BOOL	through=FALSE;
	if(!EOprFlag[ESC_WAITCHIPCHARANILOOP])
	{
		EOprFlag[ESC_WAITCHIPCHARANILOOP] = 1;
		through = EscParam[1].num;
		if(EscParam[1].num==-1) 
			through = ON;
		if(through) 
			AVG_ThroughChipCharAni( EscParam[0].num );
		AVG_ThroughChipCharAniLoop( EscParam[0].num );
	}
	if( !AVG_WaitChipCharAni( EscParam[0].num ) )
	{
		EOprFlag[ESC_WAITCHIPCHARANILOOP] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 169
static void ESC_EOprWaitChipCharMove( void )
{
	if( !AVG_WaitChipCharMove( EscParam[0].num ) )
	{
		EXEC_AddPC( EscCnt );
	}
}

// 170
static void ESC_EOprGetBack( void )
{	
	switch(EOprFlag[ESC_GETBACK])
	{
		case 0:
			EOprFlag[ESC_GETBACK] = 1;
			MainWindow.draw_flag = 1;
			break;

		case 1:
			EOprFlag[ESC_GETBACK] = 0;
			AVG_GetBack();
			EXEC_AddPC( EscCnt );
			break;
	}
}

// 171
static void ESC_EOprSetBack( void )
{
	if(!EOprFlag[ESC_SETBACK])
	{
		EOprFlag[ESC_SETBACK] = 1;
		AVG_SetBack( EscParam[0].num, 0, 0, EscParam[1].num, 0, 0, -2 );
	}
	if( !AVG_WaitBack() )
	{
		EOprFlag[ESC_SETBACK] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 172
static void ESC_EOprSetBack2( void )
{	
	if(!EOprFlag[ESC_SETBACK2])
	{
		EOprFlag[ESC_SETBACK2] = 1;
		AVG_SetBack( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[5].num, 0, EscParam[6].num );
	}
	if( !AVG_WaitBack() )
	{
		EOprFlag[ESC_SETBACK2] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 173
static void ESC_EOprSetCg( void )
{
	if(!EOprFlag[ESC_SETCG])
	{
		EOprFlag[ESC_SETCG] = 1;
		AVG_SetBack( EscParam[0].num, 0, 0, EscParam[1].num, 0, 1, -2 );
	}
	if( !AVG_WaitBack() )
	{
		EOprFlag[ESC_SETCG] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 174
static void ESC_EOprSetCg2( void )
{	
	if(!EOprFlag[ESC_SETCG2])
	{
		EOprFlag[ESC_SETCG2] = 1;
		AVG_SetBack( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, 0, 1, EscParam[4].num );
	}
	if( !AVG_WaitBack() )
	{
		EOprFlag[ESC_SETCG2] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 175
static void ESC_EOprSetBackPos( void )
{	
	AVG_SetBackPos( EscParam[0].num, EscParam[1].num );	
	EXEC_AddPC( EscCnt );
}

// 176
static void ESC_EOprSetBackPosZ( void )
{	
	AVG_SetBackPosZoom( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );	
	EXEC_AddPC( EscCnt );
}

// 177
static void ESC_EOprSetBackScroll( void )
{	
	if(!EOprFlag[ESC_SETBACKSCROLL])
	{
		EOprFlag[ESC_SETBACKSCROLL] = 1;
		AVG_SetBackScroll( EscParam[0].num, EscParam[1].num, DISP_X, DISP_Y, EscParam[2].num, EscParam[3].num );
	}
	if( !AVG_WaitBackScroll() )
	{
		EOprFlag[ESC_SETBACKSCROLL] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 178
static void ESC_EOprSetBackScrollZ( void )
{	
	if(!EOprFlag[ESC_SETBACKSCROLLZ])
	{
		EOprFlag[ESC_SETBACKSCROLLZ] = 1;
		AVG_SetBackScroll( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num+3 );
	}
	if( !AVG_WaitBackScroll() )
	{
		EOprFlag[ESC_SETBACKSCROLLZ] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 179
static void ESC_EOprSetBackCScope( void )
{	
	AVG_SetBackCScope( EscParam[0].num, -1 );	
	EXEC_AddPC( EscCnt );
}

// 180
static void ESC_EOprWaitBackCScope( void )
{	
	if( !AVG_WaitBackCScope() )
	{	
		EXEC_AddPC( EscCnt );
	}
}

// 181
static void ESC_EOprLockBackCScope( void )
{	
	AVG_LockBackCScope( EscParam[0].num );	
	EXEC_AddPC( EscCnt );
}

// 182
static void ESC_EOprSetBackFadeIn( void )
{	
	if(EscParam[1].num==-1) 
		EscParam[1].num=OFF;
	if(!EOprFlag[ESC_SETBACKFADEIN])
	{
		EOprFlag[ESC_SETBACKFADEIN] = 1;
		AVG_SetBackFade( 128, 128, 128, EscParam[0].num );
	}
	if( !AVG_WaitBackFade() || EscParam[1].num )
	{	
		EOprFlag[ESC_SETBACKFADEIN] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 183
static void ESC_EOprSetBackFadeOut( void )
{	
	if(EscParam[1].num==-1) EscParam[1].num=OFF;
	if(!EOprFlag[ESC_SETBACKFADEOUT])
	{
		EOprFlag[ESC_SETBACKFADEOUT] = 1;
		AVG_SetBackFade( 0, 0, 0, EscParam[0].num );
	}
	if( !AVG_WaitBackFade() || EscParam[1].num )
	{	
		EOprFlag[ESC_SETBACKFADEOUT] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 184
static void ESC_EOprSetBackFadeRGB( void )
{	
	if(EscParam[4].num==-1) 
		EscParam[4].num=OFF;
	if(!EOprFlag[ESC_SETBACKFADERGB])
	{
		EOprFlag[ESC_SETBACKFADERGB] = 1;
		AVG_SetBackFade( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );
	}
	if( !AVG_WaitBackFade() || EscParam[4].num )
	{	
		EOprFlag[ESC_SETBACKFADERGB] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 185
static void ESC_EOprSetShake( void )
{	
	if(!EOprFlag[ESC_SETSHAKE])
	{
		EOprFlag[ESC_SETSHAKE] = 1;
		if(EscParam[4].num==-1) 
			EscParam[4].num=256;
		AVG_SetShake( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	}
	if( !AVG_WaitShake() )
	{	
		EOprFlag[ESC_SETSHAKE] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 186
static void ESC_EOprStopShake( void )
{	
	AVG_StopShake( );	
	EXEC_AddPC( EscCnt );
}

// 187
static void ESC_EOprSetFlash( void )
{	
	if(!EOprFlag[ESC_SETFLASH])
	{
		EOprFlag[ESC_SETFLASH] = 1;
		AVG_SetFlash( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	}
	if( !AVG_WaitFade() )
	{	
		EOprFlag[ESC_SETFLASH] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 188
static void ESC_EOprSetChar( void )
{	
	if(EscParam[2].num==-1) 
		EscParam[2].num = 1;
	if(EscParam[3].num==-1) 
		EscParam[3].num = 0;
	if(EscParam[4].num==-1) 
		EscParam[4].num = 0;
	if(EscParam[5].num==-1) 
		EscParam[5].num = 128;
	if(EscParam[6].num==-1) 
		EscParam[6].num = 256;
	if(EscParam[7].num==-1) 
		EscParam[7].num = -1;
	AVG_SetChar( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[4].num, EscParam[3].num, EscParam[5].num, EscParam[6].num, EscParam[7].num );
	EXEC_AddPC( EscCnt );
}

// 189
static void ESC_EOprResetChar( void )
{
	if(EscParam[1].num==-1) 
		EscParam[1].num = 0;
	if(EscParam[2].num==-1) 
		EscParam[2].num = -1;
	AVG_ResetChar( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 190
static void ESC_EOprSetCharPose( void )
{	
	AVG_SetCharPose( EscParam[0].num, EscParam[1].num, EscParam[2].num, -1 );	
	EXEC_AddPC( EscCnt );
}

// 191
static void ESC_EOprSetCharLocate( void )
{	
	AVG_SetCharLocate( EscParam[0].num, EscParam[1].num, -1 );	
	EXEC_AddPC( EscCnt );
}

// 192
static void ESC_EOprSetCharLayer( void )
{	
	AVG_SetCharLayer( EscParam[0].num, EscParam[1].num );	
	EXEC_AddPC( EscCnt );
}

// 193
static void ESC_EOprWaitChar( void )
{
	if( !AVG_WaitChar( EscParam[0].num ) )
	{		
		EXEC_AddPC( EscCnt );
	}
}

// 194
static void ESC_EOprSetBlock(void)
{	
	AVG_SetScenarioFlag( EscParam[0].num );
	EXEC_AddPC( EscCnt );
}

// 195
static void ESC_EOprSetWindow(void)
{	
	if(!EOprFlag[ESC_SETWINDOW])
	{
		EOprFlag[ESC_SETWINDOW] = 1;
		if(EscParam[0].num==-1) 
			EscParam[0].num = OFF;
		AVG_SetWindow(EscParam[0].num);
	}
	if( AVG_WaitWindow() )
	{
		EOprFlag[ESC_SETWINDOW] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 196
static void ESC_EOprResetWindow(void)
{
	
	if(!EOprFlag[ESC_RESETWINDOW])
	{
		EOprFlag[ESC_RESETWINDOW] = 1;
		if(EscParam[0].num==-1) 
			EscParam[0].num = OFF;
		AVG_ResetWindow(EscParam[0].num);
	}
	if( AVG_WaitWindow() )
	{
		EOprFlag[ESC_RESETWINDOW] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 197
static void ESC_EOprSetMessage(void)
{
	if(!EOprFlag[ESC_SETMESSAGE])
	{
		EOprFlag[ESC_SETMESSAGE] = 1;
		AVG_SetMessage( EscParam[0].num, EscParam[1].str, 0, 0, -1, -1 );
		AVG_SetScenarioFlag( EscParam[2].num );
	}
	if( AVG_WaitMessage() )
	{
		EOprFlag[ESC_SETMESSAGE] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 198
static void ESC_EOprSetMessage2(void)
{	
	if(!EOprFlag[ESC_SETMESSAGE2])
	{
		EOprFlag[ESC_SETMESSAGE2] = 1;
		AVG_SetNovelMessage( EscParam[0].str, EscParam[1].num );
		AVG_SetScenarioFlag( EscParam[2].num );
	}
	if( AVG_WaitNovelMessage() )
	{
		EOprFlag[ESC_SETMESSAGE2] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 199
static void ESC_EOprSetMessageEx(void)
{	
	if(!EOprFlag[ESC_SETMESSAGEEX])
	{
		EOprFlag[ESC_SETMESSAGEEX] = 1;
		AVG_SetMessage( 0, EscParam[2].str, 0, EscParam[3].num, EscParam[0].num, EscParam[1].num );
		AVG_SetScenarioFlag( EscParam[4].num );
	}
	if( AVG_WaitMessage() )
	{
		EOprFlag[ESC_SETMESSAGEEX] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 200
static void ESC_EOprSetChipMessage(void)
{	
	AVG_SetMessage( 0, EscParam[0].str, 0, 0,-1,-1 );
	AVG_SetScenarioFlag( EscParam[1].num );	
	EXEC_AddPC( EscCnt );
}

// 201
static void ESC_EOprAddMessage(void)
{	
	if(!EOprFlag[ESC_ADDMESSAGE])
	{
		EOprFlag[ESC_ADDMESSAGE] = 1;
		AVG_AddMessage( EscParam[0].num, EscParam[1].str, 0, 0 );
	}
	if( AVG_WaitMessage() )
	{
		EOprFlag[ESC_ADDMESSAGE] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 202
static void ESC_EOprAddMessage2(void)
{	
	if(!EOprFlag[ESC_ADDMESSAGE2])
	{
		EOprFlag[ESC_ADDMESSAGE2] = 1;
		AVG_AddNovelMessage( EscParam[0].str, EscParam[1].num );
	}
	if( AVG_WaitNovelMessage() )
	{
		EOprFlag[ESC_ADDMESSAGE2] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 203
static void ESC_EOprSetMessageWait(void)
{	
	AVG_SetMessageWait( EscParam[0].num, EscParam[1].num );	
	EXEC_AddPC( EscCnt );
}

// 204
static void ESC_EOprResetMessage(void)
{	
	AVG_ResetHalfTone();
	AVG_SetNovelMessageDisp(OFF);
	MainWindow.draw_flag = 1;	
	EXEC_AddPC( EscCnt );
}

// 205
static void ESC_EOprWaitKey(void)
{	
	if( AVG_WaitKey() )
	{	
		EXEC_AddPC( EscCnt );
	}
}

// 206
static void ESC_EOprSetSelectMes(void)
{
	if(EscParam[1].num==-1) 
		EscParam[1].num = -1;
	if(EscParam[2].num==-1) 
		EscParam[2].num = 0;
	if(EscParam[3].num==-1) 
		EscParam[3].num = 0;
	AVG_SetSelectMessage( EscParam[0].str, EscParam[1].num, EscParam[2].num, EscParam[3].num );
	EXEC_AddPC( EscCnt );
}

// 207
static void ESC_EOprSetSelectMesEx(void)
{
	if(EscParam[2].num==-1) 
		EscParam[2].num = -1;
	if(EscParam[3].num==-1) 
		EscParam[3].num = 0;
	AVG_SetSelectMessageEx( EscParam[0].str, EscParam[1].str, EscParam[2].num, EscParam[3].num );
	EXEC_AddPC( EscCnt );
}

// 208
static void ESC_EOprSetSelect(void)
{	
	if(!EOprFlag[ESC_SETSELECT])
	{
		EOprFlag[ESC_SETSELECT] = 1;
		AVG_SetSelectWindow();
	}
	EscParam[0].reg = AVG_WaitSelect();
	if( EscParam[0].reg!=-1 )
	{
		EOprFlag[ESC_SETSELECT] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 209
static void ESC_EOprSetSelectEx(void)
{
	TCHAR	*ret;
	if(!EOprFlag[ESC_SETSELECT])
	{
		EOprFlag[ESC_SETSELECT] = 1;
		AVG_SetSelectWindow();
	}
	ret = AVG_WaitSelectEx();
	if( ret!=NULL )
	{
		EOprFlag[ESC_SETSELECT] = 0;
		LoadScript( ret );
	}
}

// 210
static void ESC_EOprPlayBgm(void)
{
	AVG_PlayBGM( EscParam[0].num, EscParam[1].num, 1, 0xff, 0 );
	EXEC_AddPC( EscCnt );
}

// 211
static void ESC_EOprPlayBgmEx(void)
{
	switch(EscParam[0].num)
	{
		default:
		case 0:	
			AVG_PlayBGM( EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, 0 );		
			break;
		
		case 1:	
			AVG_PlayMusic( 2, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, 0 );	
			break;
		
		case 2:	
			AVG_PlayMusic( 3, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, 0 );	
			break;
	}
	EXEC_AddPC( EscCnt );
}

// 212
static void ESC_EOprStopBgm(void)
{
	AVG_StopBGM( EscParam[0].num );
	EXEC_AddPC( EscCnt );
}

// 213
static void ESC_EOprStopBgmEx(void)
{
	switch(EscParam[0].num)
	{
		default:
		case 0:	
			AVG_StopBGM( EscParam[1].num );			
			break;
		
		case 1:	
			AVG_StopMusic( 2, EscParam[1].num );	
			break;
		
		case 2:	
			AVG_StopMusic( 3, EscParam[1].num );	
			break;
	}
	EXEC_AddPC( EscCnt );
}

// 214
static void ESC_EOprSetVolumeBgm(void)
{
	AVG_SetVolumeBGM( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 215
static void ESC_EOprSetVolumeBgmEx(void)
{
	switch(EscParam[0].num)
	{
	default:
	case 0:	
		AVG_SetVolumeBGM( EscParam[1].num, EscParam[2].num );		
		break;
	
	case 1:	
		AVG_SetVoumeMusic( 2, EscParam[1].num, EscParam[2].num );	
		break;
	
	case 2:	
		AVG_SetVoumeMusic( 3, EscParam[1].num, EscParam[2].num );	
		break;
	}
	EXEC_AddPC( EscCnt );
}

// 216
static void ESC_EOprPlaySe(void)
{
	AVG_PlaySE3( EscParam[0].num, 0xff );
	EXEC_AddPC( EscCnt );
}

// 217
static void ESC_EOprPlaySeEx(void)
{
	AVG_PlaySE2( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );
	EXEC_AddPC( EscCnt );
}

// 218
static void ESC_EOprStopSeEx(void)
{
	AVG_StopSE2( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 219
static void ESC_EOprSetVolumeSe(void)
{
	AVG_SetVolumeSE( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 220
static void ESC_EOprSetWeather(void)
{	
	AVG_SetWeather( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );	
	EXEC_AddPC( EscCnt );
}

// 221
static void ESC_EOprChangeWeather(void)
{	
	AVG_SetWeatherWind( EscParam[0].num );
	AVG_SetWeatherSpeed( EscParam[1].num );
	AVG_SetWeatherAmount( EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 222
static void ESC_EOprResetWeather(void)
{	
	if(!EOprFlag[ESC_RESETWEATHER])
	{
		EOprFlag[ESC_RESETWEATHER] = 1;
		AVG_ResetWeather( );
	}
	if( AVG_WaitWeather() )
	{
		EOprFlag[ESC_RESETWEATHER] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 223
static void ESC_EOprSetLensFrea(void)
{	
	AVG_SetLensFrea( EscParam[0].num, EscParam[1].num, EscParam[2].num );	
	EXEC_AddPC( EscCnt );
}

// 224
static void ESC_EOprSetWavEffect(void)
{
	if(EscParam[0].num) 
		EscParam[0].num = 128;
	if(EscParam[1].num) 
		EscParam[1].num = 30;
	if(EscParam[2].num) 
		EscParam[2].num = 0;
	if(!EOprFlag[ESC_SETWAVEFFECT])
	{
		EOprFlag[ESC_SETWAVEFFECT] = 1;
		AVG_SetWavEffect( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	}
	if( AVG_WaitWavEffect() )
	{
		EOprFlag[ESC_SETWAVEFFECT] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 225
static void ESC_EOprResetWavEffect(void)
{
	AVG_ResetWavEffect( );
	EXEC_AddPC( EscCnt );
}

// 226
static void ESC_EOprSetWarp(void)
{
	if(EscParam[0].num==-1) 
		EscParam[0].num = 0;
	if(EscParam[1].num==-1) 
		EscParam[1].num = 128;
	if(EscParam[2].num==-1) 
		EscParam[2].num = 60;
	if(EscParam[3].num==-1) 
		EscParam[3].num = 1;
	if(EscParam[4].num==-1) 
		EscParam[4].num = 1;
	if(EscParam[5].num==-1) 
		EscParam[5].num = 400;
	if(EscParam[6].num==-1) 
		EscParam[6].num = 300;
	if(EscParam[7].num==-1) 
		EscParam[7].num = 256;
	if(!EOprFlag[ESC_SETWARP])
	{
		EOprFlag[ESC_SETWARP] = 1;
		AVG_SetWarp( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num, EscParam[6].num, EscParam[7].num );
	}
	if( !AVG_WaitWarp() )
	{
		EOprFlag[ESC_SETWARP] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 227
static void ESC_EOprResetWarp(void)
{
	if(EscParam[0].num==-1) 
		EscParam[0].num = 1;
	if(!EOprFlag[ESC_RESETWARP])
	{
		EOprFlag[ESC_RESETWARP] = 1;
		AVG_StopWarp( EscParam[0].num );
	}
	if( !AVG_WaitWarp() )
	{
		EOprFlag[ESC_RESETWARP] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 228
static void ESC_EOprWaitFrame(void)
{
	if(!EOprFlag[ESC_WAITFRAME])
	{
		EOprFlag[ESC_WAITFRAME] = 1;
		AVG_SetWaitFrame( -1, EscParam[0].num );
	}
	if( AVG_WaitFrame() )
	{	
		EOprFlag[ESC_WAITFRAME] = 0;	
		EXEC_AddPC( EscCnt );
	}
}

// 229
static void ESC_EOprSetBmp( void )
{	
	int		bmp_bit;
	TCHAR	*pack;	
	switch(EscParam[1].num)
	{
	case 0:	
		bmp_bit = BMP_256P;						
		break;
	
	default:
	case 1:	
		bmp_bit = BPP(MainWindow.draw_mode2);	
		break;
	
	case 2:	
		bmp_bit = (MainWindow.draw_mode2!=16)? BMP_TRUE: BMP_HIGH;	
		break;
	}
	if( EscParam[6].num==-1 )
		EscParam[6].num=256;
	if( EscParam[7].num==-1 )
		EscParam[7].num=128;
	if(EscParam[8].str[0]==0)
	{
		pack = PAC_GRP;
	}
	else
	{
		pack = EscParam[8].str;
	}
	AVG_LoadBmp( EscParam[0].num, bmp_bit, EscParam[2].str, EscParam[3].num, EscParam[4].num, -1, EscParam[5].str, EscParam[6].num, EscParam[7].num, pack );
	EXEC_AddPC( EscCnt );
}

// 230
static void ESC_EOprSetBmpEx( void )
{	
	int		bmp_bit;
	TCHAR	*pack;	
	switch(EscParam[1].num)
	{
		case 0:	
			bmp_bit = BMP_256P;						
			break;
		default:
		case 1:	
			bmp_bit = BPP(MainWindow.draw_mode2);	
			break;
		
		case 2:	
			bmp_bit = (MainWindow.draw_mode2!=16)? BMP_TRUE: BMP_HIGH;	
			break;
	}
	if(EscParam[5].num==-1) 
		EscParam[5].num = 1;
	if(EscParam[6].str[0]==0)
	{
		pack = PAC_GRP;
	}
	else
	{
		pack = EscParam[6].str;
	}
	AVG_LoadBmp( EscParam[0].num, bmp_bit, EscParam[2].str, EscParam[3].num, EscParam[4].num, EscParam[5].num, NULL, 256, 128, pack );
	EXEC_AddPC( EscCnt );
}

// 231
static void ESC_EOprSetBmp4Bmp( void )
{	
	EXEC_AddPC( EscCnt );
}

// 232
static void ESC_EOprSetBmpPrim( void )
{
	DSP_SetGraphPrim( GRP_SCRIPT+EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, ON );
	AVG_SetBmpZoom( GRP_SCRIPT+EscParam[0].num, 0, 0, DISP_X, DISP_Y );
	MainWindow.draw_flag=ON;	
	EXEC_AddPC( EscCnt );
}

// 233
static void ESC_EOprResetBmp( void )
{	
	AVG_ReleaseBmp(EscParam[0].num);	
	EXEC_AddPC( EscCnt );
}

// 234
static void ESC_EOprResetBmpAll( void )
{
	int	i;
	for(i=0;i<MAX_SCRIPT_OBJ;i++)
	{
		AVG_ReleaseBmp( i );
	}
	EXEC_AddPC( EscCnt );
}

// 235
static void ESC_EOprSetBmpAnime( void )
{
	if(EscParam[2].num==-1) 
		EscParam[2].num=0;
	if(EscParam[4].num==-1) 
		EscParam[4].num=1;
	if(EscParam[3].num==-1) 
		EscParam[3].num=-1;
	AVG_LoadBmpAnime( EscParam[0].num, 0, EscParam[1].str, EscParam[4].num, EscParam[2].num, EscParam[3].num );
	EXEC_AddPC( EscCnt );
}

// 236
static void ESC_EOprResetBmpAnime( void )
{	
	AVG_ReleaseBmp(EscParam[0].num);	
	EXEC_AddPC( EscCnt );
}

// 237
static void ESC_EOprWaitBmpAnime( void )
{
	if( AVG_WaitBmpAnime( EscParam[0].num ) )
	{	
		EXEC_AddPC( EscCnt );	
	}
}

// 238
static void ESC_EOprSetTitle( void )
{
	AVG_SetGotoTitle();
}

// 239
static void ESC_EOprSetEnding( void )
{	
	if(!EOprFlag[ESC_SETENDING])
	{
		EOprFlag[ESC_SETENDING] = 1;
		if( EscParam[1].num==1)
		{
			AVG_SetMovie( 1, 0 );
		}
		else
		{
			if(EscParam[0].num==10)
			{
				AVG_SetMovie( 1, 0 );
			}
			else
			{
				AVG_SetMovie( 1, EscParam[0].num );
			}	
		}
	}
	if( !AVG_WaitMovie() )
	{
		EOprFlag[ESC_SETENDING] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 240
static void ESC_EOprSetOpening( void )
{
	if(!EOprFlag[ESC_SETOPENING])
	{
		EOprFlag[ESC_SETOPENING] = 1;
		AVG_SetOpening( );
	}
	if( !AVG_WaitMovie() )
	{
		EOprFlag[ESC_SETOPENING] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 241
static void ESC_EOprSetAvi( void )
{
	AVG_SetAvi( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].str );	
	EXEC_AddPC( EscCnt );
}

// 242
static void ESC_EOprResetAvi( void )
{	
	AVG_ResetAvi(EscParam[0].num);	
	EXEC_AddPC( EscCnt );
}

// 243
static void ESC_EOprWaitAvi( void )
{
	if( AVG_WaitAvi(EscParam[0].num) )
	{
		EXEC_AddPC( EscCnt );
	}
}

// 244
static void ESC_EOprSetAviFull( void )
{
	EXEC_AddPC( EscCnt );
}

// 245
static void ESC_EOprWaitAviFull( void )
{
	EXEC_AddPC( EscCnt );
}

// 246
static void ESC_EOprSetBmpDisp( void )
{	
	AVG_SetBmpDisp( EscParam[0].num, EscParam[1].num );	
	EXEC_AddPC( EscCnt );
}

// 247
static void ESC_EOprSetBmpLayer( void )
{	
	AVG_SetBmpLayer( EscParam[0].num, EscParam[1].num );	
	EXEC_AddPC( EscCnt );
}

// 248
static void ESC_EOprSetBmpParam( void )
{
	if(EscParam[2].num==-1) 
		EscParam[2].num=0;	
	AVG_SetBmpParam( EscParam[0].num, DRW_PRM(EscParam[1].num,EscParam[2].num) );
	EXEC_AddPC( EscCnt );
}

// 249
static void ESC_EOprSetBmpRevParam(void)
{	
	AVG_SetBmpRevParam( EscParam[0].num, EscParam[1].num );	
	EXEC_AddPC( EscCnt );
}

// 250
static void ESC_EOprSetBmpBright( void )
{
	EscParam[1].num=LIM(EscParam[1].num,0,255);
	if(EscParam[2].num==-1) 
		EscParam[2].num=EscParam[1].num;
	if(EscParam[3].num==-1) 
		EscParam[3].num=EscParam[1].num;
	AVG_SetBmpBright( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );
	EXEC_AddPC( EscCnt );
}

// 251
static void ESC_EOprSetBmpMove( void )
{	
	AVG_SetBmpMove( EscParam[0].num, EscParam[1].num, EscParam[2].num );	
	EXEC_AddPC( EscCnt );
}

// 252
static void ESC_EOprSetBmpPos( void )
{	
	AVG_SetBmpPos( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num, EscParam[6].num );	
	EXEC_AddPC( EscCnt );
}

// 253
static void ESC_EOprSetBmpZoom( void )
{	
	AVG_SetBmpZoom( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num );	
	EXEC_AddPC( EscCnt );
}

// 254
static void ESC_EOprSetBmpZoom2( void )
{	
	AVG_SetBmpZoom2( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );	
	EXEC_AddPC( EscCnt );
}

// 255
static void ESC_EOprSetDemoFlag(void)
{
	AVG_SetDemoFlag( EscParam[0].num, EscParam[1].num );	
	EXEC_AddPC( EscCnt );
}

// 256
static void ESC_EOprSetSceneNo(void)
{	
	EXEC_AddPC( EscCnt );
}

// 257
static void ESC_EOprSetEndingNo( void )
{	
	EXEC_AddPC( EscCnt );
}

// 258
static void ESC_EOprSetReplayNo( void )
{	
	AVG_SetReplayFlag( EscParam[0].num );	
	EXEC_AddPC( EscCnt );
}

// 259
static void ESC_EOprSetSoundEvent( void )
{
	if(EscParam[6].num==-1) 
		EscParam[6].num = OFF;
	AVG_SetSoundEvent( EscParam[0].num, EscParam[1].num,
						EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num, EscParam[6].num );
	EXEC_AddPC( EscCnt );
}

// 260
static void ESC_EOprSetSoundEventVolume( void )
{
	if(EscParam[2].num==-1) 
		EscParam[2].num = -1;	
	if(EscParam[3].num==-1) 
		EscParam[3].num = 0xff;
	AVG_SetSoundEventVolume( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num );
	EXEC_AddPC( EscCnt );
}

// 261
static void ESC_EOprSetPotaPota( void )
{
	if(EscParam[0].num==-1) 
		EscParam[0].num = 2;
	if(EscParam[1].num==-1) 
		EscParam[1].num = 5;
	if(EscParam[2].num==-1) 
		EscParam[2].num = 128;
	if( AVG_SetPotaPota( EscParam[0].num, EscParam[1].num, EscParam[2].num ) )
	{
		EXEC_AddPC( EscCnt );
	}
}

// 262
static void ESC_EOprGetTime( void )
{
	EscParam[0].reg = STD_timeGetTime();
	EXEC_AddPC( EscCnt );
}

// 263
static void ESC_EOprWaitTime( void )
{
	if( STD_timeGetTime() >= EscParam[0].num )
	{
		EXEC_AddPC( EscCnt );
	}
}

static int	TextExCounter=0;

// 264
static void ESC_EOprSetTextFormat( void )
{
	if(EscParam[1].num==-1) 
		EscParam[1].num = 20;
	if(EscParam[2].num==-1) 
		EscParam[2].num = 0;
	if(EscParam[3].num==-1) 
		EscParam[3].num = 0;
	if(EscParam[4].num==-1) 
		EscParam[4].num = DISP_X;
	if(EscParam[5].num==-1) 
		EscParam[5].num = DISP_Y;
	if(EscParam[6].num==-1) 
		EscParam[6].num = 0;
	if(EscParam[7].num==-1) 
		EscParam[7].num = 0;
	if(EscParam[8].num==-1) 
		EscParam[8].num = 0;
	if(EscParam[9].num==-1) 
		EscParam[9].num = -1;
	if(EscParam[10].num==-1) 
		EscParam[10].num = 0;
	AVG_SetTextFormat(	EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].num, EscParam[4].num, EscParam[5].num,
						EscParam[6].num, EscParam[7].num, EscParam[8].num, EscParam[9].num, EscParam[10].num );
	EXEC_AddPC( EscCnt );
	if(EscParam[0].num==0)
	{
		TextExCounter=0;
	}
}

// 265
static void ESC_EOprSetTextSync( void )
{
	AVG_SetTextSync( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 266
static void ESC_EOprSetText( void )
{
	if(!EOprFlag[ESC_SETTEXT])
	{
		EOprFlag[ESC_SETTEXT] = 1;
		AVG_SetText( EscParam[0].num, EscParam[1].str );
	}
	if( AVG_WaitText(EscParam[0].num, OFF ) )
	{
		EOprFlag[ESC_SETTEXT] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 267
static void ESC_EOprSetTextEx( void )
{
	if(!EOprFlag[ESC_SETTEXTEX])
	{
		EOprFlag[ESC_SETTEXTEX] = 1;
		if(TextExCounter>0)	AVG_SetTextSync( TextExCounter, TextExCounter-1 );
		AVG_SetText( TextExCounter, EscParam[0].str );
	}
	if( AVG_WaitText(TextExCounter, OFF ) )
	{
		EOprFlag[ESC_SETTEXTEX] = 0;
		TextExCounter++;
		EXEC_AddPC( EscCnt );
	}
}

// 268
static void ESC_EOprResetText( void )
{
	AVG_ResetText( EscParam[0].num );
	EXEC_AddPC( EscCnt );
}

// 269
static void ESC_EOprWaitText( void )
{
	if( AVG_WaitText(EscParam[0].num, ON ) )
	{
		EXEC_AddPC( EscCnt );
	}
}

// 270
static void ESC_EOprResetTextAll( void )
{
	TextExCounter=0;
	AVG_ResetTextAll();
	EXEC_AddPC( EscCnt );
}

// 271
static void ESC_EOprSetDemoFedeFlag( void )
{
	EXEC_AddPC( EscCnt );
}

// 272
static void ESC_EOprMov2( void )
{
	EscParam[0].reg = EscParam[1].num;
	EXEC_AddPC( EscCnt );
}

// 273
static void ESC_EOprSin( void )
{
	if(EscParam[2].num==-1) 
		EscParam[2].num = 4096;
	EscParam[0].reg = (int)( sin(( LOOP(EscParam[1].num,3600) * PAI)/1800.0)*EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 274
static void ESC_EOprCos( void )
{
	if(EscParam[2].num==-1) 
		EscParam[2].num = 4096;
	EscParam[0].reg = (int)( cos(( LOOP(EscParam[1].num,3600) * PAI)/1800.0)*EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 275
static void ESC_EOprAbs( void )
{
	EscParam[0].reg = abs(EscParam[1].num);
	EXEC_AddPC( EscCnt );
}

// 276
static void ESC_EOprSetCutCut( void )
{
	MainWindow.cutcut = EscParam[0].num;
	EXEC_AddPC( EscCnt );
}

// 277
static void ESC_EOprSetNoise( void )
{	
	if(!EOprFlag[ESC_SETNOISE])
	{
		EOprFlag[ESC_SETNOISE] = 1;
		if(EscParam[1].num==-1) 
			EscParam[1].num=32;
		if(EscParam[2].num==-1) 
			EscParam[2].num=30;
		AVG_SetNoise( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	}
	if( AVG_WaitNoise() )
	{
		EOprFlag[ESC_SETNOISE] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 278
static void ESC_EOprSetTextDisp(void)
{	
	if(EscParam[1].num==-1) 
		EscParam[1].num = OFF;
	if(EscParam[0].num)
	{
		if(EscParam[1].num)
		{
			AVG_SetHalfTone();
		}
		AVG_SetNovelMessageDisp(ON);
	}
	else
	{
		if(EscParam[1].num)
		{
			AVG_ResetHalfTone();
		}
		AVG_SetNovelMessageDisp(OFF);
	}
	MainWindow.draw_flag = 1;
	EXEC_AddPC( EscCnt );
}

// 279
static void ESC_EOprSetUsoErr( void )
{
	WIN_OsoErrDialog();
	EXEC_AddPC( EscCnt );
}

// 280
static void ESC_EOprLoadScriptNum(void)
{
	LoadScriptNum(EscParam[0].num);
}

// 281
static void ESC_EOprSetRipple( void )
{
	AVG_SetRipple( EscParam[0].num, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 282
static void ESC_EOprSetRippleSet( void )
{
	if( EscParam[2].num == -1) 
		EscParam[2].num = 0;
	AVG_SetRippleSet(  EscParam[0].num, EscParam[1].num, EscParam[2].num );
	EXEC_AddPC( EscCnt );
}

// 283
static void ESC_EOprWaitRipple( void )
{
	if( !AVG_WaitRipple() )
	{
		EXEC_AddPC( EscCnt );
	}
}

// 284
static void ESC_EOprSetRippleLost( void )
{
	if(!EOprFlag[ESC_SETRIPPLELOST])
	{
		EOprFlag[ESC_SETRIPPLELOST] = 1;
		AVG_SetRippleLost( );
	}
	if( !AVG_WaitRippleLost() )
	{
		EOprFlag[ESC_SETRIPPLELOST] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 285
static void ESC_EOprMLW(void)
{
	if( AVG_WaitLoopBGM() )
	{
		EXEC_AddPC( EscCnt );
	}
}

// 286
static void ESC_EOprVT( void )
{
	int	bak_no;
	if(EOprFlag[ESC_VT]==0)
	{
		EOprFlag[ESC_VT] = 1;
		bak_no = EscParam[1].num*10;
		if(EscParam[2].num!=-1) 
			bak_no += EscParam[2].num;
		if(EscParam[3].num==-1) 
			EscParam[3].num = -2;
		if(EscParam[4].num==-1) 
			EscParam[4].num = OFF;
		if(EscParam[5].num==-1) 
			EscParam[5].num = 0;
		if(EscParam[6].num==-1) 
			EscParam[6].num = 0;
		if(EscParam[7].num==-1) 
			EscParam[7].num = 128;	
		AVG_ResetBackHalfTone( bak_no, EscParam[0].num );
		AVG_SetBack( bak_no, EscParam[5].num, EscParam[6].num, EscParam[0].num, EscParam[4].num, 1, EscParam[3].num, EscParam[7].num );
	}
	if( !AVG_WaitBack() )
	{
		EOprFlag[ESC_VT] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 287
static void ESC_EOprHT( void )
{
	int	bak_no;
	if(EOprFlag[ESC_HT]==0)
	{
		EOprFlag[ESC_HT] = 1;
		bak_no = EscParam[1].num*10;
		if(EscParam[2].num!=-1) 
			bak_no += EscParam[2].num;
		if(EscParam[3].num==-1) 
			EscParam[3].num = -2;
		if(EscParam[4].num==-1) 
			EscParam[4].num = OFF;
		if(EscParam[5].num==-1) 
			EscParam[5].num = 0;
		if(EscParam[6].num==-1) 
			EscParam[6].num = 0;
		if(EscParam[7].num==-1) 
			EscParam[7].num = 128;	
		AVG_ResetBackHalfTone( bak_no, EscParam[0].num );
		AVG_SetBack( bak_no, EscParam[5].num, EscParam[6].num, EscParam[0].num, EscParam[4].num, 2, EscParam[3].num, EscParam[7].num );
	}
	if( !AVG_WaitBack() )
	{
		EOprFlag[ESC_HT] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 288
static void ESC_EOprSetMapEvent( void )
{
	AVG_SetMapEvent( EscParam[0].num, EscParam[1].num, EscParam[2].num, EscParam[3].str );
	EXEC_AddPC( EscCnt );
}

// 289
static void ESC_EOprVIB( void )
{
	EXEC_AddPC( EscCnt );
}

// 290
static void ESC_EOprViewCalender(void)
{
	if(AVG_SetCalender(EscParam[0].num, EscParam[1].num))
	{
		EXEC_AddPC(EscCnt);
	}
}

// 291
static void ESC_EOprSetSakura( void )
{
	AVG_SetSakura( EscParam[0].num, EscParam[1].num, EscParam[2].num, 1 );	
	EXEC_AddPC( EscCnt );
}

// 292
static void ESC_EOprStopSakura( void )
{
	AVG_StopSakura();

	EXEC_AddPC( EscCnt );
}

// 293
static void ESC_EOprSkipDate( void )
{
	AVG_SkipDate( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 294
static void ESC_EOprSetMovie( void )
{
	if(!EOprFlag[ESC_SETMOVIE])
	{
		EOprFlag[ESC_SETMOVIE] = 1;
		AVG_SetMovie(EscParam[0].num, 0 );
	}
	if( !AVG_WaitMovie() )
	{
		EOprFlag[ESC_SETMOVIE] = 0;
		EXEC_AddPC( EscCnt );
	}
}

// 295
static void ESC_EOprDebugBox( void )
{
	DebugBox(NULL, _T("[%d][%s]"), EscParam[0].num, EscParam[1].str );
	EXEC_AddPC( EscCnt );
}

// 296
static void ESC_EOprVHFlag( void )
{
	AVG_SetCgFlag( EscParam[0].num, EscParam[1].num );
	EXEC_AddPC( EscCnt );
}

// 297
static void ESC_EOprGetSystemTime( void )
{
	SYSTEMTIME	sys_time;
	GetLocalTime( &sys_time );
	EscParam[0].reg = sys_time.wHour;
	EscParam[1].reg = sys_time.wDay;
	EscParam[2].reg = sys_time.wMonth;
	EscParam[3].reg = sys_time.wYear;
	EXEC_AddPC( EscCnt );
}

int ESC_OprControl( int opr )
{
	int	i=ESC_LOADSCRIPT;
	EscCnt = GetScriptParamControl( opr );
	switch( opr ) 
	{
	case ESC_B:					
		ESC_EOprB();			
		break;	
	
	case ESC_BT:				
		ESC_EOprBT();			
		break;	
	
	case ESC_BC:				
		ESC_EOprBC();			
		break;	

	case ESC_BCT:				
		ESC_EOprBCT();			
		break;	

	case ESC_BD:				
		ESC_EOprBD();			
		break;	
	
	case ESC_BR:				
		ESC_EOprBR();			
		break;	
	
	case ESC_BF:				
		ESC_EOprBF();			
		break;	
	
	case ESC_V:					
		ESC_EOprV();			
		break;	
	
	case ESC_H:					
		ESC_EOprH();			
		break;	
	
	//286
	case ESC_VT:				
		ESC_EOprVT();			
		break;	
	
	case ESC_HT:				
		ESC_EOprHT();			
		break;	
	
	//296
	case ESC_VHFLAG:			
		ESC_EOprVHFlag();		
		break;	

	//73
	case ESC_S:					
		ESC_EOprS();			
		break;	
	
	case ESC_Z:					
		ESC_EOprZ();			
		break;	
	
	case ESC_FI:				
		ESC_EOprFI();			
		break;	
	
	case ESC_FIF:				
		ESC_EOprFIF();			
		break;	
	
	case ESC_FO:				
		ESC_EOprFO();			
		break;	
	
	case ESC_FOF:				
		ESC_EOprFOF();			
		break;	
	
	case ESC_FB:				
		ESC_EOprFB();			
		break;	
	
	case ESC_PFI:				
		ESC_EOprPFI();			
		break;	
	
	case ESC_PFO:				
		ESC_EOprPFO();			
		break;	
	
	case ESC_PWI:				
		ESC_EOprPWI();			
		break;	
	
	case ESC_PWO:				
		ESC_EOprPWO();			
		break;	

	case ESC_Q:					
		ESC_EOprQ();			
		break;

	case ESC_F:					
		ESC_EOprF();			
		break;	

	case ESC_C:					
		ESC_EOprC();			
		break;

	case ESC_CR:				
		ESC_EOprCR();			
		break;	

	case ESC_CP:				
		ESC_EOprCP();			
		break;	
	
	case ESC_CL:				
		ESC_EOprCL();			
		break;	
	
	case ESC_CY:				
		ESC_EOprCY();			
		break;	
	
	case ESC_CB:				
		ESC_EOprCB();			
		break;	
	
	case ESC_CA:				
		ESC_EOprCA();			
		break;	
	
	case ESC_CW:				
		ESC_EOprCW();			
		break;	
	
	case ESC_CRW:				
		ESC_EOprCRW();			
		break;	

	case ESC_W:					
		ESC_EOprW();			
		break;	
	
	case ESC_WR:				
		ESC_EOprWR();			
		break;	

	case ESC_KW:				
		ESC_EOprKW();			
		break;	
	
	case ESC_K:					
		ESC_EOprK();			
		break;	

	case ESC_M:					
		ESC_EOprM();			
		break;	
	
	case ESC_MS:				
		ESC_EOprMS();			
		break;	
	
	case ESC_MP:				
		ESC_EOprMP();			
		break;	
	
	case ESC_MV:				
		ESC_EOprMV();			
		break;	
	
	case ESC_MW:				
		ESC_EOprMW();			
		break;	
	
	// 285
	case ESC_MLW:				
		ESC_EOprMLW();			
		break;	

	case ESC_SE:				
		ESC_EOprSE();			
		break;	
	
	case ESC_SEP:				
		ESC_EOprSEP();			
		break;	
	
	case ESC_SES:				
		ESC_EOprSES();			
		break;	
	
	case ESC_SEW:				
		ESC_EOprSEW();			
		break;	
	
	case ESC_SEV:				
		ESC_EOprSEV();			
		break;	
	
	case ESC_SEVW:				
		ESC_EOprSEVW();			
		break;	

	case ESC_SETVOLUMEVOICECHAR:	
		ESC_EOprSetVolumeVoiceChar();	
		break;

	case ESC_SETVOLUMEVOICESCRIPT:	
		ESC_EOprSetVolumeVoiceScript();	
		break;

	case ESC_SV:				
		ESC_EOprSV();			
		break;	

	case ESC_SVEX:				
		ESC_EOprSVEX();			
		break;	

	case ESC_VV:				
		ESC_EOprVV();			
		break;	

	case ESC_VA:				
		ESC_EOprVA();			
		break;	

	case ESC_VB:				
		ESC_EOprVB();			
		break;	

	case ESC_VC:				
		ESC_EOprVC();			
		break;	

	case ESC_VX:				
		ESC_EOprVX();			
		break;	

	case ESC_VW:				
		ESC_EOprVW();			
		break;	

	case ESC_VS:				
		ESC_EOprVS();			
		break;	

	case ESC_VI:				
		ESC_EOprVI();			
		break;	

	case ESC_R:					
		ESC_EOprR();			
		break;	

	case ESC_RC:				
		ESC_EOprRC();			
		break;	

	case ESC_RR:				
		ESC_EOprRR();			
		break;	

	case ESC_LF:				
		ESC_EOprLF();			
		break;	

	case ESC_WE:				
		ESC_EOprWE();			
		break;	

	case ESC_WER:				
		ESC_EOprWER();			
		break;	

	case ESC_SETFLAG:			
		ESC_EOprSetFlag();			
		break;	

	case ESC_GETFLAG:			
		ESC_EOprGetFlag();			
		break;	

	case ESC_SETGAMEFLAG:		
		ESC_EOprSetGameFlag();		
		break;	

	case ESC_GETGAMEFLAG:		
		ESC_EOprGetGameFlag();		
		break;	

	case ESC_LOADSCRIPT:		
		ESC_EOprLoadScript();		
		break;	

	case ESC_GAMEEND:			
		ESC_EOprGameEnd();			
		break;	

	case ESC_CALLFUNC:			
		ESC_EOprCallFunc();			
		break;	

	case ESC_VIEWCLOCK:			
		ESC_EOprViewClock();	
		break;	

	case ESC_SETWEATHERMODE:	
		ESC_EOprSetWeatherMode();	
		break;	

	case ESC_SETTIMEMODE:		
		ESC_EOprSetTimeMode();		
		break;	

	case ESC_SETTIMEBACK:		
		ESC_EOprSetTimeBack();		
		break;	

	case ESC_SETTIMECHAR:		
		ESC_EOprSetTimeChar();		
		break;	

	case ESC_SETMATRIX:				
		ESC_EOprSetMatrix();			
		break;	

	case ESC_SHOWCHIPBACK:			
		ESC_EOprShowChipBack();			
		break;	
	
	case ESC_POSECHIPBACK:			
		ESC_EOprPoseChipBack();			
		break;	
	
	case ESC_PLAYCHIPBACK:			
		ESC_EOprPlayChipBack();			
		break;	

	case ESC_SETCHIPBACK:			
		ESC_EOprSetChipBack();			
		break;	

	case ESC_SETCHIPPARTS:			
		ESC_EOprSetChipParts();			
		break;	

	case ESC_SETCHIPSCROLL:			
		ESC_EOprSetChipScroll();		
		break;	

	case ESC_SETCHIPSCROLL2:		
		ESC_EOprSetChipScroll2();		
		break;	

	case ESC_SETCHIPSCROLLSPEED:	
		ESC_EOprSetChipScrollSpeed();	
		break;	

	case ESC_WAITCHIPSCROLL:		
		ESC_EOprWaitChipScroll();		
		break;	

	case ESC_WAITCHIPSCROLLSPEED:	
		ESC_EOprWaitChipScrollSpeed();	
		break;	

	case ESC_SETCHIPCHARCASH:		
		ESC_EOprSetChipCharCash();		
		break;	

	case ESC_RESETCHIPCHARCASH:		
		ESC_EOprResetChipCharCash();	
		break;	

	case ESC_SETCHIPCHAR:			
		ESC_EOprSetChipChar();			
		break;	

	case ESC_SETCHIPCHARANI:		
		ESC_EOprSetChipCharAni();		
		break;	

	case ESC_RESETCHIPCHAR:			
		ESC_EOprResetChipChar();		
		break;	

	case ESC_SETCHIPCHARPARAM:		
		ESC_EOprSetChipCharParam();		
		break;	

	case ESC_SETCHIPCHARBRIGHT:		
		ESC_EOprSetChipCharBright();	
		break;	

	case ESC_SETCHIPCHARMOVE:		
		ESC_EOprSetChipCharMove();	
		break;	

	case ESC_SETCHIPCHARMOVE2:		
		ESC_EOprSetChipCharMove2();		
		break;	

	case ESC_SETCHIPCHARMOVESPEED:	
		ESC_EOprSetChipCharMoveSpeed();
		break;	

	case ESC_GETCHIPCHARMOVE:		
		ESC_EOprGetChipCharMove();		
		break;	

	case ESC_GETCHIPCHARMOVESPEED:	
		ESC_EOprGetChipCharMoveSpeed();	
		break;	

	case ESC_COPYCHIPCHARPOS:		
		ESC_EOprCopyChipCharPos();		
		break;	

	case ESC_SETCHIPCHARREV:		
		ESC_EOprSetChipCharRev();		
		break;	

	case ESC_THROUGHCHIPCHARANI:	
		ESC_EOprThroughChipCharAni();	
		break;	

	case ESC_WAITCHIPCHARREPEAT:	
		ESC_EOprWaitChipCharRepeat();	
		break;

	case ESC_WAITCHIPCHARANI:		
		ESC_EOprWaitChipCharAni();		
		break;

	case ESC_WAITCHIPCHARANILOOP:	
		ESC_EOprWaitChipCharAniLoop();	
		break;	
	
	case ESC_WAITCHIPCHARAMOVE:		
		ESC_EOprWaitChipCharMove();		
		break;	

	case ESC_GETBACK:			
		ESC_EOprGetBack();			
		break;

	case ESC_SETBACK:			
		ESC_EOprSetBack();			
		break;	

	case ESC_SETBACK2:			
		ESC_EOprSetBack2();			
		break;	

	//277
	case ESC_SETNOISE:			
		ESC_EOprSetNoise();			
		break;	

	case ESC_SETCG:				
		ESC_EOprSetCg();			
		break;	

	case ESC_SETCG2:			
		ESC_EOprSetCg2();			
		break;	

	case ESC_SETBACKPOS:		
		ESC_EOprSetBackPos();		
		break;	

	case ESC_SETBACKPOSZ:		
		ESC_EOprSetBackPosZ();		
		break;	

	case ESC_SETBACKSCROLL:		
		ESC_EOprSetBackScroll();	
		break;	

	case ESC_SETBACKSCROLLZ:	
		ESC_EOprSetBackScrollZ();	
		break;	

	case ESC_SETBACKCSCOPE:		
		ESC_EOprSetBackCScope();	
		break;	

	case ESC_WAITBACKCSCOPE:	
		ESC_EOprWaitBackCScope();	
		break;	

	case ESC_LOCKBACKCSCOPE:	
		ESC_EOprLockBackCScope();	
		break;	

	case ESC_SETBACKFADEIN:		
		ESC_EOprSetBackFadeIn();	
		break;	

	case ESC_SETBACKFADEOUT:	
		ESC_EOprSetBackFadeOut();	
		break;	

	case ESC_SETBACKFADERGB:	
		ESC_EOprSetBackFadeRGB();	
		break;	

	case ESC_SETSHAKE:			
		ESC_EOprSetShake();			
		break;	

	case ESC_STOPSHAKE:			
		ESC_EOprStopShake();		
		break;	

	case ESC_SETFLASH:			
		ESC_EOprSetFlash();			
		break;	

	case ESC_SETCHAR:			
		ESC_EOprSetChar();			
		break;	

	case ESC_RESETCHAR:			
		ESC_EOprResetChar();		
		break;	

	case ESC_SETCHARPOSE:		
		ESC_EOprSetCharPose();		
		break;	

	case ESC_SETCHARLOCATE:		
		ESC_EOprSetCharLocate();	
		break;	

	case ESC_SETCHARLAYER:		
		ESC_EOprSetCharLayer();	
		break;	

	case ESC_WAITCHAR:			
		ESC_EOprWaitChar();			
		break;	

	case ESC_SETBLOCK:			
		ESC_EOprSetBlock();			
		break;	

	case ESC_SETWINDOW:			
		ESC_EOprSetWindow();		
		break;	

	case ESC_RESETWINDOW:		
		ESC_EOprResetWindow();		
		break;	

	case ESC_SETMESSAGE:		
		ESC_EOprSetMessage();		
		break;	

	case ESC_SETMESSAGE2:		
		ESC_EOprSetMessage2();		
		break;	

	case ESC_SETMESSAGEEX:		
		ESC_EOprSetMessageEx();		
		break;	

	case ESC_SETCHIPMESSAGE:	
		ESC_EOprSetChipMessage();	
		break;	

	case ESC_ADDMESSAGE:		
		ESC_EOprAddMessage();		
		break;	

	case ESC_ADDMESSAGE2:		
		ESC_EOprAddMessage2();		
		break;	

	case ESC_SETMESSAGEWAIT:	
		ESC_EOprSetMessageWait();	
		break;	

	case ESC_RESETMESSAGE:		
		ESC_EOprResetMessage();		
		break;	

	// 278
	case ESC_T:					
		ESC_EOprSetTextDisp();		
		break;

	case ESC_WAITKEY:			
		ESC_EOprWaitKey();			
		break;	

	case ESC_SETSELECTMES:		
		ESC_EOprSetSelectMes();		
		break;	

	case ESC_SETSELECTMESEX:	
		ESC_EOprSetSelectMesEx();	
		break;	

	case ESC_SETSELECT:			
		ESC_EOprSetSelect();		
		break;

	case ESC_SETSELECTEX:		
		ESC_EOprSetSelectEx();		
		break;	

	case ESC_PLAYBGM:			
		ESC_EOprPlayBgm();			
		break;	

	case ESC_PLAYBGMEX:			
		ESC_EOprPlayBgmEx();		
		break;	

	case ESC_STOPBGM:			
		ESC_EOprStopBgm();			
		break;	

	case ESC_STOPBGMEX:			
		ESC_EOprStopBgmEx();		
		break;	

	case ESC_SETVOLUMEBGM:		
		ESC_EOprSetVolumeBgm();		
		break;	

	case ESC_SETVOLUMEBGMEX:	
		ESC_EOprSetVolumeBgmEx();	
		break;	

	case ESC_PLAYSE:			
		ESC_EOprPlaySe();			
		break;	

	case ESC_PLAYSEEX:			
		ESC_EOprPlaySeEx();			
		break;	

	case ESC_STOPSEEX:			
		ESC_EOprStopSeEx();			
		break;	

	case ESC_SETVOLUMESE:		
		ESC_EOprSetVolumeSe();		
		break;	

	case ESC_SETWEATHER:		
		ESC_EOprSetWeather();		
		break;	

	case ESC_CHANGEWEATHER:		
		ESC_EOprChangeWeather();	
		break;	

	case ESC_RESETWEATHER:		
		ESC_EOprResetWeather();		
		break;	

	case ESC_SETLENSFREA:		
		ESC_EOprSetLensFrea();		
		break;	

	case ESC_SETWAVEFFECT:		
		ESC_EOprSetWavEffect();		
		break;	

	case ESC_RESETWAVEFFECT:	
		ESC_EOprResetWavEffect();	
		break;	

	case ESC_SETWARP:			
		ESC_EOprSetWarp();			
		break;	

	case ESC_RESETWARP:			
		ESC_EOprResetWarp();		
		break;	

	case ESC_WAITFRAME:			
		ESC_EOprWaitFrame();		
		break;	

	case ESC_SETBMP:			
		ESC_EOprSetBmp();			
		break;	

	case ESC_SETBMPEX:			
		ESC_EOprSetBmpEx();			
		break;	

	case ESC_SETBMP4BMP:		
		ESC_EOprSetBmp4Bmp();		
		break;	

	case ESC_SETBMPPRIM:		
		ESC_EOprSetBmpPrim();		
		break;	

	case ESC_RESETBMP:			
		ESC_EOprResetBmp();			
		break;	

	case ESC_RESETBMPALL:		
		ESC_EOprResetBmpAll();		
		break;	

	case ESC_SETBMPANIME:		
		ESC_EOprSetBmpAnime();		
		break;	

	case ESC_RESETBMPANIME:		
		ESC_EOprResetBmpAnime();	
		break;	

	case ESC_WAITBMPANIME:		
		ESC_EOprWaitBmpAnime();		
		break;	

	case ESC_SETTITLE:			
		ESC_EOprSetTitle();			
		break;	

	case ESC_SETENDING:			
		ESC_EOprSetEnding();		
		break;	
	
	case ESC_SETOPENING:		
		ESC_EOprSetOpening();		
		break;	

	//246
	case ESC_SETBMPDISP:		
		ESC_EOprSetBmpDisp();		
		break;	

	case ESC_SETBMPLAYER:		
		ESC_EOprSetBmpLayer();		
		break;	

	case ESC_SETBMPPARAM:		
		ESC_EOprSetBmpParam();		
		break;	

	case ESC_SETBMPREVPARAM:	
		ESC_EOprSetBmpRevParam();	
		break;	

	case ESC_SETBMPBRIGHT:		
		ESC_EOprSetBmpBright();		
		break;	

	case ESC_SETBMPMOVE:		
		ESC_EOprSetBmpMove();		
		break;	

	case ESC_SETBMPPOS:			
		ESC_EOprSetBmpPos();		
		break;	

	case ESC_SETBMPZOOM:		
		ESC_EOprSetBmpZoom();		
		break;	

	case ESC_SETBMPZOOM2:		
		ESC_EOprSetBmpZoom2();		
		break;	

	// 241
	case ESC_SETAVI:			
		ESC_EOprSetAvi();			
		break;	

	case ESC_RESETAVI:			
		ESC_EOprResetAvi();			
		break;	

	case ESC_WAITAVI:			
		ESC_EOprWaitAvi();			
		break;	

	case ESC_SETAVIFULL:		
		ESC_EOprSetAviFull();		
		break;	

	case ESC_WAITAVIFULL:		
		ESC_EOprWaitAviFull();		
		break;	

	// 255
	case ESC_SETDEMOFLAG:		
		ESC_EOprSetDemoFlag();		
		break;	

	case ESC_SETSCENENO:		
		ESC_EOprSetSceneNo();		
		break;	

	case ESC_SETENDINGNO:		
		ESC_EOprSetEndingNo();		
		break;	

	case ESC_SETREPLAYNO:		
		ESC_EOprSetReplayNo();		
		break;	

	case ESC_SETSOUNDEVENT:			
		ESC_EOprSetSoundEvent();		
		break;	

	case ESC_SETSOUNDEVENTVOLUME:	
		ESC_EOprSetSoundEventVolume();	
		break;	

	case ESC_SETPOTAPOTA:		
		ESC_EOprSetPotaPota();		
		break;	

	case ESC_GETTIME:			
		ESC_EOprGetTime();			
		break;	

	case ESC_WAITTIME:			
		ESC_EOprWaitTime();			
		break;	

	case ESC_SETTEXTFORMAT:		
		ESC_EOprSetTextFormat();	
		break;	

	case ESC_SETTEXTSYNC:		
		ESC_EOprSetTextSync();		
		break;	
		
	case ESC_SETTEXT:			
		ESC_EOprSetText();			
		break;	

	case ESC_SETTEXTEX:			
		ESC_EOprSetTextEx();		
		break;	

	case ESC_RESETTEXT:			
		ESC_EOprResetText();		
		break;	

	case ESC_WAITTEXT:			
		ESC_EOprWaitText();			
		break;	

	case ESC_RESETTEXTALL:		
		ESC_EOprResetTextAll();		
		break;	

	case ESC_SETDEMOFADEFLAG:	
		ESC_EOprSetDemoFedeFlag();	
		break;	
	
	case ESC_MOV2:				
		ESC_EOprMov2();				
		break;	

	case ESC_SIN:				
		ESC_EOprSin();				
		break;	

	case ESC_COS:				
		ESC_EOprCos();				
		break;	

	case ESC_ABS:				
		ESC_EOprAbs();				
		break;	

	case ESC_SETCUTCUT:			
		ESC_EOprSetCutCut();		
		break;	

	// 279
	case ESC_SETUSOERR:			
		ESC_EOprSetUsoErr();		
		break;	

	case ESC_LOADSCRIPTNUM:		
		ESC_EOprLoadScriptNum();	
		break;	

	case ESC_SETRIPPLE:			
		ESC_EOprSetRipple();		
		break;	

	case ESC_SETRIPPLESET:		
		ESC_EOprSetRippleSet();		
		break;	

	case ESC_WAITRIPPLE:		
		ESC_EOprWaitRipple();		
		break;	

	case ESC_SETRIPPLELOST:		
		ESC_EOprSetRippleLost();	
		break;	

	// 288
	case ESC_SETMAPEVENT:		
		ESC_EOprSetMapEvent();		
		break;
	
	case ESC_VIB:				
		ESC_EOprVIB();		
		break;

	case ESC_VIEWCALENDER:		
		ESC_EOprViewCalender();		
		break;	

	case ESC_SETSAKURA:			
		ESC_EOprSetSakura();		
		break;	
	
	case ESC_STOPSAKURA:		
		ESC_EOprStopSakura();		
		break;	

	case ESC_SKIPDATE:			
		ESC_EOprSkipDate();		
		break;	
		
	case ESC_SETMOVIE:			
		ESC_EOprSetMovie();			
		break;	
	
	case ESC_DEBUGBOX:			
		ESC_EOprDebugBox();			
		break;	
	
	case ESC_GETSYSTEMTIME:		
		ESC_EOprGetSystemTime();			
		break;			
	}
	SetScriptRegisterControl( opr );
	return EScroptOpr[opr-64].ret;
}

#include "CTRL.h"
static HWND	VoiceDialog;
static BOOL CALLBACK VoiceDialogProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	long		px,py;
	switch( wmes )
	{
		case WM_INITDIALOG:
			SetRegistryKey( REG_KEY );
			if( ReadRegistryAmount( _T("pos_x"), &px ) ) 
				px=0;
			if( ReadRegistryAmount( _T("pos_y"), &py ) ) 
				py=0;
			SetWindowPos( hwnd, NULL, px-232, py, 0, 0, SWP_NOSIZE|SWP_NOZORDER );
			break;

		case WM_HSCROLL:
		case WM_VSCROLL:
			px=0;
			break;

		case WM_COMMAND:
			break;

		case WM_DESTROY:
			VoiceDialog = NULL;
			break;

		case WM_ACTIVATE:
			break;

		case WM_CLOSE:
			DestroyWindow( hwnd );
			break;
	}
	return 0;
}

void SetVoiceDialog( int flag )
{
	if(flag)
	{
		if(!VoiceDialog)
		{
			VoiceDialog = CreateDialog( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG10), MainWindow.hwnd, VoiceDialogProc );
			ShowWindow(VoiceDialog,SW_SHOW);
		}
	}
	else
	{
		if(VoiceDialog)
		{
			DestroyWindow( VoiceDialog );
		}
	}
	return ;
}

void AddVoiceDialogMes( char *mes, ...)
{
	if( !Debug.flag ) 
		return ;
	if(VoiceDialog)
	{
		char	buf[1024];
		wvsprintf( buf, mes, (char *)(&mes+1));
		ListAddString( VoiceDialog, IDC_LIST2, buf );
		int	index=ListGetMaxCount(VoiceDialog, IDC_LIST2);
		SendDlgItemMessage(VoiceDialog, IDC_LIST2,LB_SETTOPINDEX, index - 24, 0);
		index = SendDlgItemMessage(VoiceDialog, IDC_LIST2,LB_GETTOPINDEX, 0, 0);
	}
}

static HWND	ScriptDialog;
static BOOL CALLBACK ScriptDialogProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	switch( wmes )
	{
		case WM_INITDIALOG:
			break;
		
		case WM_VSCROLL:
			break;
		
		case WM_COMMAND:
			break;

		case WM_DESTROY:
			ScriptDialog = NULL;
			break;

		case WM_ACTIVATE:
			break;

		case WM_CLOSE:
			DestroyWindow( hwnd );
			break;
	}
	return 0;
}

#include "mm_std.h" //#include <mm_std.h>
#include "comp_pac.h" //#include <comp_pac.h>

char	*PackScriptDir=NULL;

void EXEC_SetPackDir( char *pac_dir )
{
	PackScriptDir = pac_dir;
}

/*
code   > opr
Code   > Opr
Script > Lang
case 文中の EXEC_ を　OPRS_に
case 文中の SCRIPT_ を　SCCODE_OPRS_に
各種並べ替え、改行入れ
コメントは全削除

SCR_ > EXEC_ 一括変換
*/

char		NowLangFileName[64] = "";

char LangOprList[64][32] = 
{
	"Start",	"End",		"MovR",		"MovV",		
	"Swap",		"Rand",		"IfR",		"IfV",		
	"IfElseR",	"IfElseV",	"Loop",		"Goto",		
	"Inc",		"Dec",		"Not",		"Neg",		
	"AddR",		"AddV",		"SubR",		"SubV",		
	"MulR",		"MulV",		"DivR",		"DivV",		
	"ModR",		"ModV",		"AndR",		"AndV",		
	"OrR",		"OrV",		"XorR",		"XorV",		
	"Calc",		"Pusha",	"Popa",		"Call",		
	"Ret",		"Wait",		"TWait",	"Run",		
	"SLoad",	
};

int			RunLangBlockNo = -1;
int			PrevRunLangBlockNo = -1;

void ESC_CodeCheck(void)
{
	TCHAR	buf[1024];
	int	opr,i;
	i=0;

	if(!ScriptDialog)
	{
		ScriptDialog = CreateDialog( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG6), MainWindow.hwnd, ScriptDialogProc );
		ShowWindow(ScriptDialog,SW_SHOW);
	}
	
	
	while( EXEC_LangBuf[ i ]!=0 ) 
	{
		opr = EXEC_LangBuf[ i ];
		if( opr < OPRS_OPREND )
		{
			switch( opr ) 
			{
				case OPRS_END:		
					i+=2;			
					return;			
				
				case OPRS_MOVR:		
					i+=4;			
					break;	
				
				case OPRS_MOVV:		
					i+=7;			
					break;	
				
				case OPRS_SWAP:		
					i+=4;			
					break;	
				
				case OPRS_RAND:		
					i+=3;			
					break;	
				
				case OPRS_IFR:		
					i+=9;			
					break;	
				
				case OPRS_IFV:		
					i+=12;			
					break;	
				
				case OPRS_IFELSER:	
					i+=9;			
					break;	
				
				case OPRS_IFELSEV:	
					i+=12;			
					break;	
				
				case OPRS_LOOP:		
					i+=7;			
					break;	
				
				case OPRS_GOTO:		
					i+=6;			
					break;	

				case OPRS_INC:     	
					i+=3;			
					break;		
				
				case OPRS_DEC:     	
					i+=3;			
					break;		
				
				case OPRS_NOT:     	
					i+=3;			
					break;		
				
				case OPRS_NEG:		
					i+=3;			
					break;	

				case OPRS_ADDR:										
				case OPRS_SUBR:										
				case OPRS_MULR:										
				case OPRS_DIVR:										
				case OPRS_MODR:										
				case OPRS_ANDR:										
				case OPRS_ORR:										
				case OPRS_XORR:	
					i=4;	
					break;						

				case OPRS_ADDV:
				case OPRS_SUBV:
				case OPRS_MULV:
				case OPRS_DIVV:
				case OPRS_MODV:
				case OPRS_ANDV:
				case OPRS_ORV:
				case OPRS_XORV:	
					i+=7;	
					break;

				case OPRS_CALC:	
					i=2;						
					break;	
				
				case OPRS_PUSHA:	
					i=2;						
					break;	
				
				case OPRS_POPA:	
					i=2;						
					break;	
				
				case OPRS_CALL:	
					i=7;						
					break;	
				
				case OPRS_RET:	
					i=2;					
					break;	
				
				case OPRS_WAIT:	
					i=4;					
					break;	
				
				case OPRS_TWAIT:	
					i=4;					
					break;	
				
				case OPRS_RUN:	
					i=2;					
					break;
				
				case OPRS_SLOAD:					
					break;
			}
			DebugPrintf( "%s()\n", LangOprList[opr] );
		}else{
			wsprintf( buf, "%s( ", EventOprList[opr-64] );
			i+=GetScriptParamControlEx( i, opr, buf );
			int len2 = strlen(buf)-4;
			while( !strncmp( buf+len2, "-1, ", 4 ) ){
				len2-=4;
			}
			int len = len2+2;
			if( buf[len]==',' )
			{
				buf[len]=' ';
				buf[len+1]=')';
				buf[len+2]='\0';
			}
			ListAddString( ScriptDialog, IDC_LIST2, buf );
			DebugPrintf( _T("%s\n"), buf );
		}
	}
}



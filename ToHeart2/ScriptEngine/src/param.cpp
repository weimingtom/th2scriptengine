#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include "param.h"
#include "escr.h"
#include "escript.h"
#include "calc.h"
#include "mm_std.h"
#include "exec.h"

//-------------------------------------------------

ESC_PARAM EscParam[15];
int	EscCnt = 0;

//-------------------------------------------------
static int GetScriptParamCal(char *cal)
{
	int	i = 0, j = 0, reg;
	TCHAR buf[256];
	while( cal[i] )
	{
		if(!strncmp( &cal[i], "reg", 3 ))
		{
			i += 3;
			if( isdigit(cal[i]) )
			{
				reg = cal[i] - '0';
				i++;
				if(isdigit(cal[i]))
				{
					reg *= 10;
					reg += cal[i] - '0';
					i++;
				}
				j += wsprintf(&buf[j], _T("%d"), EXEC_LangInfo->reg[reg]);
			}
		}
		else
		{
			buf[j++] = cal[i++];
		}
	}
	buf[j] = '\0';
	return Cal2Disgit( buf );
}

//-------------------------------------------------

int GetScriptParamByte(int pc, int *start)
{
	return EXEC_LangBuf[ pc + (*start)++ ];						
}

int GetScriptParamWord(int pc, int *start)
{
	int	ret;
	ret  = EXEC_LangBuf[ pc + (*start)++ ];	
	ret |= EXEC_LangBuf[ pc + (*start)++ ] << 8;	
	return ret;
}

int GetScriptParam_str( int pc, int *start, TCHAR *buf )
{
	int	i,len;
	len = EXEC_LangBuf[pc + (*start)++];
	for(i=0; i<len; i++)
	{ 
		buf[i] = EXEC_LangBuf[pc + (*start)++]; 
	}
	buf[i] = 0x00;
	return *start;
}

int GetScriptParam_str2(int pc, int *start, TCHAR *buf)
{
	int	i,len;
	len  = EXEC_LangBuf[pc + (*start)++];	
	len |= EXEC_LangBuf[pc + (*start)++] << 8;	
	for(i = 0; i < len; i++) 
	{ 
		buf[i] = EXEC_LangBuf[pc + (*start)++]; 
	}
	buf[i] = 0x00;
	return *start;
}

int GetScriptParam( int pc, int *start,int size )
{
	int mode;
	int	ret;
	mode = EXEC_LangBuf[pc + (*start)++];						
	if(mode == 2)
	{
		int	i,len;
		char buf[256];
		len = EXEC_LangBuf[pc + (*start)++];		
		for(i = 0; i < len; i++) 
		{ 
			buf[i] = EXEC_LangBuf[pc + (*start)++]; 
		}
		buf[i] = 0x00;
		ret = GetScriptParamCal(buf);
	}
	else
	{		
		switch(size)
		{
		default:
		case 1:	
			ret = (unsigned char)EXEC_LangBuf[pc + *start];			
			(*start) += 1;	
			break;	
		
		case 2: 
			ret = EXEC_GetShortValue(&EXEC_LangBuf[pc + *start]);	
			(*start) += 2;
			break;
		
		case 4: 
			ret = EXEC_GetLongValue(&EXEC_LangBuf[pc + *start]);	
			(*start) += 4;
			break;
		}
		if(mode == 0) 
		{ 
			ret = EXEC_LangInfo->reg[ret]; 
		}
	}
	return ret;
}

int GetScriptParamControl(int opr)
{
	int	i;
	int	cnt = 2;
	if(opr < 64)
	{
		DebugPrintf(_T("イベントスクリプトの命令コードとして不適です[%d]"), opr);
	}
	opr -= 64;
	for(i=0;i<15;i++)
	{
		switch( EScroptOpr[opr].type[i] )
		{
		default:	
			DebugPrintf(_T("引数タイプが不適です[%d]"), EScroptOpr[opr].type[i]);
			//TODO:no break;
		
		case ESC_NOT:	
			return cnt;												
		
		case ESC_REG:	
		case ESC_ADD:	
		case ESC_ASC:	
			EscParam[i].num = GetScriptParamByte(EXEC_LangInfo->pc, &cnt);	
			break;	
		
		case ESC_CNT:
			EscParam[i].num = GetScriptParamWord(EXEC_LangInfo->pc, &cnt);	
			break;	

		case ESC_VCNT:	
			EscParam[i].num = GetScriptParamWord(EXEC_LangInfo->pc, &cnt);	
			break;	

		case ESC_NUM:	
			EscParam[i].num = GetScriptParam(EXEC_LangInfo->pc, &cnt, 4);	
			break;	
		
		case ESC_STR:	
			GetScriptParam_str( EXEC_LangInfo->pc, &cnt, EscParam[i].str );	
			break;	
		
		case ESC_STR2:	
			GetScriptParam_str2( EXEC_LangInfo->pc, &cnt, EscParam[i].str );	
			break;	
		
		case ESC_CMP:
			{
				int	param1, param2, param3;
				param1 = EXEC_LangBuf[ EXEC_LangInfo->pc + cnt++ ];	
				param1 = EXEC_GetParam( param1 );						
				param2 = EXEC_LangBuf[ EXEC_LangInfo->pc + cnt++ ];	
				param3 = GetScriptParam( EXEC_LangInfo->pc, &cnt, 4 );		
				switch( param2 ) 
				{
				case CALL_SMALL:	
					EscParam[i].num = ( param1 <  param3 ) ? 1 : 0;		
					break;		
				
				case CALL_ESMALL:	
					EscParam[i].num = ( param1 <= param3 ) ? 1 : 0;		
					break;		
				
				case CALL_LARGE:	
					EscParam[i].num = ( param1 >  param3 ) ? 1 : 0;		
					break;		
				
				case CALL_ELARGE:	
					EscParam[i].num = ( param1 >= param3 ) ? 1 : 0;		
					break;		
				
				case CALL_EQUAL:	
					EscParam[i].num = ( param1 == param3 ) ? 1 : 0;		
					break;		
				
				case CALL_NEQUAL:	
					EscParam[i].num = ( param1 != param3 ) ? 1 : 0;		
					break;		
				}
			}
			break;	
		}
	}
	return cnt;
}

void SetScriptRegisterControl( int opr )
{
	int	i;
	int	cnt = 2;
	if(opr < 64)
	{
		DebugPrintf(_T("イベントスクリプトの命令コードとして不適です[%d]"), opr);
	}
	opr -= 64;
	for(i = 0; i < 15; i++)
	{
		switch(EScroptOpr[opr].type[i])
		{
		default:
			DebugPrintf(_T("引数タイプが不適です[%d]"), EScroptOpr[opr].type[i]);
			//TODO:no break;

		case ESC_NOT:	
			return;												
		
		case ESC_ASC:	
			break;															
		
		case ESC_NUM:	
			break;															
		
		case ESC_STR2:	
			break;
		
		case ESC_STR:	
			break;															
		
		case ESC_REG:	
			EXEC_SetParam( EscParam[i].num, EscParam[i].reg );	
			break;		
		
		case ESC_ADD:	
			break;															
		
		case ESC_CNT:	
			break;															
		
		case ESC_VCNT:	
			break;															
		}
	}
	return;
}

//-------------------------------------------------

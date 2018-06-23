#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include <tchar.h>
#include <windows.h>
#include "mm_std.h"
#include "calc.h"

//----------------------------------------------
typedef struct{
	int		type;	
	int		factor;	
	void	*next;
}CAL_LIST;

static TCHAR * CalStr = NULL;
static int CalP = 0;

static int StackDigit[32];
static int StackP = 0;

//----------------------------------------------

static CAL_LIST *NewList( int type, int factor )
{
	CAL_LIST	*list = (CAL_LIST *)GAlloc( sizeof(CAL_LIST) );
	list->type   = type;
	list->factor = factor;
	list->next   = NULL;
	return list;
}

static void AddList( CAL_LIST *list, CAL_LIST *add_list )
{
	while(list->next)
	{
		list = (CAL_LIST *)list->next;
	}
	list->next     = add_list;
}

static void ReleaseList( CAL_LIST *list )
{
	CAL_LIST *next;
	if(list)
	{
		next = (CAL_LIST *)list->next;
		GFree(list);
		ReleaseList( next );
	}
	return ;
}

static CAL_LIST *factor( void )
{
	BOOL		minus = FALSE;
	int			digit;
	TCHAR		buf[256], i=0;
	while( CalStr[CalP]=='-' )
	{
		minus = !minus;
		CalP++;
	}
	if( !strncmp(&CalStr[CalP], _T("0x"),2) )
	{
		CalP+=2;
		while( isdigit(CalStr[CalP]) ||
				('a'<=CalStr[CalP] && CalStr[CalP]<='f') ||
				('A'<=CalStr[CalP] && CalStr[CalP]<='F') )
		{
			buf[i++]=CalStr[CalP++];
		}
		buf[i] = '\0';
		digit = Str2Hex(buf);
	}
	else
	{
		while( isdigit(CalStr[CalP]) )
		{
			buf[i++] = CalStr[CalP++];
		}
		buf[i]='\0';
		digit = atoi(buf);
	}
	if(minus) 
		digit = -digit;
	return NewList( 0, digit );
}

static CAL_LIST *expr1( void )
{
	CAL_LIST	*list = factor();
	int			type;
	while( CalStr[CalP]=='*' || CalStr[CalP]=='/' || CalStr[CalP]=='%' )
	{
		type = CalStr[CalP];
		CalP++;
		AddList( list, factor() );
		AddList( list, NewList(1,type) );
	}
	return list;
}

static CAL_LIST *expr0( void )
{
	CAL_LIST	*list = expr1();
	int			type;
	while( CalStr[CalP]=='+' || CalStr[CalP]=='-' )
	{
		type = CalStr[CalP];
		CalP++;
		AddList( list, expr1() );
		AddList( list, NewList(1,type) );
	}
	return list;
}

static int GoCal2Digit(TCHAR *cal)
{
	CAL_LIST	*start_list, *work_list;
	int			opr1, opr2;
	CalStr=cal;
	CalP=0;
	work_list = start_list = expr0();
	while(work_list)
	{
		if(work_list->type)
		{
			opr1 = StackDigit[--StackP];
			opr2 = StackDigit[--StackP];
			switch(work_list->factor)
			{
			case '-':	
				opr1 = opr2-opr1;	
				break;
			
			case '+':	
				opr1 = opr2+opr1;	
				break;
			
			case '*':	
				opr1 = opr2*opr1;	
				break;
			
			case '/':	
				opr1 = opr2/opr1;	
				break;
			
			case '%':	
				opr1 = opr2%opr1;	
				break;
			}
			StackDigit[StackP] = opr1;
			StackP++;
		}
		else
		{
			StackDigit[StackP] = work_list->factor;
			StackP++;
		}
		work_list = (CAL_LIST *)work_list->next;
	}
	ReleaseList( start_list );
	StackP=0;
	return StackDigit[0];
}

//----------------------------------------------

int	Cal2Disgit( TCHAR *cal )
{
	TCHAR	buf[256];
	int		i=0, j=0;
	int		flag=0;
	while(1)
	{
		buf[j] = cal[i];
		switch(cal[i])
		{
		case '\0':
			return GoCal2Digit(buf);

		case '[':
			j += wsprintfA( &buf[j], _T("%d"), Cal2Disgit( &cal[i+1] ) )-1;
			while(cal[i]!=']'){ i++; }
			break;

		case ']':
			buf[i]='\0';
			return GoCal2Digit( buf );
		}
		i++;
		j++;
	}
	return 0;
}

//----------------------------------------------


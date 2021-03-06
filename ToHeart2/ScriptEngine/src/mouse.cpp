﻿#include "mm_std.h"
#include "mouse.h"

typedef struct {
	int			mx, my;			
	short		on_win;
	char		bl,bm,br;		
	char		tl,tm,tr;		
	char		btl,btm,btr;	
	char		rtl,rtm,rtr;
	POINT		dragl;
	POINT		dragm;
	POINT		dragr;
	short		wheel;			
	short		wheel_get;
	short		no;			
	int			interval;	
	char		lno;		
	char		flag;
	char		click_through;
	char		same_time;
	short		disp_w, disp_h;		
	long		active;
}MOUSE_STRUCT;
MOUSE_STRUCT	MouseStruct;

typedef struct {
	char		flag;
	short		sx, sy;
	short		w,  h;
	int			rect_no;
}MOUSE_CHECK;
MOUSE_CHECK	MouseCheck[MOUSE_LAYER_MAX][MOUSE_REST_MAX];

void MUS_SetClickThrough( void )
{
	MouseStruct.click_through = 1;
}

void MUS_InitMouse( int disp_w, int disp_h )
{
	int		i,j;
	ZeroMemory( &MouseStruct, sizeof(MOUSE_STRUCT) );
	for( i=0; i<MOUSE_LAYER_MAX ;i++ )
		for( j=0; j<MOUSE_REST_MAX ;j++ )
			ZeroMemory( &MouseCheck[i][j], sizeof(MOUSE_CHECK) );
	MouseStruct.flag = ON;
	MouseStruct.disp_w = disp_w;
	MouseStruct.disp_h = disp_h;
}

void MUS_SetFlag( int flag )
{
	MouseStruct.flag = flag;
}

void MUS_RenewWheel( short wheel )
{
	MouseStruct.wheel_get += wheel;
}

void MUS_RenewMouseButton( int button, int flag )
{
	flag = (flag)? 1: 0;
	switch( button)
	{
	case MOUSE_LBUTTON:	
		MouseStruct.bl=flag;	
		break;

	case MOUSE_RBUTTON:
		MouseStruct.br=flag;
		break;

	case MOUSE_MBUTTON:	
		MouseStruct.bm=flag;
		break;
	}
}

void MUS_RenewMouse( HWND hwnd, int active, int Flag_MenuSelected )
{
	int				i;
	int				x,y;
	int				mx,my;
	MOUSE_CHECK		*mc;
	POINT			mp;
	static int		bl=OFF,bm=OFF,br=OFF;
	static int		lcnt=0, mcnt=0, rcnt=0;
	int				rect_err;
	static int		activ_bak=0;
	GetCursorPos( &mp );
	ScreenToClient( hwnd, &mp );
	MouseStruct.mx = mx = mp.x;
	MouseStruct.my = my = mp.y;
	MouseStruct.active = activ_bak && active;
	activ_bak = active;
	x = mx-400;
	y = my-300;
	MouseStruct.interval = x*x + y*y;
	rect_err = !(0 < mx && 0 < my && MouseStruct.disp_w >= mx && MouseStruct.disp_h >= my);
	MouseStruct.no = -1;
	if( MouseStruct.active )
	{
		if( !rect_err )
		{
			for(i=0; i<MOUSE_REST_MAX ;i++)
			{
				mc = &MouseCheck[MouseStruct.lno][i];
				if( mc->flag )
				{
					if( mc->sx <= mx && mc->sy <= my && mc->sx+mc->w > mx && mc->sy+mc->h > my )
					{
						MouseStruct.no = mc->rect_no;
						break;
					}
				}
			}
		}
	}
	MouseStruct.wheel = MouseStruct.wheel_get;
	if( MouseStruct.wheel_get )
	{
		MouseStruct.wheel_get = 0;
	}
	if( !MouseStruct.click_through && MouseStruct.active )
	{
		if(MouseStruct.same_time)
		{
			if( !(MouseStruct.bl || MouseStruct.br || MouseStruct.bm) )
			{	
				MouseStruct.bl = (GetAsyncKeyState(VK_LBUTTON)&0x8001)?ON:OFF;
				if( !MouseStruct.bl )
				{
					MouseStruct.br = (GetAsyncKeyState(VK_RBUTTON)&0x8001)?ON:OFF;
					if( !MouseStruct.br )
					{
						MouseStruct.bm = (GetAsyncKeyState(VK_MBUTTON)&0x8001)?ON:OFF;
					}
				}
			}
			else
			{	
				if(MouseStruct.bl)	
					MouseStruct.bl = (GetAsyncKeyState(VK_LBUTTON)&0x8001)?ON:OFF;
				else				
					GetAsyncKeyState(VK_LBUTTON);
				if(MouseStruct.br)	
					MouseStruct.br = (GetAsyncKeyState(VK_RBUTTON)&0x8001)?ON:OFF;
				else				
					GetAsyncKeyState(VK_RBUTTON);
				if(MouseStruct.bm)	
					MouseStruct.bm = (GetAsyncKeyState(VK_MBUTTON)&0x8001)?ON:OFF;
				else				
					GetAsyncKeyState(VK_MBUTTON);
				if( !(MouseStruct.bl || MouseStruct.br || MouseStruct.bm) )
				{	
					MouseStruct.bl = (GetAsyncKeyState(VK_LBUTTON)&0x8001)?ON:OFF;
					if( !MouseStruct.bl )
					{
						MouseStruct.br = (GetAsyncKeyState(VK_RBUTTON)&0x8001)?ON:OFF;
						if( !MouseStruct.br)
						{
							MouseStruct.bm = (GetAsyncKeyState(VK_MBUTTON)&0x8001)?ON:OFF;
						}
					}
				}
			}
		}
		else
		{
			MouseStruct.bl = (GetAsyncKeyState(VK_LBUTTON)&0x8001)?ON:OFF;
			MouseStruct.br = (GetAsyncKeyState(VK_RBUTTON)&0x8001)?ON:OFF;
			MouseStruct.bm = (GetAsyncKeyState(VK_MBUTTON)&0x8001)?ON:OFF;
		}
		if(Flag_MenuSelected>0)
		{
			Flag_MenuSelected--;
			MouseStruct.bl = OFF;
			MouseStruct.br = OFF;
			MouseStruct.bm = OFF;
		}
	}
	else
	{
		MouseStruct.click_through = 0;
		GetAsyncKeyState(VK_LBUTTON);
		GetAsyncKeyState(VK_RBUTTON);
		GetAsyncKeyState(VK_MBUTTON);
	}
	if( GetSystemMetrics(SM_SWAPBUTTON))
	{
		int	work = MouseStruct.bl;
		MouseStruct.bl = MouseStruct.br;
		MouseStruct.br = work;
	}
	lcnt = (MouseStruct.bl) ? min(15,lcnt+1) : 0;
	rcnt = (MouseStruct.br) ? min(15,rcnt+1) : 0;
	mcnt = (MouseStruct.bm) ? min(15,mcnt+1) : 0;
	MouseStruct.btl = (lcnt==15) || (lcnt==1);
	MouseStruct.btr = (rcnt==15) || (rcnt==1);
	MouseStruct.btm = (mcnt==15) || (mcnt==1);
	if(bl==OFF && MouseStruct.bl==ON )
	{
		MouseStruct.tl = ON;
		MouseStruct.dragl.x = mx;
		MouseStruct.dragl.y = my;
	}
	else
	{
		MouseStruct.tl = OFF;
	}
	if(br==OFF && MouseStruct.br==ON )
	{
		MouseStruct.tr = ON;
		MouseStruct.dragr.x = mx;
		MouseStruct.dragr.y = my;
	}
	else
	{
		MouseStruct.tr = OFF;
	}
	if(bm==OFF && MouseStruct.bm==ON )
	{
		MouseStruct.tm = ON;
		MouseStruct.dragm.x = mx;
		MouseStruct.dragm.y = my;
	}
	else
	{
		MouseStruct.tm = OFF;
	}
	
	if( bl==ON && MouseStruct.bl==OFF )	
	{	
		MouseStruct.rtl = ON;	
	}
	else
	{	
		MouseStruct.rtl = OFF;	
	}
	if( br==ON && MouseStruct.br==OFF )	
	{	
		MouseStruct.rtr = ON;	
	}
	else
	{	
		MouseStruct.rtr = OFF;	
	}
	if( bm==ON && MouseStruct.bm==OFF )	
	{	
		MouseStruct.rtm = ON;	
	}
	else
	{	
		MouseStruct.rtm = OFF;	
	}
	if( !MouseStruct.bl ) 
	{	
		MouseStruct.dragl.x = -1;	
		MouseStruct.dragl.y = -1;	
	}
	if( !MouseStruct.br ) 
	{	
		MouseStruct.dragr.x = -1;	
		MouseStruct.dragr.y = -1;	
	}
	if( !MouseStruct.bm ) 
	{	
		MouseStruct.dragm.x = -1;	
		MouseStruct.dragm.y = -1;	
	}
	bl = MouseStruct.bl;
	br = MouseStruct.br;
	bm = MouseStruct.bm;
}

int MUS_GetMouseWheel( void )
{
	return MouseStruct.wheel;
}

BOOL MUS_GetMouseButton( int button )
{
	BOOL	ret = FALSE;
	if( 0<=MouseStruct.mx && MouseStruct.mx<MouseStruct.disp_w && 0<=MouseStruct.my && MouseStruct.my<MouseStruct.disp_h && MouseStruct.active )
	{
		ret = MUS_GetMouseButton2( button );
	}
	return ret;
}

BOOL MUS_GetMouseButton2( int button )
{
	BOOL	ret = FALSE;
	switch( button)
	{
	case MOUSE_LBUTTON: 
		ret = MouseStruct.bl;	
		break;
	
	case MOUSE_RBUTTON: 
		ret = MouseStruct.br;	
		break;
	
	case MOUSE_MBUTTON: 
		ret = MouseStruct.bm;	
		break;
	}
	return ret;
}

BOOL MUS_GetMouseTrigger( int button )
{
	BOOL	ret = FALSE;
	if( 0<=MouseStruct.mx && MouseStruct.mx<MouseStruct.disp_w && 0<=MouseStruct.my && MouseStruct.my<MouseStruct.disp_h && MouseStruct.active )
	{	
		ret = MUS_GetMouseTrigger2( button );
	}
	return ret;
}

BOOL MUS_GetMouseTrigger2( int button )
{
	BOOL	ret = FALSE;
	switch( button)
	{
	case MOUSE_LBUTTON: 
		ret = MouseStruct.tl;
		break;
	
	case MOUSE_RBUTTON: 
		ret = MouseStruct.tr;
		break;
	
	case MOUSE_MBUTTON: 
		ret = MouseStruct.tm;	
		break;
	}
	return ret;
}

BOOL MUS_GetMouseBTrigger( int button )
{
	BOOL	ret = FALSE;
	if( 0<=MouseStruct.mx && MouseStruct.mx<MouseStruct.disp_w && 0<=MouseStruct.my && MouseStruct.my<MouseStruct.disp_h && MouseStruct.active )
	{	
		ret = MUS_GetMouseBTrigger2( button );
	}
	return ret;
}

BOOL MUS_GetMouseBTrigger2( int button )
{
	BOOL	ret = FALSE;
	switch( button)
	{
	case MOUSE_LBUTTON: 
		ret = MouseStruct.btl;	
		break;

	case MOUSE_RBUTTON: 
		ret = MouseStruct.btr;	
		break;
	
	case MOUSE_MBUTTON: 
		ret = MouseStruct.btm;	
		break;
	}
	return ret;
}

BOOL MUS_GetMouseOffTrigger( int button )
{
	BOOL	ret = FALSE;
	if( 0<=MouseStruct.mx && MouseStruct.mx<MouseStruct.disp_w && 0<=MouseStruct.my && MouseStruct.my<MouseStruct.disp_h && MouseStruct.active )
	{	
		ret = MUS_GetMouseOffTrigger2( button );
	}
	return ret;
}

BOOL MUS_GetMouseOffTrigger2( int button )
{
	BOOL	ret = FALSE;
	switch( button)
	{
	case MOUSE_LBUTTON: 
		ret = MouseStruct.rtl;	
		break;
	
	case MOUSE_RBUTTON: 
		ret = MouseStruct.rtr;	
		break;
	
	case MOUSE_MBUTTON: 
		ret = MouseStruct.rtm;	
		break;
	}
	return ret;
}

int	MUS_GetMouseNo( int button )
{
	int	ret = -1;
	if(MouseStruct.active)
	{
		if( button != -1 )
		{
			if( MOUSE_LBTRIGGER<=button )
			{
				if( MUS_GetMouseBTrigger( button-8 ) )
				{
					ret = MouseStruct.no;
				}
			}
			else
			{
				if( MUS_GetMouseTrigger( button ) )
				{
					ret = MouseStruct.no;
				}
			}
		}
		else
		{	
			ret = MouseStruct.no;
		}
	}
	return ret;
}

int	MUS_GetMouseNoEx( int button, int lno )
{
	int	ret = -1;
	if(MouseStruct.lno==lno)
	{
		ret = MUS_GetMouseNo( button );
	}
	return ret;
}

void MUS_GetMouseDragPos( int button, int *mx, int *my )
{
	switch( button )
	{
	case MOUSE_LBUTTON:
		*mx = MouseStruct.dragl.x;
		*my = MouseStruct.dragl.y;
		break;

	case MOUSE_RBUTTON:
		*mx = MouseStruct.dragr.x;
		*my = MouseStruct.dragr.y;
		break;

	case MOUSE_MBUTTON:
		*mx = MouseStruct.dragm.x;
		*my = MouseStruct.dragm.y;
		break;
	}
	return ;
}

void MUS_GetMousePos( int *mx, int *my )
{
	*mx = MouseStruct.mx;
	*my = MouseStruct.my;
	return ;
}

int MUS_GetMousePosX( void )
{
	return MouseStruct.mx;
}

int MUS_GetMousePosY( void )
{
	return MouseStruct.my;
}

int	MUS_GetMousePosCustom( int *mx, int *my,int sx,int sy,int pit_w,int pit_h,int w,int h )
{
	int		x,y;
	x = MouseStruct.mx-sx;
	y = MouseStruct.my-sy;
	if( x<0 || y<0 || x>=pit_w*w || y>=pit_h*h ) 
	{
		*mx = -1;
		*my = -1;
		return FALSE;
	}
	else
	{
		*mx = x/pit_w;
		*my = y/pit_h;
		return TRUE;
	}
}

int	MUS_GetMousePosCustom2( int *mx, int *my,int sx,int sy,int pit_w,int pit_h,int pit2_w,int pit2_h,int w,int h )
{
	int		x,y,ret=FALSE;
	int		wrk_w = pit_w+pit2_w;
	int		wrk_h = pit_h+pit2_h;
	*mx = -1;
	*my = -1;
	x = MouseStruct.mx-sx;
	y = MouseStruct.my-sy;
	if( x<0 || y<0 || x>=wrk_w*w-pit2_w || y>=wrk_h*h-pit2_h ) 
	{

	}
	else
	{
		if( (x%wrk_w) < pit_w )		
			*mx = x/wrk_w;
		if( (y%wrk_h) < pit_h )		
			*my = y/wrk_h;
		if( *mx != -1 && *my != -1 )	
			ret = TRUE;
	}
	return ret;
}

int	MUS_GetMouseInterval( void )
{
	return MouseStruct.interval;
}

int	MUS_GetMouseRectFlag( int lno, int no )
{
	return MouseCheck[lno][no].flag;
}

void MUS_SetSomeTimeClickFlag( int flag )
{
	MouseStruct.same_time = !flag;
}

void MUS_SetMouseLayer( int lno )
{
	MouseStruct.lno = lno;
}

int MUS_GetMouseLayer( void )
{
	return MouseStruct.lno;
}

void MUS_SetMousePos( HWND hwnd, int x, int y )
{
	POINT	Point = { x, y };	
	ClientToScreen( hwnd, &Point );
	SetCursorPos( Point.x, Point.y );
}

void MUS_SetMousePosRect( HWND hwnd, int lno, int no )
{
	int	x,y;
	if( no < 0) 
		return ;
	if(!MouseCheck[lno][no].flag)
		return ;
	x = MouseCheck[lno][no].sx;
	y = MouseCheck[lno][no].sy + MouseCheck[lno][no].h-1;
	MUS_SetMousePos( (HWND)hwnd, x, y );
}

void MUS_SetMouseRect( int lno,int no, int sx, int sy, int w, int h,int flag )
{	
	MouseCheck[lno][no].sx = (short)sx;
	MouseCheck[lno][no].sy = (short)sy;
	MouseCheck[lno][no].w  = (short)w;
	MouseCheck[lno][no].h  = (short)h;
	MouseCheck[lno][no].flag = (char)flag;
	MouseCheck[lno][no].rect_no = no;
}

void MUS_SetMouseRectAdd( int lno,int no, int rect_no, int sx, int sy, int w, int h,int flag )
{
	MouseCheck[lno][no].sx = (short)sx;
	MouseCheck[lno][no].sy = (short)sy;
	MouseCheck[lno][no].w  = (short)w;
	MouseCheck[lno][no].h  = (short)h;
	MouseCheck[lno][no].flag = (char)flag;
	MouseCheck[lno][no].rect_no = rect_no;
}

BOOL MUS_GetMouseRect( int lno,int no, int *sx, int *sy, int *w, int *h )
{
	if(sx) 
		*sx = MouseCheck[lno][no].sx;
	if(sy) 
		*sy = MouseCheck[lno][no].sy;
	if(w)  
		*w  = MouseCheck[lno][no].w;
	if(h)  
		*h  = MouseCheck[lno][no].h;
	return MouseCheck[lno][no].flag;
}

void MUS_ResetMouseRect( int lno,int no )
{
	MouseCheck[lno][no].flag = OFF;
}

void MUS_SetMouseRectFlag( int lno, int no, int flag )
{
	MouseCheck[lno][no].flag = flag;
}

void MUS_ResetMouseRect_Layer( int lno )
{
	int	i;
	for( i=0; i<MOUSE_REST_MAX ;i++ )
		MouseCheck[lno][i].flag = OFF;
}

void MUS_ResetMouseRect_All( void )
{
	int	i;
	for( i=0; i<MOUSE_LAYER_MAX ;i++ )
		MUS_ResetMouseRect_Layer( i );
}

void MUS_SetMouseRect_Disp( int lno, int gno )
{

}

void MUS_ResetMouseRect_Disp( int gno )
{

}

void MUS_ShowCursor( int disp )
{
	if(disp)
	{
		while( ShowCursor(TRUE)<0 )
			;
	}
	else
	{
		while( ShowCursor(FALSE)>=0 )
			;
	}
}


#include <tchar.h>
#include "mm_std.h"
#include "bmp.h" //#include <bmp.h> 
#include "draw.h"
#include "comp_pac.h"
#include <math.h>
#include "font.h"
#if 0
#include <stdio.h>
#endif

static TCHAR *PackFontDir=NULL;

void FNT_SetPackDir( TCHAR *pac_dir )
{
	PackFontDir = pac_dir;
}

typedef struct{
	char	*buf;
	int		ftype;
	int		fsize;
	int		width;
	char	*kage;
}FONT_STRUCT;
FONT_STRUCT		FontStruct[8];

#define ANTI_BIT	4
#define ANTI_CNT	(8/ANTI_BIT)

HFONT FNT_SetFont( LPCTSTR face, int h, int w )
{
	HFONT	hFont;
	hFont = CreateFont( h,						
						0,						
						0,						
						0,						
						w,
						FALSE,					
						FALSE,					
						FALSE,					
						SHIFTJIS_CHARSET,		
						OUT_DEFAULT_PRECIS,		
						CLIP_DEFAULT_PRECIS,	
						PROOF_QUALITY,			
						FIXED_PITCH | FF_MODERN,
						face );					
	if(hFont==NULL)
	{
		DebugBox(NULL, _T("フォント作成に失敗しました") );
	}
    return hFont;
}

void FNT_TextOut( HWND hwnd, HFONT hFont, HBITMAP hbmp, int x, int y, int no )
{
	HDC		hDC, hMem;
	TCHAR	buf[3];	
	hDC = GetDC(hwnd);
	hMem = CreateCompatibleDC(hDC);
	SelectObject( hMem, hbmp );
	SelectObject( hMem, hFont );	
	SetTextColor( hMem, (COLORREF)BMP_RGB_F(255,255,255) );	
	SetBkColor(   hMem, (COLORREF)BMP_RGB_F(0,0,0) );		
	if(no/0x100)
	{
		wsprintf( buf, _T("%c%c"), no/0x100, no%0x100 );
		TextOut( hMem, x, y, buf, 2 );
	}
	else
	{
		wsprintf( buf, _T("%c"), no );
		TextOut( hMem, x, y, buf, 1 );
	}	
	DeleteDC(hMem);
	ReleaseDC( hwnd, hDC );
}

void FNT_TextOutStr( HWND hwnd, HFONT hFont, HBITMAP hbmp, int x, int y, TCHAR *str )
{
	HDC		hDC, hMem;
	hDC = GetDC(hwnd);
	hMem = CreateCompatibleDC(hDC);
	SelectObject( hMem, hbmp );
	SelectObject( hMem, hFont );			
	SetTextColor( hMem, (COLORREF)BMP_RGB_F(255,255,255) );	
	SetBkColor(   hMem, (COLORREF)BMP_RGB_F(0,0,0) );		
	TextOut( hMem, x, y, str, _tcslen(str)/*strlen(str)*/ );
	DeleteDC(hMem);
	ReleaseDC( hwnd, hDC );
}

typedef struct {
	int		pointer;
	WORD	st_word;
	WORD	ed_word;
}FONT_POINT;

#define MAX_FONT_GROUP	53

#if 0
static FONT_POINT	FontPoint[MAX_FONT_GROUP] = {
	{    0, '、', '〓' },	{  108, '∈', '∩' },	{  116, '∧', '〇' },	{  123, '∠', '?' },	{  158, '０', '９' },
	{  168, 'Ａ', 'Ｚ' },	{  194, 'ａ', 'ｚ' },	{  220, 'ぁ', 'ん' },	{  303, 'ァ', 'ヶ' },	{  390, 'Α', 'Ω' },
	{  414, 'α', 'ω' },	{  438, 'А', 'Я' },	{  471, 'а', 'я' },	{  505, '─', '╂' },	{  537, '①', '⊿' },
	{  627, '亜', '蔭' },	{  721, '院', '改' },	{  910, '魁', '棄' },	{ 1099, '機', '屈' },	{ 1288, '掘', '向' },
	{ 1477, '后', '刷' },	{ 1666, '察', '周' },	{ 1855, '宗', '飾' },	{ 2044, '拭', '線' },	{ 2233, '繊', '只' },
	{ 2422, '叩', '逓' },	{ 2611, '邸', '入' },	{ 2800, '如', '美' },	{ 2989, '鼻', '朋' },	{ 3178, '法', '癒' },
	{ 3367, '諭', '聯' },	{ 3556, '蓮', '腕' },	{ 3607, '弌', '傲' },	{ 3701, '僉', '咨' },	{ 3890, '咫', '奩' },
	{ 4079, '奸', '廏' },	{ 4268, '廖', '戛' },	{ 4457, '戞', '暼' },	{ 4646, '曄', '檣' },	{ 4835, '檗', '滌' },
	{ 5024, '漾', '珱' },	{ 5213, '瓠', '磬' },	{ 5402, '磧', '紆' },	{ 5591, '紂', '腋' },	{ 5780, '隋', '蕈' },
	{ 5969, '蕁', '襞' },	{ 6158, '襦', '蹊' },	{ 6347, '蹇', '錮' },	{ 6536, '錙', '顰' },	{ 6725, '顱', '鵈' },
	{ 6914, '鵝', '熙' },	{ 7015, '', ''+2 },	{ 7023, '　', '　' }
};
#else
//TODO:这里使用SJIS编码才不会出现乱码，所以不能转为gbk
static FONT_POINT	FontPoint[MAX_FONT_GROUP] = {
	{    0, '丄', '伂' },	{  108, '伕', '伩' },	{  116, '伻', '佄' },	{  123, '佢', '侟' },	{  158, '侽', '俋' },
	{  168, '俙', '倅' },	{  194, '倎', '倸' },	{  220, '偀', '傫' },	{  303, '傽', '儢' },	{  390, '儫', '兌' },
	{  414, '兛', '冎' },	{  438, '凘', '刞' },	{  471, '刾', '剳' },	{  505, '劅', '劸' },	{  537, '嘆', '嚈' },
	{  627, '垷', '堻' },	{  721, '堾', '夵' },	{  910, '夽', '婞' },	{ 1099, '婡', '孅' },	{ 1288, '孈', '岦' },
	{ 1477, '岪', '嶞' },	{ 1666, '嶡', '廃' },	{ 1855, '廆', '忺' },	{ 2044, '怈', '慄' },	{ 2233, '慇', '扅' },
	{ 2422, '扏', '掽' },	{ 2611, '揁', '擖' },	{ 2800, '擛', '旤' },	{ 2989, '旲', '朁' },	{ 3178, '朄', '桙' },
	{ 3367, '桜', '楛' },	{ 3556, '楡', '榬' },	{ 3607, '槦', '橖' },	{ 3701, '橜', '欬' },	{ 3890, '欯', '汓' },
	{ 4079, '汙', '淈' },	{ 4268, '淍', '滭' },	{ 4457, '滰', '濣' },	{ 4646, '濦', '烖' },	{ 4835, '烜', '燑' },
	{ 5024, '郂', '帱' },	{ 5213, '酅', '狳' },	{ 5402, '釦', '恻' },	{ 5591, '鉆', '泓' },	{ 5780, '銨', '潼' },
	{ 5969, '錊', '妩' },	{ 6158, '鍬', '纥' },	{ 6347, '鏎', '琰' },	{ 6536, '鐯', '椟' },	{ 6725, '锧', '辄' },
	{ 6914, '闌', '辘' },	{ 7015, '餈', '餎'+2 },	{ 7023, '丂', '丂' }
};
#endif

typedef struct {
	int		pointer;
	WORD	st_byte;
	WORD	ed_byte;
}FONT_POINT2;

static FONT_POINT2	FontPoint2[3] = {
	{0, '!', '~' },	{94, '?', '?'},	{157, ' ', ' ' } //TODO:???
};

void FNT_DrawTextBuf_T( BMP_T *dest, int dest_x, int dest_y, char *fbuf, int fsize, RECT *clip, int r, int g, int b, int alph, int kage, int han )
{
	int		x, y;
	int		w, w2 = fsize/ANTI_CNT/(han+1);
	int		h, h2 = fsize;
	RGB32	*dest_p;
	char	*src_p;
	int		drev, srev;
	int		work;
	RGB32	rgb[16];
	char	*bld_tbl;
	int		sx=0, sy=0, sw=fsize/(han+1), sh=fsize;
	if( !ClipRect( &dest_x, &dest_y, dest->sx, dest->sy, &sx, &sy, sw, sh, &sw, &sh, clip ) ) 
		return ;
	dest_p = dest->buf + dest_y * dest->sx + dest_x;
	src_p  = fbuf      +     sy * w2       + sx/ANTI_CNT;
	drev = dest->sx - sw;
	srev = w2 - sw/ANTI_CNT;
	w = sw/ANTI_CNT;
	h = sh;
	work = sx % ANTI_CNT;
	if( work )
	{
		dest_p+=ANTI_CNT-work;
		src_p ++;
	}
	if(alph==256)
	{
		if(kage)
		{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						work = *src_p & 0x0f;
						bld_tbl = BlendTable[ 255-(work<<4) ];
						dest_p->b = bld_tbl[dest_p->b];
						dest_p->g = bld_tbl[dest_p->g];
						dest_p->r = bld_tbl[dest_p->r];
					}
					if( *src_p & 0xf0 )
					{
						work = *src_p>>4;
						bld_tbl = BlendTable[ 255-*src_p ];
						(dest_p+1)->b = bld_tbl[(dest_p+1)->b];
						(dest_p+1)->g = bld_tbl[(dest_p+1)->g];
						(dest_p+1)->r = bld_tbl[(dest_p+1)->r];
					}
				}
			}
		}
		else
		{
			for( x=0; x<16 ;x++ )
			{
				rgb[x].r = (r*x)>>4;
				rgb[x].g = (g*x)>>4;
				rgb[x].b = (b*x)>>4;
			}
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						work = *src_p & 0x0f;
						bld_tbl = BlendTable[ 255-(work<<4) ];
						dest_p->b = bld_tbl[dest_p->b] + rgb[work].b;
						dest_p->g = bld_tbl[dest_p->g] + rgb[work].g;
						dest_p->r = bld_tbl[dest_p->r] + rgb[work].r;
					}
					if( *src_p & 0xf0 )
					{
						work = *src_p>>4;
						bld_tbl = BlendTable[ 255-*src_p ];
						(dest_p+1)->b = bld_tbl[(dest_p+1)->b] + rgb[work].b;
						(dest_p+1)->g = bld_tbl[(dest_p+1)->g] + rgb[work].g;
						(dest_p+1)->r = bld_tbl[(dest_p+1)->r] + rgb[work].r;
					}
				}
			}
		}
	}
	else if(alph!=0)
	{
		char	*alp_tbl = BlendTable[alph];
		if(kage)
		{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						work = *src_p & 0x0f;
						bld_tbl = BlendTable[ 255-alp_tbl[work<<4] ];
						dest_p->b = bld_tbl[dest_p->b];
						dest_p->g = bld_tbl[dest_p->g];
						dest_p->r = bld_tbl[dest_p->r];
					}
					if( *src_p & 0xf0 )
					{
						work = *src_p>>4;
						bld_tbl = BlendTable[ 255-alp_tbl[*src_p] ];	
						(dest_p+1)->b = bld_tbl[(dest_p+1)->b];
						(dest_p+1)->g = bld_tbl[(dest_p+1)->g];
						(dest_p+1)->r = bld_tbl[(dest_p+1)->r];
					}
				}
			}
		}
		else
		{
			for( x=0,y=0; x<16 ;x++, y+=alph )
			{
				rgb[x].r = (r*y)>>12;
				rgb[x].g = (g*y)>>12;
				rgb[x].b = (b*y)>>12;
			}
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						work = *src_p & 0x0f;
						bld_tbl = BlendTable[ 255-alp_tbl[work<<4] ];
						dest_p->b = bld_tbl[dest_p->b] + rgb[work].b;
						dest_p->g = bld_tbl[dest_p->g] + rgb[work].g;
						dest_p->r = bld_tbl[dest_p->r] + rgb[work].r;
					}
					if( *src_p & 0xf0 )
					{
						work = *src_p>>4;
						bld_tbl = BlendTable[ 255-alp_tbl[*src_p] ];
						(dest_p+1)->b = bld_tbl[(dest_p+1)->b] + rgb[work].b;
						(dest_p+1)->g = bld_tbl[(dest_p+1)->g] + rgb[work].g;
						(dest_p+1)->r = bld_tbl[(dest_p+1)->r] + rgb[work].r;
					}
				}
			}
		}
	}
}

void FNT_DrawTextBuf_Fx2( BMP_F *dest, int dest_x, int dest_y, char *fbuf, int fsize, RECT *clip, int r, int g, int b, int alph, int kage, int han )
{
	int		x, y;
	RGB24	*dest_p;
	char	*src_p;
	int		drev, srev;
	int		work;
	int		w,w2 = fsize/ANTI_CNT/(han+1);
	int		h,h2 = fsize;
	char	*bld_tbl, *rev_tbl;
	int		sx=0, sy=0, sw=fsize/(han+1), sh=fsize;
	if( !ClipRect( &dest_x, &dest_y, dest->sx, dest->sy, &sx, &sy, sw, sh, &sw, &sh, clip ) ) 
		return ;
	dest_p = dest->buf + dest_y * dest->sx + dest_x;
	src_p  = fbuf      +     sy * w2       + sx/ANTI_CNT;
	if( sx%ANTI_CNT )
	{
		dest_p+=ANTI_CNT-sx%ANTI_CNT;
		src_p ++;
		sw--;
	}
	drev = dest->sx*2 - (sw-sw%ANTI_CNT)*2;
	srev = w2 - sw/ANTI_CNT;
	w = sw/ANTI_CNT;
	h = sh;
	if(alph==0)
	{
		return ;
	}
	else if(alph==256)
	{
		if(kage)
		{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=4, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						rev_tbl = BlendTable16[0x0f-(*src_p&0x0f)];
						dest_p->b = rev_tbl[dest_p->b];
						dest_p->g = rev_tbl[dest_p->g];
						dest_p->r = rev_tbl[dest_p->r];
						(dest_p+1)->b = rev_tbl[(dest_p+1)->b];
						(dest_p+1)->g = rev_tbl[(dest_p+1)->g];
						(dest_p+1)->r = rev_tbl[(dest_p+1)->r];
						(dest_p+dest->sx)->b = rev_tbl[(dest_p+dest->sx)->b];
						(dest_p+dest->sx)->g = rev_tbl[(dest_p+dest->sx)->g];
						(dest_p+dest->sx)->r = rev_tbl[(dest_p+dest->sx)->r];
						(dest_p+dest->sx+1)->b = rev_tbl[(dest_p+dest->sx+1)->b];
						(dest_p+dest->sx+1)->g = rev_tbl[(dest_p+dest->sx+1)->g];
						(dest_p+dest->sx+1)->r = rev_tbl[(dest_p+dest->sx+1)->r];
					}
					if( *src_p & 0xf0 )
					{
						rev_tbl = BlendTable16[0x0f-(*src_p>>4)];
						(dest_p+2)->b = rev_tbl[(dest_p+2)->b];
						(dest_p+2)->g = rev_tbl[(dest_p+2)->g];
						(dest_p+2)->r = rev_tbl[(dest_p+2)->r];
						(dest_p+3)->b = rev_tbl[(dest_p+3)->b];
						(dest_p+3)->g = rev_tbl[(dest_p+3)->g];
						(dest_p+3)->r = rev_tbl[(dest_p+3)->r];
						(dest_p+dest->sx+2)->b = rev_tbl[(dest_p+dest->sx+2)->b];
						(dest_p+dest->sx+2)->g = rev_tbl[(dest_p+dest->sx+2)->g];
						(dest_p+dest->sx+2)->r = rev_tbl[(dest_p+dest->sx+2)->r];
						(dest_p+dest->sx+3)->b = rev_tbl[(dest_p+dest->sx+3)->b];
						(dest_p+dest->sx+3)->g = rev_tbl[(dest_p+dest->sx+3)->g];
						(dest_p+dest->sx+3)->r = rev_tbl[(dest_p+dest->sx+3)->r];
					}
				}
			}
		}
		else
		{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=4, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						bld_tbl = BlendTable16[*src_p&0x0f];
						rev_tbl = BlendTable16[0x0f-(*src_p&0x0f)];
						dest_p->b = rev_tbl[dest_p->b] + bld_tbl[b];
						dest_p->g = rev_tbl[dest_p->g] + bld_tbl[g];
						dest_p->r = rev_tbl[dest_p->r] + bld_tbl[r];
						(dest_p+1)->b = rev_tbl[(dest_p+1)->b] + bld_tbl[b];
						(dest_p+1)->g = rev_tbl[(dest_p+1)->g] + bld_tbl[g];
						(dest_p+1)->r = rev_tbl[(dest_p+1)->r] + bld_tbl[r];
						(dest_p+dest->sx)->b = rev_tbl[(dest_p+dest->sx)->b] + bld_tbl[b];
						(dest_p+dest->sx)->g = rev_tbl[(dest_p+dest->sx)->g] + bld_tbl[g];
						(dest_p+dest->sx)->r = rev_tbl[(dest_p+dest->sx)->r] + bld_tbl[r];
						(dest_p+dest->sx+1)->b = rev_tbl[(dest_p+dest->sx+1)->b] + bld_tbl[b];
						(dest_p+dest->sx+1)->g = rev_tbl[(dest_p+dest->sx+1)->g] + bld_tbl[g];
						(dest_p+dest->sx+1)->r = rev_tbl[(dest_p+dest->sx+1)->r] + bld_tbl[r];
					}
					if( *src_p & 0xf0 )
					{
						bld_tbl = BlendTable16[*src_p>>4];
						rev_tbl = BlendTable16[0x0f-(*src_p>>4)];
						(dest_p+2)->b = rev_tbl[(dest_p+2)->b] + bld_tbl[b];
						(dest_p+2)->g = rev_tbl[(dest_p+2)->g] + bld_tbl[g];
						(dest_p+2)->r = rev_tbl[(dest_p+2)->r] + bld_tbl[r];
						(dest_p+3)->b = rev_tbl[(dest_p+3)->b] + bld_tbl[b];
						(dest_p+3)->g = rev_tbl[(dest_p+3)->g] + bld_tbl[g];
						(dest_p+3)->r = rev_tbl[(dest_p+3)->r] + bld_tbl[r];
						(dest_p+dest->sx+2)->b = rev_tbl[(dest_p+dest->sx+2)->b] + bld_tbl[b];
						(dest_p+dest->sx+2)->g = rev_tbl[(dest_p+dest->sx+2)->g] + bld_tbl[g];
						(dest_p+dest->sx+2)->r = rev_tbl[(dest_p+dest->sx+2)->r] + bld_tbl[r];
						(dest_p+dest->sx+3)->b = rev_tbl[(dest_p+dest->sx+3)->b] + bld_tbl[b];
						(dest_p+dest->sx+3)->g = rev_tbl[(dest_p+dest->sx+3)->g] + bld_tbl[g];
						(dest_p+dest->sx+3)->r = rev_tbl[(dest_p+dest->sx+3)->r] + bld_tbl[r];
					}
				}
			}
		}
	}
	else
	{
		if(kage)
		{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=4, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						rev_tbl = BlendTable[ 255-BlendTable16[*src_p & 0x0f][alph] ];
						dest_p->b = rev_tbl[dest_p->b];
						dest_p->g = rev_tbl[dest_p->g];
						dest_p->r = rev_tbl[dest_p->r];
						(dest_p+1)->b = rev_tbl[(dest_p+1)->b];
						(dest_p+1)->g = rev_tbl[(dest_p+1)->g];
						(dest_p+1)->r = rev_tbl[(dest_p+1)->r];
						(dest_p+dest->sx)->b = rev_tbl[(dest_p+dest->sx)->b];
						(dest_p+dest->sx)->g = rev_tbl[(dest_p+dest->sx)->g];
						(dest_p+dest->sx)->r = rev_tbl[(dest_p+dest->sx)->r];
						(dest_p+dest->sx+1)->b = rev_tbl[(dest_p+dest->sx+1)->b];
						(dest_p+dest->sx+1)->g = rev_tbl[(dest_p+dest->sx+1)->g];
						(dest_p+dest->sx+1)->r = rev_tbl[(dest_p+dest->sx+1)->r];
					}
					if( *src_p & 0xf0 )
					{
						rev_tbl = BlendTable[ 255-BlendTable16[*src_p>>4][alph] ];
						(dest_p+1)->b = rev_tbl[(dest_p+1)->b];
						(dest_p+1)->g = rev_tbl[(dest_p+1)->g];
						(dest_p+1)->r = rev_tbl[(dest_p+1)->r];
						(dest_p+3)->b = rev_tbl[(dest_p+3)->b];
						(dest_p+3)->g = rev_tbl[(dest_p+3)->g];
						(dest_p+3)->r = rev_tbl[(dest_p+3)->r];
						(dest_p+dest->sx+2)->b = rev_tbl[(dest_p+dest->sx+2)->b];
						(dest_p+dest->sx+2)->g = rev_tbl[(dest_p+dest->sx+2)->g];
						(dest_p+dest->sx+2)->r = rev_tbl[(dest_p+dest->sx+2)->r];
						(dest_p+dest->sx+3)->b = rev_tbl[(dest_p+dest->sx+3)->b];
						(dest_p+dest->sx+3)->g = rev_tbl[(dest_p+dest->sx+3)->g];
						(dest_p+dest->sx+3)->r = rev_tbl[(dest_p+dest->sx+3)->r];
					}
				}
			}
		}
		else
		{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=4, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						work = *src_p & 0x0f;
						bld_tbl = BlendTable[ BlendTable16[work][alph] ];
						rev_tbl = BlendTable[ 255-BlendTable16[work][alph] ];
						dest_p->b = rev_tbl[dest_p->b] + bld_tbl[b];
						dest_p->g = rev_tbl[dest_p->g] + bld_tbl[g];
						dest_p->r = rev_tbl[dest_p->r] + bld_tbl[r];
						(dest_p+1)->b = rev_tbl[(dest_p+1)->b] + bld_tbl[b];
						(dest_p+1)->g = rev_tbl[(dest_p+1)->g] + bld_tbl[g];
						(dest_p+1)->r = rev_tbl[(dest_p+1)->r] + bld_tbl[r];
						(dest_p+dest->sx)->b = rev_tbl[(dest_p+dest->sx)->b] + bld_tbl[b];
						(dest_p+dest->sx)->g = rev_tbl[(dest_p+dest->sx)->g] + bld_tbl[g];
						(dest_p+dest->sx)->r = rev_tbl[(dest_p+dest->sx)->r] + bld_tbl[r];
						(dest_p+dest->sx+1)->b = rev_tbl[(dest_p+dest->sx+1)->b] + bld_tbl[b];
						(dest_p+dest->sx+1)->g = rev_tbl[(dest_p+dest->sx+1)->g] + bld_tbl[g];
						(dest_p+dest->sx+1)->r = rev_tbl[(dest_p+dest->sx+1)->r] + bld_tbl[r];
					}
					if( *src_p & 0xf0 )
					{
						work = *src_p>>4;
						bld_tbl = BlendTable[ BlendTable16[work][alph] ];
						rev_tbl = BlendTable[ 255-BlendTable16[work][alph] ];
						(dest_p+2)->b = rev_tbl[(dest_p+2)->b] + bld_tbl[b];
						(dest_p+2)->g = rev_tbl[(dest_p+2)->g] + bld_tbl[g];
						(dest_p+2)->r = rev_tbl[(dest_p+2)->r] + bld_tbl[r];
						(dest_p+3)->b = rev_tbl[(dest_p+3)->b] + bld_tbl[b];
						(dest_p+3)->g = rev_tbl[(dest_p+3)->g] + bld_tbl[g];
						(dest_p+3)->r = rev_tbl[(dest_p+3)->r] + bld_tbl[r];
						(dest_p+dest->sx+2)->b = rev_tbl[(dest_p+dest->sx+2)->b] + bld_tbl[b];
						(dest_p+dest->sx+2)->g = rev_tbl[(dest_p+dest->sx+2)->g] + bld_tbl[g];
						(dest_p+dest->sx+2)->r = rev_tbl[(dest_p+dest->sx+2)->r] + bld_tbl[r];
						(dest_p+dest->sx+3)->b = rev_tbl[(dest_p+dest->sx+3)->b] + bld_tbl[b];
						(dest_p+dest->sx+3)->g = rev_tbl[(dest_p+dest->sx+3)->g] + bld_tbl[g];
						(dest_p+dest->sx+3)->r = rev_tbl[(dest_p+dest->sx+3)->r] + bld_tbl[r];
					}
				}
			}
		}
	}
}

void FNT_DrawTextBuf_Fkage( BMP_F *dest, int dest_x, int dest_y, char *fbuf, int fsize,int width, RECT *clip, int r, int g, int b, int alph, int han )
{
	int		x, y;
	RGB24	*dest_p;
	char	*src_p;
	int		drev, srev;
	int		w,w2 = (han)?(((fsize+1)/2+1)/ANTI_CNT+width):((fsize+1)/ANTI_CNT+width);
	int		h,h2 = fsize+width*2;
	char	*rev_tbl;
	int		sx=0, sy=0, sw=(han)?((fsize+1)/2+width*2):(fsize+width*2), sh=fsize+width*2;
	if( !ClipRect( &dest_x, &dest_y, dest->sx, dest->sy, &sx, &sy, sw, sh, &sw, &sh, clip ) )
		return ;
	dest_p = dest->buf + dest_y * dest->sx + dest_x;
	src_p  = fbuf      +     sy * w2       + sx/ANTI_CNT;
	if( sx%ANTI_CNT )
	{
		dest_p+=ANTI_CNT-sx%ANTI_CNT;
		src_p ++;
		sw--;
	}
	drev = dest->sx - (sw+sw%ANTI_CNT);
	srev = w2 - (sw+1)/ANTI_CNT;
	w = (sw+1)/ANTI_CNT;
	h = sh;
	if(alph==0)
	{
		return ;
	}
	else if(alph==256)
	{
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
		{
			for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
			{
				if( *src_p & 0x0f )
				{
					rev_tbl = BlendTable16[0x0f-(*src_p&0x0f)];
					dest_p->b = rev_tbl[dest_p->b];
					dest_p->g = rev_tbl[dest_p->g];
					dest_p->r = rev_tbl[dest_p->r];
				}
				if( *src_p & 0xf0 )
				{
					rev_tbl = BlendTable16[0x0f-(*src_p>>4)];
					(dest_p+1)->b = rev_tbl[(dest_p+1)->b];
					(dest_p+1)->g = rev_tbl[(dest_p+1)->g];
					(dest_p+1)->r = rev_tbl[(dest_p+1)->r];
				}
			}
		}
	}
	else
	{
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
		{
			for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
			{
				if( *src_p & 0x0f )
				{
					rev_tbl = BlendTable[ 255-BlendTable16[*src_p & 0x0f][alph] ];
					dest_p->b = rev_tbl[dest_p->b];
					dest_p->g = rev_tbl[dest_p->g];
					dest_p->r = rev_tbl[dest_p->r];
				}
				if( *src_p & 0xf0 )
				{
					rev_tbl = BlendTable[ 255-BlendTable16[*src_p>>4][alph] ];
					(dest_p+1)->b = rev_tbl[(dest_p+1)->b];
					(dest_p+1)->g = rev_tbl[(dest_p+1)->g];
					(dest_p+1)->r = rev_tbl[(dest_p+1)->r];
				}
			}
		}
	}
}

void FNT_DrawTextBuf_F( BMP_F *dest, int dest_x, int dest_y, char *fbuf, int fsize, RECT *clip, int r, int g, int b, int alph, int kage, int han )
{
	int		x, y;
	RGB24	*dest_p;
	char	*src_p;
	int		drev, srev;
	int		work;
	int		w,w2;
	int		h,h2 = fsize;
	char	*bld_tbl, *rev_tbl;
	int		sx=0, sy=0, sw=fsize/(han+1), sh=fsize;
	if(han)	
		w2 = (fsize/ANTI_CNT+1)/2;
	else	
		w2 = fsize/ANTI_CNT;
	if( !ClipRect( &dest_x, &dest_y, dest->sx, dest->sy, &sx, &sy, sw, sh, &sw, &sh, clip ) )
		return ;
	dest_p = dest->buf + dest_y * dest->sx + dest_x;
	src_p  = fbuf      +     sy * w2       + sx/ANTI_CNT;
	if( sx%ANTI_CNT )
	{
		dest_p+=ANTI_CNT-sx%ANTI_CNT;
		src_p ++;
		sw--;
	}
	drev = dest->sx - (sw-sw%ANTI_CNT);
	srev = w2 - sw/ANTI_CNT;
	w = sw/ANTI_CNT;
	h = sh;
	if(alph==0)
	{
		return ;
	}
	else if(alph==256)
	{
		if(kage)
		{
			for( y=-1; y < h+1 ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=-1 ; x < w+1 ; x++, dest_p+=2, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						rev_tbl = BlendTable16[0x0f-(*src_p&0x0f)];
						dest_p->b = rev_tbl[dest_p->b];
						dest_p->g = rev_tbl[dest_p->g];
						dest_p->r = rev_tbl[dest_p->r];
					}
					if( *src_p & 0xf0 )
					{
						rev_tbl = BlendTable16[0x0f-(*src_p>>4)];
						(dest_p+1)->b = rev_tbl[(dest_p+1)->b];
						(dest_p+1)->g = rev_tbl[(dest_p+1)->g];
						(dest_p+1)->r = rev_tbl[(dest_p+1)->r];
					}
				}
			}
		}
		else
		{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						bld_tbl = BlendTable16[*src_p&0x0f];
						rev_tbl = BlendTable16[0x0f-(*src_p&0x0f)];
						dest_p->b = rev_tbl[dest_p->b] + bld_tbl[b];
						dest_p->g = rev_tbl[dest_p->g] + bld_tbl[g];
						dest_p->r = rev_tbl[dest_p->r] + bld_tbl[r];
					}
					if( *src_p & 0xf0 )
					{
						bld_tbl = BlendTable16[*src_p>>4];
						rev_tbl = BlendTable16[0x0f-(*src_p>>4)];
						(dest_p+1)->b = rev_tbl[(dest_p+1)->b] + bld_tbl[b];
						(dest_p+1)->g = rev_tbl[(dest_p+1)->g] + bld_tbl[g];
						(dest_p+1)->r = rev_tbl[(dest_p+1)->r] + bld_tbl[r];
					}
				}
			}
		}
	}
	else
	{
		if(kage)
		{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						rev_tbl = BlendTable[ 255-BlendTable16[*src_p & 0x0f][alph] ];
						dest_p->b = rev_tbl[dest_p->b];
						dest_p->g = rev_tbl[dest_p->g];
						dest_p->r = rev_tbl[dest_p->r];
					}
					if( *src_p & 0xf0 )
					{
						rev_tbl = BlendTable[ 255-BlendTable16[*src_p>>4][alph] ];
						(dest_p+1)->b = rev_tbl[(dest_p+1)->b];
						(dest_p+1)->g = rev_tbl[(dest_p+1)->g];
						(dest_p+1)->r = rev_tbl[(dest_p+1)->r];
					}
				}
			}
		}
		else
		{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						work = *src_p & 0x0f;
						bld_tbl = BlendTable[ BlendTable16[work][alph] ];
						rev_tbl = BlendTable[ 255-BlendTable16[work][alph] ];
						dest_p->b = rev_tbl[dest_p->b] + bld_tbl[b];
						dest_p->g = rev_tbl[dest_p->g] + bld_tbl[g];
						dest_p->r = rev_tbl[dest_p->r] + bld_tbl[r];
					}
					if( *src_p & 0xf0 )
					{
						work = *src_p>>4;
						bld_tbl = BlendTable[ BlendTable16[work][alph] ];
						rev_tbl = BlendTable[ 255-BlendTable16[work][alph] ];
						(dest_p+1)->b = rev_tbl[(dest_p+1)->b] + bld_tbl[b];
						(dest_p+1)->g = rev_tbl[(dest_p+1)->g] + bld_tbl[g];
						(dest_p+1)->r = rev_tbl[(dest_p+1)->r] + bld_tbl[r];
					}
				}
			}
		}
	}
}

void FNT_DrawTextBuf_H( BMP_H *dest, int dest_x, int dest_y, char *fbuf, int fsize, RECT *clip, int r, int g, int b, int alph, int kage, int han )
{
	int		x, y;
	WORD	*dest_p;
	char	*src_p;
	int		drev, srev;
	int		work;
	int		w,w2 = fsize/ANTI_CNT/(han+1);
	int		h,h2 = fsize;
	WORD	rgb[16];
	int		sx=0, sy=0, sw=fsize/(han+1), sh=fsize;
	if( !ClipRect( &dest_x, &dest_y, dest->sx, dest->sy, &sx, &sy, sw, sh, &sw, &sh, clip ) ) 
		return ;
	dest_p = dest->buf + dest_y * dest->sx + dest_x;
	src_p  = fbuf      +     sy * w2       + sx/ANTI_CNT;
	drev = dest->sx - sw;
	srev = w2 - sw/ANTI_CNT;
	w = sw/ANTI_CNT;
	h = sh;
	work = sx % ANTI_CNT;
	if( work )
	{
		dest_p+=ANTI_CNT-work;
		src_p ++;
	}
	if(alph==256)
	{
		if(kage)
		{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						work = 15-(*src_p&0x0f);
						*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*work)>>4 )&rgb16.mask_rb)
										| ((((*dest_p&rgb16.mask_g )*work)>>4 )&rgb16.mask_g ) );
					}
					if( *src_p & 0xf0 )
					{
						work = 15-(*src_p>>4);
						*(dest_p+1) = (WORD)( ((((*(dest_p+1)&rgb16.mask_rb)*work)>>4 )&rgb16.mask_rb)
											| ((((*(dest_p+1)&rgb16.mask_g )*work)>>4 )&rgb16.mask_g ) );
					}
				}
			}
		}
		else
		{
			for( x=1; x<=16 ;x++ )
			{
				rgb[x-1] = BMP_RGB_FH( (r*x)>>4, (g*x)>>4, (b*x)>>4 );
			}
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						work = *src_p & 0x0f;
						*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*(15-work))>>4 )&rgb16.mask_rb)
										| ((((*dest_p&rgb16.mask_g )*(15-work))>>4 )&rgb16.mask_g ) ) + rgb[work];
					}
					if( *src_p & 0xf0 )
					{
						work = *src_p>>4;
						*(dest_p+1) = (WORD)( ((((*(dest_p+1)&rgb16.mask_rb)*(15-work))>>4 )&rgb16.mask_rb)
											| ((((*(dest_p+1)&rgb16.mask_g )*(15-work))>>4 )&rgb16.mask_g ) ) + rgb[work];
					}
				}
			}
		}
	}
	else if(alph!=0)
	{
		if(kage)
		{
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						work = 16-((((*src_p&0x0f)+1)*alph)>>8);
						*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*work)>>4 )&rgb16.mask_rb)
										| ((((*dest_p&rgb16.mask_g )*work)>>4 )&rgb16.mask_g ) );
					}
					if( *src_p & 0xf0 )
					{
						work = 16-((((*src_p&0xf0)+0x10)*alph)>>12);
						*(dest_p+1) = (WORD)( ((((*(dest_p+1)&rgb16.mask_rb)*work)>>4 )&rgb16.mask_rb)
											| ((((*(dest_p+1)&rgb16.mask_g )*work)>>4 )&rgb16.mask_g ) );
					}
				}
			}
		}
		else
		{
			for( x=0; x<16 ;x++ )
			{
				rgb[x] = BMP_RGB_FH( (r*x)>>4, (g*x)>>4, (b*x)>>4 );
			}
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev )
			{
				for( x=0 ; x < w ; x++, dest_p+=2, src_p++ )
				{
					if( *src_p & 0x0f )
					{
						work = ((((*src_p&0x0f)+1)*alph)>>8);
						*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*(16-work))>>4 )&rgb16.mask_rb)
										| ((((*dest_p&rgb16.mask_g )*(16-work))>>4 )&rgb16.mask_g ) ) + rgb[work];
					}
					if( *src_p & 0xf0 )
					{
						work = ((((*src_p&0xf0)+0x10)*alph)>>12);
						*(dest_p+1) = (WORD)( ((((*(dest_p+1)&rgb16.mask_rb)*(16-work))>>4 )&rgb16.mask_rb)
											| ((((*(dest_p+1)&rgb16.mask_g )*(16-work))>>4 )&rgb16.mask_g ) ) + rgb[work];
					}
				}
			}
		}
	}
}


BOOL FNT_DrawChar( void *dest, int draw_mode, int dest_x, int dest_y, int moji, int fsize, RECT *clip, int r, int g, int b, int alph, int kage, int han )
{
	char	*buf, *kbuf;
	int		size1,ksize1;
	int		size2,ksize2;
	int		no=-1,i, fno, bai=0;
	BMP_T	*bmp_t;
	BMP_F	*bmp_f;
	BMP_H	*bmp_h;
	BMP_BT	*bmp_b;
	int		baikaku=0;
	int		width;
#if 0
	char kkkkk[1000] = {0};
		for( i=0 ; i<MAX_FONT_GROUP ; i++ )
		{
			sprintf(kkkkk, "{%d, %d, %d}\n", FontPoint[i].pointer, FontPoint[i].st_word, FontPoint[i].ed_word);
			OutputDebugString(kkkkk); 
		}
#endif
	if(dest==NULL) 
		return FALSE;
	for( i=0 ; i<8 ; i++ )
	{
		if( FontStruct[i].fsize==fsize )
		{
			width = FontStruct[i].width;
			size1 = fsize*fsize/ANTI_CNT;
			ksize1 = (fsize+width*2+1)/ANTI_CNT*(fsize+width*2);
			fno=i;
			break;
		}
	}
	if(i==8)
	{
		for( i=0 ; i<8 ; i++ )
		{
			if( FontStruct[i].fsize==fsize/2 )
			{
				width = FontStruct[i].width;
				fsize/=2;
				size1 = fsize*fsize/ANTI_CNT;
				ksize1 = (fsize+width*2+1)/ANTI_CNT*(fsize+width*2);
				fno=i;
				bai=1;
				break;
			}
		}
		if(i==8)
		{
			DebugPrintf( _T("そのサイズのフォントは読みこまれていません[%d]\n"), fsize );
			return FALSE;
		}
	}
	switch( draw_mode )
	{
	case 32:	
		bmp_t = (BMP_T*)dest;	
		break;

	case 24:	
		bmp_f = (BMP_F*)dest;	
		break;

	case 16:	
		bmp_h = (BMP_H*)dest;	
		break;

	case 8:		
		bmp_b = (BMP_BT*)dest;
		break;
	}
	if(han)
	{
		for( i=0 ; i<3 ; i++ )
		{
			if( FontPoint2[i].st_byte <= moji && moji <= FontPoint2[i].ed_byte )
			{
				no = FontPoint2[i].pointer + moji-FontPoint2[i].st_byte;
				break;
			}
		}
		if( i==2 )	
			return TRUE;
		size2 = FontPoint[MAX_FONT_GROUP-1].pointer * ((fsize+1)/ANTI_CNT*fsize);
		size1 = (fsize/2+1)/ANTI_CNT*fsize;
		ksize2 = FontPoint[MAX_FONT_GROUP-1].pointer * ((fsize+width*2+1)/ANTI_CNT*(fsize+width*2));
		ksize1 = (fsize/2+width*2+1)/ANTI_CNT*(fsize+width*2);
	}
	else
	{
		moji = (moji>>8)|((moji<<8)&0xff00);
		for( i=0 ; i<MAX_FONT_GROUP ; i++ )
		{
			if( FontPoint[i].st_word <= moji && moji <= FontPoint[i].ed_word )
			{
				no = FontPoint[i].pointer + moji-FontPoint[i].st_word;
				break;
			}
		}
		if( i==MAX_FONT_GROUP-1 )	
			return TRUE;
		size2 = 0;
		ksize2 = 0;
	}
	if( no==-1 )
	{
		DebugPrintf( _T("描画できない文字が含まれています。\n") );
		return FALSE;
	}
	buf = FontStruct[fno].buf + no * size1 + size2;
	kbuf = FontStruct[fno].kage+4 + no * ksize1 + ksize2;
	if(!FontStruct[fno].kage) kage=OFF;
	if(bai)
	{
		if(kage)
		{
			switch( draw_mode )
			{
			case 32:	
				FNT_DrawTextBuf_T( bmp_t, dest_x+kage, dest_y+kage, buf, fsize, clip, r, g, b, alph, 1, han );	
				break;

			case 24:	
				FNT_DrawTextBuf_Fx2( bmp_f, dest_x+kage, dest_y+kage, buf, fsize, clip, r, g, b, alph, 1, han );	
				break;

			case 16:	
				FNT_DrawTextBuf_H( bmp_h, dest_x+kage, dest_y+kage, buf, fsize, clip, r, g, b, alph, 1, han );	
				break;
			}
		}
		else
		{
			switch( draw_mode )
			{
			case 32:	
				FNT_DrawTextBuf_T( bmp_t, dest_x, dest_y, buf, fsize, clip, r, g, b, alph, 0, han );	
				break;

			case 24:	
				FNT_DrawTextBuf_Fx2( bmp_f, dest_x, dest_y, buf, fsize, clip, r, g, b, alph, 0, han );
				break;

			case 16:	
				FNT_DrawTextBuf_H( bmp_h, dest_x, dest_y, buf, fsize, clip, r, g, b, alph, 0, han );	
				break;
			}
		}
	}
	else
	{
		if(kage)
		{
			switch( draw_mode )
			{
			case 32:	
				FNT_DrawTextBuf_T( bmp_t, dest_x+kage, dest_y+kage, buf, fsize, clip, r, g, b, alph, 1, han );	
				break;

			case 24:
				FNT_DrawTextBuf_Fkage( bmp_f, dest_x-width+kage-1, dest_y-width+kage-1, kbuf, fsize, width, clip, 0, 0, 0, alph, han );
				break;
			
			case 16:	
				FNT_DrawTextBuf_H( bmp_h, dest_x+kage, dest_y+kage, buf, fsize, clip, r, g, b, alph, 1, han );	
				break;
			}
		}
		else
		{
			switch( draw_mode )
			{
			case 32:	
				FNT_DrawTextBuf_T( bmp_t, dest_x, dest_y, buf, fsize, clip, r, g, b, alph, 0, han );	
				break;

			case 24:	
				FNT_DrawTextBuf_F( bmp_f, dest_x, dest_y, buf, fsize, clip, r, g, b, alph, 0, han );	
				break;

			case 16:	
				FNT_DrawTextBuf_H( bmp_h, dest_x, dest_y, buf, fsize, clip, r, g, b, alph, 0, han );
				break;
			}
		}
	}
	return TRUE;
}

void FNT_DrawText( void *dest, int draw_mode, int dest_x, int dest_y, int pich_w, int pich_h, TCHAR *str, int fsize, RECT *clip, int r, int g, int b, int alph, int kage )
{
	int		px=dest_x, py=dest_y;
	int		code;
	if( dest==NULL )
	{ 
		return ;
	}
	while( *str )
	{
		if( (0x81 <= *str && *str <= 0x9f) || (0xe0 <= *str && *str <= 0xea) || (0xf0==*str) )
		{
			if( FNT_DrawChar( dest, draw_mode, px, py, *(WORD*)str, fsize, clip, r, g, b, alph, kage, 0 ) )
			{
				str+=2;
				px += fsize+pich_w;
			}
			else
			{
				str++;
			}
			continue;
		}
		else if( (0x21 <= *str && *str <= 0x7e) || (0xa1 <= *str && *str <= 0xdf) )
		{
			code = *str;
			if( FNT_DrawChar( dest, draw_mode, px, py, code, fsize, clip, r, g, b, alph, kage, 1 ) )
			{
				str++;
				px += fsize/2+pich_w;
			}
			else
			{
				str++;
			}
			continue;
		}
		if( *str=='\n' )
		{	
			px = dest_x;	
			py += fsize+pich_h;
		}	
		str++;
	}
}

void FNT_ChangeFont( int ftype )
{
	int	i, fsize;
	for(i=0;i<8;i++)
	{
		if(FontStruct[i].fsize && FontStruct[i].ftype!=ftype)
		{
			fsize = FontStruct[i].fsize;
			FNT_ReleaseFont( FontStruct[i].fsize );
			FNT_LoadFont( fsize, ftype );
		}
	}
}

void FNT_LoadFont( int fsize, int ftype )
{
	TCHAR buf[256];
	int	i;
	for(i=0;i<8;i++)
	{
		if(FontStruct[i].fsize==fsize || FontStruct[i].fsize==0)
		{
			wsprintf( buf, _T("font%02d.fd%d"), fsize, ftype );
			if( PAC_LoadFile( PackFontDir, buf, &FontStruct[i].buf ) )
			{
				wsprintf( buf, _T("font%02d.fk%d"), fsize, ftype );
				if( PAC_CheckFile( PackFontDir, buf ) )
				{
					if( PAC_LoadFile( PackFontDir, buf, &FontStruct[i].kage ) )
					{
						FontStruct[i].width = *(int*)FontStruct[i].kage;
					}
					else
					{
						DebugPrintf(_T("太字フォントファイルの読み込みに失敗しました\n"));
					}
				}
				FontStruct[i].fsize = fsize;
				FontStruct[i].ftype = ftype;
			}
			else
			{
				DebugPrintf(_T("フォントファイルの読み込みに失敗しました\n"));
			}
			break;
		}
	}
}

void FNT_ReleaseFont( int fsize )
{
	int	i;
	for(i=0;i<8;i++)
	{
		if(FontStruct[i].fsize==fsize)
		{
			GFree(FontStruct[i].buf);
			ZeroMemory( &FontStruct[i], sizeof(FONT_STRUCT) );
		}
	}
}

void FNT_Init( void )
{
	FNT_Term();
}

void FNT_Term( void )
{
	int		i;
	for(i=0;i<8;i++)
	{
		GFree(FontStruct[i].buf);
	}
	ZeroMemory( &FontStruct, sizeof(FONT_STRUCT)*8 );
}

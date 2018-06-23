#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#define STRICT

#include "enum.h" //#include <enum.h>
#include <tchar.h>
#include <windows.h>
#include <mmsystem.h>
#include <ddraw.h>
#include <dsound.h>
#include "ENUM.h"

#define MAX_DEVICES     32

DDDEVICEIDENTIFIER2	DeviceInfo[MAX_DEVICES];
int					g_iMaxDevices = 0;

BOOL WINAPI DDEnumCallback( GUID *pGUID, LPTSTR pDescription, LPTSTR strName, LPVOID pContext )
{
	LPDIRECTDRAW7 pDD = NULL;	
	HRESULT hr;
	if( FAILED( hr = DirectDrawCreateEx( pGUID, (VOID**)&pDD, IID_IDirectDraw7, NULL ) ) )
		return DDENUMRET_CANCEL;
	pDD->GetDeviceIdentifier( &DeviceInfo[g_iMaxDevices], 0 );
	if(pDD) 
	{
		pDD->Release();
		pDD = NULL;
	}
	if( g_iMaxDevices < MAX_DEVICES )
		g_iMaxDevices++;
	else
		return DDENUMRET_CANCEL;
	return DDENUMRET_OK;
}

int	EnumGraphicsDevice( void )
{
	LPDIRECTDRAWENUMERATEEX pDirectDrawEnumerateEx=NULL;
	HINSTANCE               hDDrawDLL = NULL;
	hDDrawDLL = GetModuleHandle(_T("DDRAW"));
	if( NULL == hDDrawDLL )
	{
		MessageBox( NULL, _T("LoadLibrary() FAILED"), _T("DirectDraw"), MB_OK | MB_ICONERROR );
		return NULL;
	}
	g_iMaxDevices=0;
	DirectDrawEnumerate( DDEnumCallback, NULL );
	if( 0 == g_iMaxDevices )
	{
		MessageBox( NULL, _T("No devices to enumerate."), _T("DirectDraw Sample"), MB_OK | MB_ICONERROR );
		return NULL;
	}
	return g_iMaxDevices;
}

char *GetGraphicsDeviceName( int index )
{
	return DeviceInfo[index].szDescription;
}

static int	SoundDeviceNum=0;
static char	SoundDeviceName[MAX_DEVICES][256];

INT_PTR CALLBACK DSoundEnumCallback( GUID* pGUID, LPSTR strDesc, LPSTR strDrvName, VOID* pContext )
{
	if(SoundDeviceNum>=MAX_DEVICES) return FALSE;
	if(pGUID)
	{
		strcpy( SoundDeviceName[SoundDeviceNum++], strDesc );
	}
	return TRUE;
}

int EnumSoundDevice( void )
{
	SoundDeviceNum=0;
	if( DS_OK != DirectSoundEnumerate( (LPDSENUMCALLBACK)DSoundEnumCallback, NULL))
	{
		return 0;
	}
	return SoundDeviceNum;
}

char *GetSoundDeviceName( int index )
{
	return SoundDeviceName[index];
}

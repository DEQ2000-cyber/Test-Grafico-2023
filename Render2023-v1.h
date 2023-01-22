#ifndef __RENDER2023_V1_H__
#define __RENDER2023_V1_H__
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COLOR_RGB(r,g,b) ( (b)<<0|(g)<<8|(r)<<16|(0xff)<<24 )

/* Leer la dll gdi32.dll */
typedef void* (WINAPI *GUB_PROC0)(void*,...); GUB_PROC0 ptrF0 = NULL;
typedef void* (WINAPI *GUB_PROC1)(void*);     GUB_PROC1 ptrF1 = NULL;
typedef void* (WINAPI *GUB_PROC2)();     GUB_PROC2 ptrF2 = NULL;
HMODULE __DLL_GDI32__ = NULL;
void __LOAD_EXTERN_DLL_GDI32__(void){
	__DLL_GDI32__ = LoadLibrary("gdi32.dll");
	ptrF0 = (GUB_PROC0)GetProcAddress( (HMODULE)__DLL_GDI32__, "StretchDIBits" );
	ptrF1 = (GUB_PROC1)GetProcAddress( (HMODULE)__DLL_GDI32__, "DeleteDC" );
	ptrF2 = (GUB_PROC2)GetProcAddress( (HMODULE)__DLL_GDI32__, "GdiFlush" );
}
void __FREE_EXTERN_DLL_GDI32__(void){
	FreeLibrary(__DLL_GDI32__);
	__DLL_GDI32__ = NULL;
    ptrF0 = NULL;
    ptrF1 = NULL;
    ptrF2 = NULL;
}

struct RenderConfig
{
	HWND Window_;
	unsigned char If_Close_Window;
	unsigned int Screen_W,
				 Screen_H,
				 Screen_Z,
				 BufferVideo[640*480];
};

struct RenderConfig RMEGA;

/* MSG */
LRESULT CALLBACK __GUB_APROC__(HWND hwnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_){
    switch (msg_){
        case WM_CLOSE  : printf("CLOSE WINDOWS.\n"); RMEGA.If_Close_Window = 0; DestroyWindow(hwnd_); break;
        case WM_DESTROY: PostQuitMessage(0);  break;
        default: return DefWindowProc (hwnd_, msg_, wParam_, lParam_);
    }
    return 0;
}

void CrearScreen( const unsigned int ancho, const unsigned int alto ){
	RMEGA.Screen_W = ancho;
	RMEGA.Screen_H = alto;
	RMEGA.Screen_Z = ancho * alto;
	while(RMEGA.Screen_Z--)RMEGA.BufferVideo[ RMEGA.Screen_Z ] = 0x0;
	RMEGA.Screen_Z = ancho * alto;
	WNDCLASSEX Wndclassex_;
	Wndclassex_.hInstance=NULL;
    Wndclassex_.lpszClassName="__PASSWORD__";
    Wndclassex_.lpfnWndProc=__GUB_APROC__;
    Wndclassex_.style=CS_DBLCLKS;
    Wndclassex_.cbSize=sizeof(WNDCLASSEX);
    Wndclassex_.hIcon=LoadIcon( NULL, IDI_APPLICATION);
    Wndclassex_.hIconSm=LoadIcon( NULL, IDI_APPLICATION);
    Wndclassex_.hCursor=LoadCursor(NULL, IDC_ARROW);
    Wndclassex_.lpszMenuName=NULL;
    Wndclassex_.cbClsExtra=0;
    Wndclassex_.cbWndExtra=0;
    Wndclassex_.hbrBackground=(HBRUSH)(COLOR_GRAYTEXT+1);
    if (!RegisterClassEx(&Wndclassex_)){
    	printf("ERROR: %s\n","WNDCLASSEX");
    	return;
    };
    RMEGA.Window_ = CreateWindowEx(
		0,
		"__PASSWORD__",
		"Title Test Windows Graphics",
		WS_OVERLAPPEDWINDOW|WS_BORDER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		RMEGA.Screen_W | 16,
		RMEGA.Screen_H + 36,
		HWND_DESKTOP,
		NULL,
		NULL,
		NULL
		);
    ShowWindow(RMEGA.Window_,SW_SHOW);
}

void(*__FUNC_LOOP__)(unsigned int*,int) = NULL;

void StartLoop(){
    BITMAPINFO BitmapInfo_;
	memset(&BitmapInfo_,0,sizeof(BITMAPINFO));
	BitmapInfo_.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    BitmapInfo_.bmiHeader.biPlanes      = 1;
    BitmapInfo_.bmiHeader.biBitCount    = 32;
    BitmapInfo_.bmiHeader.biCompression = BI_RGB;
	BitmapInfo_.bmiHeader.biWidth       = RMEGA.Screen_W;
	BitmapInfo_.bmiHeader.biHeight      = -RMEGA.Screen_H;
	HDC Hdc_ = GetDC( RMEGA.Window_ );
	__LOAD_EXTERN_DLL_GDI32__();
	RMEGA.If_Close_Window = 1;
	clock_t inicio,parada;
	int DeltaTime = 0;
	MSG Msg_;
	while(RMEGA.If_Close_Window){
		Sleep(DeltaTime);
		inicio=clock();
		__FUNC_LOOP__(RMEGA.BufferVideo,DeltaTime);
		ptrF0(Hdc_,0,0,RMEGA.Screen_W,RMEGA.Screen_H,0,0,RMEGA.Screen_W,RMEGA.Screen_H,RMEGA.BufferVideo,BitmapInfo_,DIB_RGB_COLORS,SRCCOPY);
        ptrF2();
        parada=clock();
		parada-=inicio*1000L/CLK_TCK;
        DeltaTime = (parada+(1>>parada));
        // printf("Time: %i\n", (int)parada );
        PeekMessage (&Msg_, NULL, 0, 0,PM_REMOVE);
        TranslateMessage(&Msg_);
        DispatchMessage(&Msg_);
	}
	__FREE_EXTERN_DLL_GDI32__();
}

void SetLoop( void(*__Func__)(unsigned int*,int) ){
	__FUNC_LOOP__ = __Func__;
}

#endif
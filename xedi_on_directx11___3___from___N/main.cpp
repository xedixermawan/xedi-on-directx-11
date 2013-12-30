/*
*  (C) 2013 XediXermawan <edi.ermawan@gmail.com>
*/

#include "stdafx.h"

#include "game.hpp"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::steady_clock;

HINSTANCE g_Instance;
HWND g_Wnd;
double g_fps_measure;
bool g_toggle_line;
bool g_toogle_graphics;
std::string g_rw_local_folder;

enum class APPSCREEN {
    HEIGHT = 480,
    WIDTH  = 800
};

#define XD_APP_TITTLE L"xedi_on_directx11"

XGame Game;

static LRESULT WINAPI WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
	case WM_CREATE:
		g_Wnd = wnd;
		Game.Initialize(g_Wnd, (unsigned int) APPSCREEN::HEIGHT , (unsigned int) APPSCREEN::WIDTH);

		return 0;

	case WM_DESTROY:
		Game.ShutDown();
		PostQuitMessage(0);
		return 0;
	case WM_MOUSEMOVE: {
	    int pposx =   ( LOWORD( lParam ) );
	    int pposy =   ( HIWORD( lParam ) );
		Game.OnHandleInput(wParam , pposx, pposy);
		return 0;
	}
	case WM_LBUTTONDOWN: {
	    int pposx =   ( LOWORD( lParam ) );
	    int pposy =   ( HIWORD( lParam ) );
		Game.OnHandleInput(wParam , pposx, pposy);
		return 0;
	}
	case WM_KEYDOWN: {
		// Game.OnHandleInput(wParam , 0, 0);
		return 0;
	}
	case WM_KEYUP : {
		Game.OnHandleInput(wParam , 0, 0);
	}
	return 0;
	}

	return DefWindowProc(wnd, msg, wParam, lParam);
}

// int WINAPI WinMain(HINSTANCE instance, HINSTANCE, char* cmdLine, int showCmd)
int main() {
	g_toggle_line =true;
	g_toogle_graphics=true;

	g_rw_local_folder ="\\data\\";

	g_Instance = GetModuleHandle(0); ;//instance;

	WNDCLASSEX wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wndClass.hbrBackground = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hInstance = g_Instance;
	wndClass.lpfnWndProc = &WndProc;
	wndClass.lpszClassName = XD_APP_TITTLE;

	ATOM classR = RegisterClassEx(&wndClass);
	assert(classR);

	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
	DWORD exStyle = 0;

	RECT rect = { 0, 0, (long) APPSCREEN::WIDTH , (long) APPSCREEN::HEIGHT };
	AdjustWindowRectEx(&rect, style, FALSE, exStyle);
	g_Wnd = CreateWindowEx(
	            exStyle,
	            XD_APP_TITTLE,
	            XD_APP_TITTLE,
	            style,
	            CW_USEDEFAULT, CW_USEDEFAULT,
	            rect.right - rect.left, rect.bottom - rect.top,
	            NULL,
	            NULL,
	            g_Instance,
	            0);
	assert(g_Wnd);

	steady_clock::time_point _time_old = steady_clock::now();
	steady_clock::time_point _time_now = _time_old;
	double _min_delta = 1 / 60.0f ;
	MSG msg;
	g_fps_measure = 0;
	double buff_g_fps_measure = 0;
	int ifps = 0;
	steady_clock::time_point tp1= steady_clock::now();

	HWND hwnd = GetConsoleWindow();
	ShowWindow(hwnd, 1);
	while( 1 ) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			steady_clock::time_point tp1= steady_clock::now();
			steady_clock::time_point _time_now = steady_clock::now();
			double _delta_time_seconds = ( duration_cast<milliseconds>(_time_now - _time_old).count() ) / 1000.0f;
			_delta_time_seconds= _delta_time_seconds > _min_delta ? _min_delta : _delta_time_seconds ;
			_time_old = _time_now;
			Game.Update(_delta_time_seconds);
			Game.Render();
			steady_clock::time_point tp2= steady_clock::now();
			buff_g_fps_measure = buff_g_fps_measure + ( 1000.0f / ( duration_cast<milliseconds>(tp2 - tp1).count() ) );
			if(ifps++ == 10 ) {
				g_fps_measure = buff_g_fps_measure / 10.0f;
				ifps = 0;
				buff_g_fps_measure =0;
			}
		}
	}
	return (int)msg.wParam;
}

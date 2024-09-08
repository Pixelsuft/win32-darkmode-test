#pragma once
#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>

class App;

class Window {
protected:
	WNDCLASSEXW wc;
	ATOM c_at;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	HWND btn;
	HFONT fnt;
public:
	App* app;
	HWND hwnd;

	Window(App* _app);
	~Window();
};

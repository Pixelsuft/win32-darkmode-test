#include <window.hpp>
#include <shit.hpp>
#include <app.hpp>
#include <cstring>
#include <iostream>
#include <Uxtheme.h>

using namespace Gdiplus;

LRESULT my_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	LRESULT _lr;
	if (UAHWndProc(hwnd, msg, wparam, lparam, &_lr))
		return _lr;
    Window* win = (Window*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rct = { 0, 0, 1024, 768 };

		HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
		SolidBrush bruh(Color(255, 0, 0));
		FillRect(hdc, &rct, brush);
		DeleteObject(brush);

		Graphics g(hdc);
		Pen      pen(Color(255, 0, 0, 255));
		// const FontFamily ofm(L"Segoe UI", NULL);
		// Font font(&ofm, 24.0f, FontStyle::FontStyleRegular, Unit::UnitPixel);
		Rect rr = { 200, 200, 100, 100 };
		g.DrawRectangle(&pen, rr);
		g.DrawLine(&pen, 0, 0, 200, 100);
		// g.DrawString(L"Test", 4, &font, { 100.0f, 100.0f }, &bruh);

		EndPaint(hwnd, &ps);
        return FALSE;
    }
    case WM_SYSCOMMAND: {
        if (wparam == SC_CLOSE) {
			win->app->running = false;
            return FALSE;
        }
        break;
    }
    case WM_CLOSE: {
        win->app->running = false;
        return FALSE;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        return FALSE;
    }
	case WM_COMMAND: {
		if (HIWORD(wparam) == BN_CLICKED)
		{
			int iMID;
			iMID = LOWORD(wparam);
			switch (iMID) {
			case 228:
			{
				MessageBoxW(hwnd, L"You just pushed me!", L"My Program!", MB_OK | MB_ICONINFORMATION);
				break;
			}
			default:
				break;
			}
		}
		break;
	}
	case WM_NCCREATE: {
		LPCREATESTRUCTW lps = (LPCREATESTRUCTW)lparam;
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)lps->lpCreateParams);
#ifdef _MSC_VER
		EnableNonClientDpiScaling(hwnd);
#endif
		break;
	}
    }
	return DefWindowProcW(hwnd, msg, wparam, lparam);
}

Window::Window(App* _app) {
	this->app = _app;
	std::memset(&this->wc, 0, sizeof(this->wc));
	GdiplusStartup(&this->gdiplusToken, &this->gdiplusStartupInput, NULL);
	HMENU menu_bar;
	if (1) {
		// Menu
		menu_bar = CreateMenu();
		HMENU hMenu = CreateMenu();
		HMENU hMenu2 = CreateMenu();
		AppendMenuW(hMenu, MF_STRING, 0, L"&New");
		AppendMenuW(hMenu, MF_STRING, 0, L"&Open");
		AppendMenuW(hMenu2, MF_STRING, 0, L"&XDDDDD");
		AppendMenuW(menu_bar, MF_POPUP, (UINT_PTR)hMenu, L"&File");
		AppendMenuW(menu_bar, MF_POPUP, (UINT_PTR)hMenu2, L"&Help Me Please");
	}
	this->wc.cbSize = sizeof(this->wc);
	this->wc.style = CS_HREDRAW | CS_VREDRAW;
	this->wc.lpfnWndProc = (WNDPROC)my_wnd_proc;
	this->wc.cbClsExtra = 0;
	this->wc.cbWndExtra = 0;
	this->wc.hInstance = (HINSTANCE)GetModuleHandleW(NULL);
	this->wc.hIcon = NULL;
	this->wc.hCursor = (HCURSOR)LoadImageW(
		NULL, (LPCWSTR)IDC_ARROW,
		IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED
	);
	this->wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	this->wc.lpszMenuName = NULL;
	this->wc.lpszClassName = L"MyApp";
	this->wc.hIconSm = NULL;
	this->c_at = RegisterClassExW(&this->wc);
	if (!this->c_at) {
		throw std::runtime_error("Failed to register class");
		return;
	}
	this->hwnd = CreateWindowExW(
		WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_NOINHERITLAYOUT,
		this->wc.lpszClassName, L"My Window",
		WS_CAPTION |
		WS_SYSMENU |
		WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX |
		WS_MAXIMIZE |
		WS_MINIMIZE |
		WS_THICKFRAME |
		WS_OVERLAPPED,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600,
		NULL, menu_bar, this->wc.hInstance, this
	);
	if (!this->hwnd) {
		throw std::runtime_error("Failed to create window");
		return;
	}
	fix_win32_theme(this->hwnd);
	this->btn = CreateWindowExW(
		0,
		L"BUTTON",
		L"OK",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		200,
		200,
		100,
		100,
		this->hwnd,
		(HMENU)228,
		this->wc.hInstance,
		NULL
	);
	this->fnt = CreateFontW(48, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 0, L"Segoe UI");
	SendMessageW(this->btn, WM_SETFONT, (WPARAM)this->fnt, MAKELPARAM(TRUE, 0));
	win_shit.AllowDarkModeForWindow(this->btn, true);
	win_shit.AllowDarkModeForWindow((HWND)menu_bar, true);

	SetWindowTheme(this->btn, L"Explorer", NULL);

	SendMessageW(this->hwnd, WM_THEMECHANGED, 0, 0);
	SendMessageW(this->btn, WM_THEMECHANGED, 0, 0);
	SendMessageW((HWND)menu_bar, WM_THEMECHANGED, 0, 0);
	ShowWindow(this->hwnd, 1);
}

Window::~Window() {
	if (this->hwnd == NULL)
		return;
	DeleteObject(this->fnt);
	GdiplusShutdown(this->gdiplusToken);
	DestroyWindow(this->hwnd);
	fix_cleanup();
	this->hwnd = NULL;
	UnregisterClassW(this->wc.lpszClassName, this->wc.hInstance);
}

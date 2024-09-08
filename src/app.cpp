#include <app.hpp>
#include <window.hpp>
#include <dwmapi.h>

App::App() {
    this->msg = { 0 };
    this->uxt = LoadLibraryExW(L"uxtheme.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_IGNORE_CODE_AUTHZ_LEVEL);
	if (1) {
		// Attach Console
        if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
            if (GetLastError() != ERROR_ACCESS_DENIED)
                AllocConsole();
        }
	}
	this->running = false;
#ifdef _MSC_VER
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#endif
	this->win = new Window(this);
}

App::~App() {
	delete this->win;
    FreeLibrary(this->uxt);
}

void App::run() {
	this->running = true;
	while (this->running) {
        while (PeekMessageW(&this->msg, NULL, 0, 0, PM_REMOVE)) {
            if (this->msg.message == WM_QUIT) {
                this->running = false;
                continue;
            }
            TranslateMessage(&this->msg);
            DispatchMessageW(&this->msg);
        }
        InvalidateRect(this->win->hwnd, NULL, FALSE);
        UpdateWindow(this->win->hwnd);
        DwmFlush();
	}
}

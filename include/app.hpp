#pragma once
#include <config.hpp>
#include <Windows.h>

class Window;

class App {
protected:
	Window* win;
	MSG msg;
public:
	HMODULE uxt;
	bool running;

	App();
	~App();
	void run();
};

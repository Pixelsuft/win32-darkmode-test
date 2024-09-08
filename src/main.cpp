#include <app.hpp>
#include <minstd.hpp>

static void main_func() {
	App* app = new App;
	app->run();
	delete app;
}

#if 1
int main(int argc, char* argv[]) {
	UNUSED(argc);
	UNUSED(argv);
	main_func();
	return 0;
}
#else
#include <Windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd) {
	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(lpCmdLine);
	UNUSED(nShowCmd);
	main_func();
	ExitProcess(0);
	return 0;
}
#endif

#include "Boturi.h"
#include <Windows.h>

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow)
{
	GameConfiguration g = {};

	g.width = 800;
	g.height = 450;

	g.title = "New Game";
	g.debugMode = false;
	Boturi::init(g);

	Boturi::exit();
	return 0;
}
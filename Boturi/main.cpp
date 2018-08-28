#include "Boturi.h"

int main(int argc, char* argv[])
{
	GameConfiguration g = {};
	g.fullscreen = false;
	g.resizable = true;
	g.title = "New Game";
	g.debugMode = false;

	Boturi::init(g);

	Boturi::exit();
	return 0;
}
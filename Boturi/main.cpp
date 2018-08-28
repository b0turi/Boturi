#include "Boturi.h"

int main(int argc, char* argv[])
{
	GameConfiguration g = {};
	g.debugMode = false;

	g.title = "New Game";
	g.setVersion(2);

	g.fullscreen = false;
	g.resizable = true;

	g.msaaSamples = VK_SAMPLE_COUNT_8_BIT;

	g.fpsCap = 200;
	g.vSync = false;

	Boturi::init(g);

	Boturi::exit();
	return 0;
}
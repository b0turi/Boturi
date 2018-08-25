#include "Boturi.h"

int main()
{
	GameConfiguration g = {};

	g.width = 800;
	g.height = 450;

	g.title = "New Game";
	g.debugMode = true;
	Boturi::init(g);

	Boturi::exit();
	std::getchar();
	return 0;
}
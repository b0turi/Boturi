#include "Boturi.h"

int main(int argc, char* argv[])
{
	GameConfiguration g = GameConfiguration::loadFromFile("config");

	Boturi::init(g);

	Boturi::meshes["Cube"] = Mesh("models/cube.obj");
	Boturi::textures["Cage"] = Texture("textures/asdf.jpg");

	Boturi::addGameObject(GameObject("Cube", "Cage", "default"));

	Boturi::run();

	Boturi::exit();
	std::getchar();
	return 0;
}
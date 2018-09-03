#include "Boturi.h"

int main(int argc, char* argv[])
{
	GameConfiguration g = GameConfiguration::loadFromFile("config");

	Boturi::init(g);

	Boturi::meshes["Cube"] = Mesh("models/cube.obj");
	Boturi::textures["Cage"] = Texture("textures/asdf.jpg");

	GameObject cube1 = GameObject("Cube", "Cage", "default");
	
	GameObject cube2 = GameObject("Cube", "Cage", "default");
	cube2.move(glm::vec3(0, 0, 1));

	Boturi::addGameObject(cube1);
	Boturi::addGameObject(cube2);

	bool shouldEnd = false;
	const int xDef = Boturi::extent.width / 2;
	const int yDef = Boturi::extent.height / 2;

	SDL_ShowCursor(SDL_FALSE);

	while (!shouldEnd)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT &&
				event.window.event == SDL_WINDOWEVENT_CLOSE))
				shouldEnd = true;

			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
				Boturi::refresh();

			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
				shouldEnd = true;
		}
		Boturi::draw();
		int mouseX, mouseY;
		uint32_t mouseState = SDL_GetMouseState(&mouseX, &mouseY);
		Boturi::camera.rotate(mouseX - xDef, mouseY - yDef);

		SDL_WarpMouseInWindow(Boturi::window, xDef, yDef);
	}


	Boturi::exit();
	return 0;
}
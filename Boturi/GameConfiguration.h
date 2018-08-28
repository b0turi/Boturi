#pragma once

/*
Holds information about the configuration of the game. Can be loaded from a .config file
*/
struct GameConfiguration
{
private:
	uint32_t version = VK_MAKE_VERSION(1, 0, 0);
public:
	int width = 1280;
	int height = 720;
	
	const char * title;
	bool debugMode = false;

	bool fullscreen = false;
	bool resizable = false;

	uint32_t getVersion() { return version; }
	void setVersion(int major, int minor, int patch) 
	{ 
		// Use Vulkan's tool for software version numbers
		version = VK_MAKE_VERSION(major, minor, patch); 
	}
};
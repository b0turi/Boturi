#pragma once

struct GameConfiguration
{
private:
	uint32_t version = VK_MAKE_VERSION(1, 0, 0);
public:
	int width;
	int height;
	
	const char * title;
	bool debugMode = false;

	uint32_t getVersion() { return version; }
	void setVersion(int major, int minor, int patch) 
	{ 
		// Use Vulkan's tool for software version numbers
		version = VK_MAKE_VERSION(major, minor, patch); 
	}
};
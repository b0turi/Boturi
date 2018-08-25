#pragma once
#include <vector>

struct GameConfiguration
{
private:
	bool debugMode = false;
	std::vector<const char *> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	uint32_t version = VK_MAKE_VERSION(1, 0, 0);
public:
	int width;
	int height;
	
	const char * title;
	std::vector<const char *> validationLayers;

	bool getDebugMode() { return debugMode; }
	void setDebugMode(bool mode)
	{
		debugMode = mode;
		if (mode)
			validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	uint32_t getVersion() { return version; }
	void setVersion(int major, int minor, int patch) 
	{ 
		// Use Vulkan's tool for software version numbers
		version = VK_MAKE_VERSION(major, minor, patch); 
	}
};
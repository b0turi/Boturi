#pragma once

#include <algorithm>
#include <iostream>
// Holds information about the configuration of the game. Can be loaded from a .config file
// A menu can be created in the game to adjust the current configuration and update the .config
// file so future sessions will have the same configuration
struct GameConfiguration
{
private:
	// The version of the game. Uses the Vulkan Macro VK_MAKE_VERSION to convert x.x.x to uint32_t
	uint32_t version = VK_MAKE_VERSION(1, 0, 0);
public:
	// Enables or Disables Vulkan's validation layers to print any error messages or
	// warnings handled by the Vulkan SDK, and enables or disables a log of all 
	// creations of Vulkan objects.
	bool debugMode = false;

	// The width of the window, in pixels. If not defined, the default is 1280.
	// If using fullscreen mode, make sure to select a resolution that is supported
	// by the display. Calling GameConfiguration::getAvailableResolutions() will return 
	// all resolutions supported by all monitors of the system
	int width = 1280;

	// The height of the window, in pixels. If not defined, the default is 720.
	// If using fullscreen mode, make sure to select a resolution that is supported
	// by the display. Calling GameConfiguration::getAvailableResolutions() will return 
	// all resolutions supported by all monitors of the system
	int height = 720;
	
	// The title that will appear on the window when it is loaded. Default is "My Boturi Game"
	const char * title = "My Boturi Game";

	// Whether or not the window will be fullscreen as defined by SDL's SDL_WINDOW_FULLSCREEN flag.
	bool fullscreen = true;

	// Whether or not the window will be resizable. If the window is fullscreen, this will
	// automatically be set to FALSE as having both resizability and fullscreen enabled
	// results in an SDL error.
	bool resizable = false;

	// The number of samples to be used by the MultiSampling AntiAliasing (MSAA) algorithm.
	// The lower the number, the better the performance but the choppier the image.
	// Must be a power of two <= 64 and is stored as a VkSampleCountFlagBits object.
	// Most computers are not capable of handling multisampling values greater than 8. 
	// You can check your machine's available sample counts with GameConfiguration::getMaxSampleCount(Boturi::physicalDevice) after calling Boturi::init()
	// 4 by default.
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_4_BIT;


	// A cap on the FPS to ensure the GPU doesn't over work itself by creating many
	// more frames than the monitor can handle
	int fpsCap = 30;

	bool vSync = false;

	// Gets the version of the software as a uint32_t created by Vulkan's VK_MAKE_VERSION macro
	uint32_t getVersion() 
	{ 
		return version; 
	}

	// Using Vulkan's VK_MAKE_VERSION macro, set the game version with 3 version numbers
	void setVersion(int major, int minor = 0, int patch = 0) 
	{ 
		version = VK_MAKE_VERSION(major, minor, patch); 
	}

	static std::vector<VkSampleCountFlagBits> getSampleCounts(VkPhysicalDevice device) 
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);

		VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);
		std::vector<VkSampleCountFlagBits> availableCounts = { VK_SAMPLE_COUNT_1_BIT };

		if (counts & VK_SAMPLE_COUNT_2_BIT) { availableCounts.push_back(VK_SAMPLE_COUNT_2_BIT); }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { availableCounts.push_back(VK_SAMPLE_COUNT_4_BIT); }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { availableCounts.push_back(VK_SAMPLE_COUNT_8_BIT); }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { availableCounts.push_back(VK_SAMPLE_COUNT_16_BIT); }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { availableCounts.push_back(VK_SAMPLE_COUNT_32_BIT); }
		if (counts & VK_SAMPLE_COUNT_64_BIT) { availableCounts.push_back(VK_SAMPLE_COUNT_64_BIT); }

		return availableCounts;
	}

	static std::vector<std::vector<SDL_DisplayMode>> getDisplayVideoModes()
	{
		std::vector<std::vector<SDL_DisplayMode>> allModes = {};
		for (int i = 0; i < SDL_GetNumVideoDisplays(); i++)
		{
			std::vector<SDL_DisplayMode> modes = {};
			for (int j = 0; j < SDL_GetNumDisplayModes(i); j++)
			{
				SDL_DisplayMode mode;
				SDL_GetDisplayMode(i, j, &mode);
				modes.push_back(mode);
			}
			allModes.push_back(modes);
		}

		return allModes;
	}
};
#pragma once

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

// Holds information about the configuration of the game. Can be loaded from a .config file
// A menu can be created in the game to adjust the current configuration and update the .config
// file so future sessions will have the same configuration
struct GameConfiguration
{
private:
	enum ConfigParameter
	{
		DebugMode,
		Width,
		Height,
		Title,
		Version,
		Fullscreen,
		Resizable,
		MSAASamples,
		FPSCap,
		VSync,
		DisplayIndex,
		RefreshRate
	};
	static std::map<std::string, ConfigParameter> getConfigParams()
	{
		std::map<std::string, ConfigParameter> params;
		params["Debug"] = DebugMode;
		params["Width"] = Width;
		params["Height"] = Height;
		params["Title"] = Title;
		params["Version"] = Version;
		params["Fullscreen"] = Fullscreen;
		params["Resizable"] = Resizable;
		params["MSAASamples"] = MSAASamples;
		params["FPSCap"] = FPSCap;
		params["VSync"] = VSync;
		params["DisplayIndex"] = DisplayIndex;
		params["RefreshRate"] = RefreshRate;
		return params;
	}
	
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
	std::string title = "My Boturi Game";

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
	// You can check your machine's available sample counts by calling:
	//   GameConfiguration::getMaxSampleCount(Boturi::physicalDevice) after calling Boturi::init()
	// Value is 4 by default.
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_4_BIT;

	// A cap on the FPS to ensure the GPU doesn't over work itself by creating many
	// more frames than the monitor can handle
	int fpsCap = 30;

	// Prevents the program from rendering frames more quickly than the monitor can handle
	bool vSync = false;

	// The index of the display the window will be rendered on, if fullscreen mode is enabled
	int displayIndex = 0;

	// The rate at which new frames will appear on the monitor, 
	// measured in Hertz (number of Frames Per Second)
	int refreshRate = 60;

	// Gets the version of the software as a uint32_t created by Vulkan's VK_MAKE_VERSION macro
	uint32_t getVersion() { return version; }

	// Using Vulkan's VK_MAKE_VERSION macro, set the game version with 3 version numbers
	void setVersion(int major, int minor = 0, int patch = 0) { version = VK_MAKE_VERSION(major, minor, patch); }

	void writeToFile(const char * filename)
	{
		std::ofstream outfile(filename);
		if (outfile.is_open())
		{
			outfile << "DebugMode " << (debugMode ? 1 : 0) << "\n";
			outfile << "Width " << width << "\n";
			outfile << "Height " << height << "\n";
			outfile << "Fullscreen " << (fullscreen ? 1 : 0) << "\n";
			outfile << "VSync " << (vSync ? 1 : 0) << "\n";
			outfile << "Resizable " << (resizable ? 1 : 0) << "\n";
			outfile << "Title " << title << "\n";
			outfile << "Version " << VK_VERSION_MAJOR(version) << "." << VK_VERSION_MINOR(version) << "." << VK_VERSION_PATCH(version) << "\n";
			outfile << "DisplayIndex " << displayIndex << "\n";
			outfile << "RefreshRate " << refreshRate << "\n";
			outfile << "FPSCap " << fpsCap << "\n";
			outfile << "MSAASamples " << msaaSamples << "\n";
			outfile.close();
		}
	}

	// Retrieve all available MSAA sample counts the given Physical Device (graphics card) can handle
	static std::vector<VkSampleCountFlagBits> getSampleCounts(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);

		VkSampleCountFlags counts = std::min(
			physicalDeviceProperties.limits.framebufferColorSampleCounts, 
			physicalDeviceProperties.limits.framebufferDepthSampleCounts
		);
		std::vector<VkSampleCountFlagBits> availableCounts = { VK_SAMPLE_COUNT_1_BIT };

		if (counts & VK_SAMPLE_COUNT_2_BIT) { availableCounts.push_back(VK_SAMPLE_COUNT_2_BIT); }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { availableCounts.push_back(VK_SAMPLE_COUNT_4_BIT); }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { availableCounts.push_back(VK_SAMPLE_COUNT_8_BIT); }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { availableCounts.push_back(VK_SAMPLE_COUNT_16_BIT); }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { availableCounts.push_back(VK_SAMPLE_COUNT_32_BIT); }
		if (counts & VK_SAMPLE_COUNT_64_BIT) { availableCounts.push_back(VK_SAMPLE_COUNT_64_BIT); }

		return availableCounts;
	}

	// Get all video modes for all displays connected to the system
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

	// Given a display, return the maximum width for that display
	static int maxWidth(int displayIndex)
	{
		int max = 0;
		std::vector<SDL_DisplayMode> displayModes = getDisplayVideoModes()[displayIndex];
		for (auto mode : displayModes)
		{
			if (mode.w > max)
				max = mode.w;
		}

		return max;
	}

	// Given a display, return the maximum height for that display
	static int maxHeight(int displayIndex)
	{
		int max = 0;
		std::vector<SDL_DisplayMode> displayModes = getDisplayVideoModes()[displayIndex];
		for (auto mode : displayModes)
		{
			if (mode.h > max)
				max = mode.h;
		}

		return max;
	}

	static GameConfiguration loadFromFile(const char * filename)
	{
		std::ifstream infile(filename);
		std::map<std::string, ConfigParameter> params = getConfigParams();

		GameConfiguration config = {};

		std::string line;
		while (std::getline(infile, line))
		{
			// Ignore comments
			if (line.size() == 0 || line.at(0) == '#')
				continue;

			std::istringstream iss(line);
			std::string var;
			std::string value;
			
			iss >> var >> value;

			switch (params[var])
			{
			case Width:
				config.width = std::stoi(value);
				break;
			case Height:
				config.height = std::stoi(value);
				break;
			case DebugMode:
				config.debugMode = std::stoi(value) == 1;
				break;
			case Title:
				config.title = line.substr(6);
				break;
			case Version:
				int major, minor, patch;
				major = stoi(value.substr(0, value.find('.')));
				value = value.substr(value.find('.') + 1);
				minor = stoi(value.substr(0, value.find('.')));
				value = value.substr(value.find('.') + 1);
				patch = stoi(value);
				config.setVersion(major, minor, patch);
				break;
			case Fullscreen:
				config.fullscreen = std::stoi(value) == 1;
				break;
			case Resizable:
				config.resizable = std::stoi(value) == 1;
				break;
			case MSAASamples:
				config.msaaSamples = (VkSampleCountFlagBits)std::stoi(value);
				break;
			case FPSCap:
				config.fpsCap = std::stoi(value);
				break;
			case RefreshRate:
				config.refreshRate = std::stoi(value);
				break;
			case VSync:
				config.vSync = std::stoi(value) == 1;
				break;
			case DisplayIndex:
				config.displayIndex = std::stoi(value);
				break;
			}
		}

		return config;
	}
};
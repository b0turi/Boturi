#pragma once

#include "SwapChainSupportDetails.h"
#include "GameConfiguration.h"
#include <iostream>

class Boturi
{
public:
	static bool resizedWindow;
	static VkPhysicalDevice physicalDevice;

	static void init(GameConfiguration config);
	static void exit();

	static GLFWwindow * window;
	static void printError(const char* message);

	static int graphicsQueueIndex;
	static int presentQueueIndex;

	static SwapChainSupportDetails swapChainDetails;

	static VkInstance instance;
	static VkDebugUtilsMessengerEXT debugger;
	static VkSurfaceKHR surface;

	static std::vector<const char *> deviceExtensions;
private:
	static bool debugMode;

	
};
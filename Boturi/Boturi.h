#pragma once

#include "SwapChainSupportDetails.h"
#include "GameConfiguration.h"
#include <iostream>

class Boturi
{
public:
	static bool resizedWindow;

	static void init(GameConfiguration config);
	static void exit();

	static GLFWwindow * window;
	static void printError(const char* message);

	static int graphicsQueueIndex;
	static int presentQueueIndex;

	static VkQueue graphicsQueue;
	static VkQueue presentQueue;

	static SwapChainSupportDetails swapChainDetails;

	static VkInstance instance;
	static VkDebugUtilsMessengerEXT debugger;
	static VkSurfaceKHR surface;
	static VkPhysicalDevice physicalDevice;
	static VkDevice device;

	static std::vector<const char *> validationLayers;
	static std::vector<const char *> deviceExtensions;
	static bool debugMode;
};
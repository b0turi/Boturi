#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

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
private:
	static bool debugMode;

	static VkInstance instance;
	static VkDebugUtilsMessengerEXT debugger;
	static VkSurfaceKHR surface;
};
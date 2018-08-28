#pragma once

#include <SDL.h>
#include <vulkan/vulkan.h>

#include <vector>

// A struct to store all relevant information
// about the features the physical device (graphics card)
// has to offer for swap chain creation
struct SwapChainSupportDetails {

	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};
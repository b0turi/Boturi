#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "SwapChainSupportDetails.h"
#include "GameConfiguration.h"

VkResult makeVulkanInstance(GameConfiguration config, VkInstance & instance);

VkResult makeVulkanDebugger(VkDebugUtilsMessengerEXT & debugger);
void destroyVulkanDebugger(VkDebugUtilsMessengerEXT & debugger);

bool selectPhysicalDevice(
	VkPhysicalDevice & physicalDevice,
	int * graphicsQueueIndex,
	int * presentQueueIndex,
	SwapChainSupportDetails * swapChainDetails);
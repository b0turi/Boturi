#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "SwapChainSupportDetails.h"
#include "GameConfiguration.h"
#include <vector>
#include <set>
#include <string>

VkResult makeVulkanInstance(GameConfiguration config, VkInstance & instance);

VkResult makeVulkanDebugger(VkDebugUtilsMessengerEXT & debugger);
void destroyVulkanDebugger(VkDebugUtilsMessengerEXT debugger);

bool selectPhysicalDevice(
	VkPhysicalDevice & physicalDevice,
	int * graphicsQueueIndex,
	int * presentQueueIndex,
	SwapChainSupportDetails * swapChainDetails);

VkResult makeVulkanDevice(VkDevice & device);

VkResult makeSwapChain(VkSwapchainKHR & swapChain, uint32_t & numImages, VkFormat & imageFormat, VkExtent2D & extent);
void fillSwapChain();

VkResult makeRenderPass(VkRenderPass & renderPass);

VkResult makeFrameBuffers(std::vector<VkFramebuffer> & frameBuffers);
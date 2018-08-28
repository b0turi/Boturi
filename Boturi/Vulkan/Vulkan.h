#pragma once

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include "SwapChainSupportDetails.h"
#include "GameConfiguration.h"
#include "Uniforms.h"
#include <vector>
#include <set>
#include <string>

VkResult makeVulkanInstance(GameConfiguration config, SDL_Window * window, VkInstance & instance);

VkResult makeVulkanDebugger(VkDebugUtilsMessengerEXT & debugger);
void destroyVulkanDebugger(VkDebugUtilsMessengerEXT debugger);

bool selectPhysicalDevice(
	VkPhysicalDevice & physicalDevice,
	int * graphicsQueueIndex,
	int * presentQueueIndex,
	SwapChainSupportDetails * swapChainDetails);

VkResult makeVulkanDevice(VkDevice & device);

VkResult makeSwapChain(VkSwapchainKHR & swapChain, 
						uint32_t & numImages,
						VkFormat & imageFormat, 
						VkExtent2D & extent);

void fillSwapChain();

VkResult makeRenderPass(VkRenderPass & renderPass);

VkResult makeFrameBuffers(std::vector<VkFramebuffer> & frameBuffers);
VkResult makeSyncObjects(std::vector<VkSemaphore> & imgSemaphores, 
						 std::vector<VkSemaphore> & renSemaphores, 
						 std::vector<VkFence> & iffs);
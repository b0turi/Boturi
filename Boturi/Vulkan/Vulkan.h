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

// This is a master header for functions defined in various subheaders
// that, because of their static nature, did not warrant the creation
// of a class to hold them, so instead they are all defined here
// and implemented in separate headers, organized by category of
// which Vulkan object they assist in the creation of

// Instance.h
#pragma region VulkanInstance

// Creates a VkInstance in the location passed in, and returns whether the creation was successful
VkResult makeVulkanInstance(GameConfiguration config, SDL_Window * window, VkInstance * instance);

#pragma endregion VulkanInstance


// VulkanDebugger.h
#pragma region VulkanDebugger

// Creates a Vulkan Debug Utils Messenger in the location passed in, and returns whether the creation was successful
VkResult makeVulkanDebugger(VkDebugUtilsMessengerEXT & debugger);

// Destroys the existing Vulkan Debug Utils Messenger
void destroyVulkanDebugger(VkDebugUtilsMessengerEXT debugger);

#pragma endregion VulkanDebugger

// PhysicalDevice.h
#pragma region PhysicalDevice

// Enumerates all physical devices available on the system, and selects the one
// that best supports Vulkan. After a device is chosen, it sets values in the Boturi
// environment for 
bool selectPhysicalDevice(VkPhysicalDevice & physicalDevice);

#pragma endregion PhysicalDevice

VkResult makeVulkanDevice(VkDevice & device);

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
VkResult makeSwapChain(VkSwapchainKHR & swapChain, 
						uint32_t & numImages,
						VkFormat & imageFormat, 
						VkExtent2D & extent,
						VkPresentModeKHR presentMode);

void fillSwapChain();

VkResult makeRenderPass(VkRenderPass & renderPass);

VkResult makeFrameBuffers(std::vector<VkFramebuffer> & frameBuffers);
VkResult makeSyncObjects(std::vector<VkSemaphore> & imgSemaphores, 
						 std::vector<VkSemaphore> & renSemaphores, 
						 std::vector<VkFence> & iffs);
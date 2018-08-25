#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "GameConfiguration.h"

VkResult makeVulkanInstance(GameConfiguration config, VkInstance & instance);

VkResult makeVulkanDebugger(VkInstance instance, VkDebugUtilsMessengerEXT & debugger);
void destroyVulkanDebugger(VkInstance instance, VkDebugUtilsMessengerEXT & debugger);
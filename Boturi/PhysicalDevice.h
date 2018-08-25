// Helper functions for selecting a physical device (graphics card)
#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "Boturi.h"

#include <iostream>
#include <vector>
#include <set>
#include <string>

std::vector<int> getQueueIndices(VkPhysicalDevice device)
{
	std::vector<int> indices;
	indices.resize(2);

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices[0] = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, Boturi::surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
			indices[1] = i;

		if (indices[0] >= 0 && indices[1] >= 0) 
			break;

		i++;
	}

	return indices;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device) 
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(Boturi::deviceExtensions.begin(), 
											Boturi::deviceExtensions.end());

	for (const auto& extension : availableExtensions) 
		requiredExtensions.erase(extension.extensionName);

	return requiredExtensions.empty();
}

SwapChainSupportDetails getSwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, Boturi::surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, Boturi::surface, &formatCount, nullptr);

	if (formatCount != 0) 
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, Boturi::surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, Boturi::surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) 
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, Boturi::surface, 
												  &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool isDeviceSuitable(
	VkPhysicalDevice device, 
	int * graphicsQueueIndex, 
	int * presentQueueIndex, 
	SwapChainSupportDetails * swapChainDetails)
{
	std::vector<int> indices = getQueueIndices(device);
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	SwapChainSupportDetails swapChainSupport = getSwapChainSupport(device);
	bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	bool cond = indices[0] >= 0 && indices[1] >= 0 && 
				extensionsSupported && 
				swapChainAdequate  && 
				supportedFeatures.samplerAnisotropy;

	if (cond)
	{
		*graphicsQueueIndex = indices[0];
		*presentQueueIndex = indices[1];
		*swapChainDetails = swapChainSupport;
	}

	return true;
}
#pragma once

#include <iostream>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cout << "VULKAN ERROR: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

VkResult createDebugger(
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
	const VkAllocationCallbacks* pAllocator, 
	VkDebugUtilsMessengerEXT* pCallback) 
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
					Boturi::instance, "vkCreateDebugUtilsMessengerEXT");
	
	if (func != nullptr) 
		return func(Boturi::instance, pCreateInfo, pAllocator, pCallback);
	else 
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

VkResult makeVulkanDebugger(VkDebugUtilsMessengerEXT & debugger)
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
							 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
							 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	createInfo.pfnUserCallback = debugCallback;

	return createDebugger(&createInfo, nullptr, &debugger);
}

void destroyVulkanDebugger(VkDebugUtilsMessengerEXT debugger)
	
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
					Boturi::instance, "vkDestroyDebugUtilsMessengerEXT");
	
	if (func != nullptr) 
		func(Boturi::instance, debugger, nullptr);
}
// Helper functions for creating a Vulkan Debugger
#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "Boturi.h"

#include <iostream>

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

void destroyDebugger(
	VkDebugUtilsMessengerEXT callback, 
	const VkAllocationCallbacks* pAllocator) 
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
					Boturi::instance, "vkDestroyDebugUtilsMessengerEXT");
	
	if (func != nullptr) 
		func(Boturi::instance, callback, pAllocator);
}


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) 
{
	std::cout << "VULKAN ERROR: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}
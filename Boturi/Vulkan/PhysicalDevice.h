#pragma once

namespace PhysicalDevice
{
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

	VkFormat findSupportedFormat(
		const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features,
		VkPhysicalDevice device) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(device, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;
		}

		throw std::runtime_error("failed to find supported format!");
	}

	VkFormat findDepthFormat(VkPhysicalDevice device) {
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
			device
		);
	}

	bool isDeviceSuitable(VkPhysicalDevice device)
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

		// This is the correct device, fill in the Boturi environment fields 
		// that are determined by the Physical Device and its capabilities
		if (cond)
		{
			Boturi::graphicsQueueIndex = indices[0];
			Boturi::presentQueueIndex = indices[1];

			Boturi::swapChainDetails = swapChainSupport;

			Boturi::depthFormat = findDepthFormat(device);
		}

		return cond;
	}
}

bool selectPhysicalDevice(VkPhysicalDevice & physicalDevice)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(Boturi::instance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(Boturi::instance, &deviceCount, devices.data());

	for (const auto& device : devices)
		if (PhysicalDevice::isDeviceSuitable(device))
		{
			physicalDevice = device;
			break;
		}

	return physicalDevice == VK_NULL_HANDLE;
}
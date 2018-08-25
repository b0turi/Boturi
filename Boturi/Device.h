#pragma once

VkResult makeVulkanDevice(VkDevice & device)
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = {
		Boturi::graphicsQueueIndex,
		Boturi::presentQueueIndex
	};

	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(Boturi::deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = Boturi::deviceExtensions.data();

	if (Boturi::debugMode) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(Boturi::validationLayers.size());
		createInfo.ppEnabledLayerNames = Boturi::validationLayers.data();
	}
	else 
		createInfo.enabledLayerCount = 0;

	return vkCreateDevice(Boturi::physicalDevice, &createInfo, nullptr, &device);
}
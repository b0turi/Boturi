#include "Vulkan.h"
#include "VulkanDebugger.h"
#include "PhysicalDevice.h"

// Used by makeVulkanInstance
std::vector<const char*> getRequiredExtensions(bool debugMode) {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (debugMode) 
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	return extensions;
}

VkResult makeVulkanInstance(GameConfiguration config, VkInstance & instance)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = config.title;
	appInfo.applicationVersion = config.getVersion();

	appInfo.pEngineName = "Boturi Game Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions(config.debugMode);
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (config.debugMode) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(Boturi::validationLayers.size());
		createInfo.ppEnabledLayerNames = Boturi::validationLayers.data();
	}
	else 
		createInfo.enabledLayerCount = 0;

	return vkCreateInstance(&createInfo, nullptr, &instance);
}

VkResult makeVulkanDebugger(VkDebugUtilsMessengerEXT & debugger)
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	return createDebugger(&createInfo, nullptr, &debugger);
}

void destroyVulkanDebugger(VkDebugUtilsMessengerEXT & debugger)
{
	destroyDebugger(debugger, nullptr);
}

bool selectPhysicalDevice(
	VkPhysicalDevice & physicalDevice,
	int * graphicsQueueIndex,
	int * presentQueueIndex,
	SwapChainSupportDetails * swapChainDetails)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(Boturi::instance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(Boturi::instance, &deviceCount, devices.data());

	for (const auto& device : devices) 
		if (isDeviceSuitable(device, graphicsQueueIndex, presentQueueIndex, swapChainDetails))
		{
			physicalDevice = device;
			break;
		}

	return physicalDevice == VK_NULL_HANDLE;
}

VkResult makeVulkanDevice(VkDevice & device, VkQueue & graphicsQueue, VkQueue & presentQueue)
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
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(Boturi::physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, Boturi::graphicsQueueIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(device, Boturi::presentQueueIndex, 0, &presentQueue);
}
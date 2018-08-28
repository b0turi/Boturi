#pragma once
std::vector<const char*> getRequiredExtensions(SDL_Window * window, bool debugMode) {
	uint32_t SDLextensionCount = 0;
	const char ** names;
	SDL_Vulkan_GetInstanceExtensions(window, &SDLextensionCount, nullptr);

	names = new const char *[SDLextensionCount];

	SDL_Vulkan_GetInstanceExtensions(window, &SDLextensionCount, names);

	std::vector<const char*> extensions(names, names + SDLextensionCount);

	if (debugMode)
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	return extensions;
}

VkResult makeVulkanInstance(GameConfiguration config, SDL_Window * window, VkInstance & instance)
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

	auto extensions = getRequiredExtensions(window, config.debugMode);
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
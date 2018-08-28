#pragma once

// Interfaces with SDL to retrieve all Vulkan extensions necessary 
// for SDL to create a Vulkan surface, and returns their names in a vector
std::vector<const char*> getRequiredExtensions(SDL_Window * window, bool debugMode) {
	uint32_t SDLextensionCount = 0;
	const char ** names;

	// First, get the number of SDL extensions that will be necessary
	SDL_Vulkan_GetInstanceExtensions(window, &SDLextensionCount, nullptr);

	// Size the array appropriately
	names = new const char *[SDLextensionCount];

	// Now, call the function again to fill the array with the names of the extensions
	SDL_Vulkan_GetInstanceExtensions(window, &SDLextensionCount, names);

	// Create a vector to wrap the array to make it possible to add debug extensions if necessary
	std::vector<const char*> extensions(names, names + SDLextensionCount);
	if (debugMode)
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	return extensions;
}

VkResult makeVulkanInstance(GameConfiguration config, SDL_Window * window, VkInstance * instance)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	// Use config file information to retrieve the name of the window and the version of the software
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

	if (config.debugMode) 
	{
		// Enable all validation layers listed in the static vector of validation layers defined in Boturi.cpp
		createInfo.enabledLayerCount = static_cast<uint32_t>(Boturi::validationLayers.size());
		createInfo.ppEnabledLayerNames = Boturi::validationLayers.data();
	}
	else
		createInfo.enabledLayerCount = 0;

	return vkCreateInstance(&createInfo, nullptr, instance);
}
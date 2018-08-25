#include "Boturi.h"
#include "Vulkan.h"

bool Boturi::debugMode;
bool Boturi::resizedWindow;

VkInstance Boturi::instance;
VkDebugReportCallbackEXT Boturi::debugger;
VkSurfaceKHR Boturi::surface;
VkPhysicalDevice Boturi::physicalDevice;
VkDevice Boturi::device;

int Boturi::graphicsQueueIndex = -1;
int Boturi::presentQueueIndex = -1;

VkQueue Boturi::graphicsQueue;
VkQueue Boturi::presentQueue;

SwapChainSupportDetails Boturi::swapChainDetails;

GLFWwindow * Boturi::window;

std::vector<const char *> Boturi::deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

std::vector<const char *> Boturi::validationLayers = {
	"VK_LAYER_LUNARG_standard_validation" 
};

// Used when creating GLFW window
static void framebufferResizeCallback(GLFWwindow * window, int width, int height) {
	Boturi::resizedWindow = true;
}

void makeGlfwWindow(
	GameConfiguration config, VkSurfaceKHR & surface, GLFWwindow ** window)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	 *window = glfwCreateWindow(config.width, config.height, config.title, nullptr, nullptr);
	glfwSetFramebufferSizeCallback(*window, framebufferResizeCallback);

	glfwCreateWindowSurface(Boturi::instance, *window, nullptr, &surface);
}

void Boturi::init(GameConfiguration config)
{
	glfwInit();
	debugMode = config.debugMode;

	if (makeVulkanInstance(config, instance) != VK_SUCCESS)
		printError("Unable to create vulkan instance");

	makeGlfwWindow(config, surface, &window);

	if (debugMode)
		makeVulkanDebugger(debugger);

	selectPhysicalDevice(physicalDevice, &graphicsQueueIndex, &presentQueueIndex, &swapChainDetails);
	makeVulkanDevice(device, graphicsQueue, presentQueue);

	while (!glfwWindowShouldClose(window))
		glfwPollEvents();
}

void Boturi::printError(const char* message)
{
	std::cout << "BOTURI ERROR: " << message << std::endl;
	std::getchar();
	std::exit(EXIT_FAILURE);
}

void Boturi::exit()
{
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);

	if (debugMode)
		destroyVulkanDebugger(debugger);

	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);
	glfwTerminate();
}
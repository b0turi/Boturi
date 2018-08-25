#include "Boturi.h"
#include "Vulkan.h"

bool Boturi::debugMode;
bool Boturi::resizedWindow;

VkInstance Boturi::instance;
VkDebugReportCallbackEXT Boturi::debugger;

GLFWwindow * Boturi::window;

// Used when creating GLFW window
static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	Boturi::resizedWindow = true;
}

void makeGlfwWindow(GameConfiguration config, GLFWwindow ** window)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	 *window = glfwCreateWindow(config.width, config.height, config.title, nullptr, nullptr);
	glfwSetFramebufferSizeCallback(*window, framebufferResizeCallback);
}

void Boturi::init(GameConfiguration config)
{
	glfwInit();
	debugMode = config.getDebugMode();

	makeGlfwWindow(config, &window);

	if (makeVulkanInstance(config, instance) != VK_SUCCESS)
		printError("Unable to create vulkan instance");

	if (debugMode)
		makeVulkanDebugger(instance, debugger);

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
	if (debugMode)
		destroyVulkanDebugger(instance, debugger);
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}
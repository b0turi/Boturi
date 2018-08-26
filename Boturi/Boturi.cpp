#include "Boturi.h"
#include "Vulkan.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <chrono>
#include <thread>

bool Boturi::debugMode;
bool Boturi::resizedWindow;

const int Boturi::MAX_FRAMES_IN_FLIGHT = 2;
std::map<int, VkSampler> Boturi::textureSamplers;

// Constants across the environment
GLFWwindow * Boturi::window;

VkInstance Boturi::instance;
VkDebugReportCallbackEXT Boturi::debugger;
VkSurfaceKHR Boturi::surface;
VkPhysicalDevice Boturi::physicalDevice;
VkDevice Boturi::device;
VkSwapchainKHR Boturi::swapChain;
VkRenderPass Boturi::renderPass;
VkCommandPool Boturi::commandPool;

std::vector<VkImage> Boturi::swapChainImages;
std::vector<VkImageView> Boturi::swapChainImageViews;
std::vector<VkFramebuffer> Boturi::frameBuffers;

int Boturi::graphicsQueueIndex = -1;
int Boturi::presentQueueIndex = -1;

VkQueue Boturi::graphicsQueue;
VkQueue Boturi::presentQueue;

uint32_t Boturi::numImages;
VkSampleCountFlagBits Boturi::msaaSamples;

VkFormat Boturi::depthFormat;

SwapChainSupportDetails Boturi::swapChainDetails;
VkFormat Boturi::imageFormat;
VkExtent2D Boturi::extent;

Image Boturi::colorAttachment;
Image Boturi::depthAttachment;

std::vector<VkSemaphore> Boturi::imageAvailableSemaphores;
std::vector<VkSemaphore> Boturi::renderFinishedSemaphores;
std::vector<VkFence> Boturi::inFlightFences;
size_t Boturi::currentFrame;

// Dynamically created

std::vector<Descriptor> Boturi::descriptors;

float Boturi::aspectRatio;

Descriptor d;
Pipeline p;
Texture t;
Mesh m;
UniformBuffer u;
MVPMatrix mvp;
CommandBuffer c;


std::vector<const char *> Boturi::deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

std::vector<const char *> Boturi::validationLayers = {
	"VK_LAYER_LUNARG_standard_validation" 
};

// Used when creating GLFW window
static void framebufferResizeCallback(GLFWwindow * window, int width, int height) {
	Boturi::refresh();
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
	makeVulkanDevice(device);

	CommandBuffer::makeCommandPool(commandPool);

	vkGetDeviceQueue(device, graphicsQueueIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(device, presentQueueIndex, 0, &presentQueue);

	makeSwapChain(swapChain, numImages, imageFormat, extent);
	fillSwapChain();

	makeRenderPass(renderPass);

	colorAttachment = Image(extent, imageFormat, 1, Boturi::msaaSamples,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	depthAttachment = Image(extent, depthFormat, 1, Boturi::msaaSamples,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

	makeFrameBuffers(frameBuffers);

	makeSyncObjects(imageAvailableSemaphores, renderFinishedSemaphores, inFlightFences);

	// TEMP

	t = Texture("textures/asdf.jpg");
	m = Mesh("models/cube.obj");
	u = UniformBuffer(MVP_MATRIX);

	std::vector<BindingType> def = { UNIFORM_BUFFER, TEXTURE_SAMPLER };
	d = Descriptor(def);

	std::vector<UniformBuffer> us = { u };
	std::vector<Texture> ts = { t };

	d.makeDescriptorSets(def, us, ts);
	p = Pipeline("shaders/vert.spv", "shaders/frag.spv", d);
	c = CommandBuffer(p, m, d);

	mvp = {};
	mvp.model = glm::mat4(1.0f);
	mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	mvp.projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);
	mvp.projection[1][1] *= -1;


	// end temp

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		draw();
	}

	vkDeviceWaitIdle(device);
}

void Boturi::printError(const char* message)
{
	std::cout << "BOTURI ERROR: " << message << std::endl;
	std::getchar();
	std::exit(EXIT_FAILURE);
}

void Boturi::removeDynamics()
{
	depthAttachment.cleanup();
	colorAttachment.cleanup();

	for (auto frameBuffer : frameBuffers)
		vkDestroyFramebuffer(device, frameBuffer, nullptr);

	// Temp code

	c.cleanup();
	p.cleanup();

	// end temp

	vkDestroyRenderPass(device, renderPass, nullptr);

	for (auto imageView : swapChainImageViews)
		vkDestroyImageView(device, imageView, nullptr);

	vkDestroySwapchainKHR(device, swapChain, nullptr);
}

void Boturi::refresh()
{
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device);

	removeDynamics();

	makeSwapChain(swapChain, numImages, imageFormat, extent);
	fillSwapChain();

	makeRenderPass(renderPass);

	p = Pipeline("shaders/vert.spv", "shaders/frag.spv", d);

	colorAttachment = Image(extent, imageFormat, 1, Boturi::msaaSamples,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	depthAttachment = Image(extent, depthFormat, 1, Boturi::msaaSamples,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

	makeFrameBuffers(frameBuffers);

	c = CommandBuffer(p, m, d);
}

void Boturi::exit()
{
	removeDynamics();

	for (auto& pair : textureSamplers)
		vkDestroySampler(device, pair.second, nullptr);

	t.cleanup();
	d.cleanup();

	u.cleanup();
	m.cleanup();

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(device, commandPool, nullptr);

	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);

	if (debugMode)
		destroyVulkanDebugger(debugger);

	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);
	glfwTerminate();
}

VkSampler Boturi::getTextureSampler(int mipLevel)
{
	if (textureSamplers.find(mipLevel) == textureSamplers.end())
		textureSamplers[mipLevel] = Texture::makeTextureSampler(mipLevel);
	return textureSamplers[mipLevel];
}

size_t Boturi::getUniformSize(UniformType type)
{
	switch (type)
	{
	case MVP_MATRIX:
		return sizeof(MVPMatrix);
		break;
	default:
		Boturi::printError("The given uniform type does not exist");
		return 0;
		break;
	}
}

void Boturi::draw()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	auto start = std::chrono::system_clock::now();
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		refresh();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	mvp.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	u.update(&mvp, imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;

	VkCommandBuffer cmdBuffer = c.getCommandBuffer(imageIndex);
	submitInfo.pCommandBuffers = &cmdBuffer;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resizedWindow) {
		resizedWindow = false;
		refresh();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	auto end = std::chrono::system_clock::now();

	double fps = 600;

	std::chrono::duration<double> diff = end - start;
	if (diff.count() < double(1000 / fps)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(int(double(1000 / fps) - diff.count())));
	}
}
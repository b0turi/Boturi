#include "Boturi.h"
#include "Vulkan/Vulkan.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <chrono>
#include <thread>

bool Boturi::debugMode;

const int Boturi::MAX_FRAMES_IN_FLIGHT = 2;

SDL_Window * Boturi::window;

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

std::map<int, VkSampler> Boturi::textureSamplers;

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

static void SetSDLIcon(SDL_Window * window)
{
#include "textures/icon.c"

	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	int shift = (my_icon.bytes_per_pixel == 3) ? 8 : 0;
	rmask = 0xff000000 >> shift;
	gmask = 0x00ff0000 >> shift;
	bmask = 0x0000ff00 >> shift;
	amask = 0x000000ff >> shift;
#else // little endian, like x86
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = (icon.bytes_per_pixel == 3) ? 0 : 0xff000000;
#endif

	SDL_Surface* newIcon = SDL_CreateRGBSurfaceFrom((void*)icon.pixel_data, icon.width,
		icon.height, icon.bytes_per_pixel * 8, icon.bytes_per_pixel*icon.width,
		rmask, gmask, bmask, amask);
	SDL_SetWindowIcon(window, newIcon);

	SDL_FreeSurface(newIcon);
}

GameConfiguration makeWindow(GameConfiguration config, SDL_Window ** window)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	if(config.fullscreen)
		*window = SDL_CreateWindow(config.title, 0,0, 
			config.width, 
			config.height, 
			SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN);
	else
		*window = SDL_CreateWindow(config.title, 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			config.width, config.height, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);


	SDL_SetWindowResizable(*window, config.resizable ? SDL_TRUE : SDL_FALSE);

	SetSDLIcon(*window);

	return config;
}

void Boturi::init(GameConfiguration config)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	debugMode = config.debugMode;
	config = makeWindow(config, &window);

	makeVulkanInstance(config, window, instance);

	SDL_Vulkan_CreateSurface(window, instance, &surface);

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

	bool shouldEnd = false;
	while (!shouldEnd)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT &&
				event.window.event == SDL_WINDOWEVENT_CLOSE))
				shouldEnd = true;

			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
				refresh();
		}
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
		SDL_GetWindowSize(window, &width, &height);
		SDL_WaitEvent(0);
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

	SDL_DestroyWindow(window);
	SDL_Quit();
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
	mvp.projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);
	mvp.projection[1][1] *= -1;
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

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		refresh();
	else if (result != VK_SUCCESS) 
		Boturi::printError("failed to present swap chain image!");

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	auto end = std::chrono::system_clock::now();

	double fps = 600;

	std::chrono::duration<double> diff = end - start;
	if (diff.count() < double(1000 / fps)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(int(double(1000 / fps) - diff.count())));
	}
}

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

std::vector<Shader> Boturi::shaders;
std::map<int, VkSampler> Boturi::textureSamplers;
std::map<std::vector<BindingType>, VkDescriptorSetLayout> Boturi::descriptors;
std::map<std::string, Pipeline> Boturi::pipelines;
std::map<std::string, Mesh> Boturi::meshes;
std::map<std::string, Texture> Boturi::textures;

float Boturi::aspectRatio;

std::vector<GameObject> Boturi::objects;
CommandBuffer Boturi::cmd;

Camera Boturi::camera = Camera(90);

int Boturi::fpsCap;

VkPresentModeKHR presentMode;

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
	if (config.fullscreen)
	{
		// Do not allow a fullscreen window to be resizable, results in SDL error
		config.resizable = false;

		// Constrain the fullscreen window to the closest video mode
		SDL_DisplayMode approxMode;
		approxMode.w = config.width;
		approxMode.h = config.height;
		approxMode.refresh_rate = config.refreshRate;

		if (approxMode.w > GameConfiguration::maxWidth(config.displayIndex) ||
			approxMode.h > GameConfiguration::maxHeight(config.displayIndex))
		{
			approxMode.w = GameConfiguration::maxWidth(config.displayIndex);
			approxMode.h = GameConfiguration::maxHeight(config.displayIndex);
		}

		SDL_DisplayMode chosenMode = {};

		SDL_GetClosestDisplayMode(config.displayIndex, &approxMode, &chosenMode);
		if (approxMode.w != chosenMode.w ||
			approxMode.h != chosenMode.h ||
			approxMode.refresh_rate != chosenMode.refresh_rate)
		{
			std::cout << "The given video mode was not available on the desired display." << std::endl;
			std::cout << "New video mode: " << chosenMode.w << ", " << chosenMode.h << 
				" @" << chosenMode.refresh_rate << std::endl;
		}
		
		// Use the chosen video mode to decide the bounds in the 
		// screen space where the window should go
		SDL_Rect bounds;
		SDL_GetDisplayBounds(config.displayIndex, &bounds);
		SDL_SetWindowDisplayMode(*window, &chosenMode);

		*window = SDL_CreateWindow(
			config.title.c_str(),
			bounds.x,
			bounds.y, 
			chosenMode.w, 
			chosenMode.h, 
			SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
	}
	else
	{
		*window = SDL_CreateWindow(config.title.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			config.width, config.height, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
	}

	SDL_SetWindowResizable(*window, config.resizable ? SDL_TRUE : SDL_FALSE);

	SetSDLIcon(*window);

	// Return the new config so any changes that occurred can be reflected
	return config;
}

void Boturi::makeDescriptors()
{
	for (Shader shader : shaders)
		if (descriptors.find(shader.getDefinition()) == descriptors.end())
			descriptors[shader.getDefinition()] = Descriptor::makeDescriptorSetLayout(shader.getDefinition());
}

void Boturi::makePipelines()
{
	for (Shader shader : shaders)
		pipelines[shader.getName()] = Pipeline(shader, descriptors[shader.getDefinition()]);
}

void Boturi::destroyPipelines()
{
	for (auto& pair : pipelines)
		pair.second.cleanup();
	
	pipelines.clear();
}

void log(const char * message, bool blockingCall = false)
{
	std::cout << "BOTURI INFO: " << message << std::endl;
	if (blockingCall)
		std::getchar();
}

void Boturi::init(GameConfiguration config)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	debugMode = config.debugMode;
	config = makeWindow(config, &window);

	makeVulkanInstance(config, window, &instance);

	SDL_Vulkan_CreateSurface(window, instance, &surface);

	if (debugMode)
		makeVulkanDebugger(debugger);

	selectPhysicalDevice(physicalDevice);

	// Ensure the user provided data is adequate and adjust values if necessary
	std::vector<VkSampleCountFlagBits> sampleCounts = GameConfiguration::getSampleCounts(physicalDevice);
	VkSampleCountFlagBits maxCount = sampleCounts[sampleCounts.size() - 1];
	if (config.msaaSamples > maxCount)
	{
		config.msaaSamples = maxCount;
		if (config.debugMode)
			log("Provided MSAA Sample Count was too high, updated to max value");
	}

	msaaSamples = config.msaaSamples;

	if (config.fpsCap > 500)
	{
		config.fpsCap = 500;
		if(config.debugMode)
			log("Max FPS Cap exceeded, clamped to 500 FPS");
	}
	fpsCap = config.fpsCap;

	if (config.vSync)
		presentMode = VK_PRESENT_MODE_FIFO_KHR;
	else
		presentMode = chooseSwapPresentMode(swapChainDetails.presentModes);

	makeVulkanDevice(device);

	CommandBuffer::makeCommandPool(commandPool);

	vkGetDeviceQueue(device, graphicsQueueIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(device, presentQueueIndex, 0, &presentQueue);

	makeSwapChain(swapChain, numImages, imageFormat, extent, presentMode);
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

	config.writeToFile("config");

	shaders.push_back(Shader({ "default-v.vert", "default-f.frag" },
		{ TEXTURE_SAMPLER, UNIFORM_BUFFER, UNIFORM_BUFFER },
		{ MVP_MATRIX, LIGHT },
		true));

	makeDescriptors();
	makePipelines();

	camera.setProjectionMatrix();

	cmd = CommandBuffer(objects);
}

void Boturi::printError(const char* message)
{
	std::cout << "BOTURI ERROR: " << message << std::endl;
	std::getchar();
	std::exit(EXIT_FAILURE);
}

void Boturi::removeDynamics()
{
	cmd.cleanup();

	for (auto& pair : pipelines)
	{
		vkDestroyPipeline(device, pair.second.getPipeline(), nullptr);
		vkDestroyPipelineLayout(device, pair.second.getLayout(), nullptr);
	}

	depthAttachment.cleanup();
	colorAttachment.cleanup();

	for (auto frameBuffer : frameBuffers)
		vkDestroyFramebuffer(device, frameBuffer, nullptr);

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

	makeSwapChain(swapChain, numImages, imageFormat, extent, presentMode);
	fillSwapChain();

	makeRenderPass(renderPass);

	makePipelines();

	colorAttachment = Image(extent, imageFormat, 1, Boturi::msaaSamples,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	depthAttachment = Image(extent, depthFormat, 1, Boturi::msaaSamples,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

	makeFrameBuffers(frameBuffers);

	cmd = CommandBuffer(objects);
}

void Boturi::exit()
{
	vkDeviceWaitIdle(device);

	for (auto obj : objects)
		obj.cleanup();

	for (auto& pair : meshes)
		pair.second.cleanup();

	for (auto& pair : textures)
		pair.second.cleanup();

	removeDynamics();

	for (auto& pair : textureSamplers)
		vkDestroySampler(device, pair.second, nullptr);

	for (auto& pair : descriptors)
		vkDestroyDescriptorSetLayout(device, pair.second, nullptr);

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
	case LIGHT:
		return sizeof(Light);
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

	for (auto obj : objects)
		obj.update(imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	VkCommandBuffer buffer = cmd.getCommandBuffer(imageIndex);
	submitInfo.pCommandBuffers = &buffer;

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

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = endTime - currentTime;
	if(diff.count() < double(1000/(double)fpsCap))
		std::this_thread::sleep_for(std::chrono::milliseconds(int(double(1000 / (double)fpsCap) - diff.count())));
}

void Boturi::addGameObject(GameObject obj)
{
	objects.push_back(obj);
	cmd.cleanup();

	cmd = CommandBuffer(objects);
}

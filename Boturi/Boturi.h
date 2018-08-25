#pragma once

#include "BindingType.h"
#include "Vertex.h"
#include "SwapChainSupportDetails.h"
#include "GameConfiguration.h"
#include "Image.h"
#include "Commands.h"
#include "Descriptor.h"
#include "Pipeline.h"
#include <iostream>

class Boturi
{
public:
	static const int MAX_FRAMES_IN_FLIGHT;
	static bool resizedWindow;

	static VkSampleCountFlagBits msaaSamples;

	static void init(GameConfiguration config);
	static void exit();

	static void addDynamics();
	static void removeDynamics();
	static void refresh();

	static GLFWwindow * window;
	static void printError(const char* message);

	static int graphicsQueueIndex;
	static int presentQueueIndex;

	static VkQueue graphicsQueue;
	static VkQueue presentQueue;

	static uint32_t numImages;

	static VkFormat depthFormat;

	static SwapChainSupportDetails swapChainDetails;
	static VkFormat imageFormat;
	static VkExtent2D extent;

	static Image colorAttachment;
	static Image depthAttachment;

	static VkInstance instance;
	static VkDebugUtilsMessengerEXT debugger;
	static VkSurfaceKHR surface;
	static VkPhysicalDevice physicalDevice;
	static VkDevice device;
	static VkSwapchainKHR swapChain;
	static VkRenderPass renderPass;
	static VkCommandPool commandPool;

	static std::vector<VkImage> swapChainImages;
	static std::vector<VkImageView> swapChainImageViews;
	static std::vector<VkFramebuffer> frameBuffers;

	static std::vector<const char *> validationLayers;
	static std::vector<const char *> deviceExtensions;
	static bool debugMode;

	static std::vector<VkSemaphore> imageAvailableSemaphores;
	static std::vector<VkSemaphore> renderFinishedSemaphores;
	static std::vector<VkFence> inFlightFences;
	static size_t currentFrame;

	static std::vector<Descriptor> descriptors;
};
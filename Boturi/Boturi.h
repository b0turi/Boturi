#pragma once

#include "Vulkan/BindingType.h"
#include "Vulkan/Vertex.h"
#include "Vulkan/SwapChainSupportDetails.h"
#include "GameConfiguration.h"
#include "Vulkan/Uniforms.h"
#include "Vulkan/Image.h"
#include "Vulkan/Buffer.h"
#include "Vulkan/UniformBuffer.h"
#include "Vulkan/Texture.h"
#include "Vulkan/Descriptor.h"
#include "Vulkan/Shader.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/Mesh.h"
#include "GameObject.h"
#include "Vulkan/CommandBuffer.h"
#include <iostream>
#include <map>

// The Boturi Game Engine is implemented with a static class acting as the "environment" 
// from which all classes can pull information that is constant across the program,
// like the Vulkan instance or the Vulkan device. 
//    
// All Vulkan specific attributes are included in the Vulkan namespace so that when calling
// Boturi as an end user, the option to alter the Vulkan environment is not available
// to increase code completion readability and prevent user error.
class Boturi
{
private:
	static size_t currentFrame;
	
	static int fpsCap;
	static void removeDynamics();
	static void refresh();
	static void draw();

	static std::vector<GameObject> objects;
	static CommandBuffer cmd;
public:

	static std::vector<Shader> shaders;
	static std::map<int, VkSampler> textureSamplers;
	static std::map<std::vector<BindingType>, VkDescriptorSetLayout> descriptors;
	static std::map<std::string, Pipeline> pipelines;
	static std::map<std::string, Mesh> meshes;
	static std::map<std::string, Texture> textures;

	static bool debugMode;

	static SDL_Window * window;
	static float aspectRatio;

	static VkFormat depthFormat;
	static const int MAX_FRAMES_IN_FLIGHT;
	static uint32_t numImages;
	static VkSampleCountFlagBits msaaSamples;

	static std::vector<const char *> validationLayers;
	static std::vector<const char *> deviceExtensions;

	static VkInstance instance;
	static VkDebugUtilsMessengerEXT debugger;
	static VkSurfaceKHR surface;
	static VkPhysicalDevice physicalDevice;
	static VkDevice device;

	static int graphicsQueueIndex;
	static int presentQueueIndex;

	static VkQueue graphicsQueue;
	static VkQueue presentQueue;

	static SwapChainSupportDetails swapChainDetails;
	static VkSwapchainKHR swapChain;
	static VkRenderPass renderPass;

	static std::vector<VkImage> swapChainImages;
	static std::vector<VkImageView> swapChainImageViews;
	static std::vector<VkFramebuffer> frameBuffers;

	static VkFormat imageFormat;
	static VkExtent2D extent;

	static Image colorAttachment;
	static Image depthAttachment;

	static VkCommandPool commandPool;

	static std::vector<VkSemaphore> imageAvailableSemaphores;
	static std::vector<VkSemaphore> renderFinishedSemaphores;
	static std::vector<VkFence> inFlightFences;

	static size_t getUniformSize(UniformType type);
	static VkSampler getTextureSampler(int mipLevel);


	static void init(GameConfiguration config);
	static void run();
	static void exit();

	static void makeDescriptors();
	static void makePipelines();
	static void addGameObject(GameObject obj);
	static void destroyPipelines();

	static void printError(const char* message);
};
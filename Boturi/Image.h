#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

class Image
{
protected:
	void fillImage(VkExtent2D extent,
		VkFormat format,
		uint32_t mipLevels,
		VkSampleCountFlagBits samples,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkImageLayout layout,
		VkImageAspectFlags aspects);
private:
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;
public:
	static void makeImageView(
		VkImage image, 
		VkFormat format, 
		VkImageAspectFlags aspectFlags, 
		uint32_t mipLevels,
		VkImageView & imageView);

	static void makeImage(
		uint32_t width,
		uint32_t height,
		uint32_t mipLevels,
		VkSampleCountFlagBits samples,
		VkFormat format,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkImage & image,
		VkDeviceMemory & memory);

	static void changeImageLayout(
		VkImage image,
		VkFormat format,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		uint32_t mipLevels);

	Image();
	Image(VkExtent2D extent,
		VkFormat format,
		uint32_t mipLevels,
		VkSampleCountFlagBits samples,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkImageLayout layout,
		VkImageAspectFlags aspects = VK_IMAGE_ASPECT_COLOR_BIT);

	VkImageView getImageView();
	void cleanup();
};
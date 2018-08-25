#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

class Image
{
private:
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;
public:
	static VkImageView makeImageView(
		VkImage image, 
		VkFormat format, 
		VkImageAspectFlags aspectFlags, 
		uint32_t mipLevels);

	Image();

};
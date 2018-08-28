#include "Boturi.h"
#include "Vulkan/MemoryType.h"
#include "Vulkan/CommandBuffer.h"

void Image::makeImageView(
	VkImage image,
	VkFormat format,
	VkImageAspectFlags aspectFlags,
	uint32_t mipLevels,
	VkImageView & imageView)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(Boturi::device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		Boturi::printError("failed to create texture image view!");
}

void Image::makeImage(
	uint32_t width,
	uint32_t height,
	uint32_t mipLevels,
	VkSampleCountFlagBits samples,
	VkFormat format,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkImage & image,
	VkDeviceMemory & memory) 
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = samples;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(Boturi::device, &imageInfo, nullptr, &image) != VK_SUCCESS)
		Boturi::printError("failed to create image!");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(Boturi::device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(Boturi::device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate image memory!");

	vkBindImageMemory(Boturi::device, image, memory, 0);
}

void Image::changeImageLayout(VkImage image,
	VkFormat format,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	uint32_t mipLevels)
{
	VkCommandBuffer commandBuffer = CommandBuffer::startCommand();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else 
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED 
		&& newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL 
		  && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED 
		  && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | 
								VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED 
		  && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) 
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else 
		Boturi::printError("unsupported layout transition!");

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	CommandBuffer::endCommand(commandBuffer);
}

void Image::fillImage(VkExtent2D extent,
	VkFormat format,
	uint32_t mipLevels,
	VkSampleCountFlagBits samples,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkImageLayout layout,
	VkImageAspectFlags aspects)
{
	makeImage(extent.width, extent.height, mipLevels, samples, format, usage, properties, image, memory);
	changeImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, layout, mipLevels);
	makeImageView(image, format, aspects, mipLevels, imageView);
}

Image::Image() {}

Image::Image(VkExtent2D extent, 
	VkFormat format, 
	uint32_t mipLevels, 
	VkSampleCountFlagBits samples, 
	VkImageUsageFlags usage, 
	VkMemoryPropertyFlags properties,
	VkImageLayout layout,
	VkImageAspectFlags aspects)
{
	fillImage(extent, format, mipLevels, samples, usage, properties, layout, aspects);
}

void Image::cleanup()
{
	vkDestroyImageView(Boturi::device, imageView, nullptr);

	vkDestroyImage(Boturi::device, image, nullptr);
	vkFreeMemory(Boturi::device, memory, nullptr);
}

VkImageView Image::getImageView() { return imageView; }
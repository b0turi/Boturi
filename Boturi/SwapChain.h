#pragma once

#include "Image.h"
#include <algorithm>
#include <array>
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) 
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	for (const auto& availableFormat : availableFormats) 
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && 
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}

	return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) 
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) 
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			bestMode = availablePresentMode;
	}

	return bestMode;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;
	else 
	{
		int width, height;
		glfwGetFramebufferSize(Boturi::window, &width, &height);

		std::cout << width << ", " << height << std::endl;

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width, 
			std::min(capabilities.maxImageExtent.width, actualExtent.width));

		actualExtent.height = std::max(capabilities.minImageExtent.height, 
			std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VkResult makeSwapChain(VkSwapchainKHR & swapChain, uint32_t & numImages, VkFormat & imageFormat, VkExtent2D & extent)
{
	Boturi::swapChainDetails = getSwapChainSupport(Boturi::physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(Boturi::swapChainDetails.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(Boturi::swapChainDetails.presentModes);
	VkExtent2D ext = chooseSwapExtent(Boturi::swapChainDetails.capabilities);

	uint32_t imageCount = Boturi::swapChainDetails.capabilities.minImageCount + 1;
	if (Boturi::swapChainDetails.capabilities.maxImageCount > 0 && 
		imageCount > Boturi::swapChainDetails.capabilities.maxImageCount) 
		imageCount = Boturi::swapChainDetails.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = Boturi::surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = ext;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = 
	{ 
		(uint32_t)Boturi::graphicsQueueIndex, 
		(uint32_t)Boturi::presentQueueIndex 
	};

	if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else 
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	createInfo.preTransform = Boturi::swapChainDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	numImages = imageCount;
	imageFormat = surfaceFormat.format;
	extent = ext;
	Boturi::aspectRatio = 2;

	return vkCreateSwapchainKHR(Boturi::device, &createInfo, nullptr, &swapChain);
}

void fillSwapChain()
{
	// Get swap chain images
	uint32_t imgs = Boturi::numImages;
	vkGetSwapchainImagesKHR(Boturi::device, Boturi::swapChain, &imgs, nullptr);
	Boturi::swapChainImages.resize(Boturi::numImages);
	vkGetSwapchainImagesKHR(Boturi::device, Boturi::swapChain, &imgs, Boturi::swapChainImages.data());

	// Create swap chain image views
	Boturi::swapChainImageViews.resize(Boturi::swapChainImages.size());
	for (uint32_t i = 0; i < Boturi::swapChainImages.size(); i++)
		Image::makeImageView(
			Boturi::swapChainImages[i], 
			Boturi::imageFormat, 
			VK_IMAGE_ASPECT_COLOR_BIT, 
			1,
			Boturi::swapChainImageViews[i]);
}

VkResult makeFrameBuffers(std::vector<VkFramebuffer> & frameBuffers)
{
	frameBuffers.resize(Boturi::swapChainImageViews.size());

	for (size_t i = 0; i < Boturi::swapChainImageViews.size(); i++) 
	{
		std::array<VkImageView, 3> attachments = {
			Boturi::colorAttachment.getImageView(),
			Boturi::depthAttachment.getImageView(),
			Boturi::swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = Boturi::renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = Boturi::extent.width;
		framebufferInfo.height = Boturi::extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(Boturi::device, &framebufferInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS)
			return VK_ERROR_FEATURE_NOT_PRESENT;
	}
	
	return VK_SUCCESS;
}

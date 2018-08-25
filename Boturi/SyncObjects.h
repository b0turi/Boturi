#pragma once

VkResult makeSyncObjects(std::vector<VkSemaphore> & imgSemaphores,
						 std::vector<VkSemaphore> & renSemaphores,
						 std::vector<VkFence> & iffs)
{
	imgSemaphores.resize(Boturi::MAX_FRAMES_IN_FLIGHT);
	renSemaphores.resize(Boturi::MAX_FRAMES_IN_FLIGHT);
	iffs.resize(Boturi::MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < Boturi::MAX_FRAMES_IN_FLIGHT; i++) 
	{
		if (vkCreateSemaphore(Boturi::device, &semaphoreInfo, nullptr, &imgSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(Boturi::device, &semaphoreInfo, nullptr, &renSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(Boturi::device, &fenceInfo, nullptr, &iffs[i]) != VK_SUCCESS)
			return VK_ERROR_INITIALIZATION_FAILED;
	}

	return VK_SUCCESS;
}
#pragma once

class CommandBuffer
{
private:
	std::vector<VkCommandBuffer> commandBuffers;
public:
	int objCount = 0;

	static VkResult makeCommandPool(VkCommandPool & commandPool);
	static VkCommandBuffer startCommand();
	static void endCommand(VkCommandBuffer commandBuffer);

	CommandBuffer();
	CommandBuffer(std::vector<GameObject> objects);

	void cleanup();
	VkCommandBuffer getCommandBuffer(uint32_t index);
};
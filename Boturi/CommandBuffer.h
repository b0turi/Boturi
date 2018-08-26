#pragma once

class CommandBuffer
{
private:
	std::vector<VkCommandBuffer> commandBuffers;
public:
	static VkResult makeCommandPool(VkCommandPool & commandPool);
	static VkCommandBuffer startCommand();
	static void endCommand(VkCommandBuffer commandBuffer);

	CommandBuffer();
	CommandBuffer(Pipeline pipeline, Mesh mesh, Descriptor descriptor);

	void cleanup();
	VkCommandBuffer getCommandBuffer(uint32_t index);
};
#pragma once

class CommandBuffer
{
private:
public:
	static VkResult makeCommandPool(VkCommandPool & commandPool);
	static VkCommandBuffer startCommand();
	static void endCommand(VkCommandBuffer commandBuffer);
};
#include "Boturi.h"
#include "MemoryType.h"

VkResult makeBuffer(
	VkDeviceSize size, 
	VkBufferUsageFlags usage, 
	VkMemoryPropertyFlags properties,
	VkBuffer & buffer,
	VkDeviceMemory & memory)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(Boturi::device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		Boturi::printError("failed to create buffer!");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(Boturi::device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	VkResult result = vkAllocateMemory(Boturi::device, &allocInfo, nullptr, &memory);

	if(result != VK_SUCCESS)
		Boturi::printError("failed to allocate buffer memory!");

	vkBindBufferMemory(Boturi::device, buffer, memory, 0);

	return result;
}

void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = CommandBuffer::startCommand();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

	CommandBuffer::endCommand(commandBuffer);
}

Buffer::Buffer() {} 

Buffer::Buffer(VkDeviceSize size, void * info, VkBufferUsageFlags usage)
{
	Buffer stagingBuffer(size, info);
	makeBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory);
	copyBuffer(stagingBuffer.buffer, buffer, size);
	stagingBuffer.cleanup();
}

Buffer::Buffer(VkDeviceSize size, void * info)
{
	makeBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer, memory);

	void * data;
	vkMapMemory(Boturi::device, memory, 0, size, 0, &data);
	memcpy(data, info, (size_t)size);
	vkUnmapMemory(Boturi::device, memory);
}

void Buffer::cleanup()
{
	vkDestroyBuffer(Boturi::device, buffer, nullptr);
	vkFreeMemory(Boturi::device, memory, nullptr);
}

VkBuffer Buffer::getBuffer() { return buffer; }
#pragma once

class Buffer
{
private:
	VkBuffer buffer;
	VkDeviceMemory memory;

	
public:
	Buffer();
	//Creates a buffer passed into with a staging buffer
	Buffer(VkDeviceSize size, void * info, VkBufferUsageFlags usage);
	//Creates a staging buffer
	Buffer(VkDeviceSize size, void * info);
	//Creates a uniform buffer
	Buffer(VkDeviceSize size);

	VkBuffer getBuffer();
	VkDeviceMemory getMemory();

	void cleanup();
};
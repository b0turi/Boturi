#pragma once

class Buffer
{
private:
	VkBuffer buffer;
	VkDeviceMemory memory;

	
public:
	Buffer();
	//One constructor will handle normal buffers (vertex, index, uniform)
	Buffer(VkDeviceSize size, void * info, VkBufferUsageFlags usage);
	//The other handles staging buffers
	Buffer(VkDeviceSize size, void * info);

	VkBuffer getBuffer();

	void cleanup();
};
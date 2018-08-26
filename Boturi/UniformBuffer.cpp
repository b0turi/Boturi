#include "UniformBuffer.h"

/*UniformBuffer::UniformBuffer() {}
UniformBuffer::UniformBuffer(UniformType type) 
{
	uniformType = type;

	VkDeviceSize bufferSize = getUniformSize(type);
	buffers.resize(Boturi::numImages);

	for (int i = 0; i < Boturi::numImages; i++)
		buffers[i] = Buffer(bufferSize);
}

void UniformBuffer::update(void * data, uint32_t index)
{
	void * info;
	size_t uniformSize = getUniformSize(uniformType);
	vkMapMemory(Boturi::device, buffers[index].getMemory(), 0, uniformSize, 0, &info);
	memcpy(info, data, uniformSize);
	vkUnmapMemory(Boturi::device, buffers[index].getMemory());
}*/
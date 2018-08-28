#include "Boturi.h"

UniformBuffer::UniformBuffer() {}
UniformBuffer::UniformBuffer(UniformType type) 
{
	uniformType = type;

	VkDeviceSize bufferSize = Boturi::getUniformSize(type);
	buffers.resize(Boturi::numImages);

	for (uint32_t i = 0; i < Boturi::numImages; i++)
		buffers[i] = Buffer(bufferSize);
}

void UniformBuffer::update(void * data, uint32_t index)
{
	void * info;
	size_t uniformSize = Boturi::getUniformSize(uniformType);
	vkMapMemory(Boturi::device, buffers[index].getMemory(), 0, uniformSize, 0, &info);
	memcpy(info, data, uniformSize);
	vkUnmapMemory(Boturi::device, buffers[index].getMemory());
}

VkBuffer UniformBuffer::getBuffer(int index)
{
	return buffers[index].getBuffer();
}

UniformType UniformBuffer::getUniformType() { return uniformType; }

void UniformBuffer::cleanup()
{
	for (auto buffer : buffers)
		buffer.cleanup();
}
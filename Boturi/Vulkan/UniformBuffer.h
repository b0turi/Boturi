#pragma once
class UniformBuffer
{
private:
	std::vector<Buffer> buffers;
	UniformType uniformType;
public:
	UniformBuffer();
	UniformBuffer(UniformType type);

	void update(void * data, uint32_t index);
	VkBuffer getBuffer(int index);
	UniformType getUniformType();

	void cleanup();
};
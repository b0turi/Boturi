#include "Boturi.h"
Descriptor::Descriptor() {}

VkResult Descriptor::makeDescriptorSetLayout(std::vector<BindingType> definition)
{
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	for (size_t i = 0; i < definition.size(); i++)
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = i;
		binding.descriptorCount = 1;
		binding.pImmutableSamplers = nullptr;
		
		switch (definition[i])
		{
		case UNIFORM_BUFFER:
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case TEXTURE_SAMPLER:
			binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		}

		bindings.push_back(binding);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	return vkCreateDescriptorSetLayout(Boturi::device, &layoutInfo, nullptr, &layout);
}

VkResult Descriptor::makeDescriptorPool(std::vector<BindingType> definition)
{
	std::vector<VkDescriptorPoolSize> poolSizes;

	for (size_t i = 0; i < definition.size(); i++)
	{
		VkDescriptorPoolSize size;
		switch (definition[i])
		{
		case UNIFORM_BUFFER:
			size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			size.descriptorCount = Boturi::numImages;
			break;
		case TEXTURE_SAMPLER:
			size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			size.descriptorCount = Boturi::numImages;
			break;
		}

		poolSizes.push_back(size);
	}

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = Boturi::numImages;

	return vkCreateDescriptorPool(Boturi::device, &poolInfo, nullptr, &pool);
}

VkDescriptorBufferInfo getBufferInfo(UniformBuffer buffer, int index)
{
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = buffer.getBuffer(index);
	bufferInfo.offset = 0;
	bufferInfo.range = Boturi::getUniformSize(buffer.getUniformType());

	return bufferInfo;
}

VkDescriptorImageInfo getImageInfo(Texture texture)
{
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = texture.getImageView();
	imageInfo.sampler = Boturi::getTextureSampler(texture.getMipLevels());

	return imageInfo;
}

VkResult Descriptor::makeDescriptorSets(
	std::vector<BindingType> definition, 
	std::vector<UniformBuffer> uniforms, 
	std::vector<Texture> textures)
{
	std::vector<VkDescriptorSetLayout> layouts(Boturi::numImages, layout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = Boturi::numImages;
	allocInfo.pSetLayouts = layouts.data();

	sets.resize(Boturi::numImages);
	if (vkAllocateDescriptorSets(Boturi::device, &allocInfo, &sets[0]) != VK_SUCCESS)
		return VK_ERROR_OUT_OF_DEVICE_MEMORY;

	for (int i = 0; i < Boturi::numImages; i++) 
	{
		int uPtr = 0;
		int tPtr = 0;
		std::vector<VkWriteDescriptorSet> descriptorWrites;

		for (int j = 0; j < definition.size(); j++)
		{
			VkWriteDescriptorSet descriptorSet = {};

			descriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorSet.dstSet = sets[i];
			descriptorSet.dstBinding = j;
			descriptorSet.dstArrayElement = 0;
			descriptorSet.descriptorCount = 1;

			std::cout << definition[j] << std::endl;

			switch (definition[j])
			{
			case UNIFORM_BUFFER:
				descriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorSet.pBufferInfo = &getBufferInfo(uniforms[uPtr], i);
				uPtr++;
				break;
			case TEXTURE_SAMPLER:
				descriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSet.pImageInfo = &getImageInfo(textures[tPtr]);
				tPtr++;
				break;
			}

			descriptorWrites.push_back(descriptorSet);
		}

		vkUpdateDescriptorSets(Boturi::device, static_cast<uint32_t>(descriptorWrites.size()), 
								descriptorWrites.data(), 0, nullptr);
	}

	return VK_SUCCESS;
}

Descriptor::Descriptor(std::vector<BindingType> definition) 
{
	makeDescriptorSetLayout(definition);
	makeDescriptorPool(definition);
}

void Descriptor::cleanup()
{
	vkDestroyDescriptorPool(Boturi::device, pool, nullptr);
	vkDestroyDescriptorSetLayout(Boturi::device, layout, nullptr);
}

VkDescriptorSetLayout Descriptor::getLayout() { return layout; }
VkDescriptorSet Descriptor::getDescriptorSet(int index) { return sets[index]; }
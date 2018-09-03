#include "Boturi.h"
Descriptor::Descriptor() {}

VkDescriptorSetLayout Descriptor::makeDescriptorSetLayout(std::vector<BindingType> definition)
{
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	for (int i = 0; i < definition.size(); i++)
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = i;
		binding.descriptorCount = 1;
		binding.descriptorType = (definition[i] == 0) ? 
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.stageFlags = (definition[i] == 0) ?
			VK_SHADER_STAGE_VERTEX_BIT :
			VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings.push_back(binding);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	VkDescriptorSetLayout layout;
	vkCreateDescriptorSetLayout(Boturi::device, &layoutInfo, nullptr, &layout);
	return layout;
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

std::vector<VkDescriptorBufferInfo> fillUniforms(std::vector<UniformBuffer> uniforms, int index)
{
	std::vector<VkDescriptorBufferInfo> info = {};
	for (int i = 0; i < uniforms.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniforms[i].getBuffer(index);
		bufferInfo.offset = 0;
		bufferInfo.range = Boturi::getUniformSize(uniforms[i].getUniformType());

		info.push_back(bufferInfo);
	}

	return info;
}

VkResult Descriptor::makeDescriptorSets(
	std::vector<BindingType> definition, 
	std::vector<UniformBuffer> uniforms, 
	std::vector<Texture> textures)
{


	VkDescriptorSetLayout desc = Boturi::descriptors[definition];
	
	std::vector<VkDescriptorSetLayout> layouts(Boturi::numImages, desc);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = Boturi::numImages;
	allocInfo.pSetLayouts = layouts.data();

	sets.resize(Boturi::numImages);
	if (vkAllocateDescriptorSets(Boturi::device, &allocInfo, &sets[0]) != VK_SUCCESS)
		Boturi::printError("Unable to allocate space for descriptor sets");

	for (uint32_t i = 0; i < Boturi::numImages; i++) 
	{
		std::vector<VkDescriptorBufferInfo> unis = fillUniforms(uniforms, i);


		int uPtr = 0;
		int tPtr = 0;
		std::vector<VkWriteDescriptorSet> descriptorWrites;

		for (int j = 0; j < definition.size(); j++)
		{
			VkWriteDescriptorSet set = {};
			set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			set.dstSet = sets[i];
			set.dstBinding = j;
			set.dstArrayElement = 0;
			set.descriptorCount = 1;
			set.descriptorType = (definition[j] == 0) ?
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

			if (definition[j] == 0)
			{
				set.pBufferInfo = &unis[uPtr];
				uPtr += 1;
			}
			else
			{
				VkDescriptorImageInfo imageInfo = {};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = textures[tPtr].getImageView();
				imageInfo.sampler = Boturi::getTextureSampler(textures[tPtr].getMipLevels());

				set.pImageInfo = &imageInfo;
			}

			descriptorWrites.push_back(set);
		}

		vkUpdateDescriptorSets(Boturi::device, static_cast<uint32_t>(descriptorWrites.size()), 
								descriptorWrites.data(), 0, nullptr);
	}

	return VK_SUCCESS;
}

Descriptor::Descriptor(std::vector<BindingType> definition, 
	std::vector<UniformBuffer> uniforms, 
	std::vector<Texture> textures) 
{
	makeDescriptorPool(definition);
	makeDescriptorSets(definition, uniforms, textures);
	Descriptor::definition = definition;
}

void Descriptor::cleanup()
{
	vkDestroyDescriptorPool(Boturi::device, pool, nullptr);
}

std::vector<BindingType> Descriptor::getDefinition() { return definition; }
VkDescriptorPool Descriptor::getPool() { return pool; }
VkDescriptorSet Descriptor::getSet(int index) { return sets[index]; }
#pragma once
class Descriptor
{
private:
	VkDescriptorSetLayout layout;
	VkDescriptorPool pool;
	std::vector<VkDescriptorSet> sets;

	VkResult makeDescriptorSetLayout(std::vector<BindingType> definition);
	VkResult makeDescriptorPool(std::vector<BindingType> definition);

	
public:
	Descriptor();

	Descriptor(std::vector<BindingType> definition);
	VkResult makeDescriptorSets(
		std::vector<BindingType> definition,
		std::vector<UniformBuffer> uniforms,
		std::vector<Texture> textures);

	void cleanup();
	VkDescriptorSetLayout getLayout();

	VkDescriptorSet getDescriptorSet(int index);
};
#pragma once
class Descriptor
{
private:
	VkDescriptorPool pool;
	std::vector<VkDescriptorSet> sets;

	VkResult makeDescriptorPool(std::vector<BindingType> definition);
	VkResult makeDescriptorSets(
		std::vector<BindingType> definition,
		std::vector<UniformBuffer> uniforms,
		std::vector<Texture> textures);

	std::vector<BindingType> definition;
public:
	Descriptor();

	Descriptor(std::vector<BindingType> definition, std::vector<UniformBuffer> uniforms, std::vector<Texture> textures);

	static VkDescriptorSetLayout makeDescriptorSetLayout(std::vector<BindingType> definition);

	void cleanup();

	std::vector<BindingType> getDefinition();
	VkDescriptorPool getPool();
	VkDescriptorSet getSet(int index);
};
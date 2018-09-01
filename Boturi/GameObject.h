#pragma once

class GameObject
{
private:
	std::string mesh;
	std::string texture;
	std::string pipeline;

	std::vector<UniformBuffer> uniforms;
	std::vector<VkDescriptorSet> descriptorSets;

	Descriptor desc;
	
	glm::vec3 position;

public:
	GameObject();
	GameObject(std::string mesh, std::string texture, std::string pipeline);
	Mesh getMesh();
	Pipeline getPipeline();
	Descriptor getDescriptor();
	void update(uint32_t imageIndex);

	void move(glm::vec3 amount);

	void cleanup();
};
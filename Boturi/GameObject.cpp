#include "Boturi.h"
GameObject::GameObject() {}
GameObject::GameObject(std::string mesh, std::string texture, std::string pipeline)
{
	GameObject::mesh = mesh;
	GameObject::texture = texture;
	GameObject::pipeline = pipeline;

	position = glm::vec3(0, 0, 0);

	std::vector<BindingType> definition = Boturi::pipelines[pipeline].shader.getDefinition();
	std::vector<UniformType> uniformTypes = Boturi::pipelines[pipeline].shader.getUniformTypes();

	for (auto type : uniformTypes)
		uniforms.push_back(UniformBuffer(type));
	std::vector<Texture> textures = { Boturi::textures[texture] };

	desc = Descriptor(definition, uniforms, textures);
}

void GameObject::update(uint32_t imageIndex)
{
	MVPMatrix m = {};
	m.model = glm::translate(glm::mat4(1.0f), position);
	m.view = glm::lookAt(glm::vec3(2, 2, 2), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
	m.projection = glm::perspective(glm::radians(45.0f), Boturi::aspectRatio, 0.1f, 10.0f);
	m.projection[1][1] *= -1;

	for (auto uni : uniforms)
		uni.update(&m, imageIndex);
}

void GameObject::move(glm::vec3 amount)
{
	position += amount;
}

void GameObject::cleanup()
{
	for (auto uni : uniforms)
		uni.cleanup();
	desc.cleanup();
}

Mesh GameObject::getMesh() { return Boturi::meshes[mesh]; }
Pipeline GameObject::getPipeline() { return Boturi::pipelines[pipeline]; }
Descriptor GameObject::getDescriptor() { return desc; }
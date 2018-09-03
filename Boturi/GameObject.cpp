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
	m.model = glm::scale(glm::translate(glm::mat4(1.0f), position), glm::vec3(0.5f, 0.5f, 0.5f));
	m.view = Boturi::camera.viewMatrix();
	m.projection = Boturi::camera.getProjectionMatrix();
	
	Light l = {};
	l.position = glm::vec4(0, 1, 0,1);
	l.color = glm::vec4(1,0.9f,0.2f,1);

    uniforms[0].update(&m, imageIndex);
	uniforms[1].update(&l, imageIndex);
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
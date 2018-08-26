#pragma once
class Mesh
{
private:
	Buffer vertexBuffer;
	Buffer indexBuffer;

	uint32_t indexCount;

	std::vector<Vertex> vertices;
	std::vector<int> indices;
	void loadModel(const char * filename);
public:
	Mesh();
	Mesh(const char * filename);
	void cleanup();

	VkBuffer getVertexBuffer();
	VkBuffer getIndexBuffer();

	uint32_t getIndexCount();
};
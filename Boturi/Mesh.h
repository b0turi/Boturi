#pragma once
class Mesh
{
private:
	Buffer vertexBuffer;
	Buffer indexBuffer;

	std::vector<Vertex> vertices;
	std::vector<int> indices;
	void loadModel(const char * filename);
public:
	Mesh();
	Mesh(const char * filename);
	void cleanup();
};
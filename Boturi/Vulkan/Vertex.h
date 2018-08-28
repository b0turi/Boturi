#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

// Stores data that will be passed to the vertex shader
// Functions for the binding and attribute descriptions
// can be found in the source for the Pipeline class
struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	bool operator==(const Vertex& other) const 
	{
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

namespace std 
{
	// A hash function for the Vertex struct so equality can be calculated
	template<> struct hash<Vertex> 
	{
		size_t operator()(Vertex const& vertex) const 
		{
			return ((hash<glm::vec3>()(vertex.pos) ^ 
					(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ 
					(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}
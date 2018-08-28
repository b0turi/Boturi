#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>


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
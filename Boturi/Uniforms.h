#pragma once
#include <glm/glm.hpp>

enum UniformType
{
	MVP_MATRIX = 0
};

struct MVPMatrix
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

static size_t getUniformSize(UniformType type)
{
	switch (type)
	{
	case MVP_MATRIX:
		return sizeof(MVPMatrix);
		break;
	default:
		Boturi::printError("The given uniform type does not exist");
		return 0;
		break;
	}
}
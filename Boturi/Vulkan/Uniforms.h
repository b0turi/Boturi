#pragma once
#include <glm/glm.hpp>

enum UniformType
{
	MVP_MATRIX = 0,
	LIGHT = 1
};

struct MVPMatrix
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

struct Light
{
	glm::vec4 position;
	glm::vec4 color;
};
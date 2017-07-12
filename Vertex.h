#pragma once
#include "DefaultHeader.h"
#include "OpenGLHeader.h"

struct Vertex
{
	// defualt is public
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 uv;
	glm::vec3 normal;
};
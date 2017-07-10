#pragma once

#include "DefaultHeader.h"
#include "OpenGLHeader.h"

class Face
{
public:
	Face() {}
	virtual ~Face(){}

	Face(const Face& other){}

	glm::vec3 vertexIndex;
	glm::vec3 uvIndex;
	glm::vec3 normalIndex;

	glm::vec3 calculatedNormal;

private:
};
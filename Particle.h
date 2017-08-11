#pragma once
#include "OpenGLHeader.h"

class Particle
{
public:
	float mass;
	float density;
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 acceleration;
	float viscosity;
	float gasconstant;
	float pressure;
};
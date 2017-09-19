#pragma once
#include "OpenGLHeader.h"

class Particle
{
public:
	Particle()
	{}

	Particle(const Particle& other)
	{
		this->id = other.id;
		this->mass = other.mass;
		this->density = other.density;
		this->position = other.position;
		this->velocity = other.velocity;
		this->acceleration = other.acceleration;
		this->normal = other.normal;
		this->viscosity = other.viscosity;
		this->pressure = other.pressure;
	}

	int id;
	float mass;
	float density;
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 acceleration;
	glm::vec2 normal;
	float viscosity;
	float pressure;

	bool operator==(const Particle& r) { return this->id == r.id; }
};

class PBFParticle
{
public:
	PBFParticle()
	{}

	PBFParticle(const PBFParticle& other)
	{
		this->id = other.id;
		this->density = other.density;
		this->lambda = other.lambda;
		this->position = other.position;
		this->deltaP = other.deltaP;
		this->predictPosition = other.predictPosition;
		this->velocity = other.velocity;
		this->force = other.force;
		this->colorField = other.colorField;
	}

	int id;
	float density;
	float lambda;
	glm::vec3 position;
	glm::vec3 deltaP;
	glm::vec3 predictPosition;
	glm::vec3 velocity;
	glm::vec3 force;
	glm::vec3 colorField;

	bool operator==(const PBFParticle& r) { return this->id == r.id; }
};
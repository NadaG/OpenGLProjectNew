#pragma once

#include "SceneObject.h"

class Light : public SceneObject
{
public:

	void Awake();
	void Start();
	void Update();

	glm::vec3 color;

private:
};
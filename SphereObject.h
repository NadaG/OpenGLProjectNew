#pragma once
#include "SceneObject.h"

class SphereObject : public SceneObject
{
public:
	SphereObject(ShaderProgram shaderProgram);

	void Awake();
	void Start();
	void Update();

private:
};
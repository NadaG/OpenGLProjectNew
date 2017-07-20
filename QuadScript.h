#pragma once
#include "SceneObject.h"

class QuadObject : public SceneObject
{
public:
	QuadObject(ShaderProgram shaderProgram);

	void Awake();
	void Start();
	void Update();

private:
};
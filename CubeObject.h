#pragma once
#include "SceneObject.h"

class CubeObject : public SceneObject
{
public:
	CubeObject(ShaderProgram shaderProgram);

	void Awake();
	void Start();
	void Update();

private:
};
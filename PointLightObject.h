#pragma once
#include "SceneObject.h"

class PointLightObject : public SceneObject
{
public:
	PointLightObject(ShaderProgram shaderProgram);
	
	void Awake();
	void Start();
	void Update();

private:
};
#pragma once
#include "SceneScript.h"
#include "SceneObject.h"

class LightScript : public SceneScript
{
public:
	void Awake();
	void Start();
	void Update();

private:
	float moveScale = 0.1f;
};
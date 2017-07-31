#pragma once
#include "SceneScript.h"
#include "SceneObject.h"

class CameraScript : public SceneScript
{
public:
	void Awake();
	void Start();
	void Update();

private:
	float moveScale = 0.1f;
};
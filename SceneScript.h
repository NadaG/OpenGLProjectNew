#pragma once

#include "DefaultHeader.h"
#include "InputManager.h"

class SceneObject;

class SceneScript
{
public:
	virtual void Awake() = 0;
	virtual void Start() = 0;
	virtual void Update() = 0;

	void SetObject(SceneObject* sceneObject) { this->sceneObject = sceneObject; }

protected:
	SceneObject *sceneObject;
};
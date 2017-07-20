#include "CameraScript.h"

void CameraScript::Awake()
{
	sceneObject->Translate(glm::vec3(0.0f, 0.0f, 10.0f));
}

void CameraScript::Start()
{
}

void CameraScript::Update()
{
	if (InputManager::GetInstance()->IsKey(GLFW_KEY_J))
	{

	}

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_SPACE))
	{
		sceneObject->Rotate(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f);
	}
}
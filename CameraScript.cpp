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
		sceneObject->Translate(glm::vec3(-moveScale, 0.0f, 0.0f));
	if (InputManager::GetInstance()->IsKey(GLFW_KEY_L))
		sceneObject->Translate(glm::vec3(moveScale, 0.0f, 0.0f));

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_I))
		sceneObject->Translate(glm::vec3(0.0f, moveScale, 0.0f));
	if (InputManager::GetInstance()->IsKey(GLFW_KEY_K))
		sceneObject->Translate(glm::vec3(0.0f, -moveScale, 0.0f));

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_U))
		sceneObject->Translate(glm::vec3(0.0f, 0.0f, moveScale));
	if (InputManager::GetInstance()->IsKey(GLFW_KEY_O))
		sceneObject->Translate(glm::vec3(0.0f, 0.0f, -moveScale));

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_SPACE))
	{
		sceneObject->Rotate(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f);
	}
}
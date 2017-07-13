#include "PointLightObject.h"

PointLightObject::PointLightObject(ShaderProgram shaderProgram)
{
	this->shaderProgram = shaderProgram;
}

void PointLightObject::Awake()
{
}

void PointLightObject::Start()
{
}

void PointLightObject::Update()
{
	if (InputManager::GetInstance()->IsKey(GLFW_KEY_J))
		Translate(glm::vec3(-0.05f, 0.0f, 0.0f));
	else if (InputManager::GetInstance()->IsKey(GLFW_KEY_L))
		Translate(glm::vec3(0.05f, 0.0f, 0.0f));

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_U))
		Translate(glm::vec3(0.0f, 0.0f, 0.05f));
	else if (InputManager::GetInstance()->IsKey(GLFW_KEY_O))
		Translate(glm::vec3(0.0f, 0.0f, -0.05f));

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_I))
		Translate(glm::vec3(0.0f, 0.05f, 0.0f));
	else if (InputManager::GetInstance()->IsKey(GLFW_KEY_K))
		Translate(glm::vec3(0.0f, -0.05f, 0.0f));

}
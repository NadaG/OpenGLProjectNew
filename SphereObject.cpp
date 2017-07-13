#include "SphereObject.h"

SphereObject::SphereObject(ShaderProgram shaderProgram)
{
	this->shaderProgram = shaderProgram;
}

void SphereObject::Awake()
{
}

void SphereObject::Start()
{
}

void SphereObject::Update()
{
	if (InputManager::GetInstance()->IsKey(GLFW_KEY_A))
		Translate(glm::vec3(-0.05f, 0.0f, 0.0f));
	else if (InputManager::GetInstance()->IsKey(GLFW_KEY_D))
		Translate(glm::vec3(0.05f, 0.0f, 0.0f));

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_Q))
		Translate(glm::vec3(0.0f, 0.0f, 0.05f));
	else if (InputManager::GetInstance()->IsKey(GLFW_KEY_E))
		Translate(glm::vec3(0.0f, 0.0f, -0.05f));

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_W))
		Translate(glm::vec3(0.0f, 0.05f, 0.0f));
	else if (InputManager::GetInstance()->IsKey(GLFW_KEY_S))
		Translate(glm::vec3(0.0f, -0.05f, 0.0f));
}
#include "SceneObject.h"

int SceneObject::objectNum = 0;

SceneObject::SceneObject()
{
	id = objectNum++;
	this->modelMatrix = glm::mat4();
}

SceneObject::SceneObject(const ShaderProgram& shaderProgram)
{
	id = objectNum++;
	this->shaderProgram = shaderProgram;
	this->modelMatrix = glm::mat4();
}

SceneObject::SceneObject(const ShaderProgram& shaderProgram, const Mesh& mesh)
{
	id = objectNum++;
	this->shaderProgram = shaderProgram;
	this->mesh = mesh;
	this->modelMatrix = glm::mat4();
}

void SceneObject::Translate(const glm::vec3& vec)
{
	this->modelMatrix = glm::translate(modelMatrix, vec);
}

void SceneObject::Rotate(const glm::vec3& vec)
{
	this->modelMatrix = glm::rotate(modelMatrix, 1.0f, vec);
}

void SceneObject::Scale(const glm::vec3& vec)
{
	this->modelMatrix = glm::scale(modelMatrix, vec);
}
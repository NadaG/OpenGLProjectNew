#include "SceneObject.h"

int SceneObject::objectNum = 0;

SceneObject::SceneObject(const ShaderProgram& shaderProgram)
{
	id = objectNum++;
	this->shaderProgram = shaderProgram;
}


SceneObject::SceneObject()
{
	id = objectNum++;
}

SceneObject::SceneObject(const ShaderProgram& shaderProgram, const Mesh& mesh)
{
	id = objectNum++;
	this->shaderProgram = shaderProgram;
	this->mesh = mesh;
}
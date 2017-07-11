#pragma once

#include "DefaultHeader.h"
#include "OpenGLHeader.h"
#include "ShaderLoader.h"
#include "Mesh.h"

class SceneObject
{
public:
	SceneObject();
	SceneObject(const ShaderProgram& shaderProgram);
	SceneObject(const ShaderProgram& shaderProgram, const Mesh& mesh);
	virtual ~SceneObject(){}

	void SetMesh(const Mesh& mesh) { this->mesh = mesh; }
	const Mesh& GetMesh() { return this->mesh; }
	const GLuint& GetShaderProgramID() { return shaderProgram.GetShaderProgram(); }

private:
	int id;
	static int objectNum;

	ShaderProgram shaderProgram;
	Mesh mesh;
};
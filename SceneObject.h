#pragma once

#include "DefaultHeader.h"
#include "OpenGLHeader.h"
#include "ShaderLoader.h"
#include "Mesh.h"
#include "InputManager.h"

class SceneObject
{
public:
	SceneObject();
	SceneObject(const ShaderProgram& shaderProgram);
	SceneObject(const ShaderProgram& shaderProgram, const Mesh& mesh);
	virtual ~SceneObject(){}

	void SetMesh(const Mesh& mesh) { this->mesh = mesh; }
	Mesh& GetMesh() { return this->mesh; }
	const GLuint& GetShaderProgramID() { return this->shaderProgram.GetShaderProgramID(); }
	ShaderProgram GetShaderProgram() { return this->shaderProgram; }
	const glm::mat4& GetModelMatrix() { return this->modelMatrix; }

	void Translate(const glm::vec3& vec);
	void Rotate(const glm::vec3& vec);
	void Scale(const glm::vec3& vec);

	glm::vec3 GetPosition() { return glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]); }

	virtual void Awake() = 0;
	virtual void Start() = 0;
	virtual void Update() = 0;

protected:
	int id;
	static int objectNum;

	ShaderProgram shaderProgram;
	Mesh mesh;
	glm::mat4 modelMatrix;
};
#pragma once

#include "DefaultHeader.h"
#include "OpenGLHeader.h"
#include "ShaderLoader.h"
#include "Mesh.h"
#include "InputManager.h"
#include "LightScript.h"

class SceneObject
{
public:
	SceneObject();
	SceneObject(const ShaderProgram& shaderProgram);
	SceneObject(const ShaderProgram& shaderProgram, const Mesh& mesh);
	virtual ~SceneObject(){}

	void GenerateVBO(const Mesh& mesh);
	Mesh& GetMesh() { return this->mesh; }
	const GLuint& GetShaderProgramID() { return this->shaderProgram.GetShaderProgramID(); }
	ShaderProgram GetShaderProgram() { return this->shaderProgram; }
	const glm::mat4 GetModelMatrix();
	const GLuint& GetVBO() { return vertexBuffer; }
	const GLuint& GetVAO() { return vertexArrayObject; }

	void Translate(const glm::vec3& vec);
	void Rotate(const glm::vec3& vec, float angle);
	void Scale(const glm::vec3& vec);

	void AttachScript(SceneScript* sceneScript);

	void SetPosition(const glm::vec3 position) { positionVector = position; }
	void SetRotate(const glm::mat4 mat) { rotationMatrix = mat; }
	void SetScale(const glm::vec3 scale) { scaleVector= scale; }

	glm::vec3 GetPosition() { return positionVector; }
	glm::vec3 GetScale() { return scaleVector; }
	glm::mat4 GetRotate() { return rotationMatrix; }

	void ScriptsAwake();
	void ScriptsStart();
	void ScriptsUpdate();

protected:
	int id;
	static int objectNum;

	ShaderProgram shaderProgram;
	Mesh mesh;
	GLuint vertexBuffer;
	GLuint vertexArrayObject;

	glm::vec3 scaleVector;
	glm::mat4 rotationMatrix;
	glm::vec3 positionVector;
	
	std::vector<SceneScript*> scripts;
};
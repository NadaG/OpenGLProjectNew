#pragma once
#include "OpenGLHeader.h"
#include "DefaultHeader.h"

class ShaderProgram
{
private:
	// main에서 여러 개의 ShaderLoader 객체를 사용할 수 있도록 하기
	GLuint shaderProgram;

	GLuint vertexShader;
	GLuint fragmentShader;

	int floatNum;
	std::vector<std::pair<LayoutType, int> > layouts;

public:

	ShaderProgram();
	ShaderProgram(const char * vertex_file_path, const char * fragment_file_path);
	virtual ~ShaderProgram();

	GLuint GetShaderProgramID(){ return shaderProgram; }

	void LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
	GLuint LoadShader(const char* shaderFilePath, int shaderType);

	void Use();

	void SetUniform1f(string name, int value);
	void SetUniform1i(string name, int value);
	void SetUniformMatrix4f(string name, glm::mat4 mat);
	void SetUniformVector3f(string name, glm::vec3 vec);
	void SetUniformVector3f(string name, float x, float y, float z);
};
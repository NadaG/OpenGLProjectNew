#pragma once
#include "OpenGLHeader.h"

class ShaderProgram
{
private:

	// main에서 여러 개의 ShaderLoader 객체를 사용할 수 있도록 하기
	GLuint shaderProgram;

	GLuint vertexShader;
	GLuint fragmentShader;

public:

	ShaderProgram();
	virtual ~ShaderProgram();

	GLuint GetShaderProgram(){ return shaderProgram; }

	void LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
	GLuint LoadShader(const char* shaderFilePath, int shaderType);
};
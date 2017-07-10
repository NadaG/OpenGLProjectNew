#pragma once
#include "OpenGLHeader.h"

class ShaderProgram
{
private:

	// main���� ���� ���� ShaderLoader ��ü�� ����� �� �ֵ��� �ϱ�
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
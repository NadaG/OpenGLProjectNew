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

	void AddLayout(LayoutType type, int size);
	const int& GetLayoutNum() { return layouts.size(); }
	const bool& IsLayoutExist(LayoutType type) { return GetLayoutSize(type) != 0; }
	const int& GetLayoutSize(LayoutType type);
	const int& GetFloatNum() { return floatNum; }
	void SetFloatNum(const int& value) { this->floatNum = value; }
};
#include "ShaderLoader.h"
#include "DefaultHeader.h"

ShaderProgram::ShaderProgram()
{
	shaderProgram = glCreateProgram();
}

ShaderProgram::~ShaderProgram() 
{
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void ShaderProgram::LoadShaders(const char * vertex_file_path, const char * fragment_file_path)
{
	vertexShader = LoadShader(vertex_file_path, GL_VERTEX_SHADER);
	fragmentShader = LoadShader(fragment_file_path, GL_FRAGMENT_SHADER);
}

GLuint ShaderProgram::LoadShader(const char* shaderFilePath, int shaderType)
{
	// ���̴� ����
	GLuint shaderID = glCreateShader(shaderType);

	// ���̴� �ڵ带 ���Ͽ��� �б�
	std::string shaderCode;
	std::ifstream shaderStream(shaderFilePath, std::ios::in);
	if (shaderStream.is_open())
	{
		std::string Line = "";
		while (getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}
	else
	{
		printf("���� %s �� ���� �� ����. ��Ȯ�� ���丮�� ��� ���Դϱ� ? FAQ �� �켱 �о�� �� ���� ������!\n", shaderFilePath);
		getchar();
		return -1;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// ���̴��� ������
	printf("Compiling shader : %s\n", shaderFilePath);
	char const * sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);

	// ���̴��� �˻�
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> shaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &shaderErrorMessage[0]);
		printf("%s\n", &shaderErrorMessage[0]);
	}

	// ���α׷��� ��ũ
	printf("Linking program\n");

	glAttachShader(shaderProgram, shaderID);
	glLinkProgram(shaderProgram);

	// ���α׷� �˻�
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &Result);
	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(shaderProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	return shaderID;
}
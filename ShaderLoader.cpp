#include "ShaderLoader.h"
#include "DefaultHeader.h"

ShaderProgram::ShaderProgram()
{
	shaderProgram = glCreateProgram();
	floatNum = 0;
}

ShaderProgram::ShaderProgram(const char * vertex_file_path, const char * fragment_file_path)
{
	shaderProgram = glCreateProgram();
	LoadShaders(vertex_file_path, fragment_file_path);
	floatNum = 0;
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
	// 쉐이더 생성
	GLuint shaderID = glCreateShader(shaderType);

	// 쉐이더 코드를 파일에서 읽기
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
		printf("파일 %s 를 읽을 수 없음. 정확한 디렉토리를 사용 중입니까 ? FAQ 를 우선 읽어보는 걸 잊지 마세요!\n", shaderFilePath);
		getchar();
		return -1;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// 쉐이더를 컴파일
	printf("Compiling shader : %s\n", shaderFilePath);
	char const * sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);

	// 쉐이더를 검사
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> shaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &shaderErrorMessage[0]);
		printf("%s\n", &shaderErrorMessage[0]);
	}

	// 프로그램에 링크
	printf("Linking program\n");

	glAttachShader(shaderProgram, shaderID);
	glLinkProgram(shaderProgram);

	// 프로그램 검사
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

void ShaderProgram::AddLayout(LayoutType type, int size)
{
	floatNum += size;
	layouts.push_back(std::make_pair(type, size));
}

const int& ShaderProgram::GetLayoutSize(LayoutType type)
{
	for (int i = 0; i < layouts.size(); i++)
	{
		if (type == layouts[i].first)
			return layouts[i].second;
	}
	return 0;
}

void ShaderProgram::CalcFloatNum()
{
	int ret = 0;
	if (IsLayoutExist(LAYOUT_POSITION))
		ret += GetLayoutSize(LAYOUT_POSITION);
	if (IsLayoutExist(LAYOUT_COLOR))
		ret += GetLayoutSize(LAYOUT_COLOR);
	if (IsLayoutExist(LAYOUT_NORMAL))
		ret += GetLayoutSize(LAYOUT_NORMAL);
	if (IsLayoutExist(LAYOUT_UV))
		ret += GetLayoutSize(LAYOUT_UV);
	
	floatNum = ret;
}
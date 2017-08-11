#pragma once
#include "DefaultHeader.h"
#include "OpenGLHeader.h"

static int WindowWidth = 128 * 5;
static int WindowHeight = 72 * 5;
static int ZNear = -100;
static int ZFar = 100;

class Debug
{
public:
	static Debug* GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new Debug();
		}
		return instance;
	}

	void Log(const std::size_t& num);
	void Log(const int& inum);
	void Log(const unsigned int& inum);
	void Log(const float& fnum);
	void Log(const string& str);
	void Log(const glm::mat4& mat);
	void Log(const glm::vec3 vec);
	void Log(const glm::vec2 vec);

private:
	Debug() {}
	Debug(const Debug& other) {}
	~Debug() {}

	static Debug* instance;
};

#pragma once

#include<gl/glew.h>
#include<GL/wglew.h>
#include<GLFW\glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtx/transform.hpp>

enum LayoutType
{
	LAYOUT_POSITION = 0,
	LAYOUT_COLOR = 1,
	LAYOUT_NORMAL = 2,
	LAYOUT_UV = 3
};
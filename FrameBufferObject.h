#pragma once

#include "DefaultHeader.h"
#include "OpenGLHeader.h"

class FrameBufferObject
{
public:
	FrameBufferObject(const int& width, const int& height);
	const GLuint& GenerateColorTexture();
	const GLuint& GenerateRenderBuffer();

	const GLuint& GetID() { return frameBuffer; }

private:
	GLuint frameBuffer;
	int width, height;
};
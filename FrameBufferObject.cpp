#include "FrameBufferObject.h"

FrameBufferObject::FrameBufferObject(const int& width, const int& height)
{
	glGenFramebuffers(1, &frameBuffer);
	this->width = width;
	this->height = height;
}

const GLuint& FrameBufferObject::GenerateColorTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	GLuint colorTex;
	glGenTextures(1, &colorTex);
	glBindTexture(GL_TEXTURE_2D, colorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// frame buffer에 color에 대한 texture 연결시키기
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

	return colorTex;
}

const GLuint& FrameBufferObject::GenerateRenderBuffer()
{
	GLuint renderBuffer;
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	// frame buffer에 depth와 stencil에 대한 texture 연결시키기
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	return renderBuffer;
}

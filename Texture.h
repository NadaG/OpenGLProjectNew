#pragma once
#include "DefaultHeader.h"
#include "OpenGLHeader.h"

class Texture
{
public:
	Texture(){}

	const int& GenerateShadowTexture();
	const int& GenerateTexture(string file, int textureIndex);
	const int& GenerateFloatingPointFramebufferTexture();
	const int& GenerateDepthRenderBufferObject();

private:
	const int SCR_WIDTH = 1280, SCR_HEIGHT = 720;
};
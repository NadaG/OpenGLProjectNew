#pragma once
#include "DefaultHeader.h"
#include "OpenGLHeader.h"

class Texture
{
public:
	Texture(){}
	Texture(string file, int TextureID){}

	const int& GenerateShadowTexture(int TextureIndex);
	const int& GenerateTexture(string file, int TextureIndex);

private:
};
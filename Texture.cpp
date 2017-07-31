#include "Texture.h"

void GenerateDatas(GLubyte* data, cv::Mat image)
{
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				data[(i * image.cols + j) * 3 + k] = image.at<cv::Vec3b>(j, i)[2 - k];
			}
		}
	}
}

const int& Texture::GenerateTexture(string file, int TextureIndex)
{
	// load image
	cv::Mat image = cv::imread(file, 1);
	GLubyte* imageData = new GLubyte[image.rows * image.cols * 3];
	GenerateDatas(imageData, image);

	GLuint imageTexture;
	glGenTextures(1, &imageTexture);
	glBindTexture(GL_TEXTURE_2D, imageTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glActiveTexture(TextureIndex);
	glBindTexture(GL_TEXTURE_2D, imageTexture);

	return imageTexture;
}

const int& Texture::GenerateShadowTexture()
{
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	GLuint imageTexture;
	glGenTextures(1, &imageTexture);
	glBindTexture(GL_TEXTURE_2D, imageTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	return imageTexture;
}
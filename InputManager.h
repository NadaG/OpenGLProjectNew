#pragma once

#include "DefaultHeader.h"
#include "OpenGLHeader.h"

class InputManager
{
public:
	static InputManager* GetInstance();

	void SetLeftMouseClicked(bool isClicked) { this->isLeftMouseClicked = isClicked; }
	bool IsLeftMouseClicked() { return this->isLeftMouseClicked; }
	void SetWindow(GLFWwindow* window);

private:
	InputManager() {};
	InputManager(const InputManager& other);
	~InputManager() {};

	bool isLeftMouseClicked = false;

	static InputManager* instance;
	GLFWwindow* window;
};
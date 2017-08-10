#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "FrameBufferObject.h"
#include "CameraScript.h"
#include "Debug.h"

using namespace std;

const int width = 1280;
const int height = 720;

void Render(ShaderProgram& shader);

GLuint VAO;
int pointNum = 100;

int main(int argc, char **argv)
{
	if (!glfwInit())
		cout << "glfw init error" << endl;

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window;
	window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
	InputManager::GetInstance()->SetWindow(window);

	if (window == NULL)
	{
		fprintf(stderr, "GLFW 윈도우를 여는데 실패했습니다. Intel GPU 를 사용한다면, 3.3 지원을 하지 않습니다. 2.1 버전용 튜토리얼을 시도하세요.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	ShaderProgram sphShader("SPH.vs", "SPH.fs");
	sphShader.Use();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLfloat* particlesPoses;
	// (x1,y1), (x2,y2)...
	particlesPoses = new GLfloat[pointNum * 2];

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			particlesPoses[(i * 10 + j) * 2] = j * 0.01f;
			particlesPoses[(i * 10 + j) * 2 + 1] = i * 0.01f;
		}
	}

	glGenVertexArrays(1, &VAO);

	GLuint particleBuffer;
	glGenBuffers(1, &particleBuffer);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	glBufferData(GL_ARRAY_BUFFER, pointNum * sizeof(GLfloat) * 2, particlesPoses, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		Render(sphShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glfwTerminate();
	return 0;
}

void Render(ShaderProgram& shader)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.Use();

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, pointNum * 2);
}
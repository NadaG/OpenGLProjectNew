#include "DefaultHeader.h"
#include "loadShader.h"
#include "ShaderLoader.h"

// TODO uniform and location value refacto
// TODO vertex buffer seperate
// TODO obj loader �ϱ�
// torus obj load

using namespace std;

int width = 1024;
int height = 768;
// w = 1 �̸� (x, y, z, 1)�̹Ƿ� ��ġ�̴�
// w = 0 �̸� (x, y, z, 0)�̹Ƿ� �����̴�

// �� �� ȭ�鿡 �� �ﰢ���� ���ÿ� �����Ͽ����Ѵ�.
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

	if (window == NULL)
	{
		fprintf(stderr, "GLFW �����츦 ���µ� �����߽��ϴ�. Intel GPU �� ����Ѵٸ�, 3.3 ������ ���� �ʽ��ϴ�. 2.1 ������ Ʃ�丮���� �õ��ϼ���.\n");
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

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// VAO ����
	// ������ ���� �߿��ϰ� �������� �ʾƵ� ��
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	int shaderProgramNum = 2;
	vector<ShaderProgram> shaderPrograms;
	vector<GLuint> programIDs;

	for (int i = 0; i < 2; i++)
	{
		ShaderProgram shaderProgram;
		// TODO refacto, object�� ���� ������ ���� ��� ����
		if (i)
			shaderProgram.LoadShaders("SimpleVertexShader.vs", "SimpleFragmentShader.fs");
		else
			shaderProgram.LoadShaders("SecondVertexShader.vs", "SecondFragmentShader.fs");
		shaderPrograms.push_back(shaderProgram);
		GLuint id;
		id = shaderPrograms[i].GetShaderProgram();
		programIDs.push_back(id);
	}

	GLuint matrixID = glGetUniformLocation(programIDs[0], "MVP");
	GLuint matrixID2 = glGetUniformLocation(programIDs[1], "MVP");

	glm::mat4 model = glm::mat4(0.5f);
	glm::mat4 model2 = glm::mat4(0.5f);
	model2 = glm::translate(model2, glm::vec3(2.0f, 0.0f, 0.0f));

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt
	(
		glm::vec3(4, 3, 3),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

	glm::mat4 mvp = projection * view * model;
	glm::mat4 mvp2 = projection * view * model2;

	static const GLfloat g_vertex_buffer_data[] =
	{
		-1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f
	};



	// ���ؽ����� ���� ���۸� �����
	// bufferdata �Լ��� ���� ���ۿ� �����͸� �������
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// SRT�� ������� ���� �����Ѵ�.
	do
	{
		glClear(GL_COLOR_BUFFER_BIT);

		// ����� ���̴� ���α׷�
		for (int i = 0; i < shaderProgramNum; i++)
		{
			////////////////////////////////////////////
			glUseProgram(programIDs[i]);

			if (i)
				glUniformMatrix4fv(matrixID2, 1, GL_FALSE, &mvp2[0][0]);
			else
				glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
				0,
				3,
				GL_FLOAT,
				GL_FALSE,
				sizeof(GL_FLOAT) * 6,
				(void*)0
			);

			glVertexAttribPointer(
				1, // layout(location)�� �� �����̴�.
				3,
				GL_FLOAT,
				GL_FALSE,
				sizeof(GL_FLOAT) * 6, // stride, 0�� �ϸ� GL_FLOAT�� 2��° ���� ������ ���� ����Ѵ�.
				(void*)(sizeof(GL_FLOAT) * 3) // �ϳ��� vertex ���� set���� �ش� layout�� �󸶳� ������ �ִ���
			);

			glDrawArrays(GL_TRIANGLES, 0, 3);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);

	for (int i = 0; i < shaderProgramNum; i++)
	{
		glDeleteProgram(programIDs[i]);
	}

	glfwTerminate();

	return 0;
}
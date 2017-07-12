#include "SceneObject.h"

// TODO texture mapping using load png file
// TODO materials
// TODO lighting map

using namespace std;

int width = 1024;
int height = 768;
// w = 1 이면 (x, y, z, 1)이므로 위치이다
// w = 0 이면 (x, y, z, 0)이므로 방향이다

// 단 한 화면에 두 삼각형이 동시에 존재하여야한다.
int main(int argc, char **argv)
{
	//cv::imread("wall.jpg", 1);

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

	// VAO 생성
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	int objNum = 2;
	SceneObject sceneObject[] =
	{
		ShaderProgram("SecondVertexShader.vs", "SecondFragmentShader.fs"),
		ShaderProgram("SecondVertexShader.vs", "SecondFragmentShader.fs")
	};

	// uniform location
	GLuint matrixID = glGetUniformLocation(sceneObject[0].GetShaderProgramID(), "MVP");
	GLuint directionalLightID = glGetUniformLocation(sceneObject[0].GetShaderProgramID(), "lightDirection");
	GLuint eyePosID = glGetUniformLocation(sceneObject[0].GetShaderProgramID(), "eyePos");

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	glm::vec3 eyePos(0.0f, 0.0f, 10.0f);

	Mesh *meshes = new Mesh[objNum];
	meshes[0].LoadMesh(QUAD);
	meshes[1].LoadMesh(SPHERE);
	
	for (int i = 0; i < objNum; i++)
	{
		sceneObject[i].SetMesh(meshes[i]);
	}
	
	// position + color + normal
	int floatNum = 9;
	GLfloat** vertexBufferDatas = new GLfloat*[objNum];
	
	for (int i = 0; i < objNum; i++)
	{
		Mesh mesh = sceneObject[i].GetMesh();		
		vertexBufferDatas[i] = new GLfloat[mesh.GetVertexNum() * floatNum];
		for (int j = 0; j < mesh.GetVertexNum(); j++)
		{
			// position
			vertexBufferDatas[i][j * floatNum] = mesh.GetVertice(j).position.x;
			vertexBufferDatas[i][j * floatNum + 1] = mesh.GetVertice(j).position.y;
			vertexBufferDatas[i][j * floatNum + 2] = mesh.GetVertice(j).position.z;
			
			// color
			vertexBufferDatas[i][j * floatNum + 3] = 1.0f;
			vertexBufferDatas[i][j * floatNum + 4] = 1.0f;
			vertexBufferDatas[i][j * floatNum + 5] = 1.0f;

			// normal
			vertexBufferDatas[i][j * floatNum + 6] = mesh.GetVertice(j).normal.x;
			vertexBufferDatas[i][j * floatNum + 7] = mesh.GetVertice(j).normal.y;
			vertexBufferDatas[i][j * floatNum + 8] = mesh.GetVertice(j).normal.z;
		}
	}

	// 버텍스들을 담을 버퍼를 만들고
	// bufferdata 함수를 통해 버퍼에 데이터를 집어넣음
	// buffer를 하나로 만들어 glBufferData를 한번만 호출하도록 하여 overhead를 줄일 수 있다.
	GLuint* vertexBuffers = new GLuint[objNum];
	glGenBuffers(objNum, vertexBuffers);
	for (int i = 0; i < objNum; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * meshes[i].GetVertexNum() * floatNum, vertexBufferDatas[i], GL_STATIC_DRAW);
	}

	double mousePosX = 0.0, mousePosY = 0.0, lastMousePosX = 0.0, lastMousePosY = 0.0;
	float cameraDeltaMove = 0.3f;

	// SRT의 순서대로 곱이 동작한다.
	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_A))
			sceneObject[1].Translate(glm::vec3(-0.05f, 0.0f, 0.0f));
		else if (glfwGetKey(window, GLFW_KEY_D))
			sceneObject[1].Translate(glm::vec3(0.05f, 0.0f, 0.0f));

		if (glfwGetKey(window, GLFW_KEY_Q))
			sceneObject[1].Translate(glm::vec3(0.0f, 0.0f, 0.05f));
		else if (glfwGetKey(window, GLFW_KEY_E))
			sceneObject[1].Translate(glm::vec3(0.0f, 0.0f, -0.05f));

		if (glfwGetKey(window, GLFW_KEY_W))
			sceneObject[1].Translate(glm::vec3(0.0f, 0.05f, 0.0f));
		else if (glfwGetKey(window, GLFW_KEY_S))
			sceneObject[1].Translate(glm::vec3(0.0f, -0.05f, 0.0f));

		glfwGetCursorPos(window, &mousePosX, &mousePosY);
		
		if (InputManager::GetInstance()->IsLeftMouseClicked())
		{
			// 오른쪽으로 갔다면
			if (mousePosX - lastMousePosX > 0)
				eyePos.x -= cameraDeltaMove;
			// 왼쪽으로 갔다면
			else if (mousePosX - lastMousePosX < 0)
				eyePos.x += cameraDeltaMove;

			if (mousePosY - lastMousePosY > 0)
				eyePos.y += cameraDeltaMove;
			else if (mousePosY - lastMousePosY < 0)
				eyePos.y -= cameraDeltaMove;
		}

		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;
		// 사용할 셰이더 프로그램 및 오브젝트 프로그램
		for (int i = 0; i < objNum; i++)
		{
			glUseProgram(sceneObject[i].GetShaderProgramID());
			glm::mat4 view = glm::lookAt
			(
				eyePos,
				glm::vec3(0, 0, 0),
				glm::vec3(0, 1, 0)
			);

			glm::mat4 mvp = projection * view * sceneObject[i].GetModelMatrix();
			glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

			glUniform3f(directionalLightID, 1.0f, 0.0f, -1.0f);
			glUniform3f(eyePosID, eyePos.x, eyePos.y, eyePos.z);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[i]);
			glVertexAttribPointer(
				0,
				3,
				GL_FLOAT,
				GL_FALSE,
				sizeof(GLfloat) * floatNum,
				(void*)0
			);

			glVertexAttribPointer(
				1, // layout(location)에 들어갈 숫자이다.
				3,
				GL_FLOAT,
				GL_FALSE,
				sizeof(GLfloat) * floatNum, // stride, 0로 하면 GL_FLOAT와 2번째 인자 정보를 통해 계산한다.
				(void*)(sizeof(GLfloat) * 3) // 하나의 vertex 정보 set에서 해당 layout이 얼마나 떨어져 있는지
			);

			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				sizeof(GLfloat) * floatNum,
				(void*)(sizeof(GLfloat) * 6)
			);

			glDrawArrays(GL_TRIANGLES, 0, meshes[i].GetVertexNum() * 3);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// input routine
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(objNum, vertexBuffers);
	glDeleteVertexArrays(1, &VertexArrayID);

	for (int i = 0; i < objNum; i++)
	{
		glDeleteProgram(sceneObject[i].GetShaderProgramID());
	}

	glfwTerminate();
	return 0;
}

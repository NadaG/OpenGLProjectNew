// SSAO�� ���ؼ� �˾ƺ���
// Ambient Occlusion�̶� ���� ���� ���� �κ�, ���� �κ� �� ambient light�� ����� ������ �ʴ� �κ��� ��Ӱ� ó���ϴ� ���̴�
// Screen Space ambient occlusion ������ geometry data�� ������ ambient occlusion�� ����� ���� �ƴ϶�
// Screen Space�� ���ؼ� �����
// ���� �ȼ��� ���� ���� �ȼ� �� ���� �ȼ����� depth�� ���� ���� �ִٸ�(������, ���̰� ���� ��, depth map������ ������)
// �ش� �ȼ��� ������ �ִ� ���̹Ƿ� occlusion factor�� ���� ����
// ambient ���� �ٲ�� ��, ������ sphere�� sampling�� �ϰ� �� ��� ������ ���� �� �ֱ� ������
// �ش� pixel�� normal ���� ���� �� normal�� ���ϴ� �ݱ��� sampling�� ��
// ���� sampling�� �� �� ������ ��� �ȼ��� �ϸ� performance�� �� �������Ƿ�
// random���� ���� �ȼ��� �̾Ƴ� �� rotate �����ν� performance�� ����
#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "FrameBufferObject.h"
#include "CameraScript.h"
#include "Debug.h"

using namespace std;

const int shadowWidth = 1024;
const int shadowHeight = 1024;

const int width = 1280;
const int height = 720;

int objNum;

glm::mat4 projection;
glm::mat4 view;

float near_plane = -10.f;
float far_plane = 20.0f;

const int layoutNum = 5;

void Render(Camera* camera, Light* light[], SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader);
void renderQuad();
float lerp(float a, float b, float f);

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

	ShaderProgram pbrShader("PBR.vs", "PBR.fs");
	ShaderProgram lightShader("LightShader.vs", "LightShader.fs");

	pbrShader.Use();

	Camera* camera = new Camera;
	int lightNum = 1;
	Light* lights[1];
	lights[0] = new Light;
	for (int i = 0; i < lightNum; i++)
		lights[i]->Scale(glm::vec3(0.1f, 0.1f, 0.1f));

	objNum = 1;
	SceneObject** sceneObjects = new SceneObject*[objNum];
	for (int i = 0; i < objNum; i++)
		sceneObjects[i] = new SceneObject();

	sceneObjects[0]->Scale(glm::vec3(0.01f, 0.01f, 0.01f));
	sceneObjects[0]->Translate(glm::vec3(0.0f, 0.0f, 0.0f));
	
	Mesh sphereMesh, trexMesh;
	sphereMesh.LoadMesh(SPHERE);
	trexMesh.LoadMesh(TREX);
	// ���ο��� vao id�� ������
	sceneObjects[0]->GenerateVBO(trexMesh);
	lights[0]->GenerateVBO(sphereMesh);

	// TODO �ϳ��� ��ũ��Ʈ�� ���� ���� object�� �ٿ��� �� �ִµ� �װ� �ȵ�..
	SceneScript *lightScript, *cameraScript;
	lightScript = new LightScript;
	cameraScript = new CameraScript;
	camera->AttachScript(cameraScript);
	lights[0]->AttachScript(lightScript);

	camera->ScriptsAwake();
	for (int i = 0; i < lightNum; i++)
		lights[i]->ScriptsAwake();
	for (int i = 0; i < objNum; i++)
		sceneObjects[i]->ScriptsAwake();

	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// SRT�� ������� ���� �����Ѵ�. ���� �Ҷ��� �ݴ����
	do
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Render(camera, lights, sceneObjects, pbrShader, lightShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// input routine
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteProgram(pbrShader.GetShaderProgramID());
	glDeleteProgram(lightShader.GetShaderProgramID());

	glfwTerminate();


	return 0;
}

float moveScale = 0.1f;

void Render(Camera* camera, Light* lights[], SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	view = glm::lookAt
	(
		camera->testPosition(),
		glm::vec3(0.0f, camera->testPosition().y, 0.0f),
		glm::vec3(0, 1.0f, 0.0f)
	);

	shader.Use();

	lights[0]->ScriptsUpdate();
	camera->ScriptsUpdate();

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_2))
		camera->Rotate(glm::vec3(0.0, 1.0, 0.0), 0.01);

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_3))
		camera->Rotate(glm::vec3(0.0, -1.0, 0.0), 0.01);

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_Z))
		lights[0]->Scale(glm::vec3(1.2f, 1.2f, 1.2f));
	
	if (InputManager::GetInstance()->IsKey(GLFW_KEY_X))
		lights[0]->Scale(glm::vec3(0.9f, 0.9f, 0.9f));

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_A))
	{
		glm::vec4 v = glm::inverse(view)*glm::vec4(-moveScale, 0.0f, 0.0f, 0.0f);
		lights[0]->Translate(glm::vec3(v));
	}
	
	if (InputManager::GetInstance()->IsKey(GLFW_KEY_D))
	{
		glm::vec4 v = glm::inverse(view)*glm::vec4(moveScale, 0.0f, 0.0f, 0.0f);
		lights[0]->Translate(glm::vec3(v));
	}

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_W))
	{
		glm::vec4 v = glm::inverse(view)*glm::vec4(0.0f, moveScale, 0.0f, 0.0f);
		lights[0]->Translate(glm::vec3(v));
	}

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_S))
	{
		glm::vec4 v = glm::inverse(view)*glm::vec4(0.0f, -moveScale, 0.0f, 0.0f);
		lights[0]->Translate(glm::vec3(v));
	}

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_Q))
	{
		glm::vec4 v = glm::inverse(view)*glm::vec4(0.0f, 0.0f, moveScale, 0.0f);
		lights[0]->Translate(glm::vec3(v));
	}

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_E))
	{
		glm::vec4 v = glm::inverse(view)*glm::vec4(0.0f, 0.0f, -moveScale, 0.0f);
		lights[0]->Translate(glm::vec3(v));
	}

	// ����� ���̴� ���α׷� �� ������Ʈ ���α׷�
	for (int i = 0; i < objNum; i++)
	{
		sceneObjects[i]->ScriptsUpdate();
		shader.SetUniformMatrix4f("model", sceneObjects[i]->GetModelMatrix());
		shader.SetUniformMatrix4f("view", view);
		shader.SetUniformMatrix4f("projection", projection);
		
		// 5�� layout �� ����
		// position, color, normal, uv
		for (int j = 0; j < layoutNum; j++)
			glEnableVertexAttribArray(j);

		// glDrawElements�� ����Ϸ��� VAO�� �ʿ���, ���� ��� indices�� ����� �־����  �ϰ�
		// VAO�� ���� ��� indices �ڸ��� NULL�� �־��ִ��� �˾Ƽ� bind �� VAO�� ���� �׷���
		glBindVertexArray(sceneObjects[i]->GetVAO());
		glDrawElements(GL_TRIANGLES, sceneObjects[i]->GetMesh().GetIndexNum(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// 5�� layout �� ����
		for (int j = 0; j < layoutNum; j++)
			glDisableVertexAttribArray(j);
	}

	glBindVertexArray(lights[0]->GetVAO());
	lightShader.Use();
	lightShader.SetUniformMatrix4f("model", lights[0]->GetModelMatrix());
	lightShader.SetUniformMatrix4f("view", view);
	lightShader.SetUniformMatrix4f("projection", projection);
	lightShader.SetUniformVector3f("lightBallColor", glm::vec3(1.0f, 1.0f, 0.0f));

	for (int j = 0; j < layoutNum; j++)
		glEnableVertexAttribArray(j);

	glDrawArrays(GL_TRIANGLES, 0, lights[0]->GetMesh().GetVertexNum() * 3);
	
	for (int j = 0; j < layoutNum; j++)
		glDisableVertexAttribArray(j);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = 
		{
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

float lerp(float a, float b, float f)
{
	return a + f*(b - a);
}
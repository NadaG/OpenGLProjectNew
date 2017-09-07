// SSAO에 대해서 알아보자
// Ambient Occlusion이라 함은 옷의 접은 부분, 구석 부분 등 ambient light가 제대로 들어오지 않는 부분을 어둡게 처리하는 것이다
// Screen Space ambient occlusion 실제로 geometry data를 가지고 ambient occlusion을 계산한 것이 아니라
// Screen Space를 통해서 계산함
// 주위 픽셀을 봐서 주위 픽셀 중 현재 픽셀보다 depth가 얕은 곳이 있다면(가까운곳, 높이가 높은 곳, depth map에서는 검은색)
// 해당 픽셀은 구석에 있는 것이므로 occlusion factor가 값이 생겨
// ambient 값이 바뀌게 됨, 하지만 sphere로 sampling을 하게 될 경우 문제가 생길 수 있기 때문에
// 해당 pixel의 normal 값을 구한 뒤 normal을 향하는 반구로 sampling을 함
// 또한 sampling을 할 때 주위의 모든 픽셀을 하면 performance가 안 좋아지므로
// random으로 일정 픽셀만 뽑아낸 뒤 rotate 함으로써 performance를 높임
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
	// 내부에서 vao id가 정해짐
	sceneObjects[0]->GenerateVBO(trexMesh);
	lights[0]->GenerateVBO(sphereMesh);

	// TODO 하나의 스크립트가 여러 개의 object에 붙여질 수 있는데 그게 안됨..
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

	// SRT의 순서대로 곱이 동작한다. 곱을 할때는 반대로임
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

	// 사용할 셰이더 프로그램 및 오브젝트 프로그램
	for (int i = 0; i < objNum; i++)
	{
		sceneObjects[i]->ScriptsUpdate();
		shader.SetUniformMatrix4f("model", sceneObjects[i]->GetModelMatrix());
		shader.SetUniformMatrix4f("view", view);
		shader.SetUniformMatrix4f("projection", projection);
		
		// 5는 layout 총 개수
		// position, color, normal, uv
		for (int j = 0; j < layoutNum; j++)
			glEnableVertexAttribArray(j);

		// glDrawElements를 사용하려면 VAO가 필요함, 없을 경우 indices를 제대로 넣어줘야  하고
		// VAO가 있을 경우 indices 자리에 NULL을 넣어주더라도 알아서 bind 된 VAO에 따라 그려줌
		glBindVertexArray(sceneObjects[i]->GetVAO());
		glDrawElements(GL_TRIANGLES, sceneObjects[i]->GetMesh().GetIndexNum(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// 5는 layout 총 개수
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
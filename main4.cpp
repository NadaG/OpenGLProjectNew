//HDR high dynamic range rendering
// shadow mapping with small bug
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

glm::mat4 lightProjection;
glm::mat4 lightView;
glm::mat4 lightSpaceMatrix;
glm::mat4 projection;

float near_plane = -10.f;
float far_plane = 20.0f;

const int layoutNum = 5;

GLuint brickwallTex;
GLuint brickwallNormalTex;

void Render(Camera* camera, Light* light, SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader);
void renderQuad();

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

	Camera* camera = new Camera;
	camera->Translate(glm::vec3(0.0f, 0.0f, 15.0f));

	Light* light = new Light;
	light->Scale(glm::vec3(0.1f, 0.1f, 0.1f));
	light->Translate(glm::vec3(0.0f, 0.0f, 10.0f));

	ShaderProgram phongShader("PhongShader.vs", "PhongShader.fs");
	ShaderProgram depthShader("SimpleDepthShader.vs", "SimpleDepthShader.fs");
	ShaderProgram quadShader("DebugQuad.vs", "DebugQuad.fs");
	ShaderProgram shadowShader("Shadow.vs", "Shadow.fs");
	ShaderProgram lightShader("LightShader.vs", "LightShader.fs");
	ShaderProgram textureShader("TextureShader.vs", "TextureShader.fs");


	objNum = 2;
	SceneObject** sceneObjects = new SceneObject*[objNum];
	for (int i = 0; i < objNum; i++)
		sceneObjects[i] = new SceneObject();

	sceneObjects[0]->Scale(glm::vec3(5.0, 5.0, 0.1f));
	sceneObjects[0]->Translate(glm::vec3(0.0f, -3.0f, 0.0f));
	sceneObjects[0]->Rotate(glm::vec3(1.0, 0.0, 0.0), glm::radians(90.0f));

	sceneObjects[1]->Translate(glm::vec3(0.0f, 0.0f, -5.0f));
	sceneObjects[1]->Scale(glm::vec3(5.0f, 5.0f, 0.1f));

	Mesh sphereMesh, cubeMesh;
	sphereMesh.LoadMesh(SPHERE);
	cubeMesh.LoadMesh(BOX);
	// 내부에서 vao id가 정해짐
	sceneObjects[0]->GenerateVBO(cubeMesh);
	sceneObjects[1]->GenerateVBO(cubeMesh);
	light->GenerateVBO(sphereMesh);

	// TODO 하나의 스크립트가 여러 개의 object에 붙여질 수 있는데 그게 안됨..
	SceneScript *lightScript, *cameraScript;
	lightScript = new LightScript;
	cameraScript = new CameraScript;
	camera->AttachScript(cameraScript);
	light->AttachScript(lightScript);

	camera->ScriptsAwake();
	light->ScriptsAwake();
	for (int i = 0; i < objNum; i++)
		sceneObjects[i]->ScriptsAwake();

	// defualt frame buffer, 화면에 그려짐
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Texture brickwall;
	brickwallTex = brickwall.GenerateTexture("brickwall.jpg", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brickwallTex);

	Texture brickwallNormal;
	brickwallNormalTex = brickwallNormal.GenerateTexture("brickwall_normal.jpg", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, brickwallNormalTex);

	// HDR start
	GLuint hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	
	Texture colorBufferTexture;
	GLuint colorBuffer = colorBufferTexture.GenerateFloatingPointFramebufferTexture(0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	Texture renderBufferObjectDepthTexture;
	GLuint renderBufferObjectDepth = renderBufferObjectDepthTexture.GenerateDepthRenderBufferObject();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferObjectDepth);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3(0.0f, 0.0f, 49.5f));
	lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
	lightPositions.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
	lightPositions.push_back(glm::vec3(0.8f, -1.7f, 6.0f));
	
	std::vector<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
	lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
	lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));

	// SRT의 순서대로 곱이 동작한다. 곱을 할때는 반대로임
	do
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render pass가 2개임
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Render(camera, light, sceneObjects, textureShader, lightShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// input routine
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteProgram(phongShader.GetShaderProgramID());
	glDeleteProgram(depthShader.GetShaderProgramID());
	glDeleteProgram(shadowShader.GetShaderProgramID());

	glfwTerminate();
	return 0;
}

void Render(Camera* camera, Light* light, SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader)
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt
	(
		camera->GetPosition(),
		glm::vec3(0, 0, -10.0) + camera->GetPosition(),
		glm::vec3(0, 1.0f, 0.0f)
	);

	glBindVertexArray(light->GetVAO());
	glm::mat4 lightMVP = projection * view * light->GetModelMatrix();
	lightShader.Use();
	lightShader.SetUniformMatrix4f("MVP", lightMVP);

	for (int j = 0; j < layoutNum; j++)
		glEnableVertexAttribArray(j);
	glDrawArrays(GL_TRIANGLES, 0, light->GetMesh().GetVertexNum() * 3);
	for (int j = 0; j < layoutNum; j++)
		glDisableVertexAttribArray(j);

	shader.Use();

	light->ScriptsUpdate();
	camera->ScriptsUpdate();

	// 사용할 셰이더 프로그램 및 오브젝트 프로그램
	for (int i = 0; i < objNum; i++)
	{
		sceneObjects[i]->ScriptsUpdate();

		// TODO uniform들을 따로 모아두는 것이 좋을 거 같다
		glm::mat4 mvp = projection * view * sceneObjects[i]->GetModelMatrix();
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(light->GetPosition(),
			glm::vec3(0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMatrix = lightProjection * lightView;

		// bind buffer를 굳이 하지 않더라도 vertex array object만 bind하면 알아서 됨
		glBindVertexArray(sceneObjects[i]->GetVAO());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, brickwallTex);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, brickwallNormalTex);

		shader.SetUniformMatrix4f("MVP", mvp);
		shader.SetUniformVector3f("lightPos", light->GetPosition());
		shader.SetUniformVector3f("lightColor", glm::vec3(1.0, 1.0, 1.0));
		shader.SetUniformVector3f("eyePos", camera->GetPosition());
		shader.SetUniformMatrix4f("model", sceneObjects[i]->GetModelMatrix());
		shader.SetUniformMatrix4f("view", view);
		shader.SetUniformMatrix4f("projection", projection);
		shader.SetUniformMatrix4f("viewProjectionMatrix", projection * view);
		shader.SetUniformMatrix4f("lightSpaceMatrix", lightSpaceMatrix);
		shader.SetUniform1i("material.diffuse", 0);
		shader.SetUniform1f("material.shininess", 32);
		shader.SetUniform1i("normalMap", 1);

		// 5는 layout 총 개수
		// position, color, normal, uv
		for (int j = 0; j < layoutNum; j++)
			glEnableVertexAttribArray(j);

		// glDrawElements를 사용하려면 VAO가 필요함, 없을 경우 indices를 제대로 넣어줘야  하고
		// VAO가 있을 경우 indices 자리에 NULL을 넣어주더라도 알아서 bind 된 VAO에 따라 그려줌
		// glDrawElements(GL_TRIANGLES, 0, GL_FLOAT, indices);
		glDrawArrays(GL_TRIANGLES, 0, sceneObjects[i]->GetMesh().GetVertexNum() * 3);

		// 5는 layout 총 개수
		for (int j = 0; j < layoutNum; j++)
			glDisableVertexAttribArray(j);
	}
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
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
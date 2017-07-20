#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "FrameBufferObject.h"
#include "CameraScript.h"

using namespace std;

const int shadowWidth = 1024;
const int shadowHeight = 1024;

const int width = 1024;
const int height = 768;

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

	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	Camera* camera = new Camera;
	camera->Translate(glm::vec3(0.0f, 0.0f, 10.0f));

	Light* light = new Light;
	light->Translate(glm::vec3(0.0f, 0.0f, 10.0f));

	ShaderProgram phongShader("PhongShader.vs", "PhongShader.fs");
	phongShader.AddLayout(LAYOUT_POSITION, 3);
	phongShader.AddLayout(LAYOUT_COLOR, 3);
	phongShader.AddLayout(LAYOUT_NORMAL, 3);
	phongShader.AddLayout(LAYOUT_UV, 2);

	ShaderProgram depthShader("SimpleDepthShader.vs", "SimpleDepthShader.fs");
	depthShader.AddLayout(LAYOUT_POSITION, 3);

	int objNum = 4;
	SceneObject** sceneObjects = new SceneObject*[objNum];
	sceneObjects[0] = new SceneObject(phongShader);
	sceneObjects[1] = new SceneObject(phongShader);
	sceneObjects[2] = new SceneObject(depthShader);
	sceneObjects[3] = new SceneObject(phongShader);

	sceneObjects[0]->Translate(glm::vec3(-1.0f, 0.0f, 4.0f));
	sceneObjects[1]->Translate(glm::vec3(2.0f, 0.0f, 0.0f));
	sceneObjects[2]->Translate(glm::vec3(0.0f, 0.0f, 0.0f));
	sceneObjects[3]->Translate(glm::vec3(0.0f, -3.0f, 0.0f));

	sceneObjects[3]->Scale(glm::vec3(5.0f, 0.1f, 5.0f));

	Mesh sphereMesh, cubeMesh;
	sphereMesh.LoadMesh(SPHERE);
	cubeMesh.LoadMesh(BOX);
	sceneObjects[0]->GenerateVBO(sphereMesh);
	sceneObjects[1]->GenerateVBO(sphereMesh);
	sceneObjects[2]->GenerateVBO(sphereMesh);
	sceneObjects[3]->GenerateVBO(cubeMesh);

	// TODO �ϳ��� ��ũ��Ʈ�� ���� ���� object�� �ٿ��� �� �ִµ� �װ� �ȵ�..
	SceneScript *lightScript, *cameraScript;
	lightScript = new LightScript;
	cameraScript = new CameraScript;
	camera->AttachScript(cameraScript);
	light->AttachScript(lightScript);

	camera->ScriptsAwake();
	light->ScriptsAwake();
	for (int i = 0; i < objNum; i++)
		sceneObjects[i]->ScriptsAwake();

	// defualt frame buffer, ȭ�鿡 �׷���
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	

	// depth map
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	Texture shadowMapTexture;
	GLuint depthMap = shadowMapTexture.GenerateShadowTexture(GL_TEXTURE0);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
									  glm::vec3(0.0f, 0.0f, 0.0f),
									  glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	//

	// SRT�� ������� ���� �����Ѵ�. ���� �Ҷ��� �ݴ����
	do
	{
		// depth map �׸��� ����
		glViewport(0, 0, shadowWidth, shadowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// TODO �� �����Ӹ��� position �� normal�� ������� �ҵ�
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::lookAt
		(
			camera->GetPosition(),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1.0f, 0.0f)
		);

		light->ScriptsUpdate();
		camera->ScriptsUpdate();
		// ����� ���̴� ���α׷� �� ������Ʈ ���α׷�
		for (int i = 0; i < objNum; i++)
		{
			sceneObjects[i]->ScriptsUpdate();

			ShaderProgram shaderProgram = sceneObjects[i]->GetShaderProgram();
			glUseProgram(shaderProgram.GetShaderProgramID());

			// TODO uniform���� ���� ��Ƶδ� ���� ���� �� ����
			glm::mat4 mvp = projection * view * sceneObjects[i]->GetModelMatrix();

			// bind buffer�� ���� ���� �ʴ��� vertex array object�� bind�ϸ� �˾Ƽ� ��
			glBindVertexArray(sceneObjects[i]->GetVAO());
			sceneObjects[i]->SetUniformMatrix4f("MVP", mvp);
			sceneObjects[i]->SetUniformVector3f("lightDirection", glm::normalize(-light->GetPosition()));
			sceneObjects[i]->SetUniformVector3f("eyePos", camera->GetPosition());
			sceneObjects[i]->SetUniformMatrix4f("lightSpaceMatrix", lightSpaceMatrix);
			sceneObjects[i]->SetUniformMatrix4f("model", sceneObjects[i]->GetModelMatrix());

			// 4�� layout �� ����
			for (int j = 0; j < 4; j++)
			{
				if (shaderProgram.IsLayoutExist((LayoutType)j))
					glEnableVertexAttribArray(j);
			}

			// glDrawElements�� ����Ϸ��� VAO�� �ʿ���, ���� ��� indices�� ����� �־����  �ϰ�
			// VAO�� ���� ��� indices �ڸ��� NULL�� �־��ִ��� �˾Ƽ� bind �� VAO�� ���� �׷���
			// glDrawElements(GL_TRIANGLES, 0, GL_FLOAT, indices);
			glDrawArrays(GL_TRIANGLES, 0, sceneObjects[i]->GetMesh().GetVertexNum() * 3);

			// 4�� layout �� ����
			for (int j = 0; j < 4; j++)
			{
				if (shaderProgram.IsLayoutExist((LayoutType)j))
					glDisableVertexAttribArray(j);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// input routine
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	for (int i = 0; i < objNum; i++)
	{
		glDeleteProgram(sceneObjects[i]->GetShaderProgramID());
	}

	glfwTerminate();
	return 0;
}
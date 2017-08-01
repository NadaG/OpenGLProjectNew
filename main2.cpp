//// shadow mapping with small bug
//#include "Camera.h"
//#include "Light.h"
//#include "Texture.h"
//#include "FrameBufferObject.h"
//#include "CameraScript.h"
//#include "Debug.h"
//
//using namespace std;
//
//const int shadowWidth = 1024;
//const int shadowHeight = 1024;
//
//const int width = 1280;
//const int height = 720;
//
//int objNum;
//
//glm::mat4 lightProjection;
//glm::mat4 lightView;
//glm::mat4 lightSpaceMatrix;
//glm::mat4 projection;
//
//float near_plane = -10.f;
//float far_plane = 20.0f;
//
//void Render(Camera* camera, Light* light, SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader);
//void renderQuad();
//
//int main(int argc, char **argv)
//{
//	if (!glfwInit())
//		cout << "glfw init error" << endl;
//
//	glfwWindowHint(GLFW_SAMPLES, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//	GLFWwindow* window;
//	window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
//	InputManager::GetInstance()->SetWindow(window);
//
//	if (window == NULL)
//	{
//		fprintf(stderr, "GLFW �����츦 ���µ� �����߽��ϴ�. Intel GPU �� ����Ѵٸ�, 3.3 ������ ���� �ʽ��ϴ�. 2.1 ������ Ʃ�丮���� �õ��ϼ���.\n");
//		glfwTerminate();
//		return -1;
//	}
//
//	glfwMakeContextCurrent(window);
//	glewExperimental = true;
//
//	if (glewInit() != GLEW_OK)
//	{
//		fprintf(stderr, "Failed to initialize GLEW\n");
//		return -1;
//	}
//
//	glEnable(GL_DEPTH_TEST);
//	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
//	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
//
//	Camera* camera = new Camera;
//	camera->Translate(glm::vec3(0.0f, 0.0f, 15.0f));
//
//	Light* light = new Light;
//	light->Scale(glm::vec3(0.1f, 0.1f, 0.1f));
//	light->Translate(glm::vec3(0.0f, 0.0f, 10.0f));
//
//	ShaderProgram phongShader("PhongShader.vs", "PhongShader.fs");
//	ShaderProgram depthShader("SimpleDepthShader.vs", "SimpleDepthShader.fs");
//	ShaderProgram quadShader("DebugQuad.vs", "DebugQuad.fs");
//	ShaderProgram shadowShader("Shadow.vs", "Shadow.fs");
//	ShaderProgram lightShader("LightShader.vs", "LightShader.fs");
//
//	objNum = 4;
//	SceneObject** sceneObjects = new SceneObject*[objNum];
//	sceneObjects[0] = new SceneObject();
//	sceneObjects[1] = new SceneObject();
//	sceneObjects[2] = new SceneObject();
//	sceneObjects[3] = new SceneObject();
//
//	sceneObjects[0]->Translate(glm::vec3(-1.0f, 0.0f, 4.0f));
//	sceneObjects[1]->Translate(glm::vec3(2.0f, 0.0f, 0.0f));
//	sceneObjects[2]->Translate(glm::vec3(0.0f, 0.0f, 0.0f));
//	sceneObjects[3]->Translate(glm::vec3(0.0f, -3.0f, 0.0f));
//
//	sceneObjects[3]->Scale(glm::vec3(5.0f, 0.1f, 5.0f));
//
//	Mesh sphereMesh, cubeMesh;
//	sphereMesh.LoadMesh(SPHERE);
//	cubeMesh.LoadMesh(BOX);
//	// ���ο��� vao id�� ������
//	sceneObjects[0]->GenerateVBO(sphereMesh);
//	sceneObjects[1]->GenerateVBO(sphereMesh);
//	sceneObjects[2]->GenerateVBO(sphereMesh);
//	sceneObjects[3]->GenerateVBO(cubeMesh);
//	light->GenerateVBO(sphereMesh);
//
//	// TODO �ϳ��� ��ũ��Ʈ�� ���� ���� object�� �ٿ��� �� �ִµ� �װ� �ȵ�..
//	SceneScript *lightScript, *cameraScript;
//	lightScript = new LightScript;
//	cameraScript = new CameraScript;
//	camera->AttachScript(cameraScript);
//	light->AttachScript(lightScript);
//
//	camera->ScriptsAwake();
//	light->ScriptsAwake();
//	for (int i = 0; i < objNum; i++)
//		sceneObjects[i]->ScriptsAwake();
//
//	// defualt frame buffer, ȭ�鿡 �׷���
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
//
//	// depth map�� �����ϰ� frame buffer�� ���δ�.
//	GLuint depthMapFBO;
//	glGenFramebuffers(1, &depthMapFBO);
//	Texture depthMapTexture;
//	GLuint depthMap = depthMapTexture.GenerateShadowTexture();
//
//	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
//	glDrawBuffer(GL_NONE);
//	glReadBuffer(GL_NONE);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	shadowShader.Use();
//	shadowShader.SetUniform1i("shadowMap", 0);
//
//	// SRT�� ������� ���� �����Ѵ�. ���� �Ҷ��� �ݴ����
//	do
//	{
//		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		//lightProjection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
//		
//		//glCullFace(GL_FRONT);
//		// depth map �׸��� ����
//		// depth map�� �� �ȼ����� ���� 0(near, ������),1(far, ���)�� �Ÿ��� ��������
//		glViewport(0, 0, shadowWidth, shadowHeight);
//		// fbo�� ���ε� �ϰ�(fbo���� depthMap texture�� �����Ǿ� ����, color texture�� �����Ǿ� ���� ����)
//		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//		// depth buffer�� ���� �׸� ���̹Ƿ� clear
//		glClear(GL_DEPTH_BUFFER_BIT);
//		// object���� depth buffer�� draw vertex shader�� �׳� position�� ����ϴϱ� depthMap�� ������
//		Render(camera, light, sceneObjects, depthShader, lightShader);
//		//glCullFace(GL_BACK);
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//		// debug quad�� �׸��� �κ�
//		/*glViewport(0, 0, width, height);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		quadShader.Use();
//		quadShader.SetUniform1f("near_plane", near_plane);
//		quadShader.SetUniform1f("far_plane", far_plane);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, depthMap);
//		renderQuad();*/
//
//		// render pass�� 2����
//		glViewport(0, 0, width, height);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		shadowShader.Use();
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, depthMap);
//		Render(camera, light, sceneObjects, shadowShader, lightShader);
//		
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//	// input routine
//	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
//		glfwWindowShouldClose(window) == 0);
//
//	glDeleteProgram(phongShader.GetShaderProgramID());
//	glDeleteProgram(depthShader.GetShaderProgramID());
//	glDeleteProgram(shadowShader.GetShaderProgramID());
//
//	glfwTerminate();
//	return 0;
//}
//
//void Render(Camera* camera, Light* light, SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader)
//{
//	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	glm::mat4 view = glm::lookAt
//	(
//		camera->GetPosition(),
//		glm::vec3(0, 0, -10.0) + camera->GetPosition(),
//		glm::vec3(0, 1.0f, 0.0f)
//	);
//
//	glBindVertexArray(light->GetVAO());
//	glm::mat4 lightMVP = projection * view * light->GetModelMatrix();
//	lightShader.Use();
//	lightShader.SetUniformMatrix4f("MVP", lightMVP);
//
//	for (int j = 0; j < 4; j++)
//		glEnableVertexAttribArray(j);
//	glDrawArrays(GL_TRIANGLES, 0, light->GetMesh().GetVertexNum() * 3);
//	for (int j = 0; j < 4; j++)
//		glDisableVertexAttribArray(j);
//
//	shader.Use();
//	
//	light->ScriptsUpdate();
//	camera->ScriptsUpdate();	
//
//	// ����� ���̴� ���α׷� �� ������Ʈ ���α׷�
//	for (int i = 0; i < objNum; i++)
//	{
//		sceneObjects[i]->ScriptsUpdate();
//
//		// TODO uniform���� ���� ��Ƶδ� ���� ���� �� ����
//		glm::mat4 mvp = projection * view * sceneObjects[i]->GetModelMatrix();
//		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//		lightView = glm::lookAt(light->GetPosition(),
//			glm::vec3(0.0f),
//			glm::vec3(0.0f, 1.0f, 0.0f));
//		lightSpaceMatrix = lightProjection * lightView;
//
//		// bind buffer�� ���� ���� �ʴ��� vertex array object�� bind�ϸ� �˾Ƽ� ��
//		glBindVertexArray(sceneObjects[i]->GetVAO());
//		
//		shader.SetUniformMatrix4f("MVP", mvp);
//		shader.SetUniformVector3f("lightPos", light->GetPosition());
//		shader.SetUniformVector3f("eyePos", camera->GetPosition());
//		shader.SetUniformMatrix4f("model", sceneObjects[i]->GetModelMatrix());
//		shader.SetUniformMatrix4f("view", view);
//		shader.SetUniformMatrix4f("projection", projection);
//		shader.SetUniformMatrix4f("viewProjectionMatrix", projection * view);
//		shader.SetUniformMatrix4f("lightSpaceMatrix", lightSpaceMatrix);
//
//		// 4�� layout �� ����
//		// position, color, normal, uv
//		for (int j = 0; j < 4; j++)
//			glEnableVertexAttribArray(j);
//
//		// glDrawElements�� ����Ϸ��� VAO�� �ʿ���, ���� ��� indices�� ����� �־����  �ϰ�
//		// VAO�� ���� ��� indices �ڸ��� NULL�� �־��ִ��� �˾Ƽ� bind �� VAO�� ���� �׷���
//		// glDrawElements(GL_TRIANGLES, 0, GL_FLOAT, indices);
//		glDrawArrays(GL_TRIANGLES, 0, sceneObjects[i]->GetMesh().GetVertexNum() * 3);
//
//		// 4�� layout �� ����
//		for (int j = 0; j < 4; j++)
//			glDisableVertexAttribArray(j);
//	}
//}
//
//unsigned int quadVAO = 0;
//unsigned int quadVBO;
//void renderQuad()
//{
//	if (quadVAO == 0)
//	{
//		float quadVertices[] = {
//			// positions        // texture Coords
//			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
//			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
//			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
//			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
//		};
//		// setup plane VAO
//		glGenVertexArrays(1, &quadVAO);
//		glGenBuffers(1, &quadVBO);
//		glBindVertexArray(quadVAO);
//		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//	}
//	glBindVertexArray(quadVAO);
//	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//	glBindVertexArray(0);
//}
//// deffered shading
//// deffered shading�̶� 2���� pass�� ���ļ� ȭ���� �׸��� ���� ���Ѵ�
//// ���� geometry pass���� object���� position, normal, specular���� ������ �̾Ƽ� �ؽ��Ŀ� �ְ�
//// �������� ��Ƴ��� texture�� g-buffer��� �Ѵ�. MTR(multiple render target)�� �̿��ؼ� �����
//// �̰��� �ϳ��� frame buffer object���� ���� ���� color texture�� �ٿ��� �����Ѵ�
//// lighting pass���� �׸��� �׸���.
//
//// forward shading�� ��� geometry ���� vs, gs, fs�� ���� ���̵��� �Ѵ�. �̴� �ڿ� �ִ� ������Ʈ���� 
//// lighting ������ ����� ���ɼ��� �ִٴ� ���̴�.
//// �ݸ� deffered shading�� ��� geometry ���� vs, gs, fs�� ��ġ���� lighting ������ ������� �ʴ´�.
//// position, normal, uv���� texture�� �̾Ƴ� �� lighting ������ �����Ѵ�.
//
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
//const int layoutNum = 5;
//
//std::vector<glm::vec3> lightPositions;
//std::vector<glm::vec3> lightColors;
//
//bool bloom = true;
//float exposure = 1.0f;
//
//GLuint earthTex;
//GLuint earthNormalTex;
//
//void Render(Camera* camera, Light* light[], SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader);
//void renderQuad();
//
//glm::vec3 cameraInitPos;
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
//
//	int lightNum = 1;
//	Light* lights[1];
//	lights[0] = new Light;
//	
//	for (int i = 0; i < lightNum; i++)
//		lights[i]->Scale(glm::vec3(0.1f, 0.1f, 0.1f));
//	
//	ShaderProgram phongShader("PhongShader.vs", "PhongShader.fs");
//	ShaderProgram depthShader("SimpleDepthShader.vs", "SimpleDepthShader.fs");
//	ShaderProgram quadShader("DebugQuad.vs", "DebugQuad.fs");
//	ShaderProgram shadowShader("Shadow.vs", "Shadow.fs");
//	ShaderProgram lightShader("LightShader.vs", "LightShader.fs");
//	ShaderProgram textureShader("TextureShader.vs", "TextureShader.fs");
//	ShaderProgram HDRlightingShader("HDRlighting.vs", "HDRlighting.fs");
//	ShaderProgram bloomShader("Bloom.vs", "Bloom.fs");
//	ShaderProgram blurShader("GaussianBlur.vs", "GaussianBlur.fs");
//	ShaderProgram defferedShader("DefferedShading.vs", "DefferedShading.fs");
//
//	bloomShader.Use();
//	bloomShader.SetUniform1i("brightBlur", 0);
//	bloomShader.SetUniform1i("scene", 1);
//
//	defferedShader.SetUniform1i("gPosition", 0);
//	defferedShader.SetUniform1i("gNormal", 1);
//	defferedShader.SetUniform1i("gDiffuseMap", 2);
//
//	HDRlightingShader.Use();
//	HDRlightingShader.SetUniform1f("diffuseTexture", 0);
//
//	blurShader.Use();
//	blurShader.SetUniform1f("image", 0);
//
//	objNum = 1;
//	SceneObject** sceneObjects = new SceneObject*[objNum];
//	for (int i = 0; i < objNum; i++)
//		sceneObjects[i] = new SceneObject();
//
//	sceneObjects[0]->Scale(glm::vec3(0.1f, 0.1f, 0.1f));
//	sceneObjects[0]->Translate(glm::vec3(0.0f, 0.0f, 0.0f));
//	
//	Mesh sphereMesh, cubeMesh, earthMesh;
//	sphereMesh.LoadMesh(SPHERE);
//	cubeMesh.LoadMesh(BOX);
//	earthMesh.LoadMesh(EARTH);
//	// ���ο��� vao id�� ������
//	sceneObjects[0]->GenerateVBO(earthMesh);
//	
//	for (int i = 0; i < lightNum; i++)
//	{
//		lights[i]->GenerateVBO(sphereMesh);
//	}
//
//	// TODO �ϳ��� ��ũ��Ʈ�� ���� ���� object�� �ٿ��� �� �ִµ� �װ� �ȵ�..
//	SceneScript *lightScript, *cameraScript;
//	lightScript = new LightScript;
//	cameraScript = new CameraScript;
//	camera->AttachScript(cameraScript);
//	lights[0]->AttachScript(lightScript);
//
//	camera->ScriptsAwake();
//	cameraInitPos = camera->GetPosition();
//
//	for (int i = 0; i < lightNum; i++)
//		lights[i]->ScriptsAwake();
//	for (int i = 0; i < objNum; i++)
//		sceneObjects[i]->ScriptsAwake();
//
//	// defualt frame buffer, ȭ�鿡 �׷���
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	/*Texture brickwall;
//	brickwallTex = brickwall.GenerateTexture("brickwall.jpg", GL_TEXTURE0);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, brickwallTex);
//
//	Texture brickwallNormal;
//	brickwallNormalTex = brickwallNormal.GenerateTexture("brickwall_normal.jpg", GL_TEXTURE1);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, brickwallNormalTex);*/
//
//	Texture earthDiffuse;
//	earthTex = earthDiffuse.GenerateTexture("Earth/Earth_Diffuse.jpg", GL_TEXTURE0);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, earthTex);
//
//	Texture earthNormal;
//	earthNormalTex = earthNormal.GenerateTexture("Earth/Earth_Normal.jpg", GL_TEXTURE1);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, earthNormalTex);
//
//	// �� ���� �̹����� ����� ���ؼ�
//	// �� ���� framebufferobject�� ����ؼ� ������ colorbuffer�� �׸��� �׸� �� �ִ�
//	// ������ ���� ���� �ϸ� drawcall�� 2�� �ȴ�
//	// �̰��� ���̱� ���ؼ� �ϳ��� framebufferobject�� 2���� color buffer�� ���� �� shader����
//	// layout (location = 0) out ���� �� �� �ִ�, FBO�� ���鶧�� GL_COLOR_ATTACHMENT0~���� �ؾ��Ѵ�
//	// framebufferobject�� bind�� ���¿��� glDrawBuffers�� �ؾ� �Ѵ� �� �׷��� GL_COLOR_ATTACHMENT0���� �׷����� �����̴�
//	// HDR start
//	
//	GLuint gBuffer;
//	glGenFramebuffers(1, &gBuffer);
//	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
//
//	GLuint gPosition, gNormal, gDiffuse;
//
//	glGenTextures(1, &gPosition);
//	glBindTexture(GL_TEXTURE_2D, gPosition);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
//
//	glGenTextures(1, &gNormal);
//	glBindTexture(GL_TEXTURE_2D, gNormal);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
//
//	glGenTextures(1, &gDiffuse);
//	glBindTexture(GL_TEXTURE_2D, gDiffuse);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gDiffuse, 0);
//
//	GLuint  attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
//	glDrawBuffers(3, attachments);
//
//	GLuint rboDepth;
//	glGenRenderbuffers(1, &rboDepth);
//	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
//
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		cout << "framebuffer error" << endl;
//
//	lightPositions.push_back(glm::vec3(0.0f, 0.0f, -10.0f));
//	lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
//	lightPositions.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
//	lightPositions.push_back(glm::vec3(0.8f, -1.7f, 6.0f));
//	
//	lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
//	lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
//	lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
//	lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));
//
//	// SRT�� ������� ���� �����Ѵ�. ���� �Ҷ��� �ݴ����
//	do
//	{
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		// hdrFBO�� 2���� color texture�� �ޱ�� �߾����Ƿ�
//		// colorBuffers[0],[1]�� ���� ��ܹ��ȴ�!
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		Render(camera, lights, sceneObjects, phongShader, lightShader);
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
//void Render(Camera* camera, Light* lights[], SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader)
//{
//	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
//	glm::mat4 view = glm::lookAt
//	(
//		camera->testPosition(),
//		glm::vec3(0.0),
//		glm::vec3(0, 1.0f, 0.0f)
//	);
//
//	// lightnum
//	for (int i = 0; i < 1; i++)
//	{
//		glBindVertexArray(lights[i]->GetVAO());
//		glm::mat4 lightMVP = projection * view * lights[i]->GetModelMatrix();
//		lightShader.Use();
//		lightShader.SetUniformMatrix4f("MVP", lightMVP);
//		switch (i)
//		{
//		case 0:
//			lightShader.SetUniformVector3f("lightBallColor", glm::vec3(1.0f, 0.0f, 0.0f));
//			break;
//		case 1:
//			lightShader.SetUniformVector3f("lightBallColor", glm::vec3(0.0f, 1.0f, 0.0f));
//			break;
//		case 2:
//			lightShader.SetUniformVector3f("lightBallColor", glm::vec3(0.0f, 0.0f, 1.0f));
//			break;
//		case 3:
//			lightShader.SetUniformVector3f("lightBallColor", glm::vec3(1.0f, 1.0f, 1.0f));
//			break;
//		default:
//			break;
//		}
//
//		for (int j = 0; j < layoutNum; j++)
//			glEnableVertexAttribArray(j);
//		glDrawArrays(GL_TRIANGLES, 0, lights[i]->GetMesh().GetVertexNum() * 3);
//		for (int j = 0; j < layoutNum; j++)
//			glDisableVertexAttribArray(j);
//	}
//
//	shader.Use();
//
//	lights[0]->ScriptsUpdate();
//	camera->ScriptsUpdate();
//
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_LEFT))
//		exposure -= 0.02f;
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_RIGHT))
//		exposure += 0.02f;
//
//	bloom = !InputManager::GetInstance()->IsKey(GLFW_KEY_1);
//
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_2))
//	{
//		camera->Rotate(glm::vec3(0.0, 1.0, 0.0), 0.01);
//	}
//
//	// ����� ���̴� ���α׷� �� ������Ʈ ���α׷�
//	for (int i = 0; i < objNum; i++)
//	{
//		sceneObjects[i]->ScriptsUpdate();
//
//		// TODO uniform���� ���� ��Ƶδ� ���� ���� �� ����
//		glm::mat4 mvp = projection * view * sceneObjects[i]->GetModelMatrix();
//		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//		lightView = glm::lookAt(lights[0]->GetPosition(),
//			glm::vec3(0.0f),
//			glm::vec3(0.0f, 1.0f, 0.0f));
//		lightSpaceMatrix = lightProjection * lightView;
//
//		// bind buffer�� ���� ���� �ʴ��� vertex array object�� bind�ϸ� �˾Ƽ� ��
//		glBindVertexArray(sceneObjects[i]->GetVAO());
//
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, earthTex);
//
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, earthNormalTex);
//
//		shader.SetUniformMatrix4f("MVP", mvp);
//		shader.SetUniformVector3f("lightPos", lights[0]->GetPosition());
//		shader.SetUniformVector3f("lightColor", glm::vec3(1.0, 1.0, 1.0));
//		shader.SetUniformVector3f("eyePos", camera->testPosition());
//		shader.SetUniformMatrix4f("model", sceneObjects[i]->GetModelMatrix());
//		shader.SetUniformMatrix4f("view", view);
//		shader.SetUniformMatrix4f("projection", projection);
//		shader.SetUniformMatrix4f("viewProjectionMatrix", projection * view);
//		shader.SetUniformMatrix4f("lightSpaceMatrix", lightSpaceMatrix);
//		shader.SetUniform1i("material.diffuse", 0);
//		shader.SetUniform1f("material.shininess", 32);
//		shader.SetUniform1i("diffuseMap", 0);
//		shader.SetUniform1i("normalMap", 1);
//
//		for (int j = 0; j < lightPositions.size(); j++)
//		{
//			shader.SetUniformVector3f("lights[" + to_string(j) + "].position", lightPositions[j]);
//			shader.SetUniformVector3f("lights[" + to_string(j) + "].color", lightColors[j]);
//		}
//
//		// 5�� layout �� ����
//		// position, color, normal, uv
//		for (int j = 0; j < layoutNum; j++)
//			glEnableVertexAttribArray(j);
//
//		// glDrawElements�� ����Ϸ��� VAO�� �ʿ���, ���� ��� indices�� ����� �־����  �ϰ�
//		// VAO�� ���� ��� indices �ڸ��� NULL�� �־��ִ��� �˾Ƽ� bind �� VAO�� ���� �׷���
//		// glDrawElements(GL_TRIANGLES, 0, GL_FLOAT, indices);
//		glDrawArrays(GL_TRIANGLES, 0, sceneObjects[i]->GetMesh().GetVertexNum() * 3);
//
//		// 5�� layout �� ����
//		for (int j = 0; j < layoutNum; j++)
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
//
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
//// deffered shading
//// deffered shading이란 2개의 pass를 거쳐서 화면을 그리는 것을 말한다
//// 먼저 geometry pass에서 object들의 position, normal, specular등의 정보를 뽑아서 텍스쳐에 넣고
//// 정보들을 모아놓은 texture를 g-buffer라고 한다. MTR(multiple render target)을 이용해서 만든다
//// 이것은 하나의 frame buffer object에서 여러 개의 color texture를 붙여서 구현한다
//// lighting pass에서 그림을 그린다.
//
//// forward shading의 경우 geometry 마다 vs, gs, fs를 거쳐 셰이딩을 한다. 이는 뒤에 있는 오브젝트조차 
//// lighting 연산이 실행될 가능성이 있다는 것이다.
//// 반면 deffered shading의 경우 geometry 마다 vs, gs, fs를 거치지만 lighting 연산은 실행되지 않는다.
//// position, normal, uv등이 texture로 뽑아낸 후 lighting 연산을 실행한다.
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
//		fprintf(stderr, "GLFW 윈도우를 여는데 실패했습니다. Intel GPU 를 사용한다면, 3.3 지원을 하지 않습니다. 2.1 버전용 튜토리얼을 시도하세요.\n");
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
//	// 내부에서 vao id가 정해짐
//	sceneObjects[0]->GenerateVBO(earthMesh);
//	
//	for (int i = 0; i < lightNum; i++)
//	{
//		lights[i]->GenerateVBO(sphereMesh);
//	}
//
//	// TODO 하나의 스크립트가 여러 개의 object에 붙여질 수 있는데 그게 안됨..
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
//	// defualt frame buffer, 화면에 그려짐
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
//	// 두 개의 이미지를 만들기 위해서
//	// 두 개의 framebufferobject를 사용해서 각각의 colorbuffer에 그림을 그릴 수 있다
//	// 하지만 위와 같이 하면 drawcall이 2번 된다
//	// 이것을 줄이기 위해서 하나의 framebufferobject에 2개의 color buffer를 담은 후 shader에서
//	// layout (location = 0) out 으로 할 수 있다, FBO를 만들때는 GL_COLOR_ATTACHMENT0~으로 해야한다
//	// framebufferobject를 bind한 상태에서 glDrawBuffers를 해야 한다 안 그러면 GL_COLOR_ATTACHMENT0에만 그려지기 때문이다
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
//	// SRT의 순서대로 곱이 동작한다. 곱을 할때는 반대로임
//	do
//	{
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		// hdrFBO가 2개의 color texture를 받기로 했었으므로
//		// colorBuffers[0],[1]에 각각 담겨버렸다!
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
//	// 사용할 셰이더 프로그램 및 오브젝트 프로그램
//	for (int i = 0; i < objNum; i++)
//	{
//		sceneObjects[i]->ScriptsUpdate();
//
//		// TODO uniform들을 따로 모아두는 것이 좋을 거 같다
//		glm::mat4 mvp = projection * view * sceneObjects[i]->GetModelMatrix();
//		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//		lightView = glm::lookAt(lights[0]->GetPosition(),
//			glm::vec3(0.0f),
//			glm::vec3(0.0f, 1.0f, 0.0f));
//		lightSpaceMatrix = lightProjection * lightView;
//
//		// bind buffer를 굳이 하지 않더라도 vertex array object만 bind하면 알아서 됨
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
//		// 5는 layout 총 개수
//		// position, color, normal, uv
//		for (int j = 0; j < layoutNum; j++)
//			glEnableVertexAttribArray(j);
//
//		// glDrawElements를 사용하려면 VAO가 필요함, 없을 경우 indices를 제대로 넣어줘야  하고
//		// VAO가 있을 경우 indices 자리에 NULL을 넣어주더라도 알아서 bind 된 VAO에 따라 그려줌
//		// glDrawElements(GL_TRIANGLES, 0, GL_FLOAT, indices);
//		glDrawArrays(GL_TRIANGLES, 0, sceneObjects[i]->GetMesh().GetVertexNum() * 3);
//
//		// 5는 layout 총 개수
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
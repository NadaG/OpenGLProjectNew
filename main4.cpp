//// HDR high dynamic range rendering
//// LDR의 경우 밝기를 0.0에서 1.0밖에 표현하지 못한다
//// 하지만 HDR을 사용하면 더 다양한 밝기를 표현할 수 있다
//// 이는 bloom 효과나 사람의 눈이 암적응하는 효과를 표현할 수 있다
//// 코드로 HDR을 구현해서 HDR을 표현하지 못하는 모니터에 HDR이 적용된 것처럼 할 수 있다
//// HDR을 LDR로 바꾸는 것을 tonemapping이라고 한다
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
//const int layoutNum = 5;
//
//std::vector<glm::vec3> lightPositions;
//std::vector<glm::vec3> lightColors;
//
//bool bloom = true;
//float exposure = 1.0f;
//
//GLuint brickwallTex;
//GLuint brickwallNormalTex;
//
//void Render(Camera* camera, Light* light[], SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader);
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
//	camera->Translate(glm::vec3(0.0f, 0.0f, 15.0f));
//
//	int lightNum = 4;
//	Light* lights[4];
//	lights[0] = new Light;
//	lights[1] = new Light;
//	lights[2] = new Light;
//	lights[3] = new Light;
//	
//	for (int i = 0; i < lightNum; i++)
//		lights[i]->Scale(glm::vec3(0.1f, 0.1f, 0.1f));
//	lights[0]->Translate(glm::vec3(0.0f, 0.0f, 10.0f));
//	lights[1]->Translate(glm::vec3(1.0f, 0.5f, 11.0f));
//	lights[2]->Translate(glm::vec3(2.0f, -1.0f, 6.0f));
//	lights[3]->Translate(glm::vec3(-2.0f, 0.0f, 7.0f));
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
//
//	bloomShader.Use();
//	bloomShader.SetUniform1i("brightBlur", 0);
//	bloomShader.SetUniform1i("scene", 1);
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
//	sceneObjects[0]->Scale(glm::vec3(2.5f, 2.5f, 20.0f));
//	sceneObjects[0]->Translate(glm::vec3(0.0f, 0.0f, 0.0f));
//	
//	Mesh sphereMesh, cubeMesh;
//	sphereMesh.LoadMesh(SPHERE);
//	cubeMesh.LoadMesh(BOX);
//	// 내부에서 vao id가 정해짐
//	sceneObjects[0]->GenerateVBO(cubeMesh);
//	
//	for (int i = 0; i < lightNum; i++)
//		lights[i]->GenerateVBO(sphereMesh);
//
//	// TODO 하나의 스크립트가 여러 개의 object에 붙여질 수 있는데 그게 안됨..
//	SceneScript *lightScript, *cameraScript;
//	lightScript = new LightScript;
//	cameraScript = new CameraScript;
//	camera->AttachScript(cameraScript);
//	lights[0]->AttachScript(lightScript);
//
//	camera->ScriptsAwake();
//	
//	for (int i = 0; i < lightNum; i++)
//		lights[i]->ScriptsAwake();
//	for (int i = 0; i < objNum; i++)
//		sceneObjects[i]->ScriptsAwake();
//
//	// defualt frame buffer, 화면에 그려짐
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	Texture brickwall;
//	brickwallTex = brickwall.GenerateTexture("brickwall.jpg", 0);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, brickwallTex);
//
//	Texture brickwallNormal;
//	brickwallNormalTex = brickwallNormal.GenerateTexture("brickwall_normal.jpg", 1);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, brickwallNormalTex);
//
//	// 두 개의 이미지를 만들기 위해서
//	// 두 개의 framebufferobject를 사용해서 각각의 colorbuffer에 그림을 그릴 수 있다
//	// 하지만 위와 같이 하면 drawcall이 2번 된다
//	// 이것을 줄이기 위해서 하나의 framebufferobject에 2개의 color buffer를 담은 후 shader에서
//	// layout (location = 0) out 으로 할 수 있다, FBO를 만들때는 GL_COLOR_ATTACHMENT0~으로 해야한다
//	// framebufferobject를 bind한 상태에서 glDrawBuffers를 해야 한다 안 그러면 GL_COLOR_ATTACHMENT0에만 그려지기 때문이다
//	// HDR start
//	GLuint hdrFBO;
//	glGenFramebuffers(1, &hdrFBO);
//	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
//	
//	Texture colorBufferTexture;
//	GLuint colorBuffers[2];
//	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
//	for (int i = 0; i < 2; i++)
//	{
//		colorBuffers[i] = colorBufferTexture.GenerateFloatingPointFramebufferTexture();
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
//	}
//
//	Texture renderBufferObjectDepthTexture;
//	GLuint renderBufferObjectDepth = renderBufferObjectDepthTexture.GenerateDepthRenderBufferObject();
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferObjectDepth);
//	glDrawBuffers(2, attachments);
//
//	Texture pingpongBufferTexture;
//	GLuint pingpongFBOs[2];
//	GLuint pingpongBuffers[2];
//	glGenFramebuffers(2, pingpongFBOs);
//	for (int i = 0; i < 2; i++)
//	{
//		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBOs[i]);
//		pingpongBuffers[i] = pingpongBufferTexture.GenerateFloatingPointFramebufferTexture();
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffers[i], 0);
//	}
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
//	bool horizontal = true, firstIteration = true;
//	int iterAmount = 10;
//
//	// SRT의 순서대로 곱이 동작한다. 곱을 할때는 반대로임
//	do
//	{
//		horizontal = true;
//		firstIteration = true;
//		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		// hdrFBO가 2개의 color texture를 받기로 했었으므로
//		// colorBuffers[0],[1]에 각각 담겨버렸다!
//		Render(camera, lights, sceneObjects, HDRlightingShader, lightShader);
//
//		// colorBuffer에 그림을 그리던 FBO는 더 이상 쓰이지 않음
//		// 2개의 framebuffer를 switching 하면서 그리고 있음
//		// 처음에는 brightTexture를 인자로 받아서 첫번째 framebuffer에 붙은 pingpongColorBuffer[0]에 그림을 그리고
//		// 그 다음에는 pingpongColorBuffer[0]을 인자로 받아서 pingpongColorBuffer[1]에 그림을 그림
//		// 다음은 [1]을 인자로 [0]에 그림
//		blurShader.Use();
//		for (int i = 0; i < iterAmount; i++)
//		{
//			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBOs[horizontal]);
//			blurShader.SetUniform1i("horizontal", horizontal);
//			// firstIteration이라면 bright texture에 그림
//			glActiveTexture(GL_TEXTURE0);
//			// 인자로 받는 것
//			glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : pingpongBuffers[!horizontal]);
//			renderQuad();
//			horizontal = !horizontal;
//			if (firstIteration)
//				firstIteration = false;
//		}
//		
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		bloomShader.Use();
//		bloomShader.SetUniform1f("bloom", bloom);
//		bloomShader.SetUniform1f("exposure", exposure);
//
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, pingpongBuffers[!horizontal]);
//		
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
//
//		renderQuad();
//
//		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		//HDRShader.Use();
//		//// glTexImage2D 등을 할 때는 현재 어떤 texture가 active 되어있는지 중요하지 않지만
//		//// 실제로 그림을 그릴 때(셰이더에 보낼 때)는 어떤 texture가 active 되어있는지가 중요하다
//		//// OpenGL은 내부에서 하나의 currentTexture만을 가지고 있다
//		//// glBindTexture(target, object)를 하면 textures[activeTexture]->target = object가 된다
//		//glActiveTexture(GL_TEXTURE0);
//		//glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
//		//HDRShader.SetUniform1i("hdr", hdr);
//		//HDRShader.SetUniform1f("exposure", exposure);
//		//renderQuad();
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
//		camera->GetPosition(),
//		glm::vec3(0, 0, -10.0) + camera->GetPosition(),
//		glm::vec3(0, 1.0f, 0.0f)
//	);
//
//	for (int i = 0; i < 4; i++)
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
//	bloom = !InputManager::GetInstance()->IsKey(GLFW_KEY_SPACE);
//
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
//		glBindTexture(GL_TEXTURE_2D, brickwallTex);
//
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, brickwallNormalTex);
//
//		shader.SetUniformMatrix4f("MVP", mvp);
//		shader.SetUniformVector3f("lightPos", lights[0]->GetPosition());
//		shader.SetUniformVector3f("lightColor", glm::vec3(1.0, 1.0, 1.0));
//		shader.SetUniformVector3f("eyePos", camera->GetPosition());
//		shader.SetUniformMatrix4f("model", sceneObjects[i]->GetModelMatrix());
//		shader.SetUniformMatrix4f("view", view);
//		shader.SetUniformMatrix4f("projection", projection);
//		shader.SetUniformMatrix4f("viewProjectionMatrix", projection * view);
//		shader.SetUniformMatrix4f("lightSpaceMatrix", lightSpaceMatrix);
//		shader.SetUniform1i("material.diffuse", 0);
//		shader.SetUniform1f("material.shininess", 32);
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
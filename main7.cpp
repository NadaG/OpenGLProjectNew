//// SSAO�� ���ؼ� �˾ƺ���
//// Ambient Occlusion�̶� ���� ���� ���� �κ�, ���� �κ� �� ambient light�� ����� ������ �ʴ� �κ��� ��Ӱ� ó���ϴ� ���̴�
//// Screen Space ambient occlusion ������ geometry data�� ������ ambient occlusion�� ����� ���� �ƴ϶�
//// Screen Space�� ���ؼ� �����
//// ���� �ȼ��� ���� ���� �ȼ� �� ���� �ȼ����� depth�� ���� ���� �ִٸ�(������, ���̰� ���� ��, depth map������ ������)
//// �ش� �ȼ��� ������ �ִ� ���̹Ƿ� occlusion factor�� ���� ����
//// ambient ���� �ٲ�� ��, ������ sphere�� sampling�� �ϰ� �� ��� ������ ���� �� �ֱ� ������
//// �ش� pixel�� normal ���� ���� �� normal�� ���ϴ� �ݱ��� sampling�� ��
//// ���� sampling�� �� �� ������ ��� �ȼ��� �ϸ� performance�� �� �������Ƿ�
//// random���� ���� �ȼ��� �̾Ƴ� �� rotate �����ν� performance�� ����
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
//glm::mat4 projection;
//glm::mat4 view;
//
//float near_plane = -10.f;
//float far_plane = 20.0f;
//
//const int layoutNum = 5;
//
//bool bloom = true;
//float exposure = 1.0f;
//
//GLuint earthTex;
//GLuint earthNormalTex;
//
//void Render(Camera* camera, Light* light[], SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader);
//void renderQuad();
//float lerp(float a, float b, float f);
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
//	for (int i = 0; i < lightNum; i++)
//		lights[i]->Scale(glm::vec3(0.1f, 0.1f, 0.1f));
//
//	// ���⼭ PhongShader.vs�� �ܼ��� quad�� ���� ����� texcoord�� �޴� ���̴�
//	// ������ quad�� �׸��� �׸��� ���̴�
//	ShaderProgram phongShader("PhongShader.vs", "PhongShader.fs");
//	// ������ ���� ���ϴ� ���̴�
//	ShaderProgram lightShader("LightShader.vs", "LightShader.fs");
//	// Geometry ������ �޾Ƽ� g-buffer�� ä��� ���̴�
//	ShaderProgram SSAO_GeometryShader("SSAO_Geometry.vs", "SSAO_Geometry.fs");
//	// g-buffer�� �̿��ؼ� occlusion factor ���� ���ϴ� ���̴�
//	ShaderProgram SSAO_Shader("PhongShader.vs", "SSAO.fs");
//	// blur ���̴�
//	ShaderProgram SSAO_BlurShader("PhongShader.vs", "SSAO_Blur.fs");
//
//	phongShader.Use();
//	// uniform texture�� ���̵� ������
//	// texture�� ���ڷ� ����
//	phongShader.SetUniform1i("gPosition", 0);
//	phongShader.SetUniform1i("gNormal", 1);
//	phongShader.SetUniform1i("gAlbedo", 2);
//	// occlusion factor
//	phongShader.SetUniform1i("ssao", 3);
//
//	SSAO_Shader.Use();
//	// uniform texture�� ���̵� ������
//	// texture�� ���ڷ� ����
//	SSAO_Shader.SetUniform1i("gPosition", 0);
//	SSAO_Shader.SetUniform1i("gNormal", 1);
//	SSAO_Shader.SetUniform1i("texNoise", 2);
//
//	SSAO_BlurShader.Use();
//	SSAO_BlurShader.SetUniform1i("ssaoInput", 0);
//
//	objNum = 2;
//	SceneObject** sceneObjects = new SceneObject*[objNum];
//	for (int i = 0; i < objNum; i++)
//		sceneObjects[i] = new SceneObject();
//
//	sceneObjects[0]->Scale(glm::vec3(0.01f, 0.01f, 0.01f));
//	sceneObjects[0]->Translate(glm::vec3(0.0f, 0.0f, 0.0f));
//	
//	Mesh sphereMesh, trexMesh;
//	sphereMesh.LoadMesh(SPHERE);
//	trexMesh.LoadMesh(TREX);
//	// ���ο��� vao id�� ������
//	sceneObjects[0]->GenerateVBO(trexMesh);
//	lights[0]->GenerateVBO(sphereMesh);
//
//	// TODO �ϳ��� ��ũ��Ʈ�� ���� ���� object�� �ٿ��� �� �ִµ� �װ� �ȵ�..
//	SceneScript *lightScript, *cameraScript;
//	lightScript = new LightScript;
//	cameraScript = new CameraScript;
//	camera->AttachScript(cameraScript);
//	lights[0]->AttachScript(lightScript);
//
//
//	camera->ScriptsAwake();
//	for (int i = 0; i < lightNum; i++)
//		lights[i]->ScriptsAwake();
//	for (int i = 0; i < objNum; i++)
//		sceneObjects[i]->ScriptsAwake();
//
//	// defualt frame buffer, ȭ�鿡 �׷���
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	// geometry ������ ��Ƶ� gbuffer
//	GLuint gBuffer;
//	glGenFramebuffers(1, &gBuffer);
//	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
//
//	// ���� position, normal, diffuse ������ ��Ƶ� ����
//	GLuint gPosition, gNormal, gDiffuse;
//	glGenTextures(1, &gPosition);
//	glBindTexture(GL_TEXTURE_2D, gPosition);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gDiffuse, 0);
//
//	GLuint  attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
//	glDrawBuffers(3, attachments);
//
//	// render buffer�� format�� ���� ���������� �ʱ� ������ �ӵ��� ������� ������ �д� ����� �����(implementation-dependent format)
//	// texture�� format�� �������ֱ� ������ �������� �б� ����
//	GLuint rboDepth;
//	glGenRenderbuffers(1, &rboDepth);
//	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
//
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		cout << "framebuffer error" << endl;
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	// ssao �� framebufferobject
//	// ssao blur�� framebufferobject
//	GLuint ssaoFBO, ssaoBlurFBO;
//	glGenFramebuffers(1, &ssaoFBO);
//	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
//
//	// TODO ssao blur�� ���� texture��?
//	GLuint ssaoColorBuffer;
//	glGenTextures(1, &ssaoColorBuffer);
//	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
//
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		cout << "SSAO framebuffer not complete!" << endl;
//
//	glGenFramebuffers(1, &ssaoBlurFBO);
//	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
//	GLuint ssaoColorBufferBlur;
//	glGenTextures(1, &ssaoColorBufferBlur);
//	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
//
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		cout << "SSAO blur framebuffer not complete!" << endl;
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	
//	// random noise texture �����ϱ�
//	// �ش� ���� ������ ���� Ȯ���ϱ� ���� ���� ���� depth�� �˾ƾ��ϴµ�
//	// ��� ���� ����ϸ� ��������� �������� �Ϻ��� ���� Ȯ��
//	uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
//	default_random_engine generator;
//	vector<glm::vec3> ssaoKernel;
//	for (int i = 0; i < 64; i++)
//	{
//		// x, y�� -1���� 1������ �ϰ�
//		// z�� 0���� 1������
//		glm::vec3 sample(randomFloats(generator)*2.0 - 1.0, randomFloats(generator)*2.0 - 1.0, randomFloats(generator));
//		// normalizing�� �����ν� ���� ����
//		sample = glm::normalize(sample);
//		// �Ǵٽ� random ���� ����
//		sample *= randomFloats(generator);
//		float scale = float(i) / 64.0;
//
//		// lerp�� ����ؼ� �ּ��� �Ÿ��� 0.1�� �ΰ�, ������ �����ν� ����� �ʿ� ���� �ΰ� ��
//		scale = lerp(0.1f, 1.0f, scale*scale);
//		sample *= scale;
//		ssaoKernel.push_back(sample);
//	}
//
//	vector<glm::vec3> ssaoNoise;
//	for (int i = 0; i < 4 * 4; i++)
//	{
//		glm::vec3 noise(randomFloats(generator)*2.0 - 1.0, randomFloats(generator)*2.0 - 1.0, 0.0f);
//		ssaoNoise.push_back(noise);
//	}
//
//	GLuint noiseTexture;
//	glGenTextures(1, &noiseTexture);
//	glBindTexture(GL_TEXTURE_2D, noiseTexture);
//	// ���� active �� texture object�� �ٿ�� �� texture�� �����͸� �ִ´�
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	// SRT�� ������� ���� �����Ѵ�. ���� �Ҷ��� �ݴ����
//	do
//	{
//		// 1. geometry pass: render scene's geometry/color data into gbuffer
//		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		Render(camera, lights, sceneObjects, SSAO_GeometryShader, lightShader);
//
//		// 2. generate SSAO texture
//		// noise texture�� ��ǲ���� �޾� occlusion factor ���� �ƿ�ǲ���� �����ν� SSAO �ؽ��� ����
//		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
//		glClear(GL_COLOR_BUFFER_BIT);
//		SSAO_Shader.Use();
//		for (int i = 0; i < 64; i++)
//			SSAO_Shader.SetUniformVector3f("samples[" + to_string(i) + "]", ssaoKernel[i]);
//		SSAO_Shader.SetUniformMatrix4f("projection", projection);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, gPosition);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, gNormal);
//		glActiveTexture(GL_TEXTURE2);
//		glBindTexture(GL_TEXTURE_2D, noiseTexture);
//		renderQuad();
//
//		// 3. blur SSAO texture to remove noise
//		glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
//		glClear(GL_COLOR_BUFFER_BIT);
//		SSAO_BlurShader.Use();
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
//		renderQuad();
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		
//		// 4. lighting pass: traditional deffered Blinn-Phong lighting with added screen-space ambient occlusion
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		phongShader.Use();
//		view = glm::lookAt
//		(
//			camera->testPosition(),
//			glm::vec3(0.0f, camera->testPosition().y, 0.0f),
//			glm::vec3(0, 1.0f, 0.0f)
//		);
//		const float linear = 0.09;
//		const float quadratic = 0.032;
//
//		phongShader.SetUniformVector3f("light.Color", glm::vec3(0.5, 0.5, 0.5));
//		phongShader.SetUniformVector3f("eyePos", view*glm::vec4(camera->GetPosition(), 1.0));
//		phongShader.SetUniformVector3f("light.Position", view*glm::vec4(lights[0]->GetPosition(), 1.0));
//		phongShader.SetUniform1f("light.Linear", linear);
//		phongShader.SetUniform1f("light.Quadratic", quadratic);
//
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, gPosition);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, gNormal);
//		glActiveTexture(GL_TEXTURE2);
//		glBindTexture(GL_TEXTURE_2D, gDiffuse);
//		glActiveTexture(GL_TEXTURE3);
//		glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
//		renderQuad();
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//	// input routine
//	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
//		glfwWindowShouldClose(window) == 0);
//
//	glDeleteProgram(phongShader.GetShaderProgramID());
//	glDeleteProgram(lightShader.GetShaderProgramID());
//
//	glfwTerminate();
//
//
//	return 0;
//}
//
//void Render(Camera* camera, Light* lights[], SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader)
//{
//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
//	view = glm::lookAt
//	(
//		camera->testPosition(),
//		glm::vec3(0.0f, camera->testPosition().y, 0.0f),
//		glm::vec3(0, 1.0f, 0.0f)
//	);
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
//		camera->Rotate(glm::vec3(0.0, 1.0, 0.0), 0.01);
//
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_3))
//		camera->Rotate(glm::vec3(0.0, -1.0, 0.0), 0.01);
//
//	// ����� ���̴� ���α׷� �� ������Ʈ ���α׷�
//	for (int i = 0; i < objNum; i++)
//	{
//		sceneObjects[i]->ScriptsUpdate();
//		shader.SetUniformMatrix4f("model", sceneObjects[i]->GetModelMatrix());
//		shader.SetUniformMatrix4f("view", view);
//		shader.SetUniformMatrix4f("projection", projection);
//		shader.SetUniform1i("invertedNormals", 0);
//		
//		// 5�� layout �� ����
//		// position, color, normal, uv
//		for (int j = 0; j < layoutNum; j++)
//			glEnableVertexAttribArray(j);
//
//		// glDrawElements�� ����Ϸ��� VAO�� �ʿ���, ���� ��� indices�� ����� �־����  �ϰ�
//		// VAO�� ���� ��� indices �ڸ��� NULL�� �־��ִ��� �˾Ƽ� bind �� VAO�� ���� �׷���
//		glBindVertexArray(sceneObjects[i]->GetVAO());
//		glDrawElements(GL_TRIANGLES, sceneObjects[i]->GetMesh().GetIndexNum(), GL_UNSIGNED_INT, 0);
//		glBindVertexArray(0);
//
//		// 5�� layout �� ����
//		for (int j = 0; j < layoutNum; j++)
//			glDisableVertexAttribArray(j);
//	}
//
//	glBindVertexArray(lights[0]->GetVAO());
//	glm::mat4 lightMVP = projection * view * lights[0]->GetModelMatrix();
//	lightShader.Use();
//	lightShader.SetUniformMatrix4f("MVP", lightMVP);
//	lightShader.SetUniformVector3f("lightBallColor", glm::vec3(1.0f, 1.0f, 0.0f));
//
//	for (int j = 0; j < layoutNum; j++)
//		glEnableVertexAttribArray(j);
//
//	glDrawArrays(GL_TRIANGLES, 0, lights[0]->GetMesh().GetVertexNum() * 3);
//	
//	for (int j = 0; j < layoutNum; j++)
//		glDisableVertexAttribArray(j);
//}
//
//unsigned int quadVAO = 0;
//unsigned int quadVBO;
//void renderQuad()
//{
//	if (quadVAO == 0)
//	{
//		float quadVertices[] = 
//		{
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
//
//float lerp(float a, float b, float f)
//{
//	return a + f*(b - a);
//}
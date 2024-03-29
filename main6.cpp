//// SSAO에 대해서 알아보자
//// Ambient Occlusion이라 함은 옷의 접은 부분, 구석 부분 등 ambient light가 제대로 들어오지 않는 부분을 어둡게 처리하는 것이다
//// Screen Space ambient occlusion 실제로 geometry data를 가지고 ambient occlusion을 계산한 것이 아니라
//// Screen Space를 통해서 계산함
//// 주위 픽셀을 봐서 주위 픽셀 중 현재 픽셀보다 depth가 얕은 곳이 있다면(가까운곳, 높이가 높은 곳, depth map에서는 검은색)
//// 해당 픽셀은 구석에 있는 것이므로 occlusion factor가 값이 생겨
//// ambient 값이 바뀌게 됨, 하지만 sphere로 sampling을 하게 될 경우 문제가 생길 수 있기 때문에
//// 해당 pixel의 normal 값을 구한 뒤 normal을 향하는 반구로 sampling을 함
//// 또한 sampling을 할 때 주위의 모든 픽셀을 하면 performance가 안 좋아지므로
//// random으로 일정 픽셀만 뽑아낸 뒤 rotate 함으로써 performance를 높임
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
//	for (int i = 0; i < lightNum; i++)
//		lights[i]->Scale(glm::vec3(0.1f, 0.1f, 0.1f));
//
//	// 여기서 PhongShader.vs는 단순히 quad의 정점 값들과 texcoord만 받는 셰이더
//	// 마지막 quad에 그림을 그리는 셰이더
//	ShaderProgram phongShader("PhongShader.vs", "PhongShader.fs");
//	// 광원의 색을 정하는 셰이더
//	ShaderProgram lightShader("LightShader.vs", "LightShader.fs");
//	// Geometry 정보를 받아서 g-buffer를 채우는 셰이더
//	ShaderProgram SSAO_GeometryShader("SSAO_Geometry.vs", "SSAO_Geometry.fs");
//	// g-buffer를 이용해서 occlusion factor 값을 구하는 셰이더
//	ShaderProgram SSAO_Shader("PhongShader.vs", "SSAO.fs");
//	// blur 셰이더
//	ShaderProgram SSAO_BlurShader("PhongShader.vs", "SSAO_Blur.fs");
//
//	phongShader.Use();
//	// uniform texture의 아이디를 정해줌
//	// texture를 인자로 받음
//	phongShader.SetUniform1i("gPosition", 0);
//	phongShader.SetUniform1i("gNormal", 1);
//	phongShader.SetUniform1i("gAlbedo", 2);
//	// occlusion factor
//	phongShader.SetUniform1i("ssao", 3);
//
//	SSAO_Shader.Use();
//	// uniform texture의 아이디를 정해줌
//	// texture를 인자로 받음
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
//	// 내부에서 vao id가 정해짐
//	sceneObjects[0]->GenerateVBO(trexMesh);
//	lights[0]->GenerateVBO(sphereMesh);
//
//	// TODO 하나의 스크립트가 여러 개의 object에 붙여질 수 있는데 그게 안됨..
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
//	// defualt frame buffer, 화면에 그려짐
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	// geometry 정보를 담아둘 gbuffer
//	GLuint gBuffer;
//	glGenFramebuffers(1, &gBuffer);
//	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
//
//	// 각각 position, normal, diffuse 정보를 담아둘 것임
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
//	// render buffer는 format이 따로 정해져있지 않기 때문에 속도가 빠르기는 하지만 읽는 방식이 어려움(implementation-dependent format)
//	// texture는 format이 정해져있기 때문에 느리지만 읽기 쉬움
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
//	// ssao 용 framebufferobject
//	// ssao blur용 framebufferobject
//	GLuint ssaoFBO, ssaoBlurFBO;
//	glGenFramebuffers(1, &ssaoFBO);
//	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
//
//	// TODO ssao blur에 붙일 texture들?
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
//	// random noise texture 생성하기
//	// 해당 점이 구석인 것을 확인하기 위해 주위 점의 depth를 알아야하는데
//	// 모든 점을 계산하면 어려움으로 랜덤으로 일부의 점만 확인
//	uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
//	default_random_engine generator;
//	vector<glm::vec3> ssaoKernel;
//	for (int i = 0; i < 64; i++)
//	{
//		// x, y는 -1부터 1까지로 하고
//		// z는 0부터 1까지임
//		glm::vec3 sample(randomFloats(generator)*2.0 - 1.0, randomFloats(generator)*2.0 - 1.0, randomFloats(generator));
//		// normalizing을 함으로써 구를 만듦
//		sample = glm::normalize(sample);
//		// 또다시 random 수를 곱함
//		sample *= randomFloats(generator);
//		float scale = float(i) / 64.0;
//
//		// lerp를 사용해서 최소의 거리를 0.1로 두고, 제곱을 함으로써 가까운 쪽에 많이 두게 함
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
//	// 현재 active 된 texture object에 바운드 된 texture에 데이터를 넣는다
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	// SRT의 순서대로 곱이 동작한다. 곱을 할때는 반대로임
//	do
//	{
//		// 1. geometry pass: render scene's geometry/color data into gbuffer
//		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		Render(camera, lights, sceneObjects, SSAO_GeometryShader, lightShader);
//
//		// 2. generate SSAO texture
//		// noise texture를 인풋으로 받아 occlusion factor 값을 아웃풋으로 함으로써 SSAO 텍스쳐 생성
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
//	// 사용할 셰이더 프로그램 및 오브젝트 프로그램
//	for (int i = 0; i < objNum; i++)
//	{
//		sceneObjects[i]->ScriptsUpdate();
//		shader.SetUniformMatrix4f("model", sceneObjects[i]->GetModelMatrix());
//		shader.SetUniformMatrix4f("view", view);
//		shader.SetUniformMatrix4f("projection", projection);
//		shader.SetUniform1i("invertedNormals", 0);
//		
//		// 5는 layout 총 개수
//		// position, color, normal, uv
//		for (int j = 0; j < layoutNum; j++)
//			glEnableVertexAttribArray(j);
//
//		// glDrawElements를 사용하려면 VAO가 필요함, 없을 경우 indices를 제대로 넣어줘야  하고
//		// VAO가 있을 경우 indices 자리에 NULL을 넣어주더라도 알아서 bind 된 VAO에 따라 그려줌
//		glBindVertexArray(sceneObjects[i]->GetVAO());
//		glDrawElements(GL_TRIANGLES, sceneObjects[i]->GetMesh().GetIndexNum(), GL_UNSIGNED_INT, 0);
//		glBindVertexArray(0);
//
//		// 5는 layout 총 개수
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
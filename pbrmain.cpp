//// PBR(physically based rendering)에 대해서 알아보자
//// PBR은 한 점에 대해 모든 방향에서 빛이 들어왔을 때 특정 점으로 얼마 만큼의 빛을 방사하냐는 reflectance 방정식을 푸는 것으로 설명된다.
//// 이 방정식은 에너지 보존 법칙을 근거로 하고 있다.
//// 우변 식은 BRDF(Bidirectional reflectance distribution function)와 irradiance를 모든 steradian에 대해서 적분한 값으로 표현된다.
//// BRDF 는 input ray, output ray를 인자로 가지고 값은 input ray로 빛이 왔을 때 output ray 방향으로 얼마나 빛이 나가느냐를 뜻한다.
//// BRDF의 값이 1일 경우 모든 input ray가 output ray로 간다는 뜻이다.
//
//// specular는 빛의 반사이고 diffuse는 빛의 굴절이다.
//// 빛이 다른 매질로 넘어갈 때 어떤 비율로 반사하고 어떤 비율로 굴절하냐를 나타내는 방정식이 Fresnel Euqation이다.
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
//GLuint albedo, normal, metallic, roughness;
//
//void Render(Camera* camera, Light* light[], SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader);
//void renderQuad();
//void renderSphere();
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
//	ShaderProgram pbrShader("PBR.vs", "PBR.fs");
//	ShaderProgram lightShader("LightShader.vs", "LightShader.fs");
//
//	pbrShader.Use();
//	pbrShader.SetUniform1i("albedoMap", 0);
//	pbrShader.SetUniform1i("normalMap", 1);
//	pbrShader.SetUniform1i("metallicMap", 2);
//	pbrShader.SetUniform1i("roughnessMap", 3);
//
//	Camera* camera = new Camera;
//	int lightNum = 4;
//	Light** lights = new Light*[lightNum];
//	for (int i = 0; i < lightNum; i++)
//		lights[i] = new Light;
//	for (int i = 0; i < lightNum; i++)
//		lights[i]->Scale(glm::vec3(0.1f, 0.1f, 0.1f));
//
//	lights[1]->Translate(glm::vec3(2.0f, 0.0f, 1.0f));
//	lights[2]->Translate(glm::vec3(-4.0f, 2.0f, 0.0f));
//	lights[3]->Translate(glm::vec3(3.0f, -4.0f, 4.0f));
//
//	lights[0]->color = glm::vec3(1.0f, 1.0f, 0.0f);
//	lights[1]->color = glm::vec3(1.0f, 0.0f, 0.0f);
//	lights[2]->color = glm::vec3(0.0f, 1.0f, 0.0f);
//	lights[3]->color = glm::vec3(0.0f, 0.0f, 1.0f);
//
//	objNum = 1;
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
//	for (int i = 0; i < lightNum; i++)
//		lights[i]->ScriptsAwake();
//	for (int i = 0; i < objNum; i++)
//		sceneObjects[i]->ScriptsAwake();
//
//	////////////////////////////////////////////////
//	Texture albedoTex, normalTex, metallicTex, roughnessTex;
//	albedo = albedoTex.GenerateTexture("PBR/albedo.png", 3);
//	normal = normalTex.GenerateTexture("PBR/normal.png", 3);
//	metallic = metallicTex.GenerateTexture("PBR/metallic.png", 1);
//	roughness = roughnessTex.GenerateTexture("PBR/roughness.png", 1);
//	////////////////////////////////////////////////
//	
//
//	glEnable(GL_DEPTH_TEST);
//	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
//	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
//
//	// SRT의 순서대로 곱이 동작한다. 곱을 할때는 반대로임
//	do
//	{
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		Render(camera, lights, sceneObjects, pbrShader, lightShader);
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//	// input routine
//	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
//		glfwWindowShouldClose(window) == 0);
//
//	glDeleteProgram(pbrShader.GetShaderProgramID());
//	glDeleteProgram(lightShader.GetShaderProgramID());
//
//	glfwTerminate();
//
//	return 0;
//}
//
//float moveScale = 0.1f;
//int selectedLight = 0;
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
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_J))
//		camera->Rotate(glm::vec3(0.0, 1.0, 0.0), 0.01);
//
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_L))
//		camera->Rotate(glm::vec3(0.0, -1.0, 0.0), 0.01);
//
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_I))
//		camera->Translate(glm::vec3(0.0f, moveScale, 0.0f));
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_K))
//		camera->Translate(glm::vec3(0.0f, -moveScale, 0.0f));
//
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_U))
//		camera->Translate(glm::vec3(0.0f, 0.0f, moveScale));
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_O))
//		camera->Translate(glm::vec3(0.0f, 0.0f, -moveScale));
//
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_A))
//	{
//		glm::vec4 v = glm::inverse(view)*glm::vec4(-moveScale, 0.0f, 0.0f, 0.0f);
//		lights[selectedLight]->Translate(glm::vec3(v));
//	}
//	
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_D))
//	{
//		glm::vec4 v = glm::inverse(view)*glm::vec4(moveScale, 0.0f, 0.0f, 0.0f);
//		lights[selectedLight]->Translate(glm::vec3(v));
//	}
//
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_W))
//	{
//		glm::vec4 v = glm::inverse(view)*glm::vec4(0.0f, moveScale, 0.0f, 0.0f);
//		lights[selectedLight]->Translate(glm::vec3(v));
//	}
//
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_S))
//	{
//		glm::vec4 v = glm::inverse(view)*glm::vec4(0.0f, -moveScale, 0.0f, 0.0f);
//		lights[selectedLight]->Translate(glm::vec3(v));
//	}
//
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_Q))
//	{
//		glm::vec4 v = glm::inverse(view)*glm::vec4(0.0f, 0.0f, moveScale, 0.0f);
//		lights[selectedLight]->Translate(glm::vec3(v));
//	}
//
//	if (InputManager::GetInstance()->IsKey(GLFW_KEY_E))
//	{
//		glm::vec4 v = glm::inverse(view)*glm::vec4(0.0f, 0.0f, -moveScale, 0.0f);
//		lights[selectedLight]->Translate(glm::vec3(v));
//	}
//
//	for (int i = 0; i < 4; i++)
//	{
//		if (InputManager::GetInstance()->IsKey(GLFW_KEY_1 + i))
//			selectedLight = i;
//	}
//
//	// 사용할 셰이더 프로그램 및 오브젝트 프로그램
//	for (int i = 0; i < objNum; i++)
//	{
//		sceneObjects[i]->ScriptsUpdate();
//		shader.SetUniformMatrix4f("model", sceneObjects[i]->GetModelMatrix());
//		shader.SetUniformMatrix4f("view", view);
//		shader.SetUniformMatrix4f("projection", projection);
//		shader.SetUniformVector3f("eyePos", camera->GetPosition());
//		shader.SetUniformVector3f("albedo", glm::vec3(0.3f, 0.3f, 0.3f));
//		/*shader.SetUniform1f("metallic", 0.04f);
//		shader.SetUniform1f("roughness", 1.0f);*/
//
//		for (int j = 0; j < 4; j++)
//		{
//			shader.SetUniformVector3f("lightPositions[" + std::to_string(j) + "]", lights[j]->GetPosition());
//			shader.SetUniformVector3f("lightColors[" + std::to_string(j) + "]", lights[j]->color);
//		}
//
//		// 5는 layout 총 개수
//		// position, color, normal, uv
//		for (int j = 0; j < layoutNum; j++)
//			glEnableVertexAttribArray(j);
//
//		// glDrawElements를 사용하려면 VAO가 필요함, 없을 경우 indices를 제대로 넣어줘야  하고
//		// VAO가 있을 경우 indices 자리에 NULL을 넣어주더라도 알아서 bind 된 VAO에 따라 그려줌
//		glBindVertexArray(sceneObjects[i]->GetVAO());
//		//glDrawElements(GL_TRIANGLES, sceneObjects[i]->GetMesh().GetIndexNum(), GL_UNSIGNED_INT, 0);
//		glBindVertexArray(0);
//		
//		// 5는 layout 총 개수
//		for (int j = 0; j < layoutNum; j++)
//			glDisableVertexAttribArray(j);
//	}
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, albedo);
//	
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, normal);
//	
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, metallic);
//	
//	glActiveTexture(GL_TEXTURE3);
//	glBindTexture(GL_TEXTURE_2D, roughness);
//	
//	shader.SetUniformMatrix4f("model", glm::mat4());
//	renderSphere();
//
//	for (int i = 0; i < 4; i++)
//	{
//		glBindVertexArray(lights[i]->GetVAO());
//		lightShader.Use();
//		lightShader.SetUniformMatrix4f("model", lights[i]->GetModelMatrix());
//		lightShader.SetUniformMatrix4f("view", view);
//		lightShader.SetUniformMatrix4f("projection", projection);
//		lightShader.SetUniformVector3f("lightBallColor", lights[i]->color);
//		
//		for (int j = 0; j < layoutNum; j++)
//			glEnableVertexAttribArray(j);
//
//		glDrawArrays(GL_TRIANGLES, 0, lights[i]->GetMesh().GetVertexNum() * 3);
//
//		for (int j = 0; j < layoutNum; j++)
//			glDisableVertexAttribArray(j);
//	}
//}
//
//unsigned int sphereVAO = 0;
//unsigned int indexCount;
//void renderSphere()
//{
//	if (sphereVAO == 0)
//	{
//		glGenVertexArrays(1, &sphereVAO);
//
//		unsigned int vbo, ebo;
//		glGenBuffers(1, &vbo);
//		glGenBuffers(1, &ebo);
//
//		std::vector<glm::vec3> positions;
//		std::vector<glm::vec2> uv;
//		std::vector<glm::vec3> normals;
//		std::vector<unsigned int> indices;
//
//		const unsigned int X_SEGMENTS = 64;
//		const unsigned int Y_SEGMENTS = 64;
//		const float PI = 3.14159265359;
//		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
//		{
//			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
//			{
//				float xSegment = (float)x / (float)X_SEGMENTS;
//				float ySegment = (float)y / (float)Y_SEGMENTS;
//				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
//				float yPos = std::cos(ySegment * PI);
//				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
//
//				positions.push_back(glm::vec3(xPos, yPos, zPos));
//				uv.push_back(glm::vec2(xSegment, ySegment));
//				normals.push_back(glm::vec3(xPos, yPos, zPos));
//			}
//		}
//
//		bool oddRow = false;
//		for (int y = 0; y < Y_SEGMENTS; ++y)
//		{
//			if (!oddRow) // even rows: y == 0, y == 2; and so on
//			{
//				for (int x = 0; x <= X_SEGMENTS; ++x)
//				{
//					indices.push_back(y       * (X_SEGMENTS + 1) + x);
//					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
//				}
//			}
//			else
//			{
//				for (int x = X_SEGMENTS; x >= 0; --x)
//				{
//					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
//					indices.push_back(y       * (X_SEGMENTS + 1) + x);
//				}
//			}
//			oddRow = !oddRow;
//		}
//		indexCount = indices.size();
//
//		std::vector<float> data;
//		for (int i = 0; i < positions.size(); ++i)
//		{
//			data.push_back(positions[i].x);
//			data.push_back(positions[i].y);
//			data.push_back(positions[i].z);
//			
//			// color, 어차피 texture에서 뽑아 올거임
//			data.push_back(0.0f);
//			data.push_back(0.0f);
//			data.push_back(0.0f);
//
//			// normal
//			if (normals.size() > 0)
//			{
//				data.push_back(normals[i].x);
//				data.push_back(normals[i].y);
//				data.push_back(normals[i].z);
//			}
//
//			if (uv.size() > 0)
//			{
//				data.push_back(uv[i].x);
//				data.push_back(uv[i].y);
//			}
//
//			// tangent, uv와 position 방정식을 풀어서 만들려면 만들 수 있음
//			data.push_back(0.0f);
//			data.push_back(0.0f);
//			data.push_back(0.0f);
//		}
//		glBindVertexArray(sphereVAO);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo);
//		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
//		float stride = (3 + 3 + 3 + 2 + 3) * sizeof(float);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
//		glEnableVertexAttribArray(2);
//		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
//		glEnableVertexAttribArray(3);
//		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
//		glEnableVertexAttribArray(4);
//		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * sizeof(float)));
//	}
//
//	// 이미 만들어져 있다면 단순히 그리기
//	glBindVertexArray(sphereVAO);
//	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
//	glBindVertexArray(0);
//}

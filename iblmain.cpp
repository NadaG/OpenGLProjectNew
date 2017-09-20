// PBR(physically based rendering)�� ���ؼ� �˾ƺ���
// PBR�� �� ���� ���� ��� ���⿡�� ���� ������ �� Ư�� ������ �� ��ŭ�� ���� ����ϳĴ� reflectance �������� Ǫ�� ������ ����ȴ�.
// �� �������� ������ ���� ��Ģ�� �ٰŷ� �ϰ� �ִ�.
// �캯 ���� BRDF(Bidirectional reflectance distribution function)�� irradiance�� ��� steradian�� ���ؼ� ������ ������ ǥ���ȴ�.
// BRDF �� input ray, output ray�� ���ڷ� ������ ���� input ray�� ���� ���� �� output ray �������� �󸶳� ���� �������ĸ� ���Ѵ�.
// BRDF�� ���� 1�� ��� ��� input ray�� output ray�� ���ٴ� ���̴�.

// specular�� ���� �ݻ��̰� diffuse�� ���� �����̴�.
// ���� �ٸ� ������ �Ѿ �� � ������ �ݻ��ϰ� � ������ �����ϳĸ� ��Ÿ���� �������� Fresnel Euqation�̴�.
#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "FrameBufferObject.h"
#include "CameraScript.h"
#include "Debug.h"
#include "stb_image.h"

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

GLuint albedo, normal, metallic, roughness;
GLuint hdrTexture;

GLuint captureFBO, captureRBO;
GLuint envCubemap;

void Render(Camera* camera, Light* light[], SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader);
void renderSphere();
void renderCube();

unsigned int loadTexture(char const * path);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

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

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwMakeContextCurrent(window);
	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	// �׸��� ������ ������ �޴� �ٴ� ���� �������
	// ���� ���߿� ������ ���� ���� ���� �ִ� �ȼ��� ���� ������ �۰ų� ���� ���
	glDepthFunc(GL_LEQUAL);

	////////////////////////////////////////////////////////////////////////////////

	ShaderProgram pbrShader("PBR.vs", "PBR.fs");
	ShaderProgram lightShader("LightShader.vs", "LightShader.fs");
	ShaderProgram equirectangularToCubemapShader("ibl_cubemap.vs", "equirectangular_to_cubemap.fs");
	ShaderProgram backgroundShader("ibl_background.vs", "ibl_background.fs");

	backgroundShader.Use();
	backgroundShader.SetUniform1i("environmentMap", 0);

	// create framebuffer
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	// load the hdr environment map
	int w, h, nc;
	float* data = stbi_loadf("Apartment/Apt1.hdr", &w, &h, &nc, 0);
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
		cout << "Failed to load HDR image" << endl;

	// set up cube map
	glGenTextures(1, &envCubemap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		// ���� data�� null�� equirectangualr map���� ���� ���� ����
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// set up projection and view matrix
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	};

	// convert hdr equirectangular environment map to cubemap equivalent
	equirectangularToCubemapShader.Use();
	equirectangularToCubemapShader.SetUniform1i("equirectangularMap", 0);
	equirectangularToCubemapShader.SetUniformMatrix4f("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, 512, 512);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader.SetUniformMatrix4f("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);

	pbrShader.Use();
	pbrShader.SetUniform1i("albedoMap", 0);
	pbrShader.SetUniform1i("normalMap", 1);
	pbrShader.SetUniform1i("metallicMap", 2);
	pbrShader.SetUniform1i("roughnessMap", 3);

	Camera* camera = new Camera;
	int lightNum = 4;
	Light** lights = new Light*[lightNum];
	for (int i = 0; i < lightNum; i++)
		lights[i] = new Light;
	for (int i = 0; i < lightNum; i++)
		lights[i]->Scale(glm::vec3(0.1f, 0.1f, 0.1f));

	lights[1]->Translate(glm::vec3(2.0f, 0.0f, 1.0f));
	lights[2]->Translate(glm::vec3(-4.0f, 2.0f, 0.0f));
	lights[3]->Translate(glm::vec3(3.0f, -4.0f, 4.0f));

	lights[0]->color = glm::vec3(1.0f, 1.0f, 0.0f);
	lights[1]->color = glm::vec3(1.0f, 0.0f, 0.0f);
	lights[2]->color = glm::vec3(0.0f, 1.0f, 0.0f);
	lights[3]->color = glm::vec3(0.0f, 0.0f, 1.0f);

	objNum = 1;
	SceneObject** sceneObjects = new SceneObject*[objNum];
	for (int i = 0; i < objNum; i++)
		sceneObjects[i] = new SceneObject();

	sceneObjects[0]->Scale(glm::vec3(0.01f, 0.01f, 0.01f));
	sceneObjects[0]->Translate(glm::vec3(0.0f, 0.0f, 0.0f));

	Mesh sphereMesh, trexMesh;
	sphereMesh.LoadMesh(SPHERE);
	trexMesh.LoadMesh(TREX);
	// ���ο��� vao id�� ������
	sceneObjects[0]->GenerateVBO(trexMesh);
	for (int i = 0; i < lightNum; i++)
		lights[i]->GenerateVBO(sphereMesh);

	// TODO �ϳ��� ��ũ��Ʈ�� ���� ���� object�� �ٿ��� �� �ִµ� �װ� �ȵ�..
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

	////////////////////////////////////////////////
	Texture albedoTex, normalTex, metallicTex, roughnessTex;
	albedo = loadTexture("PBR/albedo.png");
	normal = loadTexture("PBR/normal.png");
	metallic = loadTexture("PBR/metallic.png");
	roughness = loadTexture("PBR/roughness.png");
	////////////////////////////////////////////////

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	// SRT�� ������� ���� �����Ѵ�. ���� �Ҷ��� �ݴ����
	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Render(camera, lights, sceneObjects, pbrShader, lightShader);

		backgroundShader.Use();
		glm::mat4 cubeMapView = glm::rotate(3.141592f, glm::vec3(0.0f, 0.0f, 1.0f));
		backgroundShader.SetUniformMatrix4f("view", view * cubeMapView);
		backgroundShader.SetUniformMatrix4f("projection", projection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		renderCube();

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
int selectedLight = 0;

void Render(Camera* camera, Light* lights[], SceneObject** sceneObjects, ShaderProgram& shader, ShaderProgram& lightShader)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_J))
		camera->Rotate(glm::vec3(0.0, 1.0, 0.0), 0.01);

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_L))
		camera->Rotate(glm::vec3(0.0, -1.0, 0.0), 0.01);

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_I))
		camera->Translate(glm::vec3(0.0f, moveScale, 0.0f));
	if (InputManager::GetInstance()->IsKey(GLFW_KEY_K))
		camera->Translate(glm::vec3(0.0f, -moveScale, 0.0f));

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_U))
		camera->Translate(glm::vec3(0.0f, 0.0f, moveScale));
	if (InputManager::GetInstance()->IsKey(GLFW_KEY_O))
		camera->Translate(glm::vec3(0.0f, 0.0f, -moveScale));

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_A))
	{
		glm::vec4 v = glm::inverse(view)*glm::vec4(-moveScale, 0.0f, 0.0f, 0.0f);
		lights[selectedLight]->Translate(glm::vec3(v));
	}

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_D))
	{
		glm::vec4 v = glm::inverse(view)*glm::vec4(moveScale, 0.0f, 0.0f, 0.0f);
		lights[selectedLight]->Translate(glm::vec3(v));
	}

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_W))
	{
		glm::vec4 v = glm::inverse(view)*glm::vec4(0.0f, moveScale, 0.0f, 0.0f);
		lights[selectedLight]->Translate(glm::vec3(v));
	}

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_S))
	{
		glm::vec4 v = glm::inverse(view)*glm::vec4(0.0f, -moveScale, 0.0f, 0.0f);
		lights[selectedLight]->Translate(glm::vec3(v));
	}

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_Q))
	{
		glm::vec4 v = glm::inverse(view)*glm::vec4(0.0f, 0.0f, moveScale, 0.0f);
		lights[selectedLight]->Translate(glm::vec3(v));
	}

	if (InputManager::GetInstance()->IsKey(GLFW_KEY_E))
	{
		glm::vec4 v = glm::inverse(view)*glm::vec4(0.0f, 0.0f, -moveScale, 0.0f);
		lights[selectedLight]->Translate(glm::vec3(v));
	}

	for (int i = 0; i < 4; i++)
	{
		if (InputManager::GetInstance()->IsKey(GLFW_KEY_1 + i))
			selectedLight = i;
	}

	shader.SetUniformVector3f("eyePos", camera->testPosition());
	// ����� ���̴� ���α׷� �� ������Ʈ ���α׷�
	for (int i = 0; i < objNum; i++)
	{
		sceneObjects[i]->ScriptsUpdate();
		shader.SetUniformMatrix4f("model", sceneObjects[i]->GetModelMatrix());
		shader.SetUniformMatrix4f("view", view);
		shader.SetUniformMatrix4f("projection", projection);

		for (int j = 0; j < 4; j++)
		{
			shader.SetUniformVector3f("lightPositions[" + std::to_string(j) + "]", lights[j]->GetPosition());
			shader.SetUniformVector3f("lightColors[" + std::to_string(j) + "]", lights[j]->color);
		}

		// 5�� layout �� ����
		// position, color, normal, uv
		for (int j = 0; j < layoutNum; j++)
			glEnableVertexAttribArray(j);

		// glDrawElements�� ����Ϸ��� VAO�� �ʿ���, ���� ��� indices�� ����� �־����  �ϰ�
		// VAO�� ���� ��� indices �ڸ��� NULL�� �־��ִ��� �˾Ƽ� bind �� VAO�� ���� �׷���
		glBindVertexArray(sceneObjects[i]->GetVAO());
		glBindVertexArray(0);

		// 5�� layout �� ����
		for (int j = 0; j < layoutNum; j++)
			glDisableVertexAttribArray(j);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedo);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, metallic);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, roughness);

	shader.SetUniformMatrix4f("model", glm::mat4());
	renderSphere();

	for (int i = 0; i < 4; i++)
	{
		glBindVertexArray(lights[i]->GetVAO());
		lightShader.Use();
		lightShader.SetUniformMatrix4f("model", lights[i]->GetModelMatrix());
		lightShader.SetUniformMatrix4f("view", view);
		lightShader.SetUniformMatrix4f("projection", projection);
		lightShader.SetUniformVector3f("lightBallColor", lights[i]->color);

		for (int j = 0; j < layoutNum; j++)
			glEnableVertexAttribArray(j);

		glDrawArrays(GL_TRIANGLES, 0, lights[i]->GetMesh().GetVertexNum() * 3);

		for (int j = 0; j < layoutNum; j++)
			glDisableVertexAttribArray(j);
	}
}

unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();

		std::vector<float> data;
		for (int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);

			// color, ������ texture���� �̾� �ð���
			data.push_back(0.0f);
			data.push_back(0.0f);
			data.push_back(0.0f);

			// normal
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}

			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}

			// tangent, uv�� position �������� Ǯ� ������� ���� �� ����
			data.push_back(0.0f);
			data.push_back(0.0f);
			data.push_back(0.0f);
		}
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 3 + 3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * sizeof(float)));
	}

	// �̹� ������� �ִٸ� �ܼ��� �׸���
	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
		    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		    1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		    1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		    1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};

		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
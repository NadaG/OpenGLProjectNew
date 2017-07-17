#include "PointLightObject.h"
#include "QuadObject.h"
#include "SphereObject.h"

// TODO object uniform seperate
// TODO point light, directional light, spot light

using namespace std;

int width = 1024;
int height = 768;
// w = 1 이면 (x, y, z, 1)이므로 위치이다
// w = 0 이면 (x, y, z, 0)이므로 방향이다

void GenerateDatas(GLubyte* data, cv::Mat image);
GLuint LoadCubeMap(vector<string> images);

// 단 한 화면에 두 삼각형이 동시에 존재하여야한다.
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

	// load image
	cv::Mat dImage = cv::imread("Container_DiffuseMap.png", 1);
	GLubyte* dImageData = new GLubyte[dImage.rows * dImage.cols * 3];
	GenerateDatas(dImageData, dImage);

	cv::Mat sImage = cv::imread("Container_SpecularMap.png", 1);
	GLubyte* sImageData = new GLubyte[sImage.rows * sImage.cols * 3];
	GenerateDatas(sImageData, sImage);
	
	// sky box image load
	vector<string> faces
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"back.jpg",
		"front.jpg"
	};
	
	//GLuint cubeMapID = LoadCubeMap(faces);
	
	// texture 생성
	GLuint diffuseMap;
	glGenTextures(1, &diffuseMap);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dImage.cols, dImage.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, dImageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);

	GLuint specularMap;
	glGenTextures(1, &specularMap);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sImage.cols, sImage.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, sImageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	// texture parameter 설정, 만들고 나서 꼭 설정해줄 것
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	
	ShaderProgram phongShader("PhongShader.vs", "PhongShader.fs");
	ShaderProgram textureShader("TextureShader.vs", "TextureShader.fs");
	ShaderProgram lightShader("LightShader.vs", "LightShader.fs");
	ShaderProgram frameBufferShader("FrameBufferObject.vs", "FrameBufferObject.fs");
	ShaderProgram skyBoxShader("SkyBoxShader.vs", "SkyBoxShader.fs");

	phongShader.AddLayout(LAYOUT_POSITION, 3);
	phongShader.AddLayout(LAYOUT_COLOR, 3);
	phongShader.AddLayout(LAYOUT_NORMAL, 3);

	textureShader.AddLayout(LAYOUT_POSITION, 3);
	textureShader.AddLayout(LAYOUT_NORMAL, 3);
	textureShader.AddLayout(LAYOUT_COLOR, 3);
	textureShader.AddLayout(LAYOUT_UV, 2);

	lightShader.AddLayout(LAYOUT_POSITION, 3);
	lightShader.AddLayout(LAYOUT_COLOR, 3);

	frameBufferShader.AddLayout(LAYOUT_POSITION, 2);
	frameBufferShader.AddLayout(LAYOUT_UV, 2);

	skyBoxShader.AddLayout(LAYOUT_POSITION, 3);

	// generate scene objects
	int objNum = 3; 
	SceneObject* sceneObjects[3];
	// uniform location
	int quadID = 0;
	int sphereID = 1;
	int lightID = 2;

	sceneObjects[quadID] = new QuadObject(textureShader);
	sceneObjects[sphereID] = new SphereObject(phongShader);
	sceneObjects[lightID] = new PointLightObject(lightShader);

	// frame buffer object 사용해보기
	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	
	// texture 생성
	GLuint colorTex;
	glGenTextures(1, &colorTex);
	glBindTexture(GL_TEXTURE_2D, colorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// frame buffer에 color에 대한 texture 연결시키기
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
	
	// render buffer 생성
	GLuint renderBuffer;
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	// frame buffer에 depth와 stencil에 대한 texture 연결시키기
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
	
	// attachment가 모두 붙여진 후
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "frame buffer generate error" << endl;
	}
	// defualt frame buffer, 화면에 그려짐
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// shader 에서 정의하지 않거나 사용하지 않는다면 -1을 리턴한다
	GLint matrixID = glGetUniformLocation(sceneObjects[sphereID]->GetShaderProgramID(), "MVP");
	GLint directionalLightID = glGetUniformLocation(sceneObjects[sphereID]->GetShaderProgramID(), "lightDirection");
	GLint eyePosID = glGetUniformLocation(sceneObjects[sphereID]->GetShaderProgramID(), "eyePos");

	GLint tsMatrixID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "MVP");
	GLint lightPosID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "lightPos");
	GLint lightColorID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "lightColor");
	GLint tsEyePosID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "eyePos");
	GLint materialDiffuseID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "material.diffuse");
	GLint materialSpecualrID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "material.specular");
	GLint materialShininessID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "material.shininess");

	GLint lsMatrixID = glGetUniformLocation(sceneObjects[lightID]->GetShaderProgramID(), "MVP");

	GLint screenTexID = glGetUniformLocation(frameBufferShader.GetShaderProgramID(), "screenTexture");
	
	GLint skyboxID = glGetUniformLocation(skyBoxShader.GetShaderProgramID(), "skybox");
	GLint skyboxViewID = glGetUniformLocation(skyBoxShader.GetShaderProgramID(), "view");
	GLint skyboxProjectionID = glGetUniformLocation(skyBoxShader.GetShaderProgramID(), "projection");

	glUseProgram(skyBoxShader.GetShaderProgramID());
	glUniform1f(skyboxID, 0);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	glm::vec3 eyePos(0.0f, 0.0f, 10.0f);

	Mesh *meshes = new Mesh[objNum];
	meshes[quadID].LoadMesh(QUAD);
	meshes[sphereID].LoadMesh(SPHERE);
	meshes[lightID].LoadMesh(SPHERE);

	for (int i = 0; i < objNum; i++)
		sceneObjects[i]->SetMesh(meshes[i]);
	
	GLfloat** vertexBufferDatas = new GLfloat*[objNum];

	for (int i = 0; i < objNum; i++)
	{
		int floatNum = sceneObjects[i]->GetShaderProgram().CalcGetFloatNum();
		sceneObjects[i]->GetShaderProgram().SetFloatNum(floatNum);

		Mesh mesh = sceneObjects[i]->GetMesh();
		vertexBufferDatas[i] = new GLfloat[mesh.GetVertexNum() * floatNum];
		for (int j = 0; j < mesh.GetVertexNum(); j++)
		{
			int offset = 0;
			if (sceneObjects[i]->GetShaderProgram().IsLayoutExist(LAYOUT_POSITION))
			{
				vertexBufferDatas[i][j * floatNum + offset] = mesh.GetVertice(j).position.x;
				vertexBufferDatas[i][j * floatNum + offset + 1] = mesh.GetVertice(j).position.y;
				vertexBufferDatas[i][j * floatNum + offset + 2] = mesh.GetVertice(j).position.z;
				offset += 3;
			}

			if (sceneObjects[i]->GetShaderProgram().IsLayoutExist(LAYOUT_COLOR))
			{
				vertexBufferDatas[i][j * floatNum + offset] = 1.0f;
				vertexBufferDatas[i][j * floatNum + offset + 1] = 1.0f;
				vertexBufferDatas[i][j * floatNum + offset + 2] = 1.0f;
				offset += 3;
			}

			if (sceneObjects[i]->GetShaderProgram().IsLayoutExist(LAYOUT_NORMAL))
			{
				vertexBufferDatas[i][j * floatNum + offset] = mesh.GetVertice(j).normal.x;
				vertexBufferDatas[i][j * floatNum + offset + 1] = mesh.GetVertice(j).normal.y;
				vertexBufferDatas[i][j * floatNum + offset + 2] = mesh.GetVertice(j).normal.z;
				offset += 3;
			}

			if (sceneObjects[i]->GetShaderProgram().IsLayoutExist(LAYOUT_UV))
			{
				vertexBufferDatas[i][j * floatNum + offset] = mesh.GetVertice(j).uv.x;
				vertexBufferDatas[i][j * floatNum + offset + 1] = mesh.GetVertice(j).uv.y;
				offset += 2;
			}
		}
	}

	// VAO 생성
	GLuint* sceneObjectVAOs = new GLuint[objNum];
	glGenVertexArrays(objNum, sceneObjectVAOs);
	
	// 버텍스들을 담을 버퍼를 만들고(오브젝트당 하나의 buffer를 사용)
	// bufferdata 함수를 통해 버퍼에 데이터를 집어넣음
	// buffer를 하나(position, color, normal, uv)등으로 만들어 glBufferData를 한번만 호출하도록 하여 overhead를 줄일 수 있다.
	GLuint* vertexBuffers = new GLuint[objNum];
	glGenBuffers(objNum, vertexBuffers);
	for (int i = 0; i < objNum; i++)
	{
		glBindVertexArray(sceneObjectVAOs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * meshes[i].GetVertexNum() * sceneObjects[i]->GetShaderProgram().GetFloatNum(), vertexBufferDatas[i], GL_STATIC_DRAW);
	
		ShaderProgram shaderProgram = sceneObjects[i]->GetShaderProgram();
		int offset = 0;
		for (int j = 0; j < 4; j++)
		{
			if (shaderProgram.IsLayoutExist((LayoutType)j))
			{
				glEnableVertexAttribArray(j);
				glVertexAttribPointer(
					j, // layout(location)에 들어갈 숫자이다.
					shaderProgram.GetLayoutSize((LayoutType)j),
					GL_FLOAT,
					GL_FALSE,
					sizeof(GLfloat) * sceneObjects[i]->GetShaderProgram().GetFloatNum(), // stride
					(void*)(sizeof(GLfloat) * offset) // 하나의 vertex 정보 set에서 해당 layout이 얼마나 떨어져 있는지
				);
				offset += shaderProgram.GetLayoutSize((LayoutType)j);
			}
		}
	}

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	// skybox VAO
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//

	// post effect
	int postEffectVertexNum = 6, postEffectFloatNum = 5;
	float sss = 1.0f;
	GLfloat postEffectVertices[] =
	{
		-sss, sss, 0.0f, 1.0f,
		-sss, -sss, 0.0f, 0.0f,
		sss, -sss, 1.0f, 0.0f,

		-sss, sss, 0.0f, 1.0f,
		sss, -sss, 1.0f, 0.0f,
		sss, sss, 1.0f, 1.0f
	};

	GLuint postEffectVAO;
	glGenVertexArrays(1, &postEffectVAO);
	GLuint postEffectVBO;
	glGenBuffers(1, &postEffectVBO);
	glBindVertexArray(postEffectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, postEffectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(postEffectVertices), &postEffectVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
	//

	//glUniform1i(screenTexID, 0);

	double mousePosX = 0.0, mousePosY = 0.0, lastMousePosX = 0.0, lastMousePosY = 0.0;
	float cameraDeltaMove = 0.3f;

	for (int i = 0; i < objNum; i++)
		sceneObjects[i]->Awake();
	// SRT의 순서대로 곱이 동작한다.
	do
	{
		// TODO 매 프레임마다 position 과 normal을 보내줘야 할듯
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glEnable(GL_DEPTH_TEST);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < objNum; i++)
			sceneObjects[i]->Update();

		glfwGetCursorPos(window, &mousePosX, &mousePosY);
		
		if (InputManager::GetInstance()->IsLeftMouseClicked())
		{
			// 오른쪽으로 갔다면
			if (mousePosX - lastMousePosX > 0)
				eyePos.x -= cameraDeltaMove;
			// 왼쪽으로 갔다면
			else if (mousePosX - lastMousePosX < 0)
				eyePos.x += cameraDeltaMove;

			if (mousePosY - lastMousePosY > 0)
				eyePos.y += cameraDeltaMove;
			else if (mousePosY - lastMousePosY < 0)
				eyePos.y -= cameraDeltaMove;
		}

		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;
		
		glm::mat4 view = glm::lookAt
		(
			eyePos,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);
		
		// 사용할 셰이더 프로그램 및 오브젝트 프로그램
		for (int i = 0; i < objNum; i++)
		{
			ShaderProgram shaderProgram = sceneObjects[i]->GetShaderProgram();
			glUseProgram(shaderProgram.GetShaderProgramID());
			

			// TODO uniform들을 따로 모아두는 것이 좋을 거 같다
			glm::mat4 mvp = projection * view * sceneObjects[i]->GetModelMatrix();
			
			glBindVertexArray(sceneObjectVAOs[i]);

			if (i == quadID)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, diffuseMap);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, specularMap);

				glUniformMatrix4fv(tsMatrixID, 1, GL_FALSE, &mvp[0][0]);
				glUniform3f(tsEyePosID, eyePos.x, eyePos.y, eyePos.z);
				glUniform3f(lightPosID, 
					sceneObjects[lightID]->GetPosition().x,
					sceneObjects[lightID]->GetPosition().y,
					sceneObjects[lightID]->GetPosition().z);
				
				glUniform3f(lightColorID, 1.0f, 1.0f, 1.0f);
				glUniform1i(materialDiffuseID, 0);
				glUniform1i(materialSpecualrID, 1);
				glUniform1f(materialShininessID, 32.0);
			}
			else
			{
				glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
				glUniform3f(eyePosID, eyePos.x, eyePos.y, eyePos.z);
				glUniform3f(directionalLightID, 1.0f, 0.0f, -1.0f);
			}
			
			// 4는 layout 총 개수
			for (int j = 0; j < 4; j++)
			{
				if (shaderProgram.IsLayoutExist((LayoutType)j))
					glEnableVertexAttribArray(j);
			}

			glDrawArrays(GL_TRIANGLES, 0, meshes[i].GetVertexNum() * 3);

			// 4는 layout 총 개수
			for (int j = 0; j < 4; j++)
			{
				if (shaderProgram.IsLayoutExist((LayoutType)j))
					glDisableVertexAttribArray(j);
			}
		}

		// skybox load
		/*glDepthFunc(GL_LEQUAL);
		glUseProgram(skyBoxShader.GetShaderProgramID());
		glUniform1f(skyboxID, 0);
		glUniformMatrix4fv(skyboxViewID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(skyboxProjectionID, 1, GL_FALSE, &projection[0][0]);
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);*/
		//

		// post effect
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(frameBufferShader.GetShaderProgramID());
		glBindVertexArray(postEffectVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// input routine
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(objNum, vertexBuffers);
	glDeleteVertexArrays(objNum, sceneObjectVAOs);

	for (int i = 0; i < objNum; i++)
	{
		glDeleteProgram(sceneObjects[i]->GetShaderProgramID());
	}

	glfwTerminate();
	return 0;
}

void GenerateDatas(GLubyte* data, cv::Mat image)
{
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				data[(i * image.cols + j) * 3 + k] = image.at<cv::Vec3b>(j, i)[2 - k];
			}
		}
	}
}

GLuint LoadCubeMap(vector<string> images)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (int i = 0; i < 6; i++)
	{
		cv::Mat image = cv::imread(images[i]);
		GLubyte* data = new GLubyte[image.rows * image.cols * 3];

		for (int j = 0; j < image.rows; j++)
		{
			for (int k = 0; k < image.cols; k++)
			{
				for (int l = 0; l < 3; l++)
				{
					data[j*image.cols * 3 + k * 3 + l] = image.at<cv::Vec3b>(k, j)[2 - l];
				}
			}
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


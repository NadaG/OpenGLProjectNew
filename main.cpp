#include "PointLightObject.h"
#include "QuadObject.h"
#include "SphereObject.h"

// TODO specular light map
// TODO object uniform seperate
// TODO

using namespace std;

int width = 1024;
int height = 768;
// w = 1 이면 (x, y, z, 1)이므로 위치이다
// w = 0 이면 (x, y, z, 0)이므로 방향이다

void GenerateDatas(GLubyte* data, cv::Mat image);

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

	// texture parameter 설정
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image
	cv::Mat dImage = cv::imread("Container_DiffuseMap.png", 1);
	GLubyte* dImageData = new GLubyte[dImage.rows * dImage.cols * 3];
	GenerateDatas(dImageData, dImage);

	// texture 생성
	GLuint diffuseMap;
	glGenTextures(1, &diffuseMap);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	// 뭐하는 함수인가?
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dImage.cols, dImage.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, dImageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);

	cv::Mat sImage = cv::imread("Container_SpecularMap.png", 1);
	GLubyte* sImageData = new GLubyte[sImage.rows * sImage.cols * 3];
	GenerateDatas(sImageData, sImage);

	GLuint specularMap;
	glGenTextures(1, &specularMap);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	
	// VAO 생성
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	int objNum = 3;
	ShaderProgram phongShader("PhongShader.vs", "PhongShader.fs");
	ShaderProgram textureShader("TextureShader.vs", "TextureShader.fs");
	ShaderProgram lightShader("LightShader.vs", "LightShader.fs");

	phongShader.AddLayout(LAYOUT_POSITION, 3);
	phongShader.AddLayout(LAYOUT_COLOR, 3);
	phongShader.AddLayout(LAYOUT_NORMAL, 3);

	textureShader.AddLayout(LAYOUT_POSITION, 3);
	textureShader.AddLayout(LAYOUT_NORMAL, 3);
	textureShader.AddLayout(LAYOUT_COLOR, 3);
	textureShader.AddLayout(LAYOUT_UV, 2);

	lightShader.AddLayout(LAYOUT_POSITION, 3);
	lightShader.AddLayout(LAYOUT_COLOR, 3);

	// generate scene objects
	SceneObject* sceneObjects[3];

	sceneObjects[0] = new QuadObject(textureShader);
	sceneObjects[1] = new SphereObject(phongShader);
	sceneObjects[2] = new PointLightObject(lightShader);

	// uniform location
	int sphereID = 1;
	int quadID = 0;
	int lightID = 2;
	GLuint matrixID = glGetUniformLocation(sceneObjects[sphereID]->GetShaderProgramID(), "MVP");
	GLuint directionalLightID = glGetUniformLocation(sceneObjects[sphereID]->GetShaderProgramID(), "lightDirection");
	GLuint eyePosID = glGetUniformLocation(sceneObjects[sphereID]->GetShaderProgramID(), "eyePos");

	GLuint tsMatrixID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "MVP");
	GLuint lightPosID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "lightPos");
	GLuint lightColorID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "lightColor");
	GLuint tsEyePosID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "eyePos");
	GLuint materialDiffuseID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "material.diffuse");
	GLuint materialSpecualrID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "material.specular");
	GLuint materialShininessID = glGetUniformLocation(sceneObjects[quadID]->GetShaderProgramID(), "material.shininess");

	GLuint lsMatrixID = glGetUniformLocation(sceneObjects[lightID]->GetShaderProgramID(), "MVP");

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	glm::vec3 eyePos(0.0f, 0.0f, 10.0f);

	Mesh *meshes = new Mesh[objNum];
	meshes[0].LoadMesh(QUAD);
	meshes[1].LoadMesh(SPHERE);
	meshes[2].LoadMesh(SPHERE);

	for (int i = 0; i < objNum; i++)
	{
		sceneObjects[i]->SetMesh(meshes[i]);
	}
	
	GLfloat** vertexBufferDatas = new GLfloat*[objNum];
	
	for (int i = 0; i < objNum; i++)
	{
		int floatNum = 0;
		if (sceneObjects[i]->GetShaderProgram().IsLayoutExist(LAYOUT_POSITION))
			floatNum += 3;
		if (sceneObjects[i]->GetShaderProgram().IsLayoutExist(LAYOUT_COLOR))
			floatNum += 3;
		if (sceneObjects[i]->GetShaderProgram().IsLayoutExist(LAYOUT_NORMAL))
			floatNum += 3;
		if (sceneObjects[i]->GetShaderProgram().IsLayoutExist(LAYOUT_UV))
			floatNum += 2;

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

	// 버텍스들을 담을 버퍼를 만들고
	// bufferdata 함수를 통해 버퍼에 데이터를 집어넣음
	// buffer를 하나로 만들어 glBufferData를 한번만 호출하도록 하여 overhead를 줄일 수 있다.
	GLuint* vertexBuffers = new GLuint[objNum];
	glGenBuffers(objNum, vertexBuffers);
	for (int i = 0; i < objNum; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * meshes[i].GetVertexNum() * sceneObjects[i]->GetShaderProgram().GetFloatNum(), vertexBufferDatas[i], GL_STATIC_DRAW);
	}

	double mousePosX = 0.0, mousePosY = 0.0, lastMousePosX = 0.0, lastMousePosY = 0.0;
	float cameraDeltaMove = 0.3f;

	//sceneObject[quadID].Translate(glm::vec3(0.0f, 0.0f, -3.0f));
	sceneObjects[lightID]->Scale(glm::vec3(0.5f, 0.5f, 0.5f));
	
	for (int i = 0; i < objNum; i++)
		sceneObjects[i]->Awake();
	// SRT의 순서대로 곱이 동작한다.
	do
	{
		// TODO 매 프레임마다 position 과 normal을 보내줘야 할듯
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
		// 사용할 셰이더 프로그램 및 오브젝트 프로그램
		for (int i = 0; i < objNum; i++)
		{
			ShaderProgram shaderProgram = sceneObjects[i]->GetShaderProgram();
			glUseProgram(shaderProgram.GetShaderProgramID());
			glm::mat4 view = glm::lookAt
			(
				eyePos,
				glm::vec3(0, 0, 0),
				glm::vec3(0, 1, 0)
			);

			// TODO uniform들을 따로 모아두는 것이 좋을 거 같다
			glm::mat4 mvp = projection * view * sceneObjects[i]->GetModelMatrix();
			
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

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[i]);

			int offset = 0;
			for (int j = 0; j < 4; j++)
			{
				if (shaderProgram.IsLayoutExist((LayoutType)j))
				{
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
			
			glDrawArrays(GL_TRIANGLES, 0, meshes[i].GetVertexNum() * 3);

			// 4는 layout 총 개수
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

	glDeleteBuffers(objNum, vertexBuffers);
	glDeleteVertexArrays(1, &VertexArrayID);

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
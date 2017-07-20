//#include "SceneObject.h"
//#include "Texture.h"
//#include "FrameBufferObject.h"
//
//// TODO object uniform seperate
//// TODO point light, directional light, spot light
//
//using namespace std;
//
//int width = 1024;
//int height = 768;
//// w = 1 이면 (x, y, z, 1)이므로 위치이다
//// w = 0 이면 (x, y, z, 0)이므로 방향이다
//
//void GenerateDatas(GLubyte* data, cv::Mat image);
//GLuint LoadCubeMap(vector<string> images);
//
//// 단 한 화면에 두 삼각형이 동시에 존재하여야한다.
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
//	// sky box image load
//	vector<string> faces
//	{
//		"right.jpg",
//		"left.jpg",
//		"top.jpg",
//		"bottom.jpg",
//		"back.jpg",
//		"front.jpg"
//	};
//	
//	// DEBUG
//	// GLuint cubeMapID = LoadCubeMap(faces);
//	
//	// texture 생성
//	Texture diffuseMapTexture, specularMapTexture;
//	GLuint diffuseMap = diffuseMapTexture.generateTexture("Container_DiffuseMap.png", GL_TEXTURE0);
//	GLuint specularMap = specularMapTexture.generateTexture("Container_SpecularMap.png", GL_TEXTURE1);
//	
//	ShaderProgram phongShader("PhongShader.vs", "PhongShader.fs");
//	ShaderProgram textureShader("TextureShader.vs", "TextureShader.fs");
//	ShaderProgram lightShader("LightShader.vs", "LightShader.fs");
//	ShaderProgram frameBufferShader("FrameBufferObject.vs", "FrameBufferObject.fs");
//	ShaderProgram skyBoxShader("SkyBoxShader.vs", "SkyBoxShader.fs");
//
//	phongShader.AddLayout(LAYOUT_POSITION, 3);
//	phongShader.AddLayout(LAYOUT_COLOR, 3);
//	phongShader.AddLayout(LAYOUT_NORMAL, 3);
//
//	textureShader.AddLayout(LAYOUT_POSITION, 3);
//	textureShader.AddLayout(LAYOUT_NORMAL, 3);
//	textureShader.AddLayout(LAYOUT_COLOR, 3);
//	textureShader.AddLayout(LAYOUT_UV, 2);
//
//	lightShader.AddLayout(LAYOUT_POSITION, 3);
//	lightShader.AddLayout(LAYOUT_COLOR, 3);
//
//	frameBufferShader.AddLayout(LAYOUT_POSITION, 2);
//	frameBufferShader.AddLayout(LAYOUT_UV, 2);
//
//	skyBoxShader.AddLayout(LAYOUT_POSITION, 3);
//
//	// generate scene objects
//	int objNum = 5;
//	SceneObject* sceneObjects[5];
//	// uniform location
//	int quadID = 0;
//	int sphereID = 1;
//	int lightID = 2;
//	int sphereID2 = 3;
//	int sphereID3 = 4;
//	int boxID = 5;
//
//	sceneObjects[quadID] = new SceneObject(textureShader);
//	sceneObjects[sphereID] = new SceneObject(phongShader);
//	sceneObjects[lightID] = new SceneObject(lightShader);
//	sceneObjects[sphereID2] = new SceneObject(phongShader);
//	sceneObjects[sphereID3] = new SceneObject(phongShader);
//	//sceneObjects[boxID] = new CubeObject(skyBoxShader);
//
//	sceneObjects[sphereID]->Translate(glm::vec3(1.0f, 0.0f, 0.0f));
//	sceneObjects[sphereID2]->Translate(glm::vec3(0.0f, 3.0f, 0.0f));
//	sceneObjects[sphereID3]->Translate(glm::vec3(1.0f, 5.0f, 0.0f));
//
//	sceneObjects[lightID]->Scale(glm::vec3(0.1, 0.1, 0.1));
//
//	Mesh sphereMesh, quadMesh;
//	sphereMesh.LoadMesh(SPHERE);
//	quadMesh.LoadMesh(QUAD);
//	sceneObjects[0]->SetMesh(quadMesh);
//	sceneObjects[1]->SetMesh(sphereMesh);
//	sceneObjects[2]->SetMesh(sphereMesh);
//	sceneObjects[3]->SetMesh(sphereMesh);
//	sceneObjects[4]->SetMesh(sphereMesh);
//	
//	SceneScript* lightScript;
//	lightScript = new LightScript;
//
//	sceneObjects[lightID]->AttachScript(lightScript);
//
//	for (int i = 0; i < objNum; i++)
//	{
//		sceneObjects[i]->ScriptsAwake();
//	}
//
//	// frame buffer object 사용해보기
//	FrameBufferObject frameBufferObject(width, height);
//	// color texture 생성
//	GLuint colorTex = frameBufferObject.GenerateColorTexture();
//	// render buffer 생성
//	GLuint renderBuffer = frameBufferObject.GenerateRenderBuffer();
//	// frame buffer id 가져오기
//	GLuint frameBufferID = frameBufferObject.GetID();
//	// attachment가 모두 붙여진 후
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//	{
//		cout << "frame buffer generate error" << endl;
//	}
//	
//	// defualt frame buffer, 화면에 그려짐
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	
//	// shader 에서 정의하지 않거나 사용하지 않는다면 -1을 리턴한다
//	GLint matrixID = glGetUniformLocation(phongShader.GetShaderProgramID(), "MVP");
//	GLint directionalLightID = glGetUniformLocation(phongShader.GetShaderProgramID(), "lightDirection");
//	GLint eyePosID = glGetUniformLocation(phongShader.GetShaderProgramID(), "eyePos");
//
//	GLint tsMatrixID = glGetUniformLocation(textureShader.GetShaderProgramID(), "MVP");
//	GLint lightPosID = glGetUniformLocation(textureShader.GetShaderProgramID(), "lightPos");
//	GLint lightColorID = glGetUniformLocation(textureShader.GetShaderProgramID(), "lightColor");
//	GLint tsEyePosID = glGetUniformLocation(textureShader.GetShaderProgramID(), "eyePos");
//	GLint materialDiffuseID = glGetUniformLocation(textureShader.GetShaderProgramID(), "material.diffuse");
//	GLint materialSpecualrID = glGetUniformLocation(textureShader.GetShaderProgramID(), "material.specular");
//	GLint materialShininessID = glGetUniformLocation(textureShader.GetShaderProgramID(), "material.shininess");
//
//	GLint lsMatrixID = glGetUniformLocation(lightShader.GetShaderProgramID(), "MVP");
//
//	GLint screenTexID = glGetUniformLocation(frameBufferShader.GetShaderProgramID(), "screenTexture");
//	
//	GLint skyboxID = glGetUniformLocation(skyBoxShader.GetShaderProgramID(), "skybox");
//	GLint skyboxViewID = glGetUniformLocation(skyBoxShader.GetShaderProgramID(), "view");
//	GLint skyboxProjectionID = glGetUniformLocation(skyBoxShader.GetShaderProgramID(), "projection");
//
//	glUseProgram(skyBoxShader.GetShaderProgramID());
//	glUniform1f(skyboxID, 0);
//
//	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
//	glm::vec3 eyePos(0.0f, 0.0f, 10.0f);
//
//	GLfloat** vertexBufferDatas = new GLfloat*[objNum];
//
//	for (int i = 0; i < objNum; i++)
//	{
//		sceneObjects[i]->GetShaderProgram().CalcFloatNum();
//		int floatNum = sceneObjects[i]->GetShaderProgram().GetFloatNum();
//
//		Mesh mesh = sceneObjects[i]->GetMesh();
//		vertexBufferDatas[i] = new GLfloat[mesh.GetVertexNum() * floatNum];
//		for (int j = 0; j < mesh.GetVertexNum(); j++)
//		{
//			int offset = 0;
//			if (sceneObjects[i]->GetShaderProgram().IsLayoutExist(LAYOUT_POSITION))
//			{
//				vertexBufferDatas[i][j * floatNum + offset] = mesh.GetVertice(j).position.x;
//				vertexBufferDatas[i][j * floatNum + offset + 1] = mesh.GetVertice(j).position.y;
//				vertexBufferDatas[i][j * floatNum + offset + 2] = mesh.GetVertice(j).position.z;
//				offset += 3;
//			}
//
//			if (sceneObjects[i]->GetShaderProgram().IsLayoutExist(LAYOUT_COLOR))
//			{
//				vertexBufferDatas[i][j * floatNum + offset] = 1.0f;
//				vertexBufferDatas[i][j * floatNum + offset + 1] = 1.0f;
//				vertexBufferDatas[i][j * floatNum + offset + 2] = 1.0f;
//				offset += 3;
//			}
//
//			if (sceneObjects[i]->GetShaderProgram().IsLayoutExist(LAYOUT_NORMAL))
//			{
//				vertexBufferDatas[i][j * floatNum + offset] = mesh.GetVertice(j).normal.x;
//				vertexBufferDatas[i][j * floatNum + offset + 1] = mesh.GetVertice(j).normal.y;
//				vertexBufferDatas[i][j * floatNum + offset + 2] = mesh.GetVertice(j).normal.z;
//				offset += 3;
//			}
//
//			if (sceneObjects[i]->GetShaderProgram().IsLayoutExist(LAYOUT_UV))
//			{
//				vertexBufferDatas[i][j * floatNum + offset] = mesh.GetVertice(j).uv.x;
//				vertexBufferDatas[i][j * floatNum + offset + 1] = mesh.GetVertice(j).uv.y;
//				offset += 2;
//			}
//		}
//	}
//
//	// VAO 생성
//	GLuint* sceneObjectVAOs = new GLuint[objNum];
//	glGenVertexArrays(objNum, sceneObjectVAOs);
//	
//	// 버텍스들을 담을 버퍼를 만들고(오브젝트당 하나의 buffer를 사용)
//	// bufferdata 함수를 통해 버퍼에 데이터를 집어넣음
//	// buffer를 하나(position, color, normal, uv)등으로 만들어 glBufferData를 한번만 호출하도록 하여 overhead를 줄일 수 있다.
//	GLuint* vertexBuffers = new GLuint[objNum];
//	glGenBuffers(objNum, vertexBuffers);
//	for (int i = 0; i < objNum; i++)
//	{
//		// 각각의 vertex array를 사용함
//		glBindVertexArray(sceneObjectVAOs[i]);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[i]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sceneObjects[i]->GetMesh().GetVertexNum() * sceneObjects[i]->GetShaderProgram().GetFloatNum(), vertexBufferDatas[i], GL_STATIC_DRAW);
//	
//		ShaderProgram shaderProgram = sceneObjects[i]->GetShaderProgram();
//		int offset = 0;
//		for (int j = 0; j < 4; j++)
//		{
//			if (shaderProgram.IsLayoutExist((LayoutType)j))
//			{
//				glEnableVertexAttribArray(j);
//				glVertexAttribPointer(
//					j, // layout(location)에 들어갈 숫자이다.
//					shaderProgram.GetLayoutSize((LayoutType)j),
//					GL_FLOAT,
//					GL_FALSE,
//					sizeof(GLfloat) * sceneObjects[i]->GetShaderProgram().GetFloatNum(), // stride
//					(void*)(sizeof(GLfloat) * offset) // 하나의 vertex 정보 set에서 해당 layout이 얼마나 떨어져 있는지
//				);
//				offset += shaderProgram.GetLayoutSize((LayoutType)j);
//			}
//		}
//	}
//
//	// post effect
//	/*int postEffectVertexNum = 6, postEffectFloatNum = 5;
//	float postEffectScale = 1.0f;
//	GLfloat postEffectVertices[] =
//	{
//		-postEffectScale, postEffectScale, 0.0f, 1.0f,
//		-postEffectScale, -postEffectScale, 0.0f, 0.0f,
//		postEffectScale, -postEffectScale, 1.0f, 0.0f,
//
//		-postEffectScale, postEffectScale, 0.0f, 1.0f,
//		postEffectScale, -postEffectScale, 1.0f, 0.0f,
//		postEffectScale, postEffectScale, 1.0f, 1.0f
//	};
//
//	GLuint postEffectVAO;
//	glGenVertexArrays(1, &postEffectVAO);
//	GLuint postEffectVBO;
//	glGenBuffers(1, &postEffectVBO);
//	glBindVertexArray(postEffectVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, postEffectVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(postEffectVertices), &postEffectVertices, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));*/
//	//
//
//	//glUniform1i(screenTexID, 0);
//
//	double mousePosX = 0.0, mousePosY = 0.0, lastMousePosX = 0.0, lastMousePosY = 0.0;
//	float cameraDeltaMove = 0.3f;
//
//	// SRT의 순서대로 곱이 동작한다. 곱을 할 때는 반대로임
//	do
//	{
//		// TODO 매 프레임마다 position 과 normal을 보내줘야 할듯
//		// glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glEnable(GL_DEPTH_TEST);
//
//		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		glfwGetCursorPos(window, &mousePosX, &mousePosY);
//		
//		if (InputManager::GetInstance()->IsLeftMouseClicked())
//		{
//			// 오른쪽으로 갔다면
//			if (mousePosX - lastMousePosX > 0)
//				eyePos.x -= cameraDeltaMove;
//			// 왼쪽으로 갔다면
//			else if (mousePosX - lastMousePosX < 0)
//				eyePos.x += cameraDeltaMove;
//
//			if (mousePosY - lastMousePosY > 0)
//				eyePos.y += cameraDeltaMove;
//			else if (mousePosY - lastMousePosY < 0)
//				eyePos.y -= cameraDeltaMove;
//		}
//
//		lastMousePosX = mousePosX;
//		lastMousePosY = mousePosY;
//		
//		glm::mat4 view = glm::lookAt
//		(
//			eyePos,
//			glm::vec3(0, 0, 0),
//			glm::vec3(0, 1, 0)
//		);
//		
//		// 사용할 셰이더 프로그램 및 오브젝트 프로그램
//		for (int i = 0; i < objNum; i++)
//		{
//			sceneObjects[i]->ScriptsUpdate();
//
//			ShaderProgram shaderProgram = sceneObjects[i]->GetShaderProgram();
//			glUseProgram(shaderProgram.GetShaderProgramID());
//			
//			// TODO uniform들을 따로 모아두는 것이 좋을 거 같다
//			glm::mat4 mvp = projection * view * sceneObjects[i]->GetModelMatrix();
//			
//			glBindVertexArray(sceneObjectVAOs[i]);
//
//			if (i == quadID)
//			{
//				glActiveTexture(GL_TEXTURE0);
//				glBindTexture(GL_TEXTURE_2D, diffuseMap);
//
//				glActiveTexture(GL_TEXTURE1);
//				glBindTexture(GL_TEXTURE_2D, specularMap);
//
//				glUniformMatrix4fv(tsMatrixID, 1, GL_FALSE, &mvp[0][0]);
//				glUniform3f(tsEyePosID, eyePos.x, eyePos.y, eyePos.z);
//				glUniform3f(lightPosID, 
//					sceneObjects[lightID]->GetPosition().x,
//					sceneObjects[lightID]->GetPosition().y,
//					sceneObjects[lightID]->GetPosition().z);
//				
//				glUniform3f(lightColorID, 1.0f, 1.0f, 1.0f);
//				glUniform1i(materialDiffuseID, 0);
//				glUniform1i(materialSpecualrID, 1);
//				glUniform1f(materialShininessID, 32.0);
//			}
//			else if (i == boxID)
//			{
//				/*glDepthFunc(GL_LEQUAL);
//				glUniform1f(skyboxID, 0);
//				glUniformMatrix4fv(skyboxViewID, 1, GL_FALSE, &view[0][0]);
//				glUniformMatrix4fv(skyboxProjectionID, 1, GL_FALSE, &projection[0][0]);
//				glActiveTexture(GL_TEXTURE0);
//				glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);
//				glDepthFunc(GL_LESS);*/
//			}
//			else
//			{
//				glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
//				glUniform3f(eyePosID, eyePos.x, eyePos.y, eyePos.z);
//				glUniform3f(directionalLightID, 1.0f, 0.0f, -1.0f);
//			}
//			
//			// 4는 layout 총 개수
//			for (int j = 0; j < 4; j++)
//			{
//				if (shaderProgram.IsLayoutExist((LayoutType)j))
//					glEnableVertexAttribArray(j);
//			}
//
//			// glDrawElements를 사용하려면 VAO가 필요함, 없을 경우 indices를 제대로 넣어줘야  하고
//			// VAO가 있을 경우 indices 자리에 NULL을 넣어주더라도 알아서 bind 된 VAO에 따라 그려줌
//			// glDrawElements(GL_TRIANGLES, 0, GL_FLOAT, indices);
//			glDrawArrays(GL_TRIANGLES, 0, sceneObjects[i]->GetMesh().GetVertexNum() * 3);
//
//			// 4는 layout 총 개수
//			for (int j = 0; j < 4; j++)
//			{
//				if (shaderProgram.IsLayoutExist((LayoutType)j))
//					glDisableVertexAttribArray(j);
//			}
//		}
//
//		// post effect
//		/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glDisable(GL_DEPTH_TEST);
//
//		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//		
//		glUseProgram(frameBufferShader.GetShaderProgramID());
//		glBindVertexArray(postEffectVAO);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, colorTex);
//		glDrawArrays(GL_TRIANGLES, 0, 6);*/
//		//
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//	// input routine
//	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
//		glfwWindowShouldClose(window) == 0);
//
//	glDeleteBuffers(objNum, vertexBuffers);
//	glDeleteVertexArrays(objNum, sceneObjectVAOs);
//
//	for (int i = 0; i < objNum; i++)
//	{
//		glDeleteProgram(sceneObjects[i]->GetShaderProgramID());
//	}
//
//	glfwTerminate();
//	return 0;
//}
//
//GLuint LoadCubeMap(vector<string> images)
//{
//	GLuint textureID;
//	glGenTextures(1, &textureID);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//
//	for (int i = 0; i < 6; i++)
//	{
//		cv::Mat image = cv::imread(images[i]);
//		GLubyte* data = new GLubyte[image.rows * image.cols * 3];
//
//		for (int j = 0; j < image.rows; j++)
//		{
//			for (int k = 0; k < image.cols; k++)
//			{
//				for (int l = 0; l < 3; l++)
//				{
//					data[j*image.cols * 3 + k * 3 + l] = image.at<cv::Vec3b>(k, j)[2 - l];
//				}
//			}
//		}
//
//		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//	}
//
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//	return textureID;
//}
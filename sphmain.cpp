//#include "Camera.h"
//#include "Light.h"
//#include "Texture.h"
//#include "FrameBufferObject.h"
//#include "CameraScript.h"
//#include "Debug.h"
//
////////// sph
//#include "Particle.h"
//
//using namespace std;
//
//const int width = 800;
//const int height = 800;
//
//void Render(ShaderProgram& shader);
//void Update(Particle* particles, int row, int col);
//void UpdateParticlesPos(Particle* particles, GLfloat* particlesPoses, int row, int col);
//
//GLuint VAO;
//int particleRow = 41;
//int particleCol = 11;
//
//const float threshold = 1.0f;
//const float dist = 0.5f;
//const float restDensity = 4.0f;
//const float surfaceTension = 1.0f;
//const float gasconstant = 45.3f;
//
//const float yPosRelative = 0.0f;
//const float xPosRelative = -7.0f;
//
//float lastTime;
//float deltaTime;
//
//const float frameTime = 0.02f;
//
//glm::vec2 GRAVITY = glm::vec2(0.0, -9.8 * 0.1);
//
//const float border = 10.0f;
//
//std::vector<Particle*>** table;
//
//glm::mat4 mvp;
//
//float PI = 3.1415926535897f;
//const float minDist = 0.00f;
//
//float Poly6SmoothingKernel(float r, float h)
//{
//	if (r <= minDist || r > h)
//		return 0.00001f;
//
//	return (315.0f * (h*h - r*r)*(h*h - r*r)*(h*h - r*r)) / (64 * PI * powf(h, 9));
//}
//
//glm::vec2 Poly6SmoothingKernelGradient(glm::vec2 r, float rDist, float h)
//{
//	if (rDist <= minDist || rDist > h)
//		return glm::vec2(0.0);
//
//	return -945 * (h*h - rDist*rDist)*(h*h - rDist*rDist)*rDist / (32 * powf(h, 9)*PI)*glm::normalize(r);
//}
//
//float Poly6SmoothingKernelLaplacian(float r, float h)
//{
//	if (r <= minDist || r > h)
//		return 0.00001f;
//
//	return -945.0f*(h*h - 5 * r*r)*(h*h - r*r) / (32.0f*PI*powf(h, 9));
//}
//
//glm::vec2 SpikySmoothingKernelGradient(glm::vec2 r, float rDist, float h)
//{
//	if (rDist <= minDist || rDist > h)
//		return glm::vec2(0.00001f);
//
//	return (-45 * (h - rDist)*(h - rDist) / (PI*powf(h, 6))) * glm::normalize(r);
//}
//
//float ViscosityLapacian(float r, float h)
//{
//	if (r <= 0.001f || r > h)
//		return 0.00001f;
//	return 45 * ((h - r) / h) / (PI*pow(h, 6));
//}
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
//	ShaderProgram sphShader("SPH.vs", "SPH.fs");
//	sphShader.Use();
//
//	glm::mat4 model = glm::mat4();
//	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 60.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
//	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
//
//	mvp = projection * view * model;
//	
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	GLfloat* particlesPoses;
//	// (x1,y1), (x2,y2)...
//	particlesPoses = new GLfloat[particleRow * particleCol * 2];
//
//	Particle* particles;
//	particles = new Particle[particleRow * particleCol];
//
//	for (int i = 0; i < particleRow; i++)
//	{
//		for (int j = 0; j < particleCol; j++)
//		{
//			int x = j - particleCol / 2;
//			int y = i - particleRow / 2;
//			int a = i*particleCol + j;
//			particles[a].position.x = x * dist + xPosRelative;
//			particles[a].position.y = y * dist + yPosRelative;
//			particles[a].mass = 1.0f;
//			particles[a].viscosity = 0.4f;
//			particles[a].acceleration = glm::vec2();
//			particles[a].velocity = glm::vec2();
//			particles[a].id = a;
//		}
//	}
//
//	glGenVertexArrays(1, &VAO);
//
//	GLuint particleBuffer;
//	glGenBuffers(1, &particleBuffer);
//
//	glBindVertexArray(VAO);
//
//	UpdateParticlesPos(particles, particlesPoses, particleRow, particleCol);
//	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
//	glBufferData(GL_ARRAY_BUFFER, particleRow * particleCol * sizeof(GLfloat) * 2, particlesPoses, GL_STATIC_DRAW);
//
//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
//	glEnableVertexAttribArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//
//	lastTime = glfwGetTime();
//	
//	// 좌우 상하단 border 각각 2크기의 padding을 둠
//	table = new std::vector<Particle*>*[border * 2 + 2];
//	for (int i = 0; i < border * 2 + 2; i++)
//		table[i] = new std::vector<Particle*>[border * 2 + 2];
//	
//	do
//	{
//		float currentTime = glfwGetTime();
//		if (currentTime - lastTime > frameTime)
//			lastTime = currentTime;
//		else
//			continue;
//
//		Update(particles, particleRow, particleCol);
//		
//		UpdateParticlesPos(particles, particlesPoses, particleRow, particleCol);
//		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
//		// target, offset, size, data
//		// 현재 target에 바인드 된 데이터의 일부를 정제한다, 위 경우는 glBindBuffer를 particleBuffer로 했음 
//		// offset 부터 시작해서 size까지
//		glBufferSubData(GL_ARRAY_BUFFER, 0, particleRow * particleCol * sizeof(GLfloat) * 2, particlesPoses);
//
//		Render(sphShader);
//
//		glfwPollEvents();
//		glfwSwapBuffers(window);
//	}
//	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
//		glfwWindowShouldClose(window) == 0);
//
//	glfwTerminate();
//	return 0;
//}
//
//void Render(ShaderProgram& shader)
//{
//	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	shader.Use();
//	shader.SetUniformMatrix4f("MVP", mvp);
//
//	glBindVertexArray(VAO);
//	glPointSize(3);
//	glDrawArrays(GL_POINTS, 0, particleRow * particleCol);
//}
//
//void Update(Particle* particles, int row, int col)
//{
//	// table 초기화
//	for (int i = 0; i < border * 2 + 2; i++)
//	{
//		for (int j = 0; j < border * 2 + 2; j++)
//		{
//			table[i][j].clear();
//		}
//	}
//
//	// hashing을 통해 table setting
//	for (int i = 0; i < row; i++)
//	{
//		for (int j = 0; j < col; j++)
//		{
//			int a = i*col + j;
//			// border padding을 상하좌우 2로 둠으로써 border 끝에 걸린 것을 처리함
//			int ii = particles[a].position.x + (int)border + 1;
//			int jj = particles[a].position.y + (int)border + 1;
//			table[ii][jj].push_back(&particles[a]);
//		}
//	}
//
//	// density, pressure 및 normal 계산
//	for (int i = 0; i < row; i++)
//	{
//		for (int j = 0; j < col; j++)
//		{
//			int a = i*col + j;
//			particles[a].density = 0.0f;
//			particles[a].normal = glm::vec2(0.0f);
//
//			for (int di = -3; di <= 3; di++)
//			{
//				for (int dj = -3; dj <= 3; dj++)
//				{
//					int ii = particles[a].position.x + (int)border + 1;
//					int jj = particles[a].position.y + (int)border + 1;
//					
//					if (ii + di < 0 || ii + di > border * 2 + 1 || jj + dj < 0 || jj + dj > border * 2 + 1)
//						continue;
//					
//					for (int k = 0; k < table[ii + di][jj + dj].size(); k++)
//					{
//						float abDistance = glm::distance(particles[a].position, table[ii + di][jj + dj][k]->position);
//
//						if (particles[a] == *table[ii + di][jj + dj][k])
//							continue;
//
//						particles[a].density += table[ii + di][jj + dj][k]->mass * Poly6SmoothingKernel(abDistance, threshold);
//						particles[a].normal += table[ii + di][jj + dj][k]->mass *
//							Poly6SmoothingKernelGradient(particles[a].position - table[ii + di][jj + dj][k]->position, abDistance, threshold)
//							/ table[ii + di][jj + dj][k]->density;
//					}
//				}
//			}
//
//			particles[a].pressure = gasconstant * (particles[a].density - restDensity);
//		}
//	}
//
//	// force와 acceleration 계산
//	for (int i = 0; i < row; i++)
//	{
//		for (int j = 0; j < col; j++)
//		{
//			int a = i*col + j;
//			glm::vec2 gravityForce = GRAVITY * particles[a].density;
//			glm::vec2 pressureForce = glm::vec2(0.0f);
//			glm::vec2 viscosityForce = glm::vec2(0.0f);
//			glm::vec2 surfaceForce = glm::vec2(0.0f);
//			float normalDist = glm::length(particles[a].normal);
//			float colorFieldLaplacian = 0.0f;
//
//			for (int di = -3; di <= 3; di++)
//			{
//				for (int dj = -3; dj <= 3; dj++)
//				{
//					int ii = particles[a].position.x + (int)border + 1;
//					int jj = particles[a].position.y + (int)border + 1;
//
//					if (ii + di < 0 || ii + di > border * 2 + 1 || jj + dj < 0 || jj + dj > border * 2 + 1)
//						continue;
//
//					for (int k = 0; k < table[ii + di][jj + dj].size(); k++)
//					{
//						Particle* particleB = table[ii + di][jj + dj][k];
//
//						if (particles[a] == *table[ii + di][jj + dj][k])
//							continue;
//
//						float abDistance = glm::distance(particles[a].position, particleB->position);
//
//						pressureForce -= particleB->mass*((particles[a].pressure + particleB->pressure) / (2 * particleB->density))*
//							SpikySmoothingKernelGradient(particles[a].position - particleB->position, abDistance, threshold);
//
//						viscosityForce += particles[a].viscosity * particleB->mass*
//							(particleB->velocity - particles[a].velocity) / particleB->density*
//							ViscosityLapacian(abDistance, threshold);
//
//						colorFieldLaplacian += particles[a].mass * Poly6SmoothingKernelLaplacian(abDistance, threshold) / particleB->density;
//						if (normalDist > 0.0001f)
//							surfaceForce = -surfaceTension * colorFieldLaplacian * particles[a].normal / normalDist;
//					}
//				}
//			}
//
//			particles[a].acceleration = (pressureForce + gravityForce + viscosityForce + surfaceForce) / particles[a].density;
//		}
//	}
//
//	// velocity와 position 계산
//	for (int i = 0; i < row; i++)
//	{
//		for (int j = 0; j < col; j++)
//		{
//			int a = i*col + j;
//			particles[a].velocity += particles[a].acceleration * frameTime;
//			particles[a].position += particles[a].velocity * frameTime;
//
//			if (particles[a].position.y <= -border)
//			{
//				particles[a].velocity = -particles[a].velocity * 0.5f;
//				particles[a].position.y = -border + 0.01f;
//			}
//
//			if (particles[a].position.x <= -border)
//			{
//				particles[a].velocity = -particles[a].velocity * 0.5f;
//				particles[a].position.x = -border + 0.01f;
//			}
//
//			if (particles[a].position.x >= border)
//			{
//				particles[a].velocity = -particles[a].velocity * 0.5f;
//				particles[a].position.x = border - 0.01f;
//			}
//		}
//	}
//}
//
//void UpdateParticlesPos(Particle* particles, GLfloat* particlesPoses, int row, int col)
//{
//	for (int i = 0; i < row; i++)
//	{
//		for (int j = 0; j < col; j++)
//		{
//			particlesPoses[(i * col + j) * 2] = particles[i * col + j].position.x;
//			particlesPoses[(i * col + j) * 2 + 1] = particles[i * col + j].position.y;
//		}
//	}
//}
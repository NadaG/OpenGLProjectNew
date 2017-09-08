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
//void Update(PBFParticle* particles, int row, int col);
//void UpdateParticlesPos(PBFParticle* particles, GLfloat* particlesPoses, int row, int col);
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
//int iterateTime = 3;
//
//const float yPosRelative = 0.0f;
//const float xPosRelative = -7.0f;
//
//float lastTime;
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
//glm::vec2 SpikySmoothingKernelGradient(glm::vec2 r, float h)
//{
//	float rDist = glm::distance(glm::vec2(), r);
//
//	if (rDist <= minDist || rDist > h)
//		return glm::vec2(0.00001f);
//
//	return (-45 * (h - rDist)*(h - rDist) / (PI*powf(h, 6))) * glm::normalize(r);
//}
//
//float DensityConstraint(float density)
//{
//	return density / restDensity - 1;
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
//	PBFParticle* particles;
//	particles = new PBFParticle[particleRow * particleCol];
//
//	for (int i = 0; i < particleRow; i++)
//	{
//		for (int j = 0; j < particleCol; j++)
//		{
//			int x = j - particleCol / 2;
//			int y = i - particleRow / 2;
//			int a = i*particleCol + j;
//			particles[a].lambda = 0.0f;
//			particles[a].density = 0.0f;
//			particles[a].position = glm::vec2(x*dist + xPosRelative, y*dist + yPosRelative);
//			particles[a].predictPosition = particles[a].position;
//			particles[a].force = glm::vec2();
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
//void Update(PBFParticle* particles, int row, int col)
//{
//	for (int i = 0; i < row * col; i++)
//	{
//		particles[i].velocity += particles[i].force*frameTime;
//		particles[i].predictPosition += particles[i].velocity*frameTime;
//	}
//
//	// find neighboring particles 
//	// for (int i = 0; i < row*col; i++)
//
//	for (int T = 0; T < iterateTime; T++)
//	{
//		for (int i = 0; i < row*col; i++)
//		{
//			float densitySum = 0.0f;
//			float gradientNormSquareSum = 0.0f;
//			for (int j = 0; j < row*col; j++)
//			{
//				gradientNormSquareSum += glm::distance(
//					SpikySmoothingKernelGradient(
//						particles[i].position - particles[j].position, threshold), glm::vec2()) / restDensity;
//
//				densitySum += Poly6SmoothingKernel(glm::distance(particles[j].position, particles[i].position), threshold);
//			}
//
//			particles[i].lambda = -DensityConstraint(particles[i].density) / gradientNormSquareSum;
//		}
//
//		for (int i = 0; i < row*col; i++)
//		{
//			// delta p
//			// collision dection and reponse
//		}
//
//		for (int i = 0; i < row*col; i++)
//			particles[i].predictPosition += particles[i].deltaP;
//	}
//
//	for (int i = 0; i < row*col; i++)
//	{
//		particles[i].velocity = (particles[i].predictPosition - particles[i].position) / frameTime;
//		// vorticity confinement(제한) and viscocity
//		particles[i].position = particles[i].predictPosition;
//	}
//}
//
//void UpdateParticlesPos(PBFParticle* particles, GLfloat* particlesPoses, int row, int col)
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
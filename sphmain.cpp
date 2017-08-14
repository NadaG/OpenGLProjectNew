#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "FrameBufferObject.h"
#include "CameraScript.h"
#include "Debug.h"

//////// sph
#include "Particle.h"

using namespace std;

const int width = 800;
const int height = 800;

void Render(ShaderProgram& shader);
void Update(Particle* particles, int row, int col);
void UpdateParticlesPos(Particle* particles, GLfloat* particlesPoses, int row, int col);

GLuint VAO;
int particleRow = 41;
int particleCol = 21;

float threshold = 0.01f;
float restDensity = 0.3f;

float lastTime;
float deltaTime;

enum KernelType
{
	POLY6, 
	SPIKY_GRADIENT,
	VISCOCITY_CURL
};

float PI = 3.1415926535897f;

//float SmoothingKernel(float rDist, float h, KernelType kernelType)
//{
//	if (rDist >= 0 && rDist <= h)
//	{
//		switch (kernelType)
//		{
//		case POLY6:
//			return 315.0f/(64.0f * PI * h*h*h*h*h*h*h*h*h)*(h*h - rDist*rDist)*(h*h - rDist*rDist)*(h*h - rDist*rDist);
//		case SPIKY_GRADIENT:
//			return -(45 * (h - rDist)*(h - rDist)) / (PI*h*h*h*h*h*h) + 45 * (h - rDist)*(h - rDist)*(-h + 2 * rDist) / (PI*h*h*h*h*h*h*h);
//		case VISCOCITY_CURL:
//			return 45 * (h - rDist) / (PI*h*h*h*h*h*h);
//		}
//	}
//	else
//		return 0;
//}

float Poly6SmoothingKernel(float r, float h)
{
	if (r < 0 || r > h)
		return 0;
	return 315.0f / (64.0f * PI * pow(h, 9)) * pow((h*h - r*r), 3);
}

glm::vec2 SpikySmoothingKernelGradient(glm::vec2 r, float rDist, float h)
{
	if (rDist < 0 || rDist > h)
		return glm::vec2();
	// TODO length를 사용해서 미분을 한 후 r vector에 곱하는 것이 맞나?
	return (-45 * (h - rDist)*(h - rDist) / (PI*pow(h, 6)))*r;
	// return glm::vec2(-45 * (h - r.x)*(h - r.x) / (PI*pow(h, 6)), -45 * (h - r.y)*(h - r.y) / (PI*pow(h, 6)));
}

float ViscosityLapacian(float r, float h)
{
	if (r < 0 || r > h)
		return 0;
	return 45 * (h - r) / (PI*pow(h, 6));
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

	ShaderProgram sphShader("SPH.vs", "SPH.fs");
	sphShader.Use();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLfloat* particlesPoses;
	// (x1,y1), (x2,y2)...
	particlesPoses = new GLfloat[particleRow * particleCol * 2];

	Particle* particles;
	particles = new Particle[particleRow * particleCol];
	float dist = 0.01f;

	for (int i = 0; i < particleRow; i++)
	{
		for (int j = 0; j < particleCol; j++)
		{
			int x = j - particleCol / 2;
			int y = i - particleRow / 2;
			particles[i * particleCol + j].position.x = x * dist;
			particles[i * particleCol + j].position.y = y * dist - 0.5f;
			particles[i * particleCol + j].density = 1.0f;
			particles[i * particleCol + j].mass = 0.01f;
			particles[i * particleCol + j].viscosity = 0.01f;
			particles[i * particleCol + j].acceleration = glm::vec2();
			particles[i * particleCol + j].velocity = glm::vec2();
			particles[i * particleCol + j].pressure = 0.01f;
			particles[i * particleCol + j].gasconstant = 45.0f;
		}
	}

	glGenVertexArrays(1, &VAO);

	GLuint particleBuffer;
	glGenBuffers(1, &particleBuffer);

	glBindVertexArray(VAO);

	UpdateParticlesPos(particles, particlesPoses, particleRow, particleCol);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	
	// drawArrays를 해서 단순히 vertex의 array를 주는 경우에는 GL_ARRAY_BUFFER를 쓰고
	// drawElements를 하려고 indices를 주는 경우에는 GL_ELEMENT_ARRAY_BUFFER를 쓴다
	glBufferData(GL_ARRAY_BUFFER, particleRow * particleCol * sizeof(GLfloat) * 2, particlesPoses, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	lastTime = glfwGetTime();
	deltaTime = 0.0f;

	do
	{
		Update(particles, particleRow, particleCol);
		
		UpdateParticlesPos(particles, particlesPoses, particleRow, particleCol);
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
		// target, offset, size, data
		// 현재 target에 바인드 된 데이터의 일부를 정제한다, 위 경우는 glBindBuffer를 particleBuffer로 했음 
		// offset 부터 시작해서 size까지
		glBufferSubData(GL_ARRAY_BUFFER, 0, particleRow * particleCol * sizeof(GLfloat) * 2, particlesPoses);
		
		Render(sphShader);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glfwTerminate();
	return 0;
}

void Render(ShaderProgram& shader)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.Use();

	glBindVertexArray(VAO);
	glPointSize(5);
	glDrawArrays(GL_POINTS, 0, particleRow * particleCol);
}

glm::vec2 GRAVITY = glm::vec2(0.0, -0.01);

void Update(Particle* particles, int row, int col)
{
	// density 및 pressure 계산
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			for (int ii = 0; ii < row; ii++)
			{
				for (int jj = 0; jj < col; jj++)
				{
					int a = i*col + j, b = ii*col + jj;
					float abDistance = glm::distance(particles[a].position, particles[b].position);
					
					if (a == b)
						continue;

					particles[a].density += particles[b].mass*Poly6SmoothingKernel(
						abDistance, 
						threshold);
					particles[a].pressure = particles[a].gasconstant*(particles[a].density - restDensity);
				}
			}
		}
	}

	float currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			int a = i*col + j;
			glm::vec2 gravityForce = particles[a].density * GRAVITY;
			glm::vec2 pressureForce = glm::vec2();
			glm::vec2 viscosityForce = glm::vec2();

			for (int ii = 0; ii < row; ii++)
			{
				for (int jj = 0; jj < col; jj++)
				{
					int b = ii*col + jj;
					if (a == b)
						continue;
					float abDistance = glm::distance(particles[a].position, particles[b].position);

					pressureForce -= particles[b].mass*
						((particles[a].pressure + particles[b].pressure) /
						(2 * particles[b].density))*
						SpikySmoothingKernelGradient(particles[i].position - particles[j].position, abDistance, threshold);
					
					viscosityForce += particles[a].viscosity * particles[b].mass*
						(particles[b].velocity - particles[a].velocity) / particles[b].density*
						ViscosityLapacian(abDistance, threshold);
				}
			}

			particles[a].acceleration = (pressureForce + gravityForce + viscosityForce) / particles[a].density;
			particles[a].velocity += particles[a].acceleration * deltaTime;
			particles[a].position += particles[a].velocity * deltaTime;
		
			if (particles[a].position.y < -1.0f)
			{
				particles[a].position.y = -1.0f;
				particles[a].velocity.y = -particles[a].velocity.y;
				particles[a].velocity *= 0.1f;
			}

			if (particles[a].position.y > 1.0f)
			{
				particles[a].velocity.y = -particles[a].velocity.y;
				particles[a].velocity *= 0.1f;
			}

			if (particles[a].position.x < -1.0f)
			{
				particles[a].position.x = -1.0f;
				particles[a].velocity.x = -particles[a].velocity.x;
				particles[a].velocity *= 0.1f;
			}

			if (particles[a].position.x > 1.0f)
			{
				particles[a].position.x = 1.0f;
				particles[a].velocity.x = -particles[a].velocity.x;
				particles[a].velocity *= 0.1f;
			}
		}
	}
}

void UpdateParticlesPos(Particle* particles, GLfloat* particlesPoses, int row, int col)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			particlesPoses[(i * col + j) * 2] = particles[i * col + j].position.x;
			particlesPoses[(i * col + j) * 2 + 1] = particles[i * col + j].position.y;
		}
	}
}
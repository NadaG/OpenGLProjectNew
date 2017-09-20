//#include "Camera.h"
//#include "Particle.h"
//
//using namespace std;
//
//const int width = 800;
//const int height = 800;
//
//void Render(ShaderProgram& shader);
//void Update(PBFParticle* particles, int row, int col, int dep, int oRow, int oCol, int oDep);
//void UpdateParticlesPos(PBFParticle* particles, GLfloat* particlesPoses, int row, int col, int dep, int oRow, int oCol, int oDep);
//
//GLuint VAO;
//const int particleRow = 12;
//const int particleCol = 12;
//const int particleDep = 3;
//
//const int obsRow = 5;
//const int obsCol = 5;
//const int obsDep = 5;
//
//const float threshold = 1.0f;
//const float dist = 0.5f;
//const float obsDist = 0.5f;
//const float restDensity = 10.0f;
//const float epsilon = 0.01f;
//
//int iterateTime = 3;
//
//const float xPosRelative = 0.0f;
//const float yPosRelative = 3.0f;
//const float zPosRelative = 0.0f;
//
//const float xObsPosRelative = 0.0f;
//const float yObsPosRelative = -1.0f;
//const float zObsPosRelative = 0.0f;
//
//float lastTime;
//const float frameTime = 0.02f;
//
//glm::vec3 GRAVITY = glm::vec3(0.0, -9.8 * 1.0, 0.0f);
//
//const float borderWidth = 8.0f;
//const float borderHeight = 8.0f;
//const float borderDepth = 8.0f;
//
//const float borderEpsilon = 0.01f;
//
//const float corrConstant = 0.01f;
//const int corrN = 2;
//const float corrThreshold = 0.08f;
//
//const float viscosityConstant = 0.05f;
//const float surfaceTensionConstant = 0.01f;
//
//std::vector<PBFParticle*>*** table;
//
//glm::mat4 mvp;
//
//float PI = 3.1415926535897f;
//const float minDist = 0.00f;
//
//const int neighborPadding = 2;
//
//// cuda + shader
//// volticity
//// marching cube
//// two way coupling
//float Poly6SmoothingKernel(float r, float h)
//{
//	if (r <= minDist || r > h)
//		return 0.00001f;
//
//	return (315.0f * (h*h - r*r)*(h*h - r*r)*(h*h - r*r)) / (64 * PI * powf(h, 9));
//}
//
//glm::vec3 SpikySmoothingKernelGradient(glm::vec3 r, float h)
//{
//	float rDist = glm::distance(glm::vec3(), r);
//
//	if (rDist <= minDist || rDist > h)
//		return glm::vec3(0.00001f);
//
//	return (-45 * (h - rDist)*(h - rDist) / (PI*powf(h, 6))) * glm::normalize(r);
//}
//
//glm::vec3 SpikySmoothingKernelGradient(glm::vec3 r, float rDist, float h)
//{
//	if (rDist <= minDist || rDist > h)
//		return glm::vec3(0.00001f);
//
//	return (-45 * (h - rDist)*(h - rDist) / (PI*powf(h, 6))) * glm::normalize(r);
//}
//
//glm::vec3 ComputeCohesion(glm::vec3 r, float rDist, float h)
//{
//	if (rDist == 0.0f)
//		return glm::vec3(0.0f);
//
//	else
//	{
//		if (rDist > 0.5f * h)
//			return (10.18592f / pow(h, 9) * pow(h - rDist, 3) * pow(rDist, 3)) / rDist * r;
//
//		else
//			return (20.37184f / pow(h, 9) * pow(h - rDist, 3) * pow(rDist, 3) - (pow(h, 6) / 64.0f)) / rDist * r;
//	}
//}
//
//glm::ivec3 FindTableIndex(glm::vec3 position)
//{
//	return glm::ivec3(position.x + borderWidth / 2 + neighborPadding, 
//				      position.y + borderHeight / 2 + neighborPadding,
//				      position.z + borderDepth / 2 + neighborPadding);
//}
//
//vector<PBFParticle*> FindNeighbors(PBFParticle particle, int neighborPadding)
//{
//	vector<PBFParticle*> neighbors;
//
//	glm::ivec3 index = FindTableIndex(particle.position);
//	for (int i = -neighborPadding; i <= neighborPadding; i++)
//	{
//		for (int j = -neighborPadding; j <= neighborPadding; j++)
//		{
//			for (int k = -neighborPadding; k <= neighborPadding; k++)
//			{
//				for (int ii = 0; ii < table[index.x + i][index.y + j][index.z + k].size(); ii++)
//				{
//					neighbors.push_back(table[index.x + i][index.y + j][index.z + k][ii]);
//				}
//			}
//		}
//	}
//
//	return neighbors;
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
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//
//	ShaderProgram sphShader("SPH.vs", "SPH.fs");
//	sphShader.Use();
//
//	glm::mat4 model = glm::mat4();
//	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 30.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
//	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
//
//	mvp = projection * view * model;
//	
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	GLfloat* particlesPoses;
//	// (x1,y1), (x2,y2)...
//	particlesPoses = new GLfloat[particleRow * particleCol * particleDep * 3 + obsRow * obsCol * obsDep * 3];
//
//	PBFParticle* particles;
//	particles = new PBFParticle[particleRow * particleCol * particleDep + obsRow * obsCol * obsDep];
//
//	for (int i = 0; i < particleRow; i++)
//	{
//		for (int j = 0; j < particleCol; j++)
//		{
//			for (int k = 0; k < particleDep; k++)
//			{
//				int y = i - particleRow / 2;
//				int x = j - particleCol / 2;
//				int z = k - particleDep / 2;
//				int a = i*particleCol*particleDep + j*particleDep + k;
//				particles[a].lambda = 0.0f;
//				particles[a].density = 0.0f;
//				particles[a].position = glm::vec3(x*dist + xPosRelative, y*dist + yPosRelative, z*dist + zPosRelative);
//				particles[a].predictPosition = particles[a].position;
//				particles[a].force = glm::vec3();
//				particles[a].velocity = glm::vec3();
//				particles[a].id = a;
//				particles[a].isObs = false;
//			}
//		}
//	}
//
//	for (int i = 0; i < obsRow; i++)
//	{
//		for (int j = 0; j < obsCol; j++)
//		{
//			for (int k = 0; k < obsDep; k++)
//			{
//				int y = i - obsRow / 2;
//				int x = j - obsCol / 2;
//				int z = k - obsDep / 2;
//				int a = i*obsCol*obsDep + j*obsDep + k + particleRow * particleCol * particleDep;
//				particles[a].lambda = 0.0f;
//				particles[a].density = 0.0f;
//				particles[a].position = glm::vec3(x*obsDist + xObsPosRelative, y*obsDist + yObsPosRelative, z*obsDist + zObsPosRelative);
//				particles[a].predictPosition = particles[a].position;
//				particles[a].force = glm::vec3();
//				particles[a].velocity = glm::vec3();
//				particles[a].id = a;
//				particles[a].isObs = true;
//			}
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
//	UpdateParticlesPos(particles, particlesPoses, particleRow, particleCol, particleDep, obsRow , obsCol, obsDep);
//	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
//	glBufferData(GL_ARRAY_BUFFER, (particleRow * particleCol * particleDep + obsRow * obsCol * obsDep) * sizeof(GLfloat) * 3, particlesPoses, GL_STATIC_DRAW);
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
//	glEnableVertexAttribArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//
//	lastTime = glfwGetTime();
//	
//	// 좌, 하단 border 각각 1 크기의 padding을 둠, 상, 우단 border에는 2 크기의 padding을 둠
//	table = new std::vector<PBFParticle*>**[(int)borderWidth * 2 + neighborPadding * 2 + 1];
//	for (int i = 0; i < (int)borderWidth * 2 + neighborPadding * 2 + 1; i++)
//	{
//		table[i] = new std::vector<PBFParticle*>*[(int)borderHeight * 2 + neighborPadding * 2 + 1];
//		for (int j = 0; j < (int)borderHeight * 2 + neighborPadding * 2 + 1; j++)
//			table[i][j] = new std::vector<PBFParticle*>[(int)borderDepth * 2 + neighborPadding * 2 + 1];
//	}
//
//	do
//	{
//		float currentTime = glfwGetTime();
//		if (currentTime - lastTime > frameTime)
//			lastTime = currentTime;
//		else
//			continue;
//
//		Update(particles, particleRow, particleCol, particleDep, obsRow, obsCol, obsDep);
//		
//		UpdateParticlesPos(particles, particlesPoses, particleRow, particleCol, particleDep, obsRow, obsCol, obsDep);
//		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, (particleRow * particleCol * particleDep + obsRow * obsCol * obsDep) * sizeof(GLfloat) * 3, particlesPoses);
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
//	glPointSize(4);
//	glDrawArrays(GL_POINTS, 0, particleRow * particleCol * particleDep + obsRow * obsCol * obsDep);
//}
//
//const int iterTime = 3;
//void Update(PBFParticle* particles, int row, int col, int dep, int oRow, int oCol, int oDep)
//{
//	int particlesNum = row*col*dep + oRow * oCol * oDep;
//
//	for (int i = 0; i < (int)borderWidth * 2 + neighborPadding * 2 + 1; i++)
//	{
//		for (int j = 0; j < (int)borderHeight * 2 + neighborPadding * 2 + 1; j++)
//		{
//			for (int k = 0; k < (int)borderDepth * 2 + neighborPadding * 2 + 1; k++)
//			{
//				table[i][j][k].clear();
//			}
//		}
//	}
//
//	// external force
//	for (int i = 0; i < particlesNum; i++)
//	{
//		if (!particles[i].isObs)
//		{
//			particles[i].force += GRAVITY;
//			particles[i].velocity += particles[i].force*frameTime;
//			particles[i].predictPosition += particles[i].velocity*frameTime;
//		}
//		glm::ivec3 index = FindTableIndex(particles[i].position);
//		table[index.x][index.y][index.z].push_back(&particles[i]);
//	}
//
//	// density constraint
//	for (int T = 0; T < iterTime; T++)
//	{
//		for (int i = 0; i < particlesNum; i++)
//		{
//
//			float density = 0.0f;
//			float sum_grad_c = 0.0f;
//			glm::vec3 grad_c = glm::vec3(0.0f);
//			glm::vec3 iPos = particles[i].predictPosition;
//
//			vector<PBFParticle*> neighbors = FindNeighbors(particles[i], neighborPadding);
//
//			for (int j = 0; j < neighbors.size(); j++)
//			{
//				glm::vec3 jPos = neighbors[j]->predictPosition;
//				glm::vec3 relPos = iPos - jPos;
//
//				float dist = glm::distance(glm::vec3(), relPos);
//
//				if (dist < threshold)
//				{
//					float poly6 = Poly6SmoothingKernel(dist, threshold);
//					glm::vec3 spiky = SpikySmoothingKernelGradient(relPos, threshold);
//
//					if (particles[i].isObs)
//						density += poly6 * 1.3f;
//					else
//						density += poly6;
//
//					glm::vec3 grad_ci = spiky / restDensity;
//					sum_grad_c += glm::dot(grad_ci, grad_ci);
//					grad_c += grad_ci;
//				}
//			}
//
//			particles[i].density = density;
//			sum_grad_c += glm::dot(grad_c, grad_c);
//
//			float densityContraint = DensityConstraint(density);
//
//			if (densityContraint < 0)
//				densityContraint = 0.0f;
//
//			particles[i].lambda = -densityContraint / (sum_grad_c + epsilon);
//		}
//
//		for (int i = 0; i < particlesNum; i++)
//		{
//			if (particles[i].isObs)
//				continue;
//
//			particles[i].deltaP = glm::vec3(0.0f);
//
//			vector<PBFParticle*> neighbors = FindNeighbors(particles[i], neighborPadding);
//			for (int j = 0; j < neighbors.size(); j++)
//			{
//				if (&particles[i] == neighbors[j])
//					continue;
//
//				float dist = glm::distance(glm::vec3(0.0f), particles[i].predictPosition - neighbors[j]->predictPosition);
//
//				if (dist < threshold)
//				{
//					float corr = -corrConstant*pow((Poly6SmoothingKernel(dist, threshold) /
//						Poly6SmoothingKernel(corrThreshold*threshold, threshold)), corrN);
//
//					particles[i].deltaP += (particles[i].lambda + neighbors[j]->lambda + corr)*
//						SpikySmoothingKernelGradient(particles[i].predictPosition - neighbors[j]->predictPosition, threshold);
//				}
//			}
//
//			particles[i].deltaP /= restDensity;
//		}
//
//		for (int i = 0; i < particlesNum; i++)
//		{
//			if (particles[i].isObs)
//				continue;
//
//			particles[i].predictPosition += particles[i].deltaP;
//
//			if (particles[i].predictPosition.y < -borderHeight / 2)
//				particles[i].predictPosition.y = -borderHeight / 2 + borderEpsilon;
//
//			if (particles[i].predictPosition.x < -borderWidth / 2)
//				particles[i].predictPosition.x = -borderWidth / 2 + borderEpsilon;
//			if (particles[i].predictPosition.x > borderWidth / 2)
//				particles[i].predictPosition.x = borderWidth / 2 - borderEpsilon;
//
//			if (particles[i].predictPosition.z < -borderDepth / 2)
//				particles[i].predictPosition.z = -borderDepth / 2 + borderEpsilon;
//			if (particles[i].predictPosition.z > borderDepth / 2)
//				particles[i].predictPosition.z = borderDepth / 2 - borderEpsilon;
//		}
//	}
//
//	// velocity
//	for (int i = 0; i < particlesNum; i++)
//	{
//		if (particles[i].isObs)
//			continue;
//
//		particles[i].velocity = (particles[i].predictPosition - particles[i].position) / frameTime;
//	}
//
//	// color
//	for (int i = 0; i < particlesNum; i++)
//	{
//		if (particles[i].isObs)
//			continue;
//
//		glm::vec3 iPos = particles[i].predictPosition;
//		glm::vec3 colorField = glm::vec3(0.0f);
//
//		vector<PBFParticle*> neighbors = FindNeighbors(particles[i], neighborPadding);
//		for (int j = 0; j < neighbors.size(); j++)
//		{
//			glm::vec3 jPos = neighbors[j]->predictPosition;
//			glm::vec3 relPos = iPos - jPos;
//
//			float dist = glm::distance(glm::vec3(0.0f), relPos);
//
//			if (dist < threshold)
//			{
//				glm::vec3 spiky = SpikySmoothingKernelGradient(relPos, threshold);
//				colorField += spiky / neighbors[j]->density;
//			}
//		}
//
//		particles[i].colorField = colorField;
//		particles[i].force = glm::vec3(0.0f);
//	}
//
//	// surface tension
//	for (int i = 0; i < particlesNum; i++)
//	{
//		if (particles[i].isObs)
//			continue;
//
//		glm::vec3 ipos = particles[i].predictPosition;
//		glm::vec3 surfacetension = glm::vec3(0.0f);
//		glm::vec3 cohesionforce = glm::vec3(0.0f);
//		glm::vec3 curvatureforce = glm::vec3(0.0f);
//
//		vector<PBFParticle*> neighbors = FindNeighbors(particles[i], neighborPadding);
//		for (int j = 0; j < neighbors.size(); j++)
//		{
//			glm::vec3 jpos = neighbors[j]->predictPosition;
//			glm::vec3 relpos = ipos - jpos;
//
//			float dist = glm::distance(glm::vec3(0.0f), relpos);
//
//			if (dist < threshold && particles[i].density + neighbors[j]->density > 1.0f)
//			{
//				float k = 2.0f * restDensity / (particles[i].density + neighbors[j]->density);
//
//				cohesionforce = -surfaceTensionConstant * ComputeCohesion(relpos, dist, threshold);
//				curvatureforce = -surfaceTensionConstant * (particles[i].colorField - neighbors[j]->colorField);
//
//				surfacetension += k * (cohesionforce + curvatureforce);
//			}
//		}
//
//		particles[i].force = surfacetension;
//	}
//
//	// viscosicty
//	for (int i = 0; i < particlesNum; i++)
//	{
//		if (particles[i].isObs)
//			continue;
//
//		glm::vec3 velDirSum = glm::vec3();
//
//		vector<PBFParticle*> neighbors = FindNeighbors(particles[i], neighborPadding);
//		for (int j = 0; j < neighbors.size(); j++)
//		{
//			glm::vec3 velDir = neighbors[j]->velocity - particles[i].velocity;
//			float dist = glm::distance(glm::vec3(), neighbors[j]->predictPosition - particles[i].predictPosition);
//			if (dist < threshold)
//				velDirSum += velDir * Poly6SmoothingKernel(dist, threshold);
//		}
//
//		particles[i].velocity += viscosityConstant * velDirSum;
//		particles[i].position = particles[i].predictPosition;
//	}
//}
//
//void UpdateParticlesPos(PBFParticle* particles, GLfloat* particlesPoses, int row, int col, int dep, int oRow, int oCol, int oDep)
//{
//	for (int i = 0; i < row; i++)
//	{
//		for (int j = 0; j < col; j++)
//		{
//			for (int k = 0; k < dep; k++)
//			{
//				particlesPoses[(i * col * dep + j * dep + k) * 3] = particles[i * col * dep + j * dep + k].position.x;
//				particlesPoses[(i * col * dep + j * dep + k) * 3 + 1] = particles[i * col * dep + j * dep + k].position.y;
//				particlesPoses[(i * col * dep + j * dep + k) * 3 + 2] = particles[i * col * dep + j * dep + k].position.z;
//			}
//		}
//	}
//
//	int p = row*col*dep;
//	for (int i = 0; i < oRow; i++)
//	{
//		for (int j = 0; j < oCol; j++)
//		{
//			for (int k = 0; k < oDep; k++)
//			{
//				particlesPoses[(p + i * oCol * oDep + j * oDep + k) * 3] = particles[p + i * oCol * oDep + j * oDep + k].position.x;
//				particlesPoses[(p + i * oCol * oDep + j * oDep + k) * 3 + 1] = particles[p + i * oCol * oDep + j * oDep + k].position.y;
//				particlesPoses[(p + i * oCol * oDep + j * oDep + k) * 3 + 2] = particles[p + i * oCol * oDep + j * oDep + k].position.z;
//			}
//		}
//	}
//}
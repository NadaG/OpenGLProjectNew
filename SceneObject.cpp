#include "SceneObject.h"

int SceneObject::objectNum = 0;

SceneObject::SceneObject()
{
	id = objectNum++;
	this->positionVector = glm::vec3(0, 0, 0);
	this->scaleVector = glm::vec3(1, 1, 1);
	this->rotationMatrix = glm::mat4(1.0);
}

SceneObject::SceneObject(const Mesh& mesh)
	: SceneObject()
{
	id = objectNum++;
	this->shaderProgram = shaderProgram;
	this->mesh = mesh;
}

void SceneObject::GenerateVBO(const Mesh& mesh)
{
	this->mesh = mesh;
	int floatNum = 14;
	GLfloat* vertexBufferDatas = new GLfloat[this->mesh.GetVertexNum() * floatNum];
	
	for (int i = 0; i < this->mesh.GetVertexNum(); i++)
	{
		vertexBufferDatas[i * floatNum] = this->mesh.GetVertice(i).position.x;
		vertexBufferDatas[i * floatNum + 1] = this->mesh.GetVertice(i).position.y;
		vertexBufferDatas[i * floatNum + 2] = this->mesh.GetVertice(i).position.z;
		vertexBufferDatas[i * floatNum + 3] = 1.0f;
		vertexBufferDatas[i * floatNum + 4] = 1.0f;
		vertexBufferDatas[i * floatNum + 5] = 1.0f;
		vertexBufferDatas[i * floatNum + 6] = this->mesh.GetVertice(i).normal.x;
		vertexBufferDatas[i * floatNum + 7] = this->mesh.GetVertice(i).normal.y;
		vertexBufferDatas[i * floatNum + 8] = this->mesh.GetVertice(i).normal.z;
		vertexBufferDatas[i * floatNum + 9] = this->mesh.GetVertice(i).uv.x;
		vertexBufferDatas[i * floatNum + 10] = this->mesh.GetVertice(i).uv.y;
		vertexBufferDatas[i * floatNum + 11] = this->mesh.GetVertice(i).tangent.x;
		vertexBufferDatas[i * floatNum + 12] = this->mesh.GetVertice(i).tangent.y;
		vertexBufferDatas[i * floatNum + 13] = this->mesh.GetVertice(i).tangent.z;
	}

	// VAO 생성
	GLuint sceneObjectVAO;
	glGenVertexArrays(1, &sceneObjectVAO);
	GLuint vertexBuffer, indexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indexBuffer);

	// 각각의 vertex array를 사용함
	// vertex array object는 밑에서 정의할 glEnableVertexAttribArray(i)의 상태를 갖게 되는 object이다
	// 현재 바인드된 VAO에 따라 vertex의 조합 정보들이 정의되고 저장된다. 
	// 그렇다고 실제로 vertex 정보가 저장된것은 아니다(단지 조합 정보)
	glBindVertexArray(sceneObjectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*this->mesh.GetVertexNum()*floatNum, vertexBufferDatas, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*this->mesh.GetIndexNum(), this->mesh.GetIndices(), GL_STATIC_DRAW);

	int offset = 0;
	int layoutSize;
	for (int j = 0; j < 5; j++)
	{
		// uv
		if (j == 3)
			layoutSize = 2;
		else
			layoutSize = 3;

		glEnableVertexAttribArray(j);
		glVertexAttribPointer(
			j, // layout(location)에 들어갈 숫자이다.
			layoutSize,
			GL_FLOAT,
			GL_FALSE,
			sizeof(GLfloat) * 14, // stride
			(void*)(sizeof(GLfloat) * offset) // 하나의 vertex 정보 set에서 해당 layout이 얼마나 떨어져 있는지
		);
		offset += layoutSize;
	}

	this->vertexBuffer = vertexBuffer;
	this->vertexArrayObject = sceneObjectVAO;
	glBindVertexArray(0);
}

const glm::mat4 SceneObject::GetModelMatrix()
{
	return glm::translate(positionVector) * rotationMatrix * glm::scale(scaleVector);
}

void SceneObject::Translate(const glm::vec3& vec)
{
	this->positionVector += vec;
}

void SceneObject::Rotate(const glm::vec3& vec, float angle)
{
	this->rotationMatrix = glm::rotate(rotationMatrix, angle, vec);
}

void SceneObject::Scale(const glm::vec3& vec)
{
	this->scaleVector.x *= vec.x;
	this->scaleVector.y *= vec.y;
	this->scaleVector.z *= vec.z;
}

void SceneObject::AttachScript(SceneScript* sceneScript)
{
	sceneScript->SetObject(this);
	scripts.push_back(sceneScript);
}

void SceneObject::ScriptsAwake()
{
	for (int i = 0; i < scripts.size(); i++)
		scripts[i]->Awake();
}

void SceneObject::ScriptsStart()
{
	for (int i = 0; i < scripts.size(); i++)
		scripts[i]->Start();
}

void SceneObject::ScriptsUpdate()
{
	for (int i = 0; i < scripts.size(); i++)
		scripts[i]->Update();
}

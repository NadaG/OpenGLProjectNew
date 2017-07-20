#include "SceneObject.h"

int SceneObject::objectNum = 0;

SceneObject::SceneObject()
{
	id = objectNum++;
	this->positionVector = glm::vec3(0, 0, 0);
	this->scaleVector = glm::vec3(1, 1, 1);
	this->rotationMatrix = glm::mat4(1.0);
}

SceneObject::SceneObject(const ShaderProgram& shaderProgram)
	: SceneObject()
{
	id = objectNum++;
	this->shaderProgram = shaderProgram;
}

SceneObject::SceneObject(const ShaderProgram& shaderProgram, const Mesh& mesh)
	: SceneObject()
{
	id = objectNum++;
	this->shaderProgram = shaderProgram;
	this->mesh = mesh;
}

void SceneObject::GenerateVBO(const Mesh& mesh)
{
	this->mesh = mesh;
	int floatNum = shaderProgram.GetFloatNum();
	GLfloat* vertexBufferDatas = new GLfloat[this->mesh.GetVertexNum() * floatNum];

	for (int i = 0; i < this->mesh.GetVertexNum(); i++)
	{
		int offset = 0;
		if (shaderProgram.IsLayoutExist(LAYOUT_POSITION))
		{
			vertexBufferDatas[i * floatNum + offset] = this->mesh.GetVertice(i).position.x;
			vertexBufferDatas[i * floatNum + offset + 1] = this->mesh.GetVertice(i).position.y;
			vertexBufferDatas[i * floatNum + offset + 2] = this->mesh.GetVertice(i).position.z;
			offset += 3;
		}

		if (shaderProgram.IsLayoutExist(LAYOUT_COLOR))
		{
			vertexBufferDatas[i * floatNum + offset] = 1.0f;
			vertexBufferDatas[i * floatNum + offset + 1] = 1.0f;
			vertexBufferDatas[i * floatNum + offset + 2] = 1.0f;
			offset += 3;
		}

		if (shaderProgram.IsLayoutExist(LAYOUT_NORMAL))
		{
			vertexBufferDatas[i * floatNum + offset] = this->mesh.GetVertice(i).normal.x;
			vertexBufferDatas[i * floatNum + offset + 1] = this->mesh.GetVertice(i).normal.y;
			vertexBufferDatas[i * floatNum + offset + 2] = this->mesh.GetVertice(i).normal.z;
			offset += 3;
		}

		if (shaderProgram.IsLayoutExist(LAYOUT_UV))
		{
			vertexBufferDatas[i * floatNum + offset] = this->mesh.GetVertice(i).uv.x;
			vertexBufferDatas[i * floatNum + offset + 1] = this->mesh.GetVertice(i).uv.y;
			offset += 2;
		}
	}

	// VAO 생성
	GLuint sceneObjectVAO;
	glGenVertexArrays(1, &sceneObjectVAO);
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);

	// 각각의 vertex array를 사용함
	glBindVertexArray(sceneObjectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->mesh.GetVertexNum() * shaderProgram.GetFloatNum(), vertexBufferDatas, GL_STATIC_DRAW);

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
				sizeof(GLfloat) * shaderProgram.GetFloatNum(), // stride
				(void*)(sizeof(GLfloat) * offset) // 하나의 vertex 정보 set에서 해당 layout이 얼마나 떨어져 있는지
			);
			offset += shaderProgram.GetLayoutSize((LayoutType)j);
		}
	}

	this->vertexBuffer = vertexBuffer;
	this->vertexArrayObject = sceneObjectVAO;
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
	this->scaleVector *= vec;
}

void SceneObject::AttachScript(SceneScript* sceneScript)
{
	sceneScript->SetObject(this);
	scripts.push_back(sceneScript);
}

void SceneObject::SetUniformMatrix4f(string name, glm::mat4 mat)
{
	glUniformMatrix4fv(glGetUniformLocation(GetShaderProgramID(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void SceneObject::SetUniformVector3f(string name, glm::vec3 vec)
{
	glUniform3f(glGetUniformLocation(GetShaderProgramID(), name.c_str()), vec.x, vec.y, vec.z);
}

void SceneObject::SetUniformVector3f(string name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(GetShaderProgramID(), name.c_str()), x, y, z);
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

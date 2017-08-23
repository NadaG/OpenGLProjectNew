#pragma once

// obj ���Ͽ��� �ҷ����� mesh
#pragma once
#include "DefaultHeader.h"
#include "OpenGLHeader.h"
#include "Face.h"
#include "Vertex.h"
#include "ObjLoader.h"

enum MeshType
{
	TORUS = 0,
	SPHERE = 1,
	QUAD = 2,
	BOX = 3,
	EARTH = 4,
	TREX = 5
};

class Mesh
{
private:
	// each face information
	Vertex* vertices;
	GLuint* indices;
	int vertexNum;
	int indexNum;

	// Question color ������ ���� ���? material information
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

	GLenum drawMode = GL_POINT;
	MeshType meshType;

public:
	Mesh();
	Mesh(MeshType meshType);

	// ���� ������ ������ ��
	Mesh(const Mesh& mesh) 
	{ 
		vertices = new Vertex[1000000];
		for (int i = 0; i < 1000000; i++)
			vertices[i] = mesh.vertices[i];
		vertexNum = mesh.vertexNum;
	}

	virtual ~Mesh()
	{
		delete[] vertices;
	}

	// file name�� ���� mesh�� ����
	void LoadMesh(const string& fileName);
	// enum�� ���� mesh ����
	void LoadMesh(const MeshType& meshType);

	const MeshType& GetMeshType() { return meshType; }
	const glm::vec3& GetAmbient() { return ambientColor; }
	const glm::vec3& GetDiffuse() { return diffuseColor; }
	const glm::vec3& GetSpecular() { return specularColor; }
	const Vertex& GetVertice(int index) { return vertices[index]; }
	const int& GetVertexNum() { return vertexNum; }
	GLuint* GetIndices() { return indices; }
	const GLuint& GetIndexNum() { return indexNum; }

	const GLenum GetDrawMode() { return drawMode; }
};

class Model
{
public:
	
};
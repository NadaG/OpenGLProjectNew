#pragma once

// obj 파일에서 불러오는 mesh
#pragma once
#include "DefaultHeader.h"
#include "OpenGLHeader.h"
#include "Face.h"
#include "Vertex.h"

using std::string;
using std::ifstream;
using std::ofstream;
using std::fstream;

enum MeshType
{
	MESH_NONE = 0,
	CUBE = 1,
	SPHERE = 2,
	CLOTH = 3,
	TWOPOINT = 4,
	COORDINATE = 5,
	IRONMAN = 6,
	QUAD = 7,
	TORUS = 8
};

class Mesh
{
private:
	ifstream inFile;
	ifstream mtlInFile;

	glm::vec3* positions;
	glm::vec3* uvs;
	glm::vec3* normals;

	// each face information
	Face* faces;

	// material information
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

	int vertexNum;
	int uvNum;
	int normalNum;
	int faceNum;

	void ParsePositions(string line, int index);
	void ParseUvs(string line, int index);
	void ParseNormals(string line, int index);

	// return true if face num is 3, return false if face num is 4
	bool ParseFace(string line, int index);
	void ParseMtl(string fileName);
	void ParseLightColor(string line, char mode);

	void GenerateVertices();

	// 일단은 변수를 여기에 모두 담아 놓자!!
	int clothColumn;
	int clothRow;
	// 각 정점간의 거리
	float vertexDist;

	GLenum drawMode = GL_POINT;
	MeshType meshType;

public:
	Mesh()
	{
		//faces = new Face[60000];
		positions = new glm::vec3[60000];
		normals = new glm::vec3[60000];
		uvs = new glm::vec3[60000];
	}

	Mesh(const Mesh& mesh) {}
	virtual ~Mesh()
	{
		delete faces;
		delete positions;
		delete normals;
		delete uvs;

		inFile.close();
		mtlInFile.close();
	}

	void LoadObj(const string& fileName);
	// 내부에서 SetMeshType 함수를 호출함
	void LoadDefaultMesh(const MeshType& meshType);

	void SetMeshType(const MeshType& meshType) { this->meshType = meshType; }
	const MeshType& GetMeshType() { return meshType; }

	Face* GetFaces() { return faces; }
	const Face& GetFaces(int index) { return faces[index]; }

	glm::vec3* GetPositions() { return positions; }
	const glm::vec3& GetPositions(int index) { return positions[index]; }

	glm::vec3* GetNormals() { return normals; }
	const glm::vec3& GetNormals(int index) { return normals[index]; }

	glm::vec3* GetUvs() { return uvs; }
	const glm::vec3& GetUvs(int index) { return uvs[index]; }

	const int& GetVertexNum() { return vertexNum; }
	const int& GetNormalNum() { return normalNum; }
	const int& GetUvNum() { return uvNum; }
	const int& GetFaceNum() { return faceNum; }

	const glm::vec3& GetAmbient() { return ambientColor; }
	const glm::vec3& GetDiffuse() { return diffuseColor; }
	const glm::vec3& GetSpecular() { return specularColor; }

	const GLenum GetDrawMode() { return drawMode; }
};
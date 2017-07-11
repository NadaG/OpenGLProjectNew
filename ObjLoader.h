#pragma once

#include "DefaultHeader.h"
#include "OpenGLHeader.h"
#include "Vertex.h"
#include "Face.h"

using std::string;
using std::ifstream;
using std::ofstream;
using std::fstream;

class ObjLoader
{
public:
	ObjLoader()
	{
		positions = new glm::vec3[60000];
		uvs = new glm::vec3[60000];
	    normals = new glm::vec3[60000];
		faces = new Face[60000];
	}

	virtual ~ObjLoader(){}

	void LoadObj(const string& fileName);
	const int& GenerateVertices(Vertex* vertices);

private:

	ifstream inFile;
	ifstream mtlInFile;

	int vertexNum;
	int uvNum;
	int normalNum;
	int faceNum;

	glm::vec3* positions;
	glm::vec3* uvs;
	glm::vec3* normals;
	Face* faces;

	void ParsePositions(string line, int index);
	void ParseUvs(string line, int index);
	void ParseNormals(string line, int index);
	// return true if face num is 3, return false if face num is 4
	void ParseMtl(string fileName);
	void ParseLightColor(string line, const char& mode);

	bool ParseFace(string line, int index);
	
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
};